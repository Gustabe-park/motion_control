/*
 * interrupt.h
 * Step 19: 인터럽트 처리 + 비상 정지
 *
 * ======== 이 모듈의 역할 ========
 * - wiringPiISR로 리미트 스위치 + 비상 정지 버튼 감지
 * - 감지 즉시 g_emergency_stop 플래그를 true로 설정
 * - 다른 모듈(accel.c, gcode_runner.c)이 플래그를 보고 즉시 정지
 *
 * ======== 폴링 vs 인터럽트 ========
 *
 *   폴링  : 루프마다 내가 직접 확인 → CPU 낭비, 반응 느림
 *   인터럽트: 하드웨어가 신호 감지 시 자동 호출 → 즉시 반응
 *
 *   호밍은 느리게 이동하므로 폴링으로 충분했음 (Step 15)
 *   G-code 실행 중 고속 이동은 인터럽트가 필수!
 *
 * ======== volatile 키워드 ========
 *
 *   ISR(인터럽트 서비스 루틴)은 메인 루프와 다른 타이밍에 실행됨.
 *   컴파일러는 "이 변수 안 바뀌겠지"하고 레지스터에 캐싱할 수 있음.
 *   volatile을 붙이면 "항상 메모리에서 직접 읽어라"고 강제함.
 *
 *   volatile bool g_emergency_stop;
 *     → 메인 루프가 ISR이 바꾼 값을 즉시 인식 가능
 *
 * ======== 디바운싱(Debouncing) ========
 *
 *   기계식 스위치는 눌릴 때 수ms~수십ms 동안 빠르게 ON/OFF 반복
 *   이를 채터링(Chattering)이라 부름.
 *
 *   해결책: 마지막 인터럽트 시각을 기록하고,
 *            DEBOUNCE_MS 이내 재진입은 무시
 */

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdbool.h>

/* ========== 전역 플래그 (extern 선언) ========== */

/*
 * g_emergency_stop: 비상 정지 플래그
 *
 * ISR에서 true로 설정됨
 * accel.c, gcode_runner.c가 이 플래그를 체크하고 정지
 * resetEmergencyStop()으로 false로 되돌림
 */
extern volatile bool g_emergency_stop;

/* ========== 함수 선언 ========== */

/*
 * 인터럽트 초기화
 * - 비상 정지 버튼 핀 설정
 * - wiringPiISR로 ISR 함수 등록
 * - main()에서 wiringPiSetupGpio() 이후 호출
 */
void initInterrupts(void);

/*
 * 비상 정지 상태 확인
 * 반환값: true = 비상 정지 발동 중
 */
bool isEmergencyStop(void);

/*
 * 비상 정지 해제 (재시작 절차)
 * - 스위치가 물리적으로 해제되었는지 먼저 확인
 * - 안전 확인 후 g_emergency_stop = false
 */
void resetEmergencyStop(void);

#endif /* INTERRUPT_H */