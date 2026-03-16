/*
 * homing.h
 * Step 15: 호밍 시퀀스 구현
 *
 * ======== 호밍(Homing)이란? ========
 * 전원 켤 때 모터는 자신의 위치를 모른다.
 * 리미트 스위치(물리적 끝점 감지 센서)에 닿을 때까지
 * 천천히 이동해서 "여기가 0mm다!" 라고 기준을 잡는 과정.
 *
 * ======== 호밍 순서 ========
 *
 *  [1] Y축 먼저 호밍 → [2] X축 호밍
 *       (충돌 방지를 위해 한 축씩)
 *
 *  각 축 호밍 절차:
 *   ① 리미트 스위치 방향(- 방향)으로 천천히 이동
 *   ② 스위치 감지되면 즉시 정지
 *   ③ 스위치에서 살짝 후퇴 (BACKOFF)
 *   ④ current_mm = 0으로 리셋 → 이 점이 원점!
 *
 * ======== 리미트 스위치 배선 ========
 *
 *   NC (Normally Closed) 방식 사용:
 *     평소: GPIO HIGH (내부 풀업 저항)
 *     눌림: GPIO LOW  (스위치가 GND에 연결)
 *
 *   NC 방식 선택 이유:
 *     선이 끊어지거나 스위치 고장 시 LOW 신호 발생
 *     → 안전하게 비상정지 처리 가능
 *
 * ======== 반환값 (에러 코드) ========
 *   HOMING_OK      (0)  : 성공
 *   HOMING_ERR_X  (-1)  : X축 호밍 실패 (스위치 미감지)
 *   HOMING_ERR_Y  (-2)  : Y축 호밍 실패 (스위치 미감지)
 */

 #ifndef HOMING_H
 #define HOMING_H
 
// 에러 코드 정의
 #define HOMING_OK			0
 #define HOMING_ERR_X		(-1)
 #define HOMING_ERR_Y		(-2)
 
/* ========== 함수 선언 ========== */

/*
 * 리미트 스위치 핀 초기화
 * - INPUT + 풀업 저항 설정
 * - main()의 wiringPiSetupGpio() 이후 호출
 */ 
 void initLimitSwitches(void);
 
/*
 * 특정 축 리미트 스위치 상태 확인
 * 반환값: 1 = 스위치 눌림(감지), 0 = 미감지
 */
 int isXLimitTriggered(void);
 int isYLimitTriggered(void); 
 
/*
 * X축 단독 호밍
 * 반환값: HOMING_OK or HOMING_ERR_X
 */
 int homeAxisX(void);
 
/*
 * Y축 단독 호밍
 * 반환값: HOMING_OK or HOMING_ERR_Y
 */
 int homeAxisY(void);

/*
 * 전체 호밍 시퀀스 (Y → X 순서)
 * 반환값: HOMING_OK, HOMING_ERR_X, HOMING_ERR_Y
 */ 
 int runHomingSequence(void);
 
 #endif //HOMING_H