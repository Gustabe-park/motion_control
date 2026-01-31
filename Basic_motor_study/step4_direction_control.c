/*
 *4단계: 방향 제어 추가하기
 *
 *학습 목표:
 * - 정방향 / 역방향 제어하기
 * - 불리언(참/거짓) 개념 이해
 */
 
 #include <stdio.h>
 #include <wiringPi.h>
 #include <stdbool.h> // bool  타입 설정
 
 //핀 번호 설정, 라즈베리파이 GPIO
 #define STEP_PIN			4 // GPIO 23
 #define DIR_PIN			3 // GPIO 22
 #define ENABLE_PIN			2
 #define STEPS_PER_REV		2000
 //모터 회전
 void rotateMotor(int steps, int delayMs) {
	 for (int i = 0 ; i < steps ; i++) {
		 digitalWrite(STEP_PIN, HIGH);
		 delayMicroseconds(delayMs);
		 digitalWrite(STEP_PIN, LOW);
		 delayMicroseconds(delayMs);
	 }
 }
 //특정 각도만큼 회전 (방향 추가)
 void rotateAngle(float angle, bool clockwise, int speedDelay) {
	 //방향 설정
	 if (clockwise) {
		 digitalWrite(DIR_PIN, HIGH);
		 printf("시계방향으로 %.1f도 회전\n", angle);
	 }
	 else {
		 digitalWrite(DIR_PIN, LOW);
		 printf("반시계 방향 %.1f도 회전\n", angle);
	 }
	 
	 int steps = (int)((angle)/360.0 * STEPS_PER_REV);
	 rotateMotor(steps, speedDelay);
 }
 
 int main(void) {
	 //wiringPi 초기화
	 if (wiringPiSetupGpio() == -1) {
		 printf("wiringPi 초기화 실패\n");
		 return 1;
	 }
	 
	 pinMode(STEP_PIN, OUTPUT);
	 pinMode(DIR_PIN, OUTPUT);
	 pinMode(ENABLE_PIN, OUTPUT);
	 
	 //모터 활성화
	 digitalWrite(ENABLE_PIN, LOW);
	 printf("모터 활성화 완료\n");
	 delay(100);
	 
	 printf("시계방향 90도\n");
	 rotateAngle(90, true, 100);
	 delay(2000);
	 
	 printf("반 시계방향 90도\n");
	 rotateAngle(90, false, 100);
	 delay(2000);
	 
	 printf("시계방향 180도\n");
	 rotateAngle(180, true, 100);
	 delay(2000);
	 
	 printf("완료!\n");
	 
	 //모터 비활성화
	 digitalWrite(ENABLE_PIN, HIGH);
	 
	 return 0;
 }
 
 
 
 
 