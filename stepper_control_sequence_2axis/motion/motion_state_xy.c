/*
 * motion_state_xy.c
 * 
 * 역할: 2축 모션 상태 구현
 */

#include <stdio.h>
#include "motion_state_xy.h"

// 현재 상태
static motion_state_t current_state = MOTION_IDLE;

// 목표 위치 (축별, steps)
static long target_steps[AXIS_COUNT] = {0, 0};

// 현재 스텝 (축별)
static long current_step[AXIS_COUNT] = {0, 0};

// 가감속 단계
static motion_phase_t current_phase = PHASE_CRUISE;

// Bresenham 변수
static long bresenham_dx = 0;
static long bresenham_dy = 0;
static long bresenham_sx = 0;
static long bresenham_sy = 0;
static long bresenham_err = 0;

/*
 * 상태 초기화
 */
void motion_state_init(void) {
    current_state = MOTION_IDLE;
    target_steps[AXIS_X] = 0;
    target_steps[AXIS_Y] = 0;
    current_step[AXIS_X] = 0;
    current_step[AXIS_Y] = 0;
    current_phase = PHASE_CRUISE;
    
    printf("[STATE] 초기화 완료\n");
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
 * Busy 확인
 */
int motion_state_is_busy(void) {
    return (current_state == MOTION_RUNNING);
}

/*
 * 목표 위치 설정 (축별)
 */
void motion_state_set_target_steps(axis_t axis, long steps) {
    if(axis >= AXIS_COUNT) return;
    target_steps[axis] = steps;
}

/*
 * 목표 위치 가져오기 (축별)
 */
long motion_state_get_target_steps(axis_t axis) {
    if(axis >= AXIS_COUNT) return 0;
    return target_steps[axis];
}

/*
 * 현재 스텝 설정 (축별)
 */
void motion_state_set_current_step(axis_t axis, long step) {
    if(axis >= AXIS_COUNT) return;
    current_step[axis] = step;
}

/*
 * 현재 스텝 가져오기 (축별)
 */
long motion_state_get_current_step(axis_t axis) {
    if(axis >= AXIS_COUNT) return 0;
    return current_step[axis];
}

/*
 * 전체 스텝 초기화
 */
void motion_state_reset_current_steps(void) {
    current_step[AXIS_X] = 0;
    current_step[AXIS_Y] = 0;
}

/*
 * 가감속 단계 설정
 */
void motion_state_set_phase(motion_phase_t phase) {
    current_phase = phase;
}

/*
 * 가감속 단계 가져오기
 */
motion_phase_t motion_state_get_phase(void) {
    return current_phase;
}

/*
 * Bresenham 변수 설정
 */
void motion_state_set_bresenham(long dx, long dy, long sx, long sy, long err) {
    bresenham_dx = dx;
    bresenham_dy = dy;
    bresenham_sx = sx;
    bresenham_sy = sy;
    bresenham_err = err;
}

/*
 * Bresenham 변수 가져오기
 */
void motion_state_get_bresenham(long *dx, long *dy, long *sx, long *sy, long *err) {
    if(dx) *dx = bresenham_dx;
    if(dy) *dy = bresenham_dy;
    if(sx) *sx = bresenham_sx;
    if(sy) *sy = bresenham_sy;
    if(err) *err = bresenham_err;
}