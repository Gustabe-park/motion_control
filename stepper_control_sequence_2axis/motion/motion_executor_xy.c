/*
 * motion_executor_xy.c
 * 
 * 역할: 2축 모션 실행 (Bresenham + 가감속)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <wiringPi.h>
#include "motion_executor_xy.h"
#include "motion_state_xy.h"
#include "motion_planner_xy.h"
#include "calibration_xy.h"
#include "../drivers/stepper_driver_xy.h"
#include "../config/hardware_config_xy.h"

// 타이밍
static unsigned long last_step_time = 0;
static int current_speed = 0;

// 방향 (축별)
static direction_t current_direction[AXIS_COUNT] = {DIR_FORWARD, DIR_FORWARD};

// Bresenham 진행 변수
static long total_steps = 0;  // 전체 이동 스텝 수

/*
 * 초기화
 */
error_code_t motion_executor_init(void) {
    // 모듈 초기화
    error_code_t err = stepper_driver_init();
    if(err != ERR_OK) return err;
    
    calibration_init();
    motion_state_init();
    motion_planner_init();
    
    stepper_driver_enable_all();
    
    printf("[EXECUTOR] 2축 초기화 완료\n");
    return ERR_OK;
}

/*
 * 모션 시작 (내부 함수)
 */
static error_code_t motion_executor_start(float x_mm, float y_mm) {
    if(motion_state_is_busy()) {
        return ERR_BUSY;
    }
    
    // 범위 체크
    if(x_mm < X_MIN || x_mm > X_MAX || 
       y_mm < Y_MIN || y_mm > Y_MAX) {
        printf("[EXECUTOR] 오류: 범위 초과 (%.1f, %.1f)\n", x_mm, y_mm);
        return ERR_OUT_OF_RANGE;
    }
    
    // 목표 위치 (steps)
    long target_x = calibration_mm_to_steps(AXIS_X, x_mm);
    long target_y = calibration_mm_to_steps(AXIS_Y, y_mm);
    
    // 현재 위치 (steps)
    long pos_x = calibration_get_position_steps(AXIS_X);
    long pos_y = calibration_get_position_steps(AXIS_Y);
    
    // 이동 거리
    long dx = abs(target_x - pos_x);
    long dy = abs(target_y - pos_y);
    
    // 이동 거리가 0이면 무시
    if(dx == 0 && dy == 0) {
        printf("[EXECUTOR] 이미 목표 위치\n");
        return ERR_OK;
    }
    
    // 방향
    long sx = (target_x > pos_x) ? 1 : -1;
    long sy = (target_y > pos_y) ? 1 : -1;
    
    current_direction[AXIS_X] = (sx > 0) ? DIR_FORWARD : DIR_BACKWARD;
    current_direction[AXIS_Y] = (sy > 0) ? DIR_FORWARD : DIR_BACKWARD;
    
    // Bresenham 초기화
    long err = dx - dy;
    motion_state_set_bresenham(dx, dy, sx, sy, err);
    
    // 전체 스텝 수 계산 (대각선 길이)
    total_steps = (dx > dy) ? dx : dy;
    
    // 상태 설정
    motion_state_set_target_steps(AXIS_X, target_x);
    motion_state_set_target_steps(AXIS_Y, target_y);
    motion_state_reset_current_steps();
    motion_state_set(MOTION_RUNNING);
    
    // 방향 설정
    stepper_driver_set_direction(AXIS_X, current_direction[AXIS_X]);
    stepper_driver_set_direction(AXIS_Y, current_direction[AXIS_Y]);
    usleep(10);  // 방향 전환 대기
    
    // 초기 속도 설정
    speed_profile_t profile;
    motion_planner_get_speed_profile(&profile);
    current_speed = profile.speed_min;
    
    // 타이밍 초기화
    last_step_time = micros();
    
    printf("[EXECUTOR] 이동 시작: (%.2f, %.2f) → (%.2f, %.2f) mm\n",
           calibration_get_position_mm(AXIS_X),
           calibration_get_position_mm(AXIS_Y),
           x_mm, y_mm);
    printf("            총 %ld 스텝\n", total_steps);
    
    return ERR_OK;
}

/*
 * X축만 이동
 */
error_code_t motion_executor_move_x(float x_mm) {
    float current_y = calibration_get_position_mm(AXIS_Y);
    return motion_executor_start(x_mm, current_y);
}

/*
 * Y축만 이동
 */
error_code_t motion_executor_move_y(float y_mm) {
    float current_x = calibration_get_position_mm(AXIS_X);
    return motion_executor_start(current_x, y_mm);
}

/*
 * XY 동시 이동
 */
error_code_t motion_executor_move_xy(float x_mm, float y_mm) {
    return motion_executor_start(x_mm, y_mm);
}

/*
 * 현재 위치 가져오기
 */
float motion_executor_get_x(void) {
    return calibration_get_position_mm(AXIS_X);
}

float motion_executor_get_y(void) {
    return calibration_get_position_mm(AXIS_Y);
}

void motion_executor_get_xy(float *x_mm, float *y_mm) {
    calibration_get_position_xy_mm(x_mm, y_mm);
}

/*
 * 원점 설정
 */
void motion_executor_set_home(void) {
    calibration_reset_position_all();
    printf("[EXECUTOR] 원점 설정 (0, 0)\n");
}

/*
 * 업데이트 (Bresenham + 가감속)
 */
int motion_executor_update(void) {
    if(!motion_state_is_busy()) {
        return 1;  // 완료
    }
    
    // 타이밍 체크
    unsigned long now = micros();
    if(now - last_step_time < (unsigned long)current_speed) {
        return 0;  // 대기
    }
    last_step_time = now;
    
    // 현재 위치
    long pos_x = calibration_get_position_steps(AXIS_X);
    long pos_y = calibration_get_position_steps(AXIS_Y);
    
    // 목표 위치
    long target_x = motion_state_get_target_steps(AXIS_X);
    long target_y = motion_state_get_target_steps(AXIS_Y);
    
    // 완료 확인
    if(pos_x == target_x && pos_y == target_y) {
        motion_state_set(MOTION_COMPLETED);
        printf("[EXECUTOR] 완료: (%.2f, %.2f) mm\n",
               calibration_get_position_mm(AXIS_X),
               calibration_get_position_mm(AXIS_Y));
        return 1;
    }
    
    // Bresenham 변수 가져오기
    long dx, dy, sx, sy, err;
    motion_state_get_bresenham(&dx, &dy, &sx, &sy, &err);
    
    long e2 = 2 * err;
    
    // X축 이동
    if(e2 > -dy && pos_x != target_x) {
        err -= dy;
        stepper_driver_make_step(AXIS_X);
        calibration_update_position(AXIS_X, current_direction[AXIS_X]);
        
        // 현재 스텝 증가
        long current_step_x = motion_state_get_current_step(AXIS_X);
        motion_state_set_current_step(AXIS_X, current_step_x + 1);
    }
    
    // Y축 이동
    if(e2 < dx && pos_y != target_y) {
        err += dx;
        stepper_driver_make_step(AXIS_Y);
        calibration_update_position(AXIS_Y, current_direction[AXIS_Y]);
        
        // 현재 스텝 증가
        long current_step_y = motion_state_get_current_step(AXIS_Y);
        motion_state_set_current_step(AXIS_Y, current_step_y + 1);
    }
    
    // Bresenham 변수 업데이트
    motion_state_set_bresenham(dx, dy, sx, sy, err);
    
    // 속도 계산 (주축 기준)
    long current_step = motion_state_get_current_step(AXIS_X);
    if(motion_state_get_current_step(AXIS_Y) > current_step) {
        current_step = motion_state_get_current_step(AXIS_Y);
    }
    
    current_speed = motion_planner_calculate_speed(current_step, total_steps);
    
    return 0;  // 진행 중
}

/*
 * 완료 대기
 */
void motion_executor_wait_complete(void) {
    while(motion_state_is_busy()) {
        motion_executor_update();
        usleep(10);
    }
}