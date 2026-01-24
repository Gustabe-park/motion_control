/*
 * motion_state.c
 * 
 * 역할: 모션 상태 관리 구현
 */
 
 #include <stdio.h>
 #include "motion_state.h"
 
 // 상태 변수
 static motion_state_t current_state = MOTION_IDLE;
 static motion_phase_t current_phase = PHASE_ACCEL;
 static long current_step = 0;
 static long target_steps = 0;
 
 /*
  * 초기화
  */
 void motion_state_init(void) {
	 current_state = MOTION_IDLE;
	 current_phase = PHASE_ACCEL;
	 current_step = 0;
	 target_steps = 0;
 }
 
 /*
  * 상태 설정
  */
 void motion_state_set(motion_state_t state) {
	 current_state = state;
 }
 
/*
 * 상태 가져오기
 */
 motion_state_t motion_state_get(void) {
	 return current_state;
 }
 
/*
 * 구간 설정
 */
 void motion_state_set_phase(motion_phase_t phase) {
	 current_phase = phase;
 }
 
/*
 * 구간 가져오기
 */ 
 motion_phase_t motion_state_get_phase(void) {
	 return current_phase;
 }
 
/*
 * 진행률 계산
 */
int motion_state_get_progress(void) {
	if(target_steps == 0) return 100;
	return (int)(current_step * 100 / target_steps);
}

/*
 * 현재 스텝 설정
 */
 void motion_state_set_current_step(long step) {
	 current_step = step;
 }
 
/*
 * 목표 스텝 설정
 */ 
 void motion_state_set_target_steps(long steps) {
	 target_steps = steps;
 }
 
/*
 * 현재 스텝 가져오기
 */ 
 long motion_state_get_current_step(void) {
	 return current_step;
 }
 
/*
 * 목표 스텝 가져오기
 */
 long motion_state_get_target_steps(void) {
	return target_steps;
 }
 
/*
 * Busy 확인
 */ 
 bool motion_state_is_busy(void) {
	 return (current_state == MOTION_RUNNING);
 }
 