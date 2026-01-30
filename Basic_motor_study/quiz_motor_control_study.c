/*
 *사용자 명령어로 모터 제어
 * 컴파일 예시: gcc -o basic_study basic_study.c -lwiringPi
 */
 #include <stdio.h>
 #include <time.h>
 #include <wiringPi.h>
 
 #define STEP_PIN 4
 #define DIR_PIN 3
 #define ENABLE_PIN 2
 //이동 거리, 이동 속도 설정
 void rotation(int steps, int delays) {
	 for (int i = 0 ; i < steps ; i++) {
		 digitalWrite(STEP_PIN, HIGH);
		 delay(delays);
		 digitalWrite(STEP_PIN, LOW);
		 delay(delays);
	 }
 }
 
 int main(void) {
	 //wiringPi 초기화
	 if (wiringPiSetupGpio() == -1) {
		 printf("wiringPi 초기화 실패\n");
		 return 1;
	 }
	 
	 //핀맵
	 pinMode(STEP_PIN, OUTPUT);
	 pinMode(DIR_PIN, OUTPUT);
	 pinMode(ENABLE_PIN, OUTPUT);
	 

	 //변수 선언 및 초기화
	 char command =0;
	 int steps = 0;
	 int delays = 0; 
	 int dirs = 0;
	 
	 //모터 초기상태
	 digitalWrite(ENABLE_PIN, HIGH);
	 
	 //회전 방향 초기 설정
	 digitalWrite(DIR_PIN, HIGH);	 
	 
	 //모터 제어 시작
	 printf("\n 모터 제어 프로그램 시작 \n");
	 
	 while(1) { //무한루프 모드로 진행
		printf("\n 명령어 입력:\n");
		printf("\n Q			: 종료\n");
		printf("\n E			: Enable 토글 \n");
		printf("\n D	1/0		: 회전 방향 설정(1=정방향, 0=역방향) \n");
		printf("\n R	200	5	: 이동거리, 이동속도 설정 \n");
		
		int result = scanf("%c", &command);
		
		if(result != 1) {
			printf("\n 명령어 오류 \n");
			while(getchar() != '\n'); //버퍼 메모리 삭제인가?
			continue;
		}
		//명령어 처리 시작
		if (command == 'Q' || command == 'q') {
			printf("\n 프로그램 종료 \n");
			break;
		}
		
		else if (command == 'E' || command == 'e') {
			static int enable_state = 1; //현재 상태 기억
			enable_state = !enable_state;
			digitalWrite(ENABLE_PIN, enable_state); //1값은 HIGH하고 같은 것인가?
			printf("\n 모터 %s \n",enable_state ? "비활성화" : "활성화");
			break;
		}
		
		else if (command == 'D' || command == 'd') {
			printf("\n 회전방향 입력 \n");
			scanf("%d", &dirs);
			if (dirs == 1) {
				printf("\n 회전방향: 정방향 \n");
				digitalWrite(DIR_PIN, HIGH);
			}
			else {
				printf("\n 회전방향: 역방향 \n");
				digitalWrite(DIR_PIN, LOW);
			}
		}
		
		else if (command == 'R' || command == 'r') {
			printf("\n 이동거리, 속도 입력 \n");
			scanf("%d %d",&steps,&delays);
			if(result == 2 && steps != 0 && delays != 0) {
				printf("\n 이동거리 %d, 이동속도 %d \n",steps,delays);
				rotation(steps, delays);
			}
			else {
				printf("\n 잘못된 명령어 입니다. 다시 입력하세요 \n");
			}
		}
		
		else {
			printf("\n 알 수 없는 명령어 입니다. \n");
		}
		
		//입력 버퍼 비우기
		while(getchar() != '\n');
	 }
	 
	 //종료 전 정리
	 digitalWrite(ENABLE_PIN, HIGH);
	 
	 return 0;
 }