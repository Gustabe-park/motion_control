/*
 * hardware_config.h
 * 
 * 역할: 하드웨어 관련 설정값
 * - GPIO 핀 번호
 * - 모터 사양
 * - 기계 사양
 * 
 * 수정: 하드웨어 변경 시에만
 */
 
 #ifndef HARDWARE_CONFIG_H
 #define HARDWARE_CONFIG_H
 
// ========================================
// GPIO 핀 설정 (BCM 번호)
// ========================================
#define PIN_STEP    4
#define PIN_DIR     3
#define PIN_ENABLE  2

// ========================================
// 모터 사양
// ========================================
#define MOTOR_STEPS_PER_REV 200     //1.8도
//#define MOTOR_STEPS_PER_REV 400     //0.9도

// ========================================
// 기계 사양 (리드스크류 / 벨트)
// ========================================
#define LEADSCREW_PITCH_MM   8.0          //T8 리드스크류
//#define LEADSCREW_PITCH_MM   2.0        //T2 리드스크류

//벨트 방식 사용 시:
// #define PULLEY_TEETH      20
// #define BELT_PITCH_MM     2.0          //GT2 벨트
// #define MM_PER_REV        (PULLEY_TEETH * BELT_PITCH_MM)

// ========================================
// 자동 계산 (수정 금지!)
// ========================================
#define STEPS_PER_MM  ((float)MOTOR_STEPS_PER_REV / LEADSCREW_PITCH_MM)

#endif  //HARDWARE_CONFIG_H