/*
 * command.c
 * 명령어 처리 및 모터 설정값 관리
 */
 
 #include "command.h"
 #include "accel.h"
 #include <stdio.h>
 #include <string.h>
 
 //========= 전역 변수 실제 생성==========
 // command.h에서는 extern으로 "선언"만 했고,
 // 여기서 실제로 "생성"하고 초기값 설정
 
 int g_maxSpeedDelay = 60;		// 기본 속도 (delay 60us)
 int g_accelSteps = 2000;		// 기본 가속 스텝
 bool g_running = true;			// 프로그램 실행 관리
 
 //===== 초기화 함수 =======
 // 프로그램 시작할 때, 한 번 호출
 // 설정값을 기본값으로 리셋
 
 void initCommandSystem(void) {
	 printf("\n=== 명령어 시스템 초기화 ===\n");
	 printf("기본 속도: %d us\n", g_maxSpeedDelay);
	 printf("가속 스텝: %d steps\n", g_accelSteps);
	 printf("준비완료\n\n");
 }
 
 void runCommandLoop(void) {
	 char command[100];		//명령어 저장 버퍼
	 char keyword[50];		//첫 단어 (명령어)
	 
	 printf("====명령어 모드 시작====\n");
	 printf("사용 가능한 명령어:\n");
	 printf(" SPEED<값>		-속도 설정\n");
	 printf(" ACCEL<값>		-가속 스텝 설정\n");
	 printf(" X <값>		-X축 이동\n");
	 printf(" Y <값>		-Y축 설정\n");
	 printf(" HOME		-원점 복귀 \n");
	 printf(" QUIT		-종료\n\n");
	 
	 while (g_running) {
		 printf("CMD> ");
		 
		 // 한 줄 입력 받기
		 if (fgets(command, sizeof(command), stdin) == NULL) {
			 break;
		 }
		 
		 // 첫 단어 추출 (명령어 종류)
		 if (sscanf(command, "%s", keyword) != 1) {
			 continue;
		 }
		 
		 // 명령어 종류 판단 및 실행
		 if (strcmp(keyword, "QUIT") == 0) {
			 printf("프로그램을 종료합니다.\n");
			 g_running = false;
		 }
		 
		 else if (strcmp(keyword, "SPEED") == 0) {
			 int value;
			 if (sscanf(command, "%*s %d", &value) == 1) {
				 g_maxSpeedDelay = value;
				 printf("-> 속도 설정: %d us\n",g_maxSpeedDelay);
			 } else {
				 printf("오류: SPEED 값을 입력하세요 (예: SPEED 60)\n");
			 }
         } 
		 else if (strcmp(keyword, "ACCEL") == 0) {
			 int value;			 
			 if (sscanf(command, "%*s %d", &value) == 1) {
				 g_accelSteps = value;
				 printf("-> 가속 스텝: %d steps\n", g_accelSteps);
			 } else {
				 printf("오류: ACCEL 값을 입력하세요 (예: ACCEL 2000)\n");
			 }
		 }
		 
		 else if (strcmp(keyword, "X") == 0) {
			 float target;
			 if (sscanf(command, "%*s %f", &target) == 1) {
				 printf("->X축 %.2fmm로 이동 중...\n", target);
				 moveAxisAccel(&x_axis, target, g_maxSpeedDelay, g_accelSteps);
				 printf("->이동 완료!\n");
			 } else {
				 printf("오류: X 좌표를 입력하세요 (예: X 50.0)\n");
			 }
         }
		 else if (strcmp(keyword, "Y") == 0) {
			 float target;
			 if (sscanf(command, "%*s %f", &target) == 1) {
				 printf("->Y축 %.2fmm로 이동 중...\n", target);
				 moveAxisAccel(&y_axis, target, g_maxSpeedDelay, g_accelSteps);
				 printf("->이동 완료!\n");
			 } else {
				 printf("오류: Y 좌표를 입력하세요 (예: Y 50.0)\n");
			 }
         }

		 else if (strcmp(keyword, "HOME") == 0) {
			 printf("-> 원점 복귀 중...\n");
			 moveAxisAccel(&x_axis, 0.0, g_maxSpeedDelay, g_accelSteps);
			 moveAxisAccel(&y_axis, 0.0, g_maxSpeedDelay, g_accelSteps);
			 printf("-> 원점 복귀 완료!\n");
		 }
		 	 
		 
		 else {
			 printf("알 수 없는 명령어: %s\n", keyword);
		 }
		 
		 printf("\n");
		 }
	 }
			 
			 
			 
			 
			 
			 
			 
			 
/*=============================================================
 // fgets 방식 (현재 사용)
char command[100];
fgets(command, sizeof(command), stdin);
// "SPEED 60\n" 전체를 command에 저장

// scanf 방식 (간단하지만 위험)
scanf("%s", keyword);  // 공백 만나면 멈춤
// 버퍼 오버플로우 위험


strcmp(keyword, "QUIT")
// 같으면: 0 반환
// 다르면: 0이 아닌 값

// 그래서 == 0 으로 비교
if (strcmp(keyword, "QUIT") == 0) {
    // keyword가 "QUIT"일 때
}


// 잘못된 방법 ❌
if (keyword == "QUIT")  // 문자열은 == 로 비교 불가!

// 올바른 방법 ✅
if (strcmp(keyword, "QUIT") == 0)



sscanf("SPEED 60", "%*s %d", &value);
//                  ^^
//                  읽지만 버림



*/	 