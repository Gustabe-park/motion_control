 /*
 *7단계: 속도 프로파일 (감속/감속) - 절대좌표
 *
 * accel.h
 * 모터 제어 및 가속도 제어
 */
 
 #ifndef ACCEL_H
 #define ACCEL_H
 
 #include "configuration.h"
 
//===========구조체 정의 ==============
 typedef struct {
	 int enable_pin;
	 int dir_pin;
	 int step_pin;
	 long current_steps;		// 현재 위치 (steps)
	 double current_mm;			// 현재 위치 (mm)
	 double steps_per_mm;		// 축마다 스텝값 다를 수 있음을 가정
 } Axis;
 
//===========전역 변수 (extern 선언)======
 
 extern Axis x_axis;
 extern Axis y_axis;
 
 
  //가속/감속 포함 회전
 void rotateMotorAccel(Axis *axis, int steps, int maxSpeedDelay, int accelSteps);
 
 //목표 X 좌표로 이동 (가속/감속 포함)
  void moveAxisAccel(Axis *axis, float target, int maxSpeedDelay, int accelSteps);
  
 #endif //ACCEL_H