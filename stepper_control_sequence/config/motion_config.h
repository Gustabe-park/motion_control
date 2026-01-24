/*
 * motion_config.h
 * 
 * 역할: 모션 제어 파라미터
 * - 속도 프리셋
 * - 가감속 설정
 * - 안전 제한값
 * 
 * 수정: 성능 튜닝 시
 */
 
 #ifndef MOTION_CONFIG_H
 #define MOTION_CONFIG_H
 
// ========================================
// 속도 프리셋 (delay 값, 마이크로초)
// 주의: 값이 작을수록 빠름!
// ========================================

//프리셋 1: 느림 (안전, 정밀)
#define SPEED_SLOW_MIN				3000
#define SPEED_SLOW_MAX				1000
#define SPEED_SLOW_ACCEL			100

//프리셋 2: 보통 (기본)
#define SPEED_NORMAL_MIN			2000
#define SPEED_NORMAL_MAX			500
#define SPEED_NORMAL_ACCEL			50

//프리셋 3: 빠름 (주의!)
#define SPEED_FAST_MIN				1000
#define SPEED_FAST_MAX				300
#define SPEED_FAST_ACCEL			30

//기본 속도
#define DEFAULT_SPEED_MIN			SPEED_NORMAL_MIN
#define DEFAULT_SPEED_MAX			SPEED_NORMAL_MAX
#define DEFAULT_ACCEL_STEPS			SPEED_NORMAL_ACCEL

// ========================================
// 안전 제한값
// ========================================
#define MAX_SPEED_LIMIT				200		//최소 DELAY (us)
#define MIN_SPEED_LIMIT				5000	//최대 DELAY (us)
#define MAX_ACCEL_LIMIT				200		//최대 가속 스텝

//위치 제한 (mm)
#define SOFT_LIMIT_MIN				-10.0	//최소 위치
#define SOFT_LIMIT_MAX				200.0	//최대 위치

#endif //MOTION_CONFIG_H
