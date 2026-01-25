/*
 * hardware_config_xy.h
 * 
 * 역할: 2축 (X, Y) 하드웨어 설정
 */

#ifndef HARDWARE_CONFIG_XY_H
#define HARDWARE_CONFIG_XY_H

// ========================================
// X축 GPIO 핀 (BCM 번호)
// ========================================
#define PIN_STEP_X        17  // GPIO17
#define PIN_DIR_X         27  // GPIO27
#define PIN_ENABLE_X      22  // GPIO22

// ========================================
// Y축 GPIO 핀 (BCM 번호)
// ========================================
#define PIN_STEP_Y        4   // GPIO18
#define PIN_DIR_Y         3   // GPIO23
#define PIN_ENABLE_Y      2   // GPIO24

// ========================================
// 모터 사양 (X, Y 동일하다고 가정)
// ========================================
#define MOTOR_STEPS_PER_REV   200     // 스텝퍼 모터의 스텝 수 (1회전)

// ========================================
// 기계 사양
// ========================================
#define LEADSCREW_PITCH_MM  8.0    // 리드스크류 피치 (mm/rev)

// ========================================
// 자동 계산
// ========================================
#define STEPS_PER_MM			((float)MOTOR_STEPS_PER_REV / LEADSCREW_PITCH_MM )

// ========================================
// 작업 영역 제한 (mm)
// ========================================
#define X_MIN	0.0
#define X_MAX	200.0
#define Y_MIN	0.0
#define Y_MAX	200.0

#endif //HARDWARE_CONFIG_XY_H