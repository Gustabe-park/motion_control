/*
 * statemachine.c
 * Step 18: 상태 머신 구현
 */

#include "statemachine.h"
#include <stdio.h>

/* ========== 전역 상태 변수 실제 생성 ========== */
/*
 * statemachine.h에서 extern으로 "선언"만 했고,
 * 여기서 실제로 "생성" + 초기값 설정
 *
 * 초기값 STATE_IDLE: 프로그램 시작 시 대기 상태
 */
 SystemState g_system_state = STATE_IDLE;
 
 /* ========== 내부 도우미: 상태 이름 문자열 배열 ========== */
/*
 * 상태 enum 값을 문자열로 변환하는 테이블
 *
 * state_names[STATE_IDLE]    = "IDLE"
 * state_names[STATE_HOMING]  = "HOMING"
 * ...
 *
 * enum 값과 배열 인덱스가 일치하므로 switch 없이 바로 접근 가능
 */
 static const char *state_names[] = {
	 "IDLE",			/* 0 = STATE_IDLE		*/
	 "HOMING",			/* 1 = STATE_HOMING		*/
	 "RUNNING",			/* 2 = STATE_RUNNING	*/
	 "PAUSED",			/* 3 = STATE_PAUSED		*/
	 "ERROR",			/* 4 = STATE_ERROR		*/
 };
 
/* ========== 초기화 ========== */
 void initStateMachine(void) {
	 g_system_state = STATE_IDLE;
	 printf("[상태 머신]초기화 완료-> 상태: %s\n", getStateName());
 }
 
/* ========== 상태 전환 ========== */
/*
 * setState: 상태 전환 + 로그 출력
 *
 * 같은 상태로 전환하는 경우(예: IDLE→IDLE)는
 * 불필요하므로 체크 후 무시
 */
void setState(SystemState new_state) {
	if (g_system_state == new_state) {
		return; // 같은 상태면 무시
	}
	
	printf("[상태 전환]%s -> %s\n",
		   state_names[g_system_state],
		   state_names[new_state]);
		   
	g_system_state = new_state;
}

/*======== 현재 상태 반환 =============*/
SystemState getState(void) {
	return g_system_state;
}

/*======== 현재 상태 이름 문자열 반환 =============*/
const char* getStateName(void) {
	return state_names[g_system_state];
}

/* ========== 명령 허용 여부 판단 ========== */
/*
 * isAllowed: 현재 상태에서 명령 실행 가능 여부 판단
 *
 * 허용 테이블:
 *
 *           IDLE  HOMING  RUNNING  PAUSED  ERROR
 * CMD_MOVE   ✅     ❌       ❌       ❌      ❌
 * CMD_HOMING ✅     ❌       ❌       ❌      ❌
 * CMD_GFILE  ✅     ❌       ❌       ❌      ❌
 * CMD_GCODE  ✅     ❌       ❌       ❌      ❌
 * CMD_TEACH  ✅     ❌       ❌       ❌      ❌
 * CMD_RESET  ❌     ❌       ❌       ❌      ✅  ← ERROR만 RESET 가능
 *
 * 설계 원칙:
 *   IDLE 상태에서는 거의 모든 명령 허용
 *   그 외 상태에서는 RESET(오류 복구)만 허용
 */
 bool isAllowed(CommandType cmd) {
	 
	 /*-CMD_RESET: ERROR 상태에서만 허용-*/
	 if (cmd == CMD_RESET) {
		 if (g_system_state == STATE_ERROR) {
			 return true;
		 }
		 printf("[차단]RESET은 ERROR 상태에서만 사용 가능함\n");
		 printf(" 현재 상태: %s\n", getStateName());
		 return false;
	 }
	 
	 /*-그 외 모든 명령: IDLE 상태에서만 허용-*/
	 if (g_system_state == STATE_IDLE) {
		 return true;
	 }
	 
    /* ── 차단 시 이유 출력 ── */
    /*
     * 상태별 안내 메시지를 다르게 출력
     * 사용자가 "왜 안 되는지" 바로 알 수 있도록
     */
    switch (g_system_state) {
        case STATE_HOMING:
            printf("[차단] 호밍 진행 중입니다. 완료 후 명령을 입력하세요.\n");
            break;
        case STATE_RUNNING:
            printf("[차단] G-code 실행 중입니다. 완료 후 명령을 입력하세요.\n");
            printf("       (실행 완료까지 기다리거나 Step 19 인터럽트로 중단 가능)\n");
            break;
        case STATE_PAUSED:
            printf("[차단] 일시 정지 상태입니다.\n");
            printf("       RESUME으로 재개하거나 RESET으로 초기화하세요.\n");
            break;
        case STATE_ERROR:
            printf("[차단] 오류 상태입니다. RESET 명령으로 복구하세요.\n");
            break;
        default:
            printf("[차단] 현재 상태(%s)에서는 불가합니다.\n", getStateName());
            break;
    }

    return false;
}

/* ========== 상태 출력 ========== */
/*
 * printState: 현재 상태를 보기 좋게 출력
 * CMD> STATUS 명령에서 호출
 */
 void printState(void) {
    printf("\n===== 시스템 상태 =====\n");
    printf("현재 상태: %s\n", getStateName());

    /* 상태별 추가 안내 */
    switch (g_system_state) {
        case STATE_IDLE:
            printf("안내: 명령 대기 중. 모든 명령 사용 가능.\n");
            break;
        case STATE_HOMING:
            printf("안내: 호밍 진행 중. 완료까지 기다리세요.\n");
            break;
        case STATE_RUNNING:
            printf("안내: G-code 실행 중. 완료까지 기다리세요.\n");
            break;
        case STATE_PAUSED:
            printf("안내: 일시 정지 상태. RESUME 또는 RESET 사용.\n");
            break;
        case STATE_ERROR:
            printf("안내: 오류 상태. RESET 명령으로 복구하세요.\n");
            break;
    }
    printf("======================\n\n");
}