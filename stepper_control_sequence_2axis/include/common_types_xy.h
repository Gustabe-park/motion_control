/*
 * common_types_xy.h
 * 
 * 역할: 2축 프로젝트 공통 타입
 */

#ifndef COMMON_TYPES_XY_H
#define COMMON_TYPES_XY_H

#include <stdint.h>
#include <stdbool.h>

// ========================================
// 에러 코드
// ========================================
typedef enum {
    ERR_OK = 0,             // 성공
    ERR_BUSY = -1,          // 이미 실행 중
    ERR_INVALID_PARAM = -2, // 잘못된 파라미터
    ERR_OUT_OF_RANGE = -3,  // 범위 초과
    ERR_TIMEOUT = -4,       // 타임아웃
    ERR_HARDWARE = -5       // 하드웨어 오류
} error_code_t;

// ========================================
// 축 정의
// ========================================
typedef enum {
    AXIS_X = 0,
    AXIS_Y = 1,
    AXIS_COUNT = 2  // 축 개수
} axis_t;

// ========================================
// 방향
// ========================================
typedef enum {
    DIR_BACKWARD = 0,
    DIR_FORWARD = 1
} direction_t;

// ========================================
// 모션 상태
// ========================================
typedef enum {
    MOTION_IDLE = 0,
    MOTION_RUNNING = 1,
    MOTION_COMPLETED = 2
} motion_state_t;

// ========================================
// 가감속 단계
// ========================================
typedef enum {
    PHASE_ACCEL = 0,    // 가속
    PHASE_CRUISE = 1,   // 정속
    PHASE_DECEL = 2     // 감속
} motion_phase_t;

// ========================================
// 속도 프로파일
// ========================================
typedef struct {
    int speed_min;      // 최소 속도 (us)
    int speed_max;      // 최대 속도 (us)
    int accel_steps;    // 가속 스텝 수
    int decel_steps;    // 감속 스텝 수
} speed_profile_t;

#endif // COMMON_TYPES_XY_H