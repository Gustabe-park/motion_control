/*
 * motion_executor.c
 * 
 * 역할: 모션 실행 구현
 */

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "motion_executor.h"
#include "motion_state.h"
#include "motion_planner.h"
#include "calibration.h"
#include "../drivers/stepper_driver.h"

// 타이밍
static unsigned long last_step_time = 0;
static int current_speed = 0;
static direction_t current_direction = DIR_FORWARD;

/*
 * 현재 시간 (마이크로초)
 */
static unsigned long micros(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000UL + ts.tv_nsec / 1000;
}

/*
 * 실행기 초기화
 */
error_code_t motion_executor_init(void) {
    // 모듈 초기화
    error_code_t err = stepper_init();
    if(err != ERR_OK) return err;
    
    calibration_init();
    motion_state_init();
    motion_planner_init();
    
    stepper_enable();
    
    printf("[EXECUTOR] 초기화 완료\n");
    return ERR_OK;
}

/*
 * 모션 시작
 */
error_code_t motion_executor_start(long steps, direction_t dir) {
    if(motion_state_is_busy()) {
        return ERR_BUSY;
    }
    
    // 상태 설정
    motion_state_set_target_steps(steps);
    motion_state_set_current_step(0);
    motion_state_set(MOTION_RUNNING);
    
    // 방향 설정
    current_direction = dir;
    stepper_set_direction(dir);
    
    // 타이밍 초기화
    last_step_time = micros();
    
    printf("[EXECUTOR] 시작: %ld steps, dir=%d\n", steps, dir);
    return ERR_OK;
}

/*
 * 상대 이동 (step)
 */
error_code_t motion_executor_move_relative_steps(long delta_steps) {
    if(delta_steps == 0) return ERR_OK;
    
    direction_t dir = (delta_steps > 0) ? DIR_FORWARD : DIR_BACKWARD;
    long steps = (delta_steps > 0) ? delta_steps : -delta_steps;
    
    return motion_executor_start(steps, dir);
}

/*
 * 상대 이동 (mm)
 */
error_code_t motion_executor_move_relative_mm(float delta_mm) {
    long delta_steps = calibration_mm_to_steps(delta_mm);
    return motion_executor_move_relative_steps(delta_steps);
}

/*
 * 절대 이동 (step)
 */
error_code_t motion_executor_move_absolute_steps(long target_steps) {
    long current = calibration_get_position_steps();
    long delta = target_steps - current;
    
    printf("[EXECUTOR] 절대 이동: %ld → %ld steps\n",
           current, target_steps);
    
    return motion_executor_move_relative_steps(delta);
}

/*
 * 절대 이동 (mm)
 */
error_code_t motion_executor_move_absolute_mm(float target_mm) {
    long target_steps = calibration_mm_to_steps(target_mm);
    return motion_executor_move_absolute_steps(target_steps);
}

/*
 * 모션 업데이트
 */
int motion_executor_update(void) {
    if(!motion_state_is_busy()) {
        return 1;  // 완료
    }
    
    // 시간 체크
    unsigned long now = micros();
    if(now - last_step_time < current_speed) {
        return 0;  // 대기
    }
    
    // 스텝 실행
    stepper_make_step();
    last_step_time = now;
    
    // 카운터 & 위치 업데이트
    long current_step = motion_state_get_current_step() + 1;
    motion_state_set_current_step(current_step);
    calibration_update_position(current_direction);
    
    // 완료 확인
    long target = motion_state_get_target_steps();
    if(current_step >= target) {
        motion_state_set(MOTION_COMPLETED);
        printf("[EXECUTOR] 완료: 위치 %.2f mm\n",
               calibration_get_position_mm());
        return 1;
    }
    
    // 다음 속도 계산
    current_speed = motion_planner_calculate_speed(current_step, target);
    
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

/*
 * 모션 정지
 */
void motion_executor_stop(void) {
    if(motion_state_is_busy()) {
        motion_state_set(MOTION_IDLE);
        printf("[EXECUTOR] 정지\n");
    }
}
