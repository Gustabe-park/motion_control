/*
 * interrupt.c
 * Step 19: 인터럽트 처리 + 비상 정지 구현
 *
 * ======== ISR(Interrupt Service Routine) 주의사항 ========
 *
 * ISR은 메인 루프와 동시에 실행될 수 있는 특수 함수.
 * 아래 규칙을 지켜야 안전:
 *
 *  1. 최대한 짧게: 플래그 세우고 바로 리턴
 *  2. printf 자제: 내부적으로 버퍼 락이 있어 deadlock 위험
 *                  (Linux에서는 대부분 괜찮지만 습관상 피하기)
 *  3. volatile 변수만 수정
 *  4. malloc/free 금지
 *
 * → 이 코드에서는 g_emergency_stop 플래그만 세움
 *   실제 출력은 메인 루프(accel.c, gcode_runner.c)에서 담당
 */
 
 #include "interrupt.h"
 #include "configuration.h"
 #include <wiringPi.h>
 #include <stdio.h>
 #include <stdbool.h>

/* ============================================================
 * 전역 변수 실제 생성
 * ============================================================ */

/* 비상 정지 플래그 */
volatile bool g_emergency_stop = false;

/*
 * 디바운싱용 마지막 인터럽트 시각 (ms)
 *
 * 각 핀마다 독립적으로 관리
 * static: 이 파일 안에서만 사용 (외부에 노출 불필요)
 */
 static volatile unsigned long g_last_x_irq_ms = 0;
 static volatile unsigned long g_last_y_irq_ms = 0;
 static volatile unsigned long g_last_estop_ms = 0;

/* ============================================================
 * ISR 함수들 (wiringPiISR에 등록할 콜백)
 * ============================================================
 *
 * wiringPiISR 콜백 형식: void func(void)
 * 매개변수도 반환값도 없음 → 어떤 핀인지 함수 이름으로 구분
 *
 * 디바운싱 로직:
 *   millis() : wiringPi 제공, 프로그램 시작 후 경과 ms
 *   now - g_last_*_ms < DEBOUNCE_MS → 너무 빠른 재진입 → 무시
 */ 

/* X축 리미트 스위치 ISR */
static void isrXLimit(void) {
	unsigned long now = millis();
	
    /* 디바운싱: DEBOUNCE_MS 이내 재진입 무시 */	
	if (now- g_last_x_irq_ms < DEBOUNCE_MS) {
		return;
	}
	g_last_x_irq_ms = now;
	
    /* 비상 정지 플래그 설정 */
	g_emergency_stop = true;
    /* 참고: printf 대신 플래그만 세움. 출력은 메인 루프에서 */
}

/* y축 리미트 스위치 ISR */
static void isrYLimit(void) {
	unsigned long now = millis();
	
	if (now- g_last_y_irq_ms < DEBOUNCE_MS) {
		return;
	}
	g_last_y_irq_ms = now;
	
	g_emergency_stop = true;

}

/* ============================================================
 * initInterrupts: 인터럽트 초기화
 * ============================================================
 *
 * wiringPiISR(pin, mode, function):
 *   pin      : 감지할 GPIO 핀
 *   mode     : 감지 엣지 종류
 *              INT_EDGE_FALLING: HIGH→LOW (NC 방식 스위치가 눌릴 때)
 *              INT_EDGE_RISING : LOW→HIGH
 *              INT_EDGE_BOTH   : 양쪽 다
 *   function : ISR 함수 포인터
 *
 * 참고: wiringPi가 내부적으로 별도 스레드를 만들어 인터럽트 감시
 *       → 메인 루프와 동시에 실행 가능
 */
 void initInterrupts(void) {
	 
    /* 비상 정지 버튼 핀 초기화 */
    pinMode(ESTOP_PIN, INPUT);
	pullUpDnControl(ESTOP_PIN, PUD_UP);  /* 내부 풀업 → 평소 HIGH */ 

    /*
     * X, Y 리미트는 homing.c의 initLimitSwitches()에서
     * 이미 INPUT + 풀업 설정 완료
     * → 여기서는 ISR 등록만
     */
	 
    /* ISR 등록 */
	if (wiringPiISR(X_LIMIT_MIN, INT_EDGE_FALLING, &isrXLimit) < 0) {
		printf("오류: X 리미트 ISR 등록 실패\n");
	}
	if (wiringPiISR(Y_LIMIT_MIN, INT_EDGE_FALLING, &isrYLimit) < 0) {
		printf("오류: Y 리미트 ISR 등록 실패\n");
	}
	if (wiringPiISR(ESTOP_PIN_MIN, INT_EDGE_FALLING, &isrEStop) < 0) {
		printf("오류: 비상 정지 ISR 등록 실패\n");
	}
	
    printf("인터럽트 초기화 완료\n");
    printf("  X 리미트  : GPIO %d → ISR 등록\n", X_LIMIT_MIN);
    printf("  Y 리미트  : GPIO %d → ISR 등록\n", Y_LIMIT_MIN);
    printf("  비상 정지 : GPIO %d → ISR 등록\n", ESTOP_PIN);
}	

/* ============================================================
 * isEmergencyStop: 비상 정지 상태 확인
 * ============================================================ */
bool isEmergencyStop(void) {
    return g_emergency_stop;
}

/* ============================================================
 * resetEmergencyStop: 비상 정지 해제
 * ============================================================
 *
 * 재시작 전 안전 확인 절차:
 *   1. 리미트 스위치가 여전히 눌려있으면 해제 거부
 *   2. 물리적으로 안전한 상태 확인 후 플래그 해제
 */
 void resetEmergencyStop(void) {

    printf("\n===== 비상 정지 해제 절차 =====\n");

    /* 리미트 스위치 물리 상태 확인 */
    if (digitalRead(X_LIMIT_MIN) == LOW) {
        printf("경고: X축 리미트 스위치가 아직 눌려 있습니다!\n");
        printf("     모터를 물리적으로 스위치에서 벗어나게 한 후 다시 시도하세요.\n");
        return;
    }
    if (digitalRead(Y_LIMIT_MIN) == LOW) {
        printf("경고: Y축 리미트 스위치가 아직 눌려 있습니다!\n");
        return;
    }
    if (digitalRead(ESTOP_PIN) == LOW) {
        printf("경고: 비상 정지 버튼이 아직 눌려 있습니다!\n");
        printf("     버튼을 해제(당기기/돌리기)한 후 다시 시도하세요.\n");
        return;
    }

    /* 모든 스위치가 해제된 경우에만 플래그 리셋 */
    g_emergency_stop = false;

    printf("비상 정지 해제 완료.\n");
    printf("-> 호밍 후 작업을 재개하세요. (권장: HOMING 명령 실행)\n");
    printf("================================\n\n");
}