/*
 * stepper_driver_xy.c
 * 
 * 역할: 2축 스텝모터 드라이버 구현
 */

#include <stdio.h>
#include <time.h>
#include <wiringPi.h>
#include "stepper_driver_xy.h"
#include "../config/hardware_config_xy.h"

// 핀 배열 (인덱스: axis_t)
static const int STEP_PINS[AXIS_COUNT]   = {PIN_STEP_X,   PIN_STEP_Y};
static const int DIR_PINS[AXIS_COUNT]    = {PIN_DIR_X,    PIN_DIR_Y};
static const int ENABLE_PINS[AXIS_COUNT] = {PIN_ENABLE_X, PIN_ENABLE_Y};

/*
 * 현재 시간 (마이크로초)
 */
//unsigned long micros(void) {
//    struct timespec ts;
//    clock_gettime(CLOCK_MONOTONIC, &ts);
//    return ts.tv_sec * 1000000UL + ts.tv_nsec / 1000UL;
//}

/*
 * 드라이버 초기화
 */
error_code_t stepper_driver_init(void) {
    // wiringPi 초기화
    if(wiringPiSetupGpio() == -1) {
        printf("[DRIVER] ERROR: wiringPi 초기화 실패\n");
        return ERR_HARDWARE;
    }
    
    // 모든 축 GPIO 설정
    for(int axis = AXIS_X; axis < AXIS_COUNT; axis++) {
        pinMode(STEP_PINS[axis], OUTPUT);
        pinMode(DIR_PINS[axis], OUTPUT);
        pinMode(ENABLE_PINS[axis], OUTPUT);
        
        // 초기 상태
        digitalWrite(STEP_PINS[axis], LOW);
        digitalWrite(DIR_PINS[axis], DIR_FORWARD);
        digitalWrite(ENABLE_PINS[axis], HIGH);  // 비활성화
    }
    
    printf("[DRIVER] 2축 초기화 완료\n");
    printf("         X축: STEP=%d, DIR=%d, ENABLE=%d\n",
           PIN_STEP_X, PIN_DIR_X, PIN_ENABLE_X);
    printf("         Y축: STEP=%d, DIR=%d, ENABLE=%d\n",
           PIN_STEP_Y, PIN_DIR_Y, PIN_ENABLE_Y);
    
    return ERR_OK;
}

/*
 * 축별 활성화
 */
void stepper_driver_enable(axis_t axis) {
    if(axis >= AXIS_COUNT) return;
    
    digitalWrite(ENABLE_PINS[axis], LOW);
    printf("[DRIVER] %c축 활성화\n", axis == AXIS_X ? 'X' : 'Y');
}

/*
 * 축별 비활성화
 */
void stepper_driver_disable(axis_t axis) {
    if(axis >= AXIS_COUNT) return;
    
    digitalWrite(ENABLE_PINS[axis], HIGH);
    printf("[DRIVER] %c축 비활성화\n", axis == AXIS_X ? 'X' : 'Y');
}

/*
 * 전체 활성화
 */
void stepper_driver_enable_all(void) {
    for(int axis = AXIS_X; axis < AXIS_COUNT; axis++) {
        digitalWrite(ENABLE_PINS[axis], LOW);
    }
    printf("[DRIVER] 전체 축 활성화\n");
}

/*
 * 전체 비활성화
 */
void stepper_driver_disable_all(void) {
    for(int axis = AXIS_X; axis < AXIS_COUNT; axis++) {
        digitalWrite(ENABLE_PINS[axis], HIGH);
    }
    printf("[DRIVER] 전체 축 비활성화\n");
}

/*
 * 방향 설정
 */
void stepper_driver_set_direction(axis_t axis, direction_t dir) {
    if(axis >= AXIS_COUNT) return;
    
    digitalWrite(DIR_PINS[axis], dir);
}

/*
 * 1 스텝 생성
 */
void stepper_driver_make_step(axis_t axis) {
    if(axis >= AXIS_COUNT) return;
    
    digitalWrite(STEP_PINS[axis], HIGH);
    delayMicroseconds(5);
    digitalWrite(STEP_PINS[axis], LOW);
}