/*
 * statemachine.h
 * Step 18: 상태 머신 설계
 *
 * ======== 상태 머신이란? ========
 * 시스템이 "지금 무엇을 하고 있는지"를 명확히 추적하는 구조
 * 상태에 따라 허용되는 동작이 달라진다.
 *
 * ======== 상태 목록 ========
 *
 *  STATE_IDLE    : 대기 중. 모든 명령 수용 가능
 *  STATE_HOMING  : 호밍 실행 중. 이동 명령 불가
 *  STATE_RUNNING : G-code 파일 실행 중. 이동 명령 불가
 *  STATE_PAUSED  : 일시 정지 (Step 19 인터럽트와 연결 예정)
 *  STATE_ERROR   : 오류 발생. RESET 명령으로만 복구
 *
 * ======== 상태 전환 다이어그램 ========
 *
 *              HOMING 명령          성공
 *   ┌─────────────────────[HOMING]──────────┐
 *   │                        │ 실패          │
 *   │                        ▼              ▼
 * [IDLE] ──GFILE────▶ [RUNNING] ──완료──▶ [IDLE]
 *   ▲                    │
 *   │   RESET            └──오류──▶ [ERROR]
 *   └────────────────────────────────────────
 *
 * ======== 설계 원칙 ========
 * 1. 상태 전환은 반드시 setState()를 통해서만
 * 2. 명령 실행 전 isAllowed()로 허용 여부 확인
 * 3. 상태 변경 시 자동으로 로그 출력 (디버깅 용이)
 */

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <stdbool.h>

/* ========== 상태 enum 정의 ========== */
/*
 * enum(열거형): 관련된 상수들에 이름을 붙이는 방법
 *
 * 아래처럼 쓰지 않고 enum을 쓰는 이유:
 *   #define STATE_IDLE    0   ← 숫자만 봐서는 의미 모름
 *   #define STATE_RUNNING 2   ← 실수로 범위 벗어난 숫자 사용 가능
 *
 * enum을 쓰면:
 *   SystemState s = STATE_RUNNING; ← 타입이 명확함
 *   switch(s) 에서 컴파일러가 누락된 case 경고 가능
 */ //enum은 신호등과 유사함. 구조체와 달리 하나의 값만 선택해서 사용
typedef enum {
    STATE_IDLE    = 0,  /* 대기 중 */
    STATE_HOMING  = 1,  /* 호밍 실행 중 */
    STATE_RUNNING = 2,  /* G-code 파일 실행 중 */
    STATE_PAUSED  = 3,  /* 일시 정지 (Step 19 예정) */
    STATE_ERROR   = 4   /* 오류 발생 */
} SystemState;

/* ========== 명령 종류 enum ========== */
/*
 * 어떤 명령인지 표현하는 enum
 * isAllowed()에서 "이 상태에서 이 명령이 가능한가?" 판단에 사용
 */
typedef enum {
    CMD_MOVE,    /* X, Y, LINEAR 이동 명령 */
    CMD_HOMING,  /* HOMING, HOMEX, HOMEY 명령 */
    CMD_GFILE,   /* GFILE (파일 실행) 명령 */
    CMD_GCODE,   /* GCODE (한 줄 실행) 명령 */
    CMD_TEACH,   /* TEACH, RUN 등 티칭 명령 */
    CMD_RESET    /* ERROR 상태 복구 명령 */
} CommandType;

/* ========== 전역 상태 변수 (extern 선언) ========== */
/*
 * g_system_state: 현재 시스템 상태
 *
 * 직접 변경 금지! 반드시 setState()를 통해서만 변경
 * 직접 변경하면 로그도 안 남고, 잘못된 전환도 걸러지지 않음
 */
extern SystemState g_system_state;

/* ========== 함수 선언 ========== */

/*
 * 상태 머신 초기화
 * - 초기 상태를 STATE_IDLE로 설정
 * - main() 또는 initCommandSystem()에서 호출
 */
void initStateMachine(void);

/*
 * 상태 전환 함수
 *
 * 매개변수:
 *   new_state : 전환할 새 상태
 *
 * 동작:
 *   - 현재 상태 → 새 상태 로그 출력
 *   - g_system_state 업데이트
 *
 * 사용 예시:
 *   setState(STATE_HOMING);   // 호밍 시작 전
 *   setState(STATE_IDLE);     // 호밍 완료 후
 */
void setState(SystemState new_state);

/*
 * 현재 상태 반환
 *
 * 반환값: 현재 SystemState 값
 *
 * 사용 예시:
 *   if (getState() == STATE_IDLE) { ... }
 */
SystemState getState(void);

/*
 * 현재 상태에서 해당 명령이 허용되는지 확인
 *
 * 반환값:
 *   true  = 허용 (명령 실행 가능)
 *   false = 차단 (명령 실행 불가, 이유도 출력)
 *
 * 사용 예시:
 *   if (!isAllowed(CMD_MOVE)) return;  // 차단되면 그냥 리턴
 *   moveAxisBySpeed(...);              // 허용되면 실행
 */
bool isAllowed(CommandType cmd);

/*
 * 현재 상태를 문자열로 반환
 *
 * 디버깅 및 상태 출력용
 *
 * 반환값 예시: "IDLE", "RUNNING", "ERROR"
 */
const char* getStateName(void);

/*
 * 현재 상태 출력
 * 사용 예시: CMD> STATUS 명령에서 호출
 */
void printState(void);

#endif /* STATEMACHINE_H */