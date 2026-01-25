/*
 * motion_state_xy.h
 * 
 * 역할: 2축 모션 상태 관리
 */

#ifndef MOTION_STATE_XY_H
#define MOTION_STATE_XY_H

#include "../include/common_types_xy.h"

/*
 * 상태 초기화
 */
void motion_state_init(void);

/*
 * 상태 설정/가져오기
 */
void motion_state_set(motion_state_t state);
motion_state_t motion_state_get(void);

/*
 * Busy 확인
 */
int motion_state_is_busy(void);

/*
 * 목표 위치 설정/가져오기 (축별, steps)
 */
void motion_state_set_target_steps(axis_t axis, long steps);
long motion_state_get_target_steps(axis_t axis);

/*
 * 현재 스텝 설정/가져오기 (축별)
 */
void motion_state_set_current_step(axis_t axis, long step);
long motion_state_get_current_step(axis_t axis);

/*
 * 전체 스텝 초기화
 */
void motion_state_reset_current_steps(void);

/*
 * 가감속 단계 설정/가져오기
 */
void motion_state_set_phase(motion_phase_t phase);
motion_phase_t motion_state_get_phase(void);

/*
 * Bresenham 변수 설정/가져오기
 */
void motion_state_set_bresenham(long dx, long dy, long sx, long sy, long err);
void motion_state_get_bresenham(long *dx, long *dy, long *sx, long *sy, long *err);

#endif // MOTION_STATE_XY_H