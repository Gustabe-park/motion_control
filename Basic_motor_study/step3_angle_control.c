/*
 *스텝 3단계: 정확한 각도로 회전하기
 *
 *학습 목표:
 *- 각도와 스텝 수 변환 이해
 *- 수학 계산을 코드로 구현하기
 */
 //라이브러리 포함
 #include <wiringPi.h>
 #include <stdio.h>
 
 //핀 정의(라즈베리파이 GPIO 기준)
 #define STEP_PIN 		4
 #define DIR_PIN 		3
 #define ENABLE_PIN 	2
 #define STEPS_PER_REV 	200
 
 //모터 회전 함수
 void rotateMotor(int steps, int delayMs) {
	 for (int i = 0; i < steps ; i++) {
		 digitalWrite(STEP_PIN, HIGH);
		 delayMicroseconds(delayMs);
		 digitalWrite(STEP_PIN, LOW);
		 delayMicroseconds(delayMs);
	 }
 }
 
 //특정 각도만큼 회전
 void rotateAngle(float angle, int speedDelay) {
	 //각도를 스텝 수로 변환
	 int steps = (int)((angle / 360.0) * STEPS_PER_REV);
	 printf("%.1f 각도 = %d 스텝\n", angle, steps);
	 rotateMotor(steps, speedDelay);
 }
 
 int main(void) {
	 //wiringPi 초기화 (BCM GPIO 번호 사용)
	 if (wiringPiSetupGpio == -1) {
		 printf("wiringPi 초기화 실패\n");
		 return 1; //비정상 종료/에러 발생, 운영체제나 호출자에게 알릴 때 사용 됨
	 }
	 //핀모드 설정
	 pinMode(STEP_PIN, OUTPUT);
	 pinMode(DIR_PIN, OUTPUT);
	 pinMode(ENABLE_PIN, OUTPUT);
	 
	 //모터 활성화
	 digitalWrite(ENABLE_PIN, LOW);
	 printf("모터 활성화\n");
	 delay(100);
	 
	 //정방향 설정
	 digitalWrite(DIR_PIN, HIGH);
	 
	 printf("90도 회전\n");
	 rotateAngle(90, 50);
	 delay(2000);
	 
	 printf("180도 회전\n");
	 rotateAngle(180, 50);
	 delay(2000);
	 
	 printf("45도 회전\n");
	 rotateAngle(45, 50);
	 delay(2000);
	 
	 printf("수행완료! \n");
	 
	 //모터 비활성화
	 digitalWrite(ENABLE_PIN, HIGH);
	 
	 return 0;
 }
 