/*
 * command.h
 * 명령어 처리 및 모터 설정값 관리
 */
 
 #ifndef COMMAND_H
 #define COMMAND_H
 
 #include <stdbool.h>
 
 //======= 모터 설정값 저장 =========
 // 사용자가 설정한 값들을 여기 저장해두고 계속 다른 코드에서도 사용
 
 extern int g_maxSpeedDelay; 	// 최대 속도 (delay 시간, 작을 수록 빠름)
 extern int g_accelSteps; 		// 가속 /감속  스텝 수록
 extern bool g_running; 		// 프로그램 실행 상태 (true: 계속, false: 종료)
 
 //======= 함수 선언 =========
 void initCommandSystem(void);
 void runCommandLoop(void);
 
 #endif //COMMAND_H