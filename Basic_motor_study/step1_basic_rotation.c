/*1단계: 가장 기본 - 모터 한 바퀴 돌리기
 *
 *
 *학습 목표:
 * - GPIO 핀 제어 이해하기
 * - 스텝모터가 펄스 신호로 움직인다는 원리 이해
 * - 가장 간단한 코드로 성공 경험 쌓기
 */
 
#include <wiringpi.h>
#include <stdio.h>

//핀 번호 설정
#define STEP_PIN 4
#define DIR_PIN 3
#define ENABLE_PIN 2

int main(void) {
	//wiringpi 초기화
	if (wiringPiSetupGpio() == -1) {
		printf("wiringpi 초기화 실패\n");
		return 1;
	}
	
	//핀을 출력 모드로 설정
	pinMode(STEP_PIN, OUTPUT);
	pinMode(DIR_PIN, OUTPUT);
	pinMode(ENABLE_PIN, OUTPUT);
	
	//모터 활성화
	printf("모터 활성화");
	digitalWrite(ENABLE_PIN, LOW);
	delay(2000);
	
	//모터 회전 방향 설정
	printf("모터 회전방향 설정");
	digitalWrite(DIR_PIN, HIGH);
	delay(2000);
	
	//모터 회전 시작
	for (int i = 0; i <200; i++) {
		digitalWrite(STEP_PIN, HIGH);
		delay(1);
		digitalWrite(STEP_PIN, LOW);
		delay(1);
	}
	
	printf("회전 완료\n");
	
	return 0;
}