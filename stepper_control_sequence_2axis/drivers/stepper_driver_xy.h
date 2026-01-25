/*
 * stepper_driver_xy.h
 * 
 * 역할: 2축 스텝모터 드라이버 (1축과 동일한 구조)
 */

#ifndef STEPPER_DRIVER_XY_H
#define STEPPER_DRIVER_XY_H

#include "../include/common_types_xy.h"

/*
 * 드라이버 초기화
 */
error_code_t stepper_driver_init(void);

/*
 * 축별 활성화/비활성화
 */
void stepper_driver_enable(axis_t axis);
void stepper_driver_disable(axis_t axis);
void stepper_driver_enable_all(void);
void stepper_driver_disable_all(void);

/*
 * 축별 방향 설정
 */
void stepper_driver_set_direction(axis_t axis, direction_t dir);

/*
 * 축별 1 스텝 생성
 */
void stepper_driver_make_step(axis_t axis);

/*
 * 현재 시간 (마이크로초)
 */
//unsigned long micros(void);

#endif // STEPPER_DRIVER_XY_H