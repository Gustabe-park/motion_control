/*
 * motion_planner.h
 * 
 * 역할: 모션 경로 계획
 * - 속도 프로파일 계산
 * - 가감속 알고리즘
 */

#ifndef MOTION_PLANNER_H
#define MOTION_PLANNER_H

#include "../include/common_types.h"
#include "../config/motion_config.h"

/*
 * 플래너 초기화
 */
void motion_planner_init(void);

/*
 * 속도 프로파일 설정
 */
void motion_planner_set_speed_profile(const speed_profile_t *profile);

/*
 * 속도 프로파일 가져오기
 */
void motion_planner_get_speed_profile(speed_profile_t *profile);

/*
 * 속도 프리셋 설정
 * preset: 1=느림, 2=보통, 3=빠름
 */
error_code_t motion_planner_set_preset(int preset);

/*
 * 현재 속도 계산 (가감속 적용)
 * current_step: 현재 스텝
 * target_steps: 목표 스텝
 * 반환: delay 값 (us)
 */
int motion_planner_calculate_speed(long current_step, long target_steps);

#endif // MOTION_PLANNER_H