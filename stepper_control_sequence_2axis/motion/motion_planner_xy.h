/*
 * motion_planner_xy.h
 * 
 * 역할: 2축 모션 플래너 (속도 계획)
 */

#ifndef MOTION_PLANNER_XY_H
#define MOTION_PLANNER_XY_H

#include "../include/common_types_xy.h"

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
 * 프리셋 설정
 */
error_code_t motion_planner_set_preset(int preset);

/*
 * 속도 계산 (가감속)
 * 
 * total_steps: 전체 이동 스텝 수
 * current_step: 현재 스텝 (0부터 시작)
 * 
 * 반환: delay (us)
 */
int motion_planner_calculate_speed(long current_step, long total_steps);

#endif // MOTION_PLANNER_XY_H