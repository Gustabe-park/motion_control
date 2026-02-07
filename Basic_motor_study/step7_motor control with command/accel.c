 /*
 *7단계: 속도 프로파일 (감속/감속) - 절대좌표
 *
 * accel.c
 * 모터 제어 및 가속도 제어
 */
 
 #include "configuration.h"
 #include "accel.h"
 #include <wiringPi.h>
 #include <stdio.h>
 #include <stdbool.h>
 #include <math.h>
 

 // 구조체 실제 생성 및 초기화
 //설명:

// .멤버이름 = 값 형식으로 초기화하는 걸 지정 초기화라고 해요
//  순서 상관없이 명확하게 어떤 값이 어디 들어가는지 보여줌
 
 
 Axis x_axis = {
	 .enable_pin 	= X_ENABLE,
	 .dir_pin 		= X_DIR,
	 .step_pin 		= X_STEP,
	 .current_steps	= 0,
	 .current_mm	= 0.0,
	 .steps_per_mm	= 20.0
 };
 
  Axis y_axis = {
	 .enable_pin 	= Y_ENABLE,
	 .dir_pin 		= Y_DIR,
	 .step_pin 		= Y_STEP,
	 .current_steps	= 0,
	 .current_mm	= 0.0,
	 .steps_per_mm	= 20.0
 };
 
 
  //가속/감속 포함 회전
 void rotateMotorAccel(Axis *axis, int steps, int maxSpeedDelay, int accelSteps) { 
	
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
		 digitalWrite(axis->step_pin, HIGH);
		 delayMicroseconds(delayMs);
		 digitalWrite(axis->step_pin, LOW);
		 delayMicroseconds(delayMs);
	 }
 }
 
 //목표 X 좌표로 이동 (가속/감속 포함)
  void moveAxisAccel(Axis *axis, float target, int maxSpeedDelay, int accelSteps) {
	  
	  //이동해야 할 거리 계산
	  float distance = target - axis->current_mm; // ← 포인터는 -> 사용!
	  
	  // 거리가 0이면 이미 목표 위치
	  if (fabs(distance) < 0.01) {
		  printf("이미 목표 위치입니다.");
		  return;
	  }
	  //방향 설정 및 이동거리 절대값
	  bool positive = (distance > 0);
	  float abs_distance = fabs(distance);
	  
     printf("%.2f -> %.2f: %s 방향으로 %.2fmm 이동\n",
        axis->current_mm, target, positive ? "+" : "-", abs_distance);
	  
	  //방향 설정
      digitalWrite(axis->dir_pin, positive ? HIGH : LOW);
	  
	  //스텝 수 계산 및 실행
	  int steps = (int)(abs_distance * axis->steps_per_mm);
	  printf("-> %d스텝 실행\n", steps);
	  
	  //이동 거리가 너무 짧으면 가속/감속 경고
	  if (steps < accelSteps *2) {
		  printf("경고: 이동거리가 짧아 가속/감속 효과가 제한됨\n");
	  }
	  
	  rotateMotorAccel(axis, steps, maxSpeedDelay, accelSteps);

	  //위치 업데이트
	  axis->current_mm = target;
	  axis->current_steps += (positive ? steps : -steps); // 이 부분은, mm말고 현재 스텝값 계산 위해 추가 된 것인지?
	  
	  printf("-> 현재 위치: %.2fmm\n\n", axis->current_mm);
  
  }