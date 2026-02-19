/*
 *accel.h
 * 모터 제어 및 가속도 제어 - mm/s % mm/s2 단위
 */
 
 #include "configuration.h"
 
 #ifndef ACCEL_H
 #define ACCEL_H
 
 //============구조체 정의 =============
 typedef struct {
	int enable_pin;
	int dir_pin;
	int step_pin;
	long current_steps;			//현재 위치 (steps)
	double current_mm;			//현재 위치 (mm)
	double steps_per_mm;		//측마다 스텝값 다름 가정
 } Axis;
 
 //============전역 변수 (extern 선언)======
 
 extern Axis x_axis;
 extern Axis y_axis;
 
 //가속 및 감속 포함 회전
 void rotateMotorAccel(Axis *axis, int steps, int maxSpeedDelay, int accelSteps);
 
 //목표 좌표로 이동 (가속/감속 포함)
 void moveAxisAccel(Axis *axis, float target, int maxSpeedDelay, int accelSteps);
 
 // mm/s -> delay(μs) 변환 ← 추가!
 int speedToDelay(float speed_mm_s, float steps_per_mm);
 
 // mm/s²와 mm/s로부터 가속 스텝 수 계산 ← 추가!
 int accelToSteps(float speed_mm_s, float accel_mm_s2, float steps_per_mm);
 
 // mm/s mm/s2 단위로 직접 이동 (변환 자동 수행)
 void moveAxisBySpeed(Axis *axis, float target, float speed_mm_s, float accel_mm_s2);
 
 
 
 
 #endif //ACCEL_H
 
 