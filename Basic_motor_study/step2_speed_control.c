/*
 *2단계: 속도 제어하기
 *
 *학습목표:
 * - 펄스 주파수와 속도의 관계 이해
 * - 함수로 코드 정리하는 방법 배우기
 */

#include <stdio.h>
#include <time.h> //펄스를 생성할 경우, time.h 포함해야 제대로 작동 됨.
#include <wiringPi>

//핀 번호 설정
#define STEP_PIN 4
#define DIR_PIN 3
#define ENABLE_PIN 2

//모터를 STEPS 만큼 회전 시키는 함수
void rotaion(int steps, int delays) {
	for (int i = 0 ; i < steps ; i++) {
		digitalWrite(STEP_PIN, HIGH);
		delay(delays);
		digitalWrite(STEP_PIN, LOW);
		delay(delays);
	}
}

int main(void) {
	
	if (wiringPiSetupGpio() == -1) {
		//WiringPi 초기화
		printf("WiringPi 초기화 실패\n");
		return 1;
	}
	
	//핀 모드 설정
	pinMode(STEP_PIN, OUTPUT);
	pinMode(DIR_PIN, OUTPUT);
	pinMode(ENABLE_PIN, OUTPUT);	
	
	//모터 활성화
	printf("모터 활성화 시작 중...\n");
	digitalWrite(ENABLE_PIN, LOW);
	delay(1000);
	
	//모터 방향 설정
	printf("모터 방향 설정중...\n");
	digitalWrite(DIR_PIN, HIGH);
	delay(1000);
	
	//모터 1회전, 느린 속도
	printf("먼저 모터를 느린속도로 회전 합니다.\n");
	rotaion(200, 5);
	delay(1000);
	
	//모터 1회전, 빠른 속도
	printf("다음은 모터를 빠른속도로 회전 합니다.\n");
	rotaion(200, 1);	
	delay(1000);
	
	//모든 과정 완료, 모터 해제
	printf("모든 과정 완료, 모터 해제\n");
	digitalWrite(ENABLE_PIN, LOW);
	delay(1000);
	
	return 0;
}