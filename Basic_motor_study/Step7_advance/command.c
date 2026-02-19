/*
 * command.c
 * 명령어 처리 및 모터 설정값 관리
 *
 * ============ 추가된 티칭 명령어 ============
 * TEACH          : 현재 위치를 티칭 포인트로 저장
 * LIST           : 저장된 포인트 목록 출력
 * DELETE <번호>  : 특정 포인트 삭제
 * CLEAR          : 모든 포인트 삭제
 * SAVE [파일명]  : 포인트를 텍스트 파일로 저장
 * LOAD [파일명]  : 텍스트 파일에서 포인트 불러오기
 * RUN            : 저장된 포인트 연속 실행
 */

#include "command.h"
#include "accel.h"
#include "teaching.h"   /* 티칭 펜던트 기능 */
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
	 printf("준비완료\n\n");
}

void runCommandLoop(void) {
	 char command[100];		// 명령어 저장 버퍼
	 char keyword[50];		// 첫 단어 (명령어)

	 printf("====명령어 모드 시작====\n");
	 printf("사용 가능한 명령어:\n");
	 printf(" SPEED<값>		-속도 설정\n");
	 printf(" ACCEL<값>		-가속 스텝 설정\n");
	 printf(" X <값>		-X축 이동\n");
	 printf(" Y <값>		-Y축 설정\n");
	 printf(" HOME		-원점 복귀 \n");

	 /*
	  * ====== 티칭 펜던트 명령어 안내 (추가) ======
	  * 사용자가 어떤 명령어를 사용할 수 있는지 표시
	  */
	 printf(" --- 티칭 명령어 ---\n");
	 printf(" TEACH		-현재 위치 저장\n");
	 printf(" LIST		-저장된 포인트 보기\n");
	 printf(" DELETE <번호>	-포인트 삭제\n");
	 printf(" CLEAR		-모든 포인트 삭제\n");
	 printf(" SAVE [파일명]	-파일로 저장\n");
	 printf(" LOAD [파일명]	-파일에서 불러오기\n");
	 printf(" RUN		-포인트 연속 실행\n");
	 printf("\n  [모션 버퍼 - Step 12]\n");
	 printf("  SEND          - 티칭 포인트 → 버퍼 전송\n");
	 printf("  EXEC          - 버퍼 실행\n");
	 printf("  BUFFER        - 버퍼 상태 확인\n");
	 printf("  FLUSH         - 버퍼 비우기\n");
	 printf(" QUIT		-종료\n\n");

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

		 else if (strcmp(keyword, "ACCEL") == 0) {
			 float value;
			 if (sscanf(command,"%*s %f", &value) == 1) {
				 g_accel_mm_s2 = value;
				 printf("-> 가속 설정: %.1f mm/s\n", g_speed_mm_s);
			 } else {
				 printf("오류: ACCEL 값을 입력하세요 (예: ACCEL 2000)\n");
			 }
		 }

		 else if (strcmp(keyword, "X") == 0) {
			 float target;
			 if (sscanf(command,"%*s %f", &target) == 1 ){
				 printf("-> X축 %.2fmm로 이동 중...\n", target);

				 //변환 함수 실행
				 moveAxisBySpeed(&x_axis, target, g_speed_mm_s, g_accel_mm_s2);

				 printf("->이동 완료!\n");
			 } else {
				 printf("오류: X 좌표를 입력하세요 (예: X 50.0)\n");
			 }
		 }

		 else if (strcmp(keyword, "Y") == 0) {
			 float target;
			 if (sscanf(command,"%*s %f", &target) == 1 ){
				 printf("-> Y축 %.2fmm로 이동 중...\n", target);

				 //변환 함수 실행
				 moveAxisBySpeed(&y_axis, target, g_speed_mm_s, g_accel_mm_s2);

				 printf("->이동 완료!\n");
			 } else {
				 printf("오류: Y 좌표를 입력하세요 (예: X 50.0)\n");
			 }
		 }

		 else if (strcmp(keyword, "HOME") == 0) {
			printf("-> 원점 복귀 중...\n");
			moveAxisBySpeed(&x_axis, 0.0, g_speed_mm_s, g_accel_mm_s2);
			moveAxisBySpeed(&y_axis, 0.0, g_speed_mm_s, g_accel_mm_s2);
		 }

		 /* ====================================================
		  * ========== 티칭 펜던트 명령어 처리 (추가) ==========
		  * ====================================================
		  */

		 /*
		  * TEACH 명령어
		  * - 현재 모터의 X, Y 위치를 티칭 포인트로 저장
		  * - 사용법: CMD> TEACH
		  * - 현재 설정된 SPEED, ACCEL 값도 함께 저장됨
		  *
		  * 사용 예시:
		  *   CMD> X 10       ← X축을 10mm로 이동
		  *   CMD> Y 20       ← Y축을 20mm로 이동
		  *   CMD> TEACH      ← 현재 위치(10, 20)를 포인트로 저장
		  */
		 else if (strcmp(keyword, "TEACH") == 0) {
			 teachCurrentPosition();
		 }

		 /*
		  * LIST 명령어
		  * - 저장된 모든 티칭 포인트를 표 형태로 출력
		  * - 사용법: CMD> LIST
		  */
		 else if (strcmp(keyword, "LIST") == 0) {
			 listTeachPoints();
		 }

		 /*
		  * DELETE 명령어
		  * - 특정 번호의 포인트를 삭제
		  * - 사용법: CMD> DELETE 2  ← 2번 포인트 삭제
		  *
		  * sscanf에서 "%*s %d":
		  *   %*s : 첫 단어(DELETE)를 읽고 버림 (*는 무시 표시)
		  *   %d  : 두 번째 값을 정수로 읽음 (삭제할 번호)
		  */
		 else if (strcmp(keyword, "DELETE") == 0) {
			 int index;
			 if (sscanf(command, "%*s %d", &index) == 1) {
				 deleteTeachPoints(index);
			 } else {
				 printf("오류: 삭제할 포인트 번호를 입력하세요 (예: DELETE 2)\n");
			 }
		 }

		 /*
		  * CLEAR 명령어
		  * - 저장된 모든 포인트를 삭제
		  * - 사용법: CMD> CLEAR
		  */
		 else if (strcmp(keyword, "CLEAR") == 0) {
			 clearTeachPoints();
		 }

		 /*
		  * SAVE 명령어
		  * - 포인트들을 텍스트 파일로 저장
		  * - 사용법: CMD> SAVE              ← 기본 파일명 (teach_points.txt)
		  *           CMD> SAVE mypath.txt   ← 지정한 파일명으로 저장
		  *
		  * sscanf 반환값 확인:
		  *   - 파일명까지 읽으면 반환값 1 → fname에 파일명 저장
		  *   - 파일명이 없으면 반환값 0 또는 EOF → NULL 전달 (기본 파일명 사용)
		  */
		 else if (strcmp(keyword, "SAVE") == 0) {
			 char fname[100];
			 if (sscanf(command, "%*s %s", fname) == 1) {
				 /* 사용자가 파일명을 지정한 경우 */
				 saveTeachPoints(fname);
			 } else {
				 /* 파일명 생략 → 기본 파일명 사용 */
				 saveTeachPoints(NULL);
			 }
		 }

		 /*
		  * LOAD 명령어
		  * - 텍스트 파일에서 포인트를 불러옴
		  * - 사용법: CMD> LOAD              ← 기본 파일명에서 불러오기
		  *           CMD> LOAD mypath.txt   ← 지정한 파일에서 불러오기
		  *
		  * 주의: LOAD하면 기존 메모리의 포인트가 삭제되고
		  *       파일 내용으로 교체된다!
		  */
		 else if (strcmp(keyword, "LOAD") == 0) {
			 char fname[100];
			 if (sscanf(command, "%*s %s", fname) == 1) {
				 loadTeachPoints(fname);
			 } else {
				 loadTeachPoints(NULL);
			 }
		 }

		 /*
		  * RUN 명령어
		  * - 저장된 포인트들을 0번부터 순서대로 자동 실행
		  * - 사용법: CMD> RUN
		  *
		  * 이것이 티칭 펜던트의 핵심!
		  * TEACH로 저장한 경로를 자동으로 재현한다.
		  */
		 else if (strcmp(keyword, "RUN") == 0) {
			 runTeachPoints();
		 }

		 // ========== Step 12: 모션 버퍼 명령어 ==========
		 
		 /*
		  * SEND 명령어
		  * - 티칭 포인트 배열 → 모션 버퍼로 전송
		  * - 사용법: CMD> SEND
		  */

		 else if (strcmp(keyword, "SEND") == 0) {
			 sendToMotionBuffer();
		 }
		
		 /*
		  * EXEC 명령어
		  * - 모션 버퍼에서 포인트를 꺼내면서 실행
		  * - 사용법: CMD> EXEC
		  */
		  
		  else if (strcmp(keyword, "EXEC") == 0) {
			  runFromMotionBuffer();
		  }
		  
		 /*
		  * BUFFER 명령어
		  * - 버퍼 상태 확인 (사용량, 여유 공간, front/rear 위치)
		  * - 사용법: CMD> BUFFER
		  */
		  
		  else if (strcmp(keyword, "BUFFER") == 0) {
			  showBufferStatus();
		  }
		  
		 /*
		  * FLUSH 명령어
		  * - 모션 버퍼 비우기
		  * - 사용법: CMD> FLUSH
		  * - 주의: 실행 대기 중인 모든 명령이 삭제됩니다
		  */

		  else if (strcmp(keyword, "FLUSH") == 0) {
			  clearMotionBuffer();
		  }

		else {
			printf("알 수 없는 명령어: %s\n", keyword);
		}

		printf("\n");
	 }
}
