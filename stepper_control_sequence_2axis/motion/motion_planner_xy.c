/*
 * motion_planner_xy.c
 * 
 * 역할: 2축 모션 플래너 구현
 */

#include <stdio.h>
#include "motion_planner_xy.h"
#include "motion_state_xy.h"

// 현재 속도 프로파일
static speed_profile_t current_profile = {
    .speed_min = 2000,      // 2ms (느림)
    .speed_max = 500,       // 0.5ms (빠름)
    .accel_steps = 50,      // 50 스텝 가속
    .decel_steps = 50       // 50 스텝 감속
};

/*
 * 플래너 초기화
 */
void motion_planner_init(void) {
    printf("[PLANNER] 초기화 완료\n");
    printf("          속도: %d ~ %d us\n",
           current_profile.speed_max, current_profile.speed_min);
    printf("          가감속: %d steps\n",
           current_profile.accel_steps);
}

/*
 * 속도 프로파일 설정
 */
void motion_planner_set_speed_profile(const speed_profile_t *profile) {
    if(!profile) return;
    
    current_profile = *profile;
    
    printf("[PLANNER] 프로파일 설정\n");
    printf("          속도: %d ~ %d us\n",
           current_profile.speed_max, current_profile.speed_min);
    printf("          가감속: %d steps\n",
           current_profile.accel_steps);
}

/*
 * 속도 프로파일 가져오기
 */
void motion_planner_get_speed_profile(speed_profile_t *profile) {
    if(!profile) return;
    *profile = current_profile;
}

/*
 * 프리셋 설정
 */
error_code_t motion_planner_set_preset(int preset) {
    switch(preset) {
        case 1:  // 느림
            current_profile.speed_min = 3000;
            current_profile.speed_max = 1000;
            current_profile.accel_steps = 30;
            current_profile.decel_steps = 30;
            printf("[PLANNER] 프리셋 1: 느림\n");
            break;
            
        case 2:  // 보통
            current_profile.speed_min = 2000;
            current_profile.speed_max = 500;
            current_profile.accel_steps = 50;
            current_profile.decel_steps = 50;
            printf("[PLANNER] 프리셋 2: 보통\n");
            break;
            
        case 3:  // 빠름
            current_profile.speed_min = 1500;
            current_profile.speed_max = 300;
            current_profile.accel_steps = 80;
            current_profile.decel_steps = 80;
            printf("[PLANNER] 프리셋 3: 빠름\n");
            break;
            
        default:
            printf("[PLANNER] 오류: 잘못된 프리셋 %d\n", preset);
            return ERR_INVALID_PARAM;
    }
    
    return ERR_OK;
}

/*
 * 속도 계산 (가감속)
 */
int motion_planner_calculate_speed(long current_step, long total_steps) {
    // 가속 구간
    if(current_step < current_profile.accel_steps) {
        motion_state_set_phase(PHASE_ACCEL);
        
        // 선형 가속
        float ratio = (float)current_step / current_profile.accel_steps;
        int speed = current_profile.speed_min - 
                   (int)((current_profile.speed_min - current_profile.speed_max) * ratio);
        return speed;
    }
    
    // 감속 구간
    else if(current_step > total_steps - current_profile.decel_steps) {
        motion_state_set_phase(PHASE_DECEL);
        
        // 선형 감속
        long decel_step = total_steps - current_step;
        float ratio = (float)decel_step / current_profile.decel_steps;
        int speed = current_profile.speed_min - 
                   (int)((current_profile.speed_min - current_profile.speed_max) * ratio);
        return speed;
    }
    
    // 정속 구간
    else {
        motion_state_set_phase(PHASE_CRUISE);
        return current_profile.speed_max;
    }
}