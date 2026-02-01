/*
 *7단계: 속도 프로파일 (감속/감속) - 절대좌표
 *
 *학습 목표:
 * - 가속/감속의 필요성 이해
 * - 반복문 안에서 변수 값 동적으로 변경하기
 * - 절대좌표 이동에 감속/감속 적용
 *
 * 기계 설정:
 * - 1회전 = 10mm 이동
 * - 200 스텝 = 10mm
 * - 1스텝 = 0.05mm
 */
 
 #include <wiringPi.h>
 #include <stdio.h>
 #include <stdbool.h>
 #include <math.h>
 
 //핀 번호 설정 
 #define STEP_PIN			4
 #define DIR_PIN			3
 #define ENABLE_PIN			2
 #define STEPS_PER_REV		200
 
 //기계적 설정
 #define MM_PER_REV			10.0
 #define STEPS_PER_MM		(STEPS_PER_REV / MM_PER_REV)
 
 //현재 위치 (전역 변수)
 float currentX = 0.0;
 
 //가속/감속 포함 회전
 void rotateMotorAccel(int steps, int maxSpeedDelay, int accelSteps) {
	 int startDelay = 300; //시작 속도 느림
	 
	 for (int i = 0; i < steps; i++) {
		 int delayMs;
		 
		 //가속 구간
		 if (i < accelSteps) {
			 //점진적으로 빨라짐
			 float ratio = (float)i / accelSteps;
             float squareRatio = ratio * ratio; //제곱 비율로 가속 효과 향상
			 delayMs = startDelay - (int)(squareRatio * (startDelay - maxSpeedDelay));
            printf("가속: step %d, delay %dms\n", i, delayMs);
		 }
		 
		 //감속 구간
		 else if (i >= steps - accelSteps) {
			 //점진적으로 느려짐
			 int remaining = steps - i;
			 float ratio = (float)remaining / accelSteps;
             float squareRatio = ratio * ratio; //제곱 비율로 감속 효과 향상
			 delayMs = startDelay - (int)(squareRatio * (startDelay - maxSpeedDelay));
            printf("감속: step %d, delay %dms\n", i, delayMs);
		 }
		 
		 //등속 구간
		 else {
			 delayMs = maxSpeedDelay;
		 }
		 
		 //펄스 생성
		 digitalWrite(STEP_PIN, HIGH);
		 delayMicroseconds(delayMs);
		 digitalWrite(STEP_PIN, LOW);
		 delayMicroseconds(delayMs);
	 }
 }
 
 //목표 X 좌표로 이동 (가속/감속 포함)
  void moveToXAccel(float targetX, int maxSpeedDelay, int accelSteps) {
	  //이동해야 할 거리 계산
	  float distance = targetX - currentX;
	  
	  // 거리가 0이면 이미 목표 위치
	  if (fabs(distance) < 0.01) {
		  printf("이미 목표 위치입니다.");
		  return;
	  }
	  //방향 설정 및 이동거리 절대값
	  bool positive;
	  float abs_distance;
	  
	  if (distance > 0) {
		  positive = true;
		  abs_distance = distance;
		  printf("X%.2f -> X%.2f: X+ 방향으로 %.2fmm 이동\n",
				currentX, targetX, abs_distance);
	  } else {
		  positive = false;
		  abs_distance = -distance;
		  printf("X%.2f -> X%.2f: X- 방향으로 %.2fmm 이동\n",
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
	  printf("-> %d스텝 실행");
	  
	  //이동 거리가 너무 짧으면 가속/감속 경고
	  if (steps < accelSteps *2) {
		  printf("경고: 이동거리가 짧아 가속/감속 효과가 제한됨\n");
	  }
	  
	  rotateMotorAccel(steps, maxSpeedDelay, accelSteps);
	  
	  //위치 업데이트
	  currentX = targetX;
	  printf("-> 현재 위치: X = %.2fmm\n\n", currentX);
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
	  printf("모터 활성화\n");
	  delay(2000);
	  
	  printf("===가속/감속 포함 절대좌표 이동===\n");
	  printf("설정: 1회전 = %.1fmm, 1mm = %.1f 스텝\n", MM_PER_REV, STEPS_PER_MM);
	  printf("가속/감속: 최고속도 %dms, 가속구간 %d스텝\n\n",1,20);
	  printf("시작 위치: X = %.2fmm\n\n",currentX);
	  
	  //테스트 이동 시퀀스
	  moveToXAccel(1000, 60, 2000);
	  delay(2000);
	  
	  moveToXAccel(2000, 60, 2000);
	  delay(2000);
	  
	  moveToXAccel(1000, 60, 2000);
	  delay(2000);
	  
	  moveToXAccel(0, 60, 2000);
	  delay(2000);
	  
	  digitalWrite(ENABLE_PIN, HIGH);
	  
	  return 0;
  }
	  
	  