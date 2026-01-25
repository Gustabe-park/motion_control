/*
 * calibration_xy.h
 * 
 * 역할: 2축 캘리브레이션 (축별 위치 관리)
 */

#ifndef CALIBRATION_XY_H
#define CALIBRATION_XY_H

#include "../include/common_types_xy.h"

/*
 * 캘리브레이션 초기화
 */
void calibration_init(void);

/*
 * 캘리브레이션 설정 (축별)
 */
void calibration_set(axis_t axis, float steps_per_mm);

/*
 * 캘리브레이션 가져오기 (축별)
 */
float calibration_get(axis_t axis);

/*
 * mm → steps 변환 (축별)
 */
long calibration_mm_to_steps(axis_t axis, float mm);

/*
 * steps → mm 변환 (축별)
 */
float calibration_steps_to_mm(axis_t axis, long steps);

/*
 * 원점 설정 (축별)
 */
void calibration_reset_position(axis_t axis);

/*
 * 전체 원점 설정
 */
void calibration_reset_position_all(void);

/*
 * 위치 업데이트 (축별)
 */
void calibration_update_position(axis_t axis, direction_t dir);

/*
 * 현재 위치 가져오기 (축별, steps)
 */
long calibration_get_position_steps(axis_t axis);

/*
 * 현재 위치 가져오기 (축별, mm)
 */
float calibration_get_position_mm(axis_t axis);

/*
 * XY 위치 동시 가져오기
 */
void calibration_get_position_xy_mm(float *x_mm, float *y_mm);

#endif // CALIBRATION_XY_H