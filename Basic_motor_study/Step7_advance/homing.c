/*
 * homing.c
 * Step 15: 호밍 시퀀스 구현
 */
 
 #include "homing.h"
 #include "configuration.h"
 #include "accel.h"
 #include <wiringPi.h>
 #include <stdio.h>
 #include <math.h>
 
 /* ================================
  * 리미트 스위치 초기화
  * ================================
  * pinMode(pin, INPUT_PULLUP):
  *   - 핀을 입력 모드로 설정
  *   - 내부 풀업 저항 활성화 → 아무것도 연결 안 하면 HIGH
  *   - 스위치가 GND에 연결되면 LOW (NC 방식)
  */
 void initLimitSwitches(void) {
	 pinMode(X_LIMIT_MIN, INPUT);
	 pullUpDnControl(X_LIMIT_MIN, PUD_UP); // 내부 풀업 저항 활성화
	 
	 pinMode(Y_LIMIT_MIN, INPUT);
	 pullUpDnControl(Y_LIMIT_MIN, PUD_UP);
	 
	 printf("리미트 스위치 초기화 완료\n");
	 printf("X_LIMIT_MIN: GPIO %d\n", X_LIMIT_MIN);
	 printf("Y_LIMIT_MIN: GPIO %d\n", Y_LIMIT_MIN);
 }
 
/* ===========================================================
 * 리미트 스위치 상태 확인
 * ===========================================================
 * NC 방식: 평소 HIGH, 눌리면 LOW
 * digitalRead() == LOW  → 스위치 감지 → 1 반환
 * digitalRead() == HIGH → 미감지     → 0 반환
 */
 int isXLimitTriggered(void){
	 return (digitalRead(X_LIMIT_MIN) == LOW) ? 1 : 0;
 }
 
 int isYLimitTriggered(void){
	 return (digitalRead(Y_LIMIT_MIN) == LOW) ? 1 : 0;
 } 
 
/* ===========================================================
 * 단일 축 호밍 (내부 공통 함수)
 * ===========================================================
 *
 * 동작 원리:
 *   1. 리미트 스위치 방향(-방향)으로 step 하나씩 이동
 *   2. 매 step마다 스위치 상태 확인 (폴링 방식)
 *   3. 스위치 감지 → 정지 → 후퇴 → 원점 리셋
 *
 * [폴링(Polling) vs 인터럽트]
 *   폴링: 루프 안에서 계속 확인 → 구현 간단, 느린 반응
 *   인터럽트: 신호 발생 시 즉시 반응 → Step 16에서 학습 예정
 *   호밍은 느린 속도로 진행하므로 폴링으로 충분히 정확함
 *
 * 매개변수:
 *   axis        : 이동할 축 (x_axis 또는 y_axis)
 *   limit_check : 리미트 스위치 확인 함수 포인터
 *   err_code    : 실패 시 반환할 에러 코드
 */
 static int homeAxis(Axis *axis,
					 int (*limit_check)(void),
					 const char *axis_name,
					 int err_code){
						 
	printf("\n[%s축 호밍 시작]\n", axis_name);
	
    /* ---- 1단계: 스위치가 이미 눌려있는지 확인 ---- */
    /*
     * 이미 스위치가 눌린 상태라면 모터가 이미 원점에 있거나
     * 스위치 배선 문제일 수 있다.
     * 일단 살짝 +방향으로 후퇴 후 다시 시도
     */
    if (limit_check()) {
        printf("  warning, Limit switch detected, retraction for safe\n");
        digitalWrite(axis->dir_pin, HIGH);  // + 방향
        int backoff_steps = (int)(HOMING_BACKOFF_MM * axis->steps_per_mm);
        for (int i = 0; i < backoff_steps; i++) {
            digitalWrite(axis->step_pin, HIGH);
            delayMicroseconds(500);
            digitalWrite(axis->step_pin, LOW);
            delayMicroseconds(500);
        }
    }
	
    /* ---- 2단계: -방향으로 천천히 이동하며 스위치 탐색 ---- */
    /*
     * HOMING_MAX_MM까지 step 하나씩 이동
     * 매 step마다 limit_check()로 스위치 확인
     */
    int max_steps = (int)(HOMING_MAX_MM * axis->steps_per_mm);

    /* 호밍 속도로 delay 계산 */
    int homing_delay = speedToDelay(HOMING_SPEED_MM_S, axis->steps_per_mm);

    printf("  -방향으로 이동 중 (최대 %.0fmm 탐색)...\n", HOMING_MAX_MM);

    /* -방향 설정 */
    digitalWrite(axis->dir_pin, LOW);

    int triggered_step = -1;  // 스위치 감지된 step 위치

    for (int i = 0; i < max_steps; i++) {

        /* step 실행 */
        digitalWrite(axis->step_pin, HIGH);
        delayMicroseconds(homing_delay);
        digitalWrite(axis->step_pin, LOW);
        delayMicroseconds(homing_delay);

        /* 스위치 확인 (폴링) */
        if (limit_check()) {
            triggered_step = i;
            printf("  스위치 감지! (%d steps 이동)\n", i + 1);
            break;
        }

        /* 100 step마다 진행 상황 표시 */
        if ((i + 1) % 100 == 0) {
            printf("  탐색 중... %.1fmm\n",
                   (float)(i + 1) / axis->steps_per_mm);
        }
    }

    /* ---- 3단계: 스위치 미감지 → 호밍 실패 ---- */
    if (triggered_step == -1) {
        printf("  오류: %s축 호밍 실패 - 리미트 스위치 미감지!\n", axis_name);
        printf("  확인 사항:\n");
        printf("    1. 리미트 스위치 배선 확인\n");
        printf("    2. HOMING_MAX_MM 값이 충분한지 확인\n");
        printf("    3. 스위치 핀 번호 확인 (configuration.h)\n");
        return err_code;
    }

    /* ---- 4단계: 스위치에서 후퇴 (Backoff) ---- */
    /*
     * 스위치를 누른 상태로 원점 삼으면 스위치가 계속 눌려있어 문제 발생
     * 살짝 +방향으로 후퇴해서 스위치가 해제된 위치에서 원점 설정
     */
    printf("  후퇴 중 (%.1fmm)...\n", HOMING_BACKOFF_MM);

    digitalWrite(axis->dir_pin, HIGH);  // + 방향으로 후퇴

    int backoff_steps = (int)(HOMING_BACKOFF_MM * axis->steps_per_mm);
    for (int i = 0; i < backoff_steps; i++) {
        digitalWrite(axis->step_pin, HIGH);
        delayMicroseconds(homing_delay);
        digitalWrite(axis->step_pin, LOW);
        delayMicroseconds(homing_delay);
    }
	
    /* ---- 5단계: 원점 설정 ---- */
    /*
     * 현재 위치를 0으로 리셋
     * 이후 모든 이동은 이 점 기준으로 계산됨
     */
    axis->current_mm    = 0.0;
    axis->current_steps = 0;

    printf("  [%s축 호밍 완료] 원점 설정: 0.0mm\n", axis_name);

    return HOMING_OK;
}

/* ===========================================================
 * X축 호밍
 * ===========================================================
 * static 함수 homeAxis()를 X축 설정으로 호출
 * isXLimitTriggered를 함수 포인터로 전달
 */
int homeAxisX(void) {
    return homeAxis(&x_axis, isXLimitTriggered, "X", HOMING_ERR_X);
}

/* ===========================================================
 * Y축 호밍
 * ===========================================================
 */
int homeAxisY(void) {
    return homeAxis(&y_axis, isYLimitTriggered, "Y", HOMING_ERR_Y);
}

/* ===========================================================
 * 전체 호밍 시퀀스
 * ===========================================================
 *
 * 순서: Y축 먼저 → X축
 *
 * Y를 먼저 하는 이유 (3D 프린터/CNC 관례):
 *   장비 구조에 따라 다르지만, 일반적으로
 *   Y(앞뒤)를 먼저 안전한 위치로 보낸 뒤 X(좌우) 호밍
 *   → 헤드/엔드이펙터 충돌 방지
 */
 int runHomingSequence(void) {
	 
     printf("\n============ 호밍 시퀀스 시작 ============\n");	 
	 printf("순서: Y축 -> X축\n");
	 printf("호밍 속도: %.1f mm/s\n\n", HOMING_SPEED_MM_S);
	 
	 // Y축 호밍
	 int result = homeAxisY();
	 if (result != HOMING_OK) {
		 printf("\n[호밍 중단]Y축 실패(에러코드: %d)\n",result);
		 printf("X축 호밍은 취소되었습니다.\n");
		 return result;
	 }
	 
    /* X축 호밍 */
    result = homeAxisX();
    if (result != HOMING_OK) {
        printf("\n[호밍 중단] X축 실패 (에러 코드: %d)\n", result);
        return result;
    }

    /* 성공 */
    printf("\n========== 호밍 시퀀스 완료 ==========\n");
    printf("현재 위치: X=%.2fmm, Y=%.2fmm (원점)\n",
           x_axis.current_mm, y_axis.current_mm);
    printf("======================================\n\n");

    return HOMING_OK;
}	 