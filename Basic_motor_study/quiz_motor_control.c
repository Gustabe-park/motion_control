/*
 * 사용자 명령어로 모터 제어
 */
#include <stdio.h>
#include <wiringPi.h>
// 핀 번호 설정
#define STEP_PIN 4 	// GPIO04 고정
#define DIR_PIN 3 	// GPIO03 고정
#define ENABLE_PIN 2	// GPIO02 고정

void rotation(int steps, int delays) {
	for (int i = 0 ; i < steps ; i ++) {
		digitalWrite(STEP_PIN, HIGH);
		delay(delays);
		digitalWrite(STEP_PIN, LOW);
		delay(delays);		
	}
}

int main(void) {
	//wiringPi 초기화 시도
	if (wiringPiSetupGpio() == -1) {
		printf("wiringpi 초기화 실패\n");
		return 1;
	}
	//핀 모드 설정
	pinMode(STEP_PIN, OUTPUT);
	pinMode(DIR_PIN, OUTPUT);
	pinMode(ENABLE_PIN, OUTPUT);
	
	//변수 설정
	char command = 0;
	int steps = 0;
	int delays = 0;
	int dirs = 0;
	
	//모터 초기 상태 - 비활성화
	digitalWrite(ENABLE_PIN, HIGH);
	
	//모터 초기 회전 방향: 정방향
	digitalWrite(DIR_PIN, HIGH);
	
	printf("\n 명령어로 모터 제어하기\n");
	
	while(1) {
		printf("\n 모터 제어 명령어 입력:\n");
		printf("Q						:프로그램 종료 \n");
		printf("E						:모터 활성화, 비활성화 \n");
		printf("D	1 or 0				:모터 방향(1:정방향 0:역방향) \n");	
		printf("R	Enter, 200  1		:모터 이동거리, 이동 속도 \n");
		
		int result = scanf("%c",&command);
		
		//명령어 입력 오류
		if (result != 1) {
			printf("명령어 입력 오류, 다시 입력하세요\n");
			while(getchar() != '\n');
			continue;
		}
		//프로그램 종료
		else if (command == 'Q' || command == 'q') {
			printf("프로그램 종료\n");
			break;
		}
		//모터 활성화 or 비활성화
		else if (command == 'E' || command == 'e') {
			static int enable_state = 1; //상태 저장
			enable_state = !enable_state;
            digitalWrite(ENABLE_PIN, enable_state);
			printf("모터 상태: %s",enable_state ? "비활성화": "활성화");
		}
		//모터 방향 설정
		else if (command == 'D' || command == 'd') {
			int result1 = scanf("%d",&dirs);
			if (result1 == 1) {
				if (dirs == 1) {
					digitalWrite(DIR_PIN, HIGH);
					printf("모터 회전 방향:정방향");
				}
				
				else if (dirs == 0){
					digitalWrite(DIR_PIN, LOW);
					printf("모터 회전 방향:역방향");
				}
			}
			else {
				printf("명령어 입력 오류\n");
			}
		}
		//모터 이동, 이동 속도
		else if (command == 'R' || command == 'r') {
			printf("이동거리, 이동속도 입력:ex) 200 5\n");
			int result2 = scanf("%d %d",&steps,&delays);
			if (result2 == 2 && steps != 0 && delays != 0) {
				printf("이동거리: %d, 이동속도: %d\n",steps,delays);
				rotation(steps,delays);
				printf("이동 완료\n");
			}
			else {
				printf("입력 오류: 입력 방법: R 엔터 후 200 5 \n");
			}
		}
		
		else {
			printf("\n 알 수 없는 명령어\n");
		}
		//입력 버퍼 비우기
		while(getchar() != '\n');
	}
	
	//종료 전 모터 비활성화
	digitalWrite(ENABLE_PIN, HIGH);
	
	return 0;
}
	