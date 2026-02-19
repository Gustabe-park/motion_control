/*
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
 
 Axis x_axis = {
	 .enable_pin 		= X_ENABLE,
	 .dir_pin 			= X_DIR,
	 .step_pin 			= X_STEP,
	 .current_steps 	= 0,
	 .current_mm		= 0.0,
	 .steps_per_mm		= 20.0
 };
 
 Axis y_axis = {
	 .enable_pin 		= Y_ENABLE,
	 .dir_pin 			= Y_DIR,
	 .step_pin 			= Y_STEP,
	 .current_steps 	= 0,
	 .current_mm		= 0.0,
	 .steps_per_mm		= 20.0
 };
 
 // 모터 회전 제어
 void rotateMotorAccel(Axis *axis, int steps, int maxSpeedDelay, int accelSteps) {
	
	//startDelay는 maxSpeedDelay보다 커야 함
	//시작 속도는 최대 속도의 약 1/5 정도로 느리게
	
	int startDelay = maxSpeedDelay * 5; //초기 지연 시간 (느린 속도)
	
	//최소값 보장 (너무 작으면 가속 효과 없음)
	if (startDelay < 500){
		startDelay = 500;
	}
	
	for (int i = 0 ; i < steps ; i++) {
		int delayUs;
		
		//가속 구간
		if (i < accelSteps) {
		//점진적으로 빨라짐
		float ratio 		= (float)i / accelSteps;
		float squareRatio 	= ratio * ratio ;//제곱 비율로 가속 효과 향상
		delayUs 			= startDelay - (int)(squareRatio *(startDelay - maxSpeedDelay));
		}
		
		//감속 구간
		else if (i >= steps - accelSteps) {
			//점진적으로 느려짐
			int remaining 		= steps - i;
			float ratio 		= (float)remaining / accelSteps;
			float squareRatio 	= ratio * ratio; //제곱 비율로 감속 효과 향상
			delayUs 			= startDelay - (int)(squareRatio *(startDelay - maxSpeedDelay)); 
		} else {
			delayUs = maxSpeedDelay;
		}
		
		//펄스 생성
		digitalWrite(axis->step_pin, HIGH);
		delayMicroseconds(delayUs);
		digitalWrite(axis->step_pin, LOW);
		delayMicroseconds(delayUs);
	}
 }
 
 // 목표 X,Y 좌표로 이동 (가속/감속 포함)
 void moveAxisAccel(Axis *axis, float target, int maxSpeedDelay, int accelSteps) {
	 
	 // 이동해야 할 거리 계산
	 float distance = target - axis->current_mm; // ->는 포인터 사용!
	 
	 //거리가 0 이면 이미 목표 위치
	 if (fabs(distance) < 0.01) {
		 printf("이미 목표 위치 입니다.");
		 return;
	 }
	 //방향 설정 및 이동거리 절대값
	 bool positive = (distance > 0);
	 float abs_distance = fabs(distance);
	 
	 printf("%.2f -> %.2f: %s 방향으로 %.2fmm 이동\n",
		axis->current_mm, target, positive ? "+" : "-", abs_distance);
		
	//방향 설정
	digitalWrite(axis->dir_pin, positive ? HIGH : LOW);

	int steps = (int)(abs_distance * axis->steps_per_mm);
	printf("-> %d스텝 실행\n",steps);
	
	//이동 거리가 너무 짧으면 가속/감속 경고
	if (steps < accelSteps *2) {
		printf("경고: 이동거리가 짧아 가속/감속 효과가 제한됨\n");
	}
	
	rotateMotorAccel(axis, steps, maxSpeedDelay, accelSteps);
	
	//위치 업데이트
	axis->current_mm = target;
	axis->current_steps += (positive ? steps: -steps); // 현재 위치 스텝 값
	
	printf("-> 현재 위치: %.2fmm\n\n", axis->current_mm);
 }
 
 // mm/s -> delay(us) 변환
 int speedToDelay(float speed_mm_s, float steps_per_mm) {
	 // step/s 계산
	 float steps_per_sec = speed_mm_s * steps_per_mm;
	 
	 //너무 느리면 최소 속도 제한
	 if (steps_per_sec < 1.0) {
		 steps_per_sec = 1.0;
	 }
	 
	 //delay(us) = 1,000,000 / steps/sbrk
	 int delay_us = (int)(1000000.0 / steps_per_sec);
	 
	 return delay_us;
 }
 
 //mm/s2 와 mm/s로부터 가속 스텝 수 계산
 int accelToSteps(float speed_mm_s, float accel_mm_s2, float steps_per_mm) {
	 //가속 거리:d = v2 / (2a)
	 float accel_distance = (speed_mm_s * speed_mm_s) / (2.0 * accel_mm_s2);
	 
	 // 스텝 수로 변환
	 int accel_steps = (int)(accel_distance * steps_per_mm);
	 
	 // 최소 값 보장 (너무 작으면 가속 효과 없음)
	 if (accel_steps < 10) {
		 accel_steps = 10;
	 }
	 
	 return accel_steps;
 }
 
 // ======== 편의 함수 (변환 자동  수행) =========
 // mm/s, mm/s2 단위로 직접 이동
 
 void moveAxisBySpeed(Axis *axis, float target, float speed_mm_s, float accel_mm_s2) {
	 // 변환 수행 (여기서 자동으로!)
	 
	 int delay_us = speedToDelay(speed_mm_s, axis->steps_per_mm);
	 int accel_steps = accelToSteps(speed_mm_s, accel_mm_s2, axis->steps_per_mm);
	 
	 //실제 이동
	 moveAxisAccel(axis, target, delay_us, accel_steps);
 }
