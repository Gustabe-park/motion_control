/*
 * motion_planner.c
 * 
 * 역할: 모션 경로 계획 구현
 */

#include <stdio.h>
#include "motion_planner.h"
#include "motion_state.h"
#include "../config/motion_config.h"

// 현재 속도 프로파일
static speed_profile_t current_profile = {
    .speed_min = DEFAULT_SPEED_MIN,
    .speed_max = DEFAULT_SPEED_MAX,
    .accel_steps = DEFAULT_ACCEL_STEPS
};

/*
 * 플래너 초기화
 */
void motion_planner_init(void) {
    printf("[PLANNER] 초기화: %d~%dus, Accel=%d\n",
           current_profile.speed_max,
           current_profile.speed_min,
           current_profile.accel_steps);
}

/*
 * 속도 프로파일 설정
 */
void motion_planner_set_speed_profile(const speed_profile_t *profile) {
    current_profile = *profile;
    
    printf("[PLANNER] 속도 설정: %d~%dus, Accel=%d\n",
           profile->speed_max,
           profile->speed_min,
           profile->accel_steps);
}

/*
 * 속도 프로파일 가져오기
 */
void motion_planner_get_speed_profile(speed_profile_t *profile) {
    *profile = current_profile;
}

/*
 * 속도 프리셋 설정
 */
error_code_t motion_planner_set_preset(int preset) {
    switch(preset) {
        case 1:  // 느림
            current_profile.speed_min = SPEED_SLOW_MIN;
            current_profile.speed_max = SPEED_SLOW_MAX;
            current_profile.accel_steps = SPEED_SLOW_ACCEL;
            printf("[PLANNER] 프리셋: 느림\n");
            break;
            
        case 2:  // 보통
            current_profile.speed_min = SPEED_NORMAL_MIN;
            current_profile.speed_max = SPEED_NORMAL_MAX;
            current_profile.accel_steps = SPEED_NORMAL_ACCEL;
            printf("[PLANNER] 프리셋: 보통\n");
            break;
            
        case 3:  // 빠름
            current_profile.speed_min = SPEED_FAST_MIN;
            current_profile.speed_max = SPEED_FAST_MAX;
            current_profile.accel_steps = SPEED_FAST_ACCEL;
            printf("[PLANNER] 프리셋: 빠름\n");
            break;
            
        default:
            return ERR_INVALID_PARAM;
    }
    
    return ERR_OK;
}

/*
 * 현재 속도 계산
 */
int motion_planner_calculate_speed(long current_step, long target_steps) {
    int speed;
    int accel = current_profile.accel_steps;
    
    // 가속 구간
    if(current_step < accel) {
        motion_state_set_phase(PHASE_ACCEL);
        speed = current_profile.speed_min - 
                (current_profile.speed_min - current_profile.speed_max) * 
                current_step / accel;
    }
    // 감속 구간
    else if(current_step >= target_steps - accel) {
        motion_state_set_phase(PHASE_DECEL);
        long decel_step = current_step - (target_steps - accel);
        speed = current_profile.speed_max + 
                (current_profile.speed_min - current_profile.speed_max) * 
                decel_step / accel;
    }
    // 등속 구간
    else {
        motion_state_set_phase(PHASE_CRUISE);
        speed = current_profile.speed_max;
    }
    
    return speed;
}
