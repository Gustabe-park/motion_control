/*
 * time_utils.h
 * 
 * 역할: 시간 관련 유틸리티
 */

#ifndef TIME_UTILS_H
#define TIME_UTILS_H

/*
 * 현재 시간 가져오기 (밀리초)
 */
unsigned long time_utils_millis(void);

/*
 * 현재 시간 가져오기 (마이크로초)
 */
unsigned long time_utils_micros(void);

/*
 * 대기 (밀리초)
 */
void time_utils_delay_ms(unsigned long ms);

/*
 * 대기 (마이크로초)
 */
void time_utils_delay_us(unsigned long us);

#endif // TIME_UTILS_H