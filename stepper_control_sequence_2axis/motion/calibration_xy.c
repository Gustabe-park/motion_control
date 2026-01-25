/*
 * calibration_xy.c
 * 
 * 역할: 2축 캘리브레이션 구현
 */

#include <stdio.h>
#include <math.h>
#include "calibration_xy.h"
#include "../config/hardware_config_xy.h"

// 축별 캘리브레이션 값 (steps/mm)
static float steps_per_mm[AXIS_COUNT] = {
    STEPS_PER_MM,  // X축
    STEPS_PER_MM   // Y축
};

// 축별 현재 위치 (steps)
static long position_steps[AXIS_COUNT] = {0, 0};

/*
 * 캘리브레이션 초기화
 */
void calibration_init(void) {
    steps_per_mm[AXIS_X] = STEPS_PER_MM;
    steps_per_mm[AXIS_Y] = STEPS_PER_MM;
    position_steps[AXIS_X] = 0;
    position_steps[AXIS_Y] = 0;
    
    printf("[CALIBRATION] 초기화 완료\n");
    printf("              X축: %.2f steps/mm\n", steps_per_mm[AXIS_X]);
    printf("              Y축: %.2f steps/mm\n", steps_per_mm[AXIS_Y]);
}

/*
 * 캘리브레이션 설정 (축별)
 */
void calibration_set(axis_t axis, float value) {
    if(axis >= AXIS_COUNT) return;
    
    steps_per_mm[axis] = value;
    printf("[CALIBRATION] %c축 설정: %.2f steps/mm\n",
           axis == AXIS_X ? 'X' : 'Y', value);
}

/*
 * 캘리브레이션 가져오기 (축별)
 */
float calibration_get(axis_t axis) {
    if(axis >= AXIS_COUNT) return 0.0f;
    return steps_per_mm[axis];
}

/*
 * mm → steps 변환 (축별)
 */
long calibration_mm_to_steps(axis_t axis, float mm) {
    if(axis >= AXIS_COUNT) return 0;
    return (long)round(mm * steps_per_mm[axis]);
}

/*
 * steps → mm 변환 (축별)
 */
float calibration_steps_to_mm(axis_t axis, long steps) {
    if(axis >= AXIS_COUNT) return 0.0f;
    return steps / steps_per_mm[axis];
}

/*
 * 원점 설정 (축별)
 */
void calibration_reset_position(axis_t axis) {
    if(axis >= AXIS_COUNT) return;
    
    position_steps[axis] = 0;
    printf("[CALIBRATION] %c축 원점 설정\n",
           axis == AXIS_X ? 'X' : 'Y');
}

/*
 * 전체 원점 설정
 */
void calibration_reset_position_all(void) {
    for(int axis = AXIS_X; axis < AXIS_COUNT; axis++) {
        position_steps[axis] = 0;
    }
    printf("[CALIBRATION] 전체 원점 설정 (0, 0)\n");
}

/*
 * 위치 업데이트 (축별)
 */
void calibration_update_position(axis_t axis, direction_t dir) {
    if(axis >= AXIS_COUNT) return;
    
    if(dir == DIR_FORWARD) {
        position_steps[axis]++;
    } else {
        position_steps[axis]--;
    }
}

/*
 * 현재 위치 가져오기 (축별, steps)
 */
long calibration_get_position_steps(axis_t axis) {
    if(axis >= AXIS_COUNT) return 0;
    return position_steps[axis];
}

/*
 * 현재 위치 가져오기 (축별, mm)
 */
float calibration_get_position_mm(axis_t axis) {
    if(axis >= AXIS_COUNT) return 0.0f;
    return calibration_steps_to_mm(axis, position_steps[axis]);
}

/*
 * XY 위치 동시 가져오기
 */
void calibration_get_position_xy_mm(float *x_mm, float *y_mm) {
    if(x_mm) *x_mm = calibration_get_position_mm(AXIS_X);
    if(y_mm) *y_mm = calibration_get_position_mm(AXIS_Y);
}