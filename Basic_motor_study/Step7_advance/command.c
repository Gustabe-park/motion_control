/*
 * command.c
 * 명령어 처리 및 모터 설정값 관리
 * Step 18: 상태 머신 연동 추가
 *
 * ===== Step 18에서 변경된 내용 =====
 * 1. #include "statemachine.h" 추가
 * 2. initCommandSystem()에서 initStateMachine() 호출
 * 3. 각 명령 실행 전 isAllowed() 체크 추가
 * 4. HOMING 명령: 실행 전후 setState() 호출
 * 5. STATUS 명령 추가: 현재 상태 출력
 * 6. RESET 명령 추가: ERROR 상태 복구
 */
 
#include "interpolate.h"	/* 선형 보간 기능 사용 */
#include "command.h"
#include "accel.h"
#include "teaching.h"   	/* 티칭 펜던트 기능 */
#include "homing.h"     	/* 호밍 기능 */
#include "gcode_parser.h" 	/*gcode파서*/
#include "gcode_runner.h"	/*gcode 파일 실행*/
#include "statemachine.h"   /* ← Step 18 추가 */
#include <stdio.h>
#include <string.h>
//========= 전역 변수 실제 생성==========
// command.h에서는 extern으로 "선언"만 했고,
// 여기서 실제로 "생성"하고 초기값 설정

float g_speed_mm_s = 50.0; 		// 기본 속도
float g_accel_mm_s2 = 200.0;		// 기본 가속도
bool g_running = true;				// 프로그램 실행 관리

// ===== 초기화 함수 =========
// 프로그램 시작할 떄,한 번 호출
// 설정값을 기본값으로 리셋
void initCommandSystem(void) {
	 printf("\n ===== 명령어 시스템 초기화 =====\n");
	 printf("기본 속도: %.1f mm/s \n", g_speed_mm_s);
	 printf("가속 스텝: %.1f mm/s2\n", g_accel_mm_s2);
	 initMotionBuffer();     // ← 이 한 줄 추가
	 initStateMachine();     /* ← Step 18 추가: 상태 머신 초기화 */
	 printf("준비완료\n\n");
}

void runCommandLoop(void) {
	 char command[100];		// 명령어 저장 버퍼
	 char keyword[50];		// 첫 단어 (명령어)
    printf("====명령어 모드 시작====\n");
    printf("사용 가능한 명령어:\n");
    printf(" SPEED <값>      - 속도 설정\n");
    printf(" ACCEL <값>      - 가속도 설정\n");
    printf(" X <값>          - X축 이동\n");
    printf(" Y <값>          - Y축 이동\n");
    printf(" HOME            - 소프트 원점 복귀 (좌표 0으로 이동)\n");
    printf(" --- 호밍 명령어 ---\n");
    printf(" HOMING          - 리미트 스위치 기반 물리적 원점 탐색 (Y->X)\n");
    printf(" HOMEX           - X축만 호밍\n");
    printf(" HOMEY           - Y축만 호밍\n");
    printf(" LIMIT           - 리미트 스위치 상태 확인\n");
    printf(" --- 보간 명령어 ---\n");
    printf(" LINEAR <X> <Y>  - XY 동시 직선 이동 (Bresenham)\n");
    printf(" --- 티칭 명령어 ---\n");
    printf(" TEACH           - 현재 위치 저장\n");
    printf(" LIST            - 저장된 포인트 보기\n");
    printf(" DELETE <번호>   - 포인트 삭제\n");
    printf(" CLEAR           - 모든 포인트 삭제\n");
    printf(" SAVE [파일명]   - 파일로 저장\n");
    printf(" LOAD [파일명]   - 파일에서 불러오기\n");
    printf(" RUN             - 포인트 연속 실행\n");
    printf(" --- 모션 버퍼 ---\n");
    printf(" SEND            - 티칭 포인트 -> 버퍼 전송\n");
    printf(" EXEC            - 버퍼 실행\n");
    printf(" BUFFER          - 버퍼 상태 확인\n");
    printf(" FLUSH           - 버퍼 비우기\n");
    printf(" --- G-code ---\n");                          /* ← 추가 */
    printf(" GCODE <명령어>  - G-code 한 줄 직접 실행\n");/* ← 추가 */
    printf(" GFILE <파일명>  - .gcode 파일 실행\n");      /* ← 추가 */
    printf(" QUIT            - 종료\n\n");

	 while (g_running) {
		 printf("CMD>");

		 // 한줄 입력 받기
		 if (fgets(command, sizeof(command), stdin) == NULL) {
			 break;
		 }

		 // 첫 단어 추출 (명령어 종류)
		 if (sscanf(command, "%s", keyword) != 1) {
			 continue;
		 }

		 // 명령어 종류 판단 및 실행
		 if (strcmp(keyword,"QUIT") == 0) {
			 printf("프로그램을 종료합니다.\n");
			 g_running = false;
		 }

		 else if (strcmp(keyword, "SPEED") == 0) {
			 float value;
			 if (sscanf(command,"%*s %f", &value) == 1) {
				 g_speed_mm_s = value;
				 printf("-> 속도 설정: %.1f mm/s\n", g_speed_mm_s);
			 } else {
				 printf("오류: SPEED 값을 입력하세요 (예: SPEED 60)\n");
			 }
		 }

//////////////////////////////여기서 부터 같음////////////////////////////////////
		//ACCEL: 상태 무관하게 설정 가능
		 else if (strcmp(keyword, "ACCEL") == 0) {
			 float value;
			 if (sscanf(command,"%*s %f", &value) == 1) {
				 g_accel_mm_s2 = value;
				 printf("-> 가속 설정: %.1f mm/s2\n", g_accel_mm_s2);
			 } else {
				 printf("오류: ACCEL 값을 입력하세요 (예: ACCEL 2000)\n");
			 }
		 }

		// X이동: isAllowed(CMD_MOVE) 체크 *현재는 싱글 스레드라, 굳이 스테이트 안바꿔도 추가 명령어가 입력되지 않음
		 else if (strcmp(keyword, "X") == 0) {
			 if (!isAllowed(CMD_MOVE)) continue; //상태 체크
			 float target;
			 if (sscanf(command,"%*s %f", &target) == 1 ){
				 printf("-> X축 %.2fmm로 이동 중...\n", target);
				 moveAxisBySpeed(&x_axis, target, g_speed_mm_s, g_accel_mm_s2);
				 printf("->이동 완료!\n");
			 } else {
				 printf("오류: X 좌표를 입력하세요 (예: X 50.0)\n");
			 }
		 }

		// Y이동
		 else if (strcmp(keyword, "Y") == 0) {
			 if (!isAllowed(CMD_MOVE)) continue; //상태 체크			 
			 float target;
			 if (sscanf(command,"%*s %f", &target) == 1 ){
				 printf("-> Y축 %.2fmm로 이동 중...\n", target);
				 moveAxisBySpeed(&y_axis, target, g_speed_mm_s, g_accel_mm_s2);
				 printf("->이동 완료!\n");
			 } else {
				 printf("오류: Y 좌표를 입력하세요 (예: X 50.0)\n");
			 }
		 }

		// Home: 소프트 원점 복귀
		 else if (strcmp(keyword, "HOME") == 0) {
            if (!isAllowed(CMD_MOVE)) continue;  /* ← 상태 체크 */			 
			printf("-> 원점 복귀 중...\n");
			moveAxisBySpeed(&x_axis, 0.0, g_speed_mm_s, g_accel_mm_s2);
			moveAxisBySpeed(&y_axis, 0.0, g_speed_mm_s, g_accel_mm_s2);
		 }

		// Teach
		 else if (strcmp(keyword, "TEACH") == 0) {
            if (!isAllowed(CMD_TEACH)) continue;  /* ← 상태 체크 */			 
			 teachCurrentPosition();
		 }

		// LIST: 상태 무관하게 허용
		 else if (strcmp(keyword, "LIST") == 0) {
			 listTeachPoints();
		 }

		// DELETE
		 else if (strcmp(keyword, "DELETE") == 0) {
			 int index;
			 if (sscanf(command, "%*s %d", &index) == 1) {
				 deleteTeachPoints(index);
			 } else {
				 printf("오류: 삭제할 포인트 번호를 입력하세요 (예: DELETE 2)\n");
			 }
		 }

		// CLEAR
		 else if (strcmp(keyword, "CLEAR") == 0) {
			 clearTeachPoints();
		 }

		// SAVE
		 else if (strcmp(keyword, "SAVE") == 0) {
			 char fname[100];
			 if (sscanf(command, "%*s %s", fname) == 1) {
				 saveTeachPoints(fname); 				 /* 사용자가 파일명을 지정한 경우 */
			 } else {				 
				 saveTeachPoints(NULL);					/* 파일명 생략 → 기본 파일명 사용 */
			 }
		 }

		// LOAD
		 else if (strcmp(keyword, "LOAD") == 0) {
			 char fname[100];
			 if (sscanf(command, "%*s %s", fname) == 1) {
				 loadTeachPoints(fname);
			 } else {
				 loadTeachPoints(NULL);
			 }
		 }

		// RUN
		 else if (strcmp(keyword, "RUN") == 0) {
			 if (!isAllowed(CMD_TEACH)) continue; //상태 체
			 runTeachPoints();
		 }

		// SEND
		 else if (strcmp(keyword, "SEND") == 0) {
			 sendToMotionBuffer();
		 }
		
		// EXEC
		  else if (strcmp(keyword, "EXEC") == 0) {
			 if (!isAllowed(CMD_MOVE)) continue; //상태 체
			  runFromMotionBuffer();
		  }
		  
		// BUFFER
		  else if (strcmp(keyword, "BUFFER") == 0) {
			  showBufferStatus();
		  }
		  
		// FLUSH
		  else if (strcmp(keyword, "FLUSH") == 0) {
			  clearMotionBuffer();
		  }

		// LINEAR
        else if (strcmp(keyword, "LINEAR") == 0) {
			 if (!isAllowed(CMD_MOVE)) continue; //상태 체크
            float x_target, y_target;
            if (sscanf(command, "%*s %f %f", &x_target, &y_target) == 2) {
                printf("-> X=%.2fmm, Y=%.2fmm 직선 이동 중...\n", x_target, y_target);
                moveLinear(x_target, y_target, g_speed_mm_s, g_accel_mm_s2);
                printf("-> 이동 완료!\n");
            } else {
                printf("오류: XY 좌표를 입력하세요 (예: LINEAR 50 30)\n");
            }
        }

        /* ── HOMING: 상태 전환 포함 ← Step 18 핵심 변경 ── */
        /*
         * 이전 코드와 달라진 점:
         *   1. isAllowed(CMD_HOMING) 체크 추가
         *   2. 실행 전 setState(STATE_HOMING)
         *   3. 성공 시 setState(STATE_IDLE)
         *   4. 실패 시 setState(STATE_ERROR)
         */
        else if (strcmp(keyword, "HOMING") == 0) {
			 if (!isAllowed(CMD_HOMING)) continue; //상태 체크
			 
			 setState(STATE_HOMING);
            int result = runHomingSequence();
			
            if (result == HOMING_OK) {
                setState(STATE_IDLE);        /* ← 상태: HOMING → IDLE */
            } else {
                printf("호밍 실패! 에러 코드: %d\n", result);
                setState(STATE_ERROR);       /* ← 상태: HOMING → ERROR */
                printf("-> RESET 명령으로 복구하세요.\n");
            }				
        }
		
		// HOME X
        else if (strcmp(keyword, "HOMEX") == 0) {
			if (!isAllowed(CMD_HOMING)) continue; //상태 체크
			
			 setState(STATE_HOMING);			
            int result = homeAxisX();
			
            if (result == HOMING_OK) {
                setState(STATE_IDLE);
            } else {
                printf("X축 호밍 실패! 에러 코드: %d\n", result);
                setState(STATE_ERROR);
                printf("-> RESET 명령으로 복구하세요.\n");
            }
        }
		
        /* ── HOMEY ── */
        else if (strcmp(keyword, "HOMEY") == 0) {
            if (!isAllowed(CMD_HOMING)) continue;
 
            setState(STATE_HOMING);
            int result = homeAxisY();
 
            if (result == HOMING_OK) {
                setState(STATE_IDLE);
            } else {
                printf("Y축 호밍 실패! 에러 코드: %d\n", result);
                setState(STATE_ERROR);
                printf("-> RESET 명령으로 복구하세요.\n");
            }
        }
 
        /* ── LIMIT: 상태 무관 ── */
        else if (strcmp(keyword, "LIMIT") == 0) {
            int x_state = isXLimitTriggered();
            int y_state = isYLimitTriggered();
            printf("\n===== 리미트 스위치 상태 =====\n");
            printf("  X_LIMIT_MIN (GPIO %d): %s\n",
                   X_LIMIT_MIN, x_state ? "감지됨 [LOW]" : "미감지 [HIGH]");
            printf("  Y_LIMIT_MIN (GPIO %d): %s\n",
                   Y_LIMIT_MIN, y_state ? "감지됨 [LOW]" : "미감지 [HIGH]");
            printf("==============================\n");
        }

		// GCODE
		 else if (strcmp(keyword,"GCODE") == 0){
            if (!isAllowed(CMD_GCODE)) continue;
			 char gcode_line[100];
			 if (sscanf(command,"%*s %[^\n]", gcode_line) == 1){
				 runGcodeLine(gcode_line);
			 } else {
				 printf("오류: G-code를 입력하세요 (예: GCODE G1 X50 Y30 F3000)\n");
			 }
		 }
		 
        /* ── GFILE: 상태 전환 포함 ← Step 18 핵심 변경 ── */
        /*
         * 이전 코드와 달라진 점:
         *   1. isAllowed(CMD_GFILE) 체크 추가
         *   2. 실행 전 setState(STATE_RUNNING)
         *   3. 실행 완료(성공/실패 모두) 후 setState(STATE_IDLE)
         *
         * 왜 실패 시에도 IDLE로 돌아오는가?
         *   파일 열기 실패는 "시스템 오류"가 아님
         *   파일이 없는 것이지 하드웨어 문제가 아니므로
         *   IDLE로 돌아와 다시 명령을 받을 수 있게 함
         */
		 else if (strcmp(keyword, "GFILE") == 0){
            if (!isAllowed(CMD_GFILE)) continue;

			 char fname[100];
			 if (sscanf(command,"%*s %s",fname) == 1){
				 setState(STATE_RUNNING);
				 int result = executeGcodeFile(fname);
				 setState(STATE_IDLE);
				 
				 if (result != 0) {
					 printf("-> 파일 실행 중 문제가 발생했습니다.\n");
				 }
			 } else {
	                printf("오류: 파일명을 입력하세요 (예: GFILE test.gcode)\n");
            }
        }			 

		// STATUS: 현재 상태 출력
		else if (strcmp(keyword,"STATUS")==0) {
			printState();
		}
		
        /* ── RESET: ERROR 상태 복구 (Step 18 신규 추가) ── */
        /*
         * ERROR 상태에서 IDLE로 복구
         * isAllowed(CMD_RESET)이 ERROR 상태에서만 true를 반환
         *
         * 실제 산업 장비에서는 RESET 전에 안전 확인 절차가 있지만
         * 여기서는 학습 목적으로 단순하게 구현
         */
		else if (strcmp(keyword,"RESET") == 0) {
			if(!isAllowed(CMD_RESET)) continue;
            printf("-> 오류 상태 복구 중...\n");
            setState(STATE_IDLE);               /* ← 상태: ERROR → IDLE */
            printf("-> 복구 완료! 명령을 다시 입력하세요.\n");
        }			
		 
		else {
			printf("알 수 없는 명령어: %s\n", keyword);
		}

		printf("\n");
	 }
}
