/*
 * motion_executor_xy.h
 * 
 * 역할: 2축 모션 실행 (Bresenham + 가감속)
 */

#ifndef MOTION_EXECUTOR_XY_H
#define MOTION_EXECUTOR_XY_H

#include "../include/common_types_xy.h"

/*
 * 초기화
 */
error_code_t motion_executor_init(void);

/*
 * X축만 이동 (절대 좌표, mm)
 */
error_code_t motion_executor_move_x(float x_mm);

/*
 * Y축만 이동 (절대 좌표, mm)
 */
error_code_t motion_executor_move_y(float y_mm);

/*
 * XY 동시 이동 (절대 좌표, mm, 직선 보간)
 */
error_code_t motion_executor_move_xy(float x_mm, float y_mm);

/*
 * 현재 위치 가져오기
 */
float motion_executor_get_x(void);
float motion_executor_get_y(void);
void motion_executor_get_xy(float *x_mm, float *y_mm);

/*
 * 원점 설정
 */
void motion_executor_set_home(void);

/*
 * 업데이트 (주기적 호출 필수!)
 * 
 * 반환: 1=완료, 0=진행중
 */
int motion_executor_update(void);

/*
 * 완료 대기 (블로킹)
 */
void motion_executor_wait_complete(void);

#endif // MOTION_EXECUTOR_XY_H