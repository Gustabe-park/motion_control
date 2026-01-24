/*
 * stepper_driver.h
 * 
 * 역할: 스텝모터 하드웨어 제어 (저수준)
 * - GPIO 초기화
 * - 1 스텝 생성
 * - 방향 제어
 * - 모터 활성화/비활성화
 */

#ifndef STEPPER_DRIVER_H
#define STEPPER_DRIVER_H

// GPIO 핀 번호 (BCM)
#define STEP_PIN   4
#define DIR_PIN    3
#define ENABLE_PIN 2

// 방향 정의
#define DIR_FORWARD  1
#define DIR_BACKWARD 0

/*
 * 드라이버 초기화
 * 반환: 0=성공, -1=실패
 */
int stepper_init(void);

/*
 * 모터 활성화
 */
void stepper_enable(void);

/*
 * 모터 비활성화
 */
void stepper_disable(void);

/*
 * 방향 설정
 * dir: DIR_FORWARD 또는 DIR_BACKWARD
 */
void stepper_set_direction(int dir);

/*
 * 1 스텝 생성
 */
void stepper_make_step(void);

/*
 * 마이크로초 단위 지연
 * us: 지연 시간 (마이크로초)
 */
void stepper_delay_us(int us);

#endif // STEPPER_DRIVER_H