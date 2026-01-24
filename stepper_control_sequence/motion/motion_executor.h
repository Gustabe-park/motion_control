/*
 * motion_executor.h
 * 
 * 역할: 모션 실행
 * - 비동기 모션 제어
 * - update() 함수
 */

#ifndef MOTION_EXECUTOR_H
#define MOTION_EXECUTOR_H

#include "../include/common_types.h"

/*
 * 실행기 초기화
 */
error_code_t motion_executor_init(void);

/*
 * 모션 시작 (저수준)
 * steps: 이동할 스텝 (절대값)
 * dir: 방향
 */
error_code_t motion_executor_start(long steps, direction_t dir);

/*
 * 상대 이동 시작 (step)
 */
error_code_t motion_executor_move_relative_steps(long delta_steps);

/*
 * 상대 이동 시작 (mm)
 */
error_code_t motion_executor_move_relative_mm(float delta_mm);

/*
 * 절대 이동 시작 (step)
 */
error_code_t motion_executor_move_absolute_steps(long target_steps);

/*
 * 절대 이동 시작 (mm)
 */
error_code_t motion_executor_move_absolute_mm(float target_mm);

/*
 * 모션 업데이트 (주기적 호출!)
 * 반환: 0=진행 중, 1=완료
 */
int motion_executor_update(void);

/*
 * 모션 완료 대기
 */
void motion_executor_wait_complete(void);

/*
 * 모션 정지
 */
void motion_executor_stop(void);

#endif // MOTION_EXECUTOR_H