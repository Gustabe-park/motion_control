/*
 * calibration.c
 *
 *역할: 캘리브레이션 구현
 */
 
 #include <stdio.h>
 #include <math.h>
 #include "calibration.h"
 
 //캘리브레이션 값
 static float steps_per_mm = STEPS_PER_MM;
 
 //절대 위치
 static long absolute_position = 0;
 
/*
 * 초기화
 */ 
 void calibration_init(void) {
	 steps_per_mm = STEPS_PER_MM;
	 absolute_position = 0;
	 
	 printf("[CALIB] 초기화: %.2f steps/mm\n", steps_per_mm);
 }
 
 /*
  * 캘리브레이션 설정
  */
 void calibration_set(float new_steps_per_mm){
	 steps_per_mm = new_steps_per_mm;
	 printf("[CALIB] 설정: %.2f steps/mm\n", steps_per_mm);
 }
 
 /*
  * 캘리브레이션 가져오기
  */
  float calibration_get(void){
	  return steps_per_mm;
  }
 
/*
 * mm → steps
 */
 long calibration_mm_to_steps(float mm){
	 return (long)round(mm * steps_per_mm); //수식 구성이 궁금해
 }
 
 /*
  * steps → mm
  */
 float calibration_steps_to_mm(long steps) {
	 return steps / steps_per_mm;
 }
 
 /*
  * 위치 설정(steps)
  */
 void calibration_set_position_steps(long steps) {
	 absolute_position = steps;
	 printf("[CALIB]위치 설정: %ld steps (%.2f mm)\n",
		    steps, calibration_steps_to_mm(steps));
 }			

 /*
  * 위치 설정(mm)
  */
 void calibration_set_position_mm(float mm){
	 absolute_position = calibration_mm_to_steps(mm);
	 printf("[CALIB]위치 설정: %.2f mm (%ld steps)\n",
		    mm, absolute_position);
 }
 
 /*
  * 위치 가져오기 (steps)
  */
 long calibration_get_position_steps(void) {
	 return absolute_position;
 }
 
 /*
  * 위치 가져오기 (mm)
  */ 
 float calibration_get_position_mm(void) {
    return calibration_steps_to_mm(absolute_position);	 
 }
 
 /*
  * 원점 리셋
  */  
  void calibration_reset_position(void) {
	  absolute_position = 0;
	  printf("[CALIB]원점 리셋\n");
  }
  
 /*
  * 위치 업데이트
  */ 
 void calibration_update_position(direction_t dir) {
	 if (dir == DIR_FORWARD) {
		 absolute_position++;
	 }else {
		 absolute_position--;
	 }
 }