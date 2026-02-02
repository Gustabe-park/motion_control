/*
 *6단계: 목표 위치로 이동하기 (절대좌표)
 *
 * 학습 목표:
 * - 목표 위치와 현재 위치 차이 계산
 * - 절대좌표 기반 이동 제어
 * - 자동 방향 결정 로직
 *
 * 기계 설정:
 * - 1회전 = 10mm 이동 (타이밍 벨트 + 풀리 가정)
 * - 200스텝 = 10mm
 * - 1스텝 = 0.05mm
 */
 
 #include <wiringPi.h>
 #include <stdio.h>
 #include <stdbool.h>
 #include <math.h>
 
 // 핀 번호 설정 (라즈베리파이 GPIO 번호)
 #define STEP_PIN			4
 #define DIR_PIN			3
 #define ENABLE_PIN			2
 #define STEPS_PER_REV		200 //1회전 당 스텝 수
 
 // 기계적 설정
 #define MM_PER_REV			10.0 // 1회전 당 이동거리
 #define STEPS_PER_MM		(STEPS_PER_REV / MM_PER_REV)
 
 // 현재 위치 (전역 변수)
 float currentx = 0.0;
 
 void rotateMotor(int steps,int delayMs) {
	 for (int i = 0 ; i < steps ; i++) {
		 digitalWrite(STEP_PIN, HIGH);
		 delayMicroseconds(delayMs);
		 digitalWrite(STEP_PIN, LOW);
		 delayMicroseconds(delayMs);
	 }
 }
 
 //목표 X 좌표로 이동 (절대좌표)
 void moveToX(float targetX, int speedDelay) {
	 //이동해야 할 거리 계산
	 float distance = targetX - currentX;
	 
	//거리가 '0'이면 이미 목표 위치
	//if (fabs(distance) < 0.01) { //0.01mm 이하는 무시, fabs()함수는 double형 절대값
	//	printf("이미 목표 위치입니다: X = %.2lfmm\n\n",currentX);
	//	return;
	//}
	 
	 //방향 결정
	 bool positive;
	 float abs_distance;
	 
	 if (distance > 0) {
		 positive = true;
		 abs_distance = distance;
		 //printf("X%.2f -> X%.2f : X+ 방향으로 %.2fmm 이동\n",
				currentX, targetX, abs_distance);
	 } else {
		 positive = false;
		 abs_distance = -distance; //절대값
		 //printf("X%.2f -> X%.2f : X- 방향으로 %.2fmm 이동\n",
				currentX, targetX, abs_distance);
	 }		 
	 
	 //방향 설정
	 if (positive) {
		 digitalWrite(DIR_PIN, HIGH);
	 } else {
		 digitalWrite(DIR_PIN, LOW);
	 }
	 
	 //스텝 수 계산 및 실행
	 int steps = (int)(abs_distance * STEPS_PER_MM);
	 rotateMotor(steps, speedDelay);
		 
	 //위치 업데이트
	 currentX = targetX;
 }
	 

 
 int main(void) {
    // wiringPi 초기화 (BCM GPIO 번호 사용)
    if (wiringPiSetupGpio() == -1) {
        printf("wiringPi 초기화 실패\n");
        return 1;
    }
	
	pinMode(STEP_PIN, OUTPUT);
	pinMode(DIR_PIN, OUTPUT);
	pinMode(ENABLE_PIN, OUTPUT);
	
	//모터 활성화
	digitalWrite(ENABLE_PIN, LOW);
	printf("모터 활성화\n");
	delay(100);
	
	moveToX(10.0 , 100);
	moveToX(25.0 , 50);
	moveToX(5.0 , 100);
	moveToX(15.0 , 50);
	moveToX(0.0 , 100);
	
	printf("===최종 결과===\n");
	printf("최종 위치: X = %.2fmm\n",currentX);
	printf("원점 복귀 완료\n");
	
	//모터 비활성화
	digitalWrite(ENABLE_PIN, HIGH);
	
	return 0;
 }
 