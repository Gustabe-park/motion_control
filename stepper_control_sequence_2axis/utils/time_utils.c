/*
 * time_utils.c
 * 
 * 역할: 시간 유틸리티 구현
 */

#include <time.h>
#include <unistd.h>
#include "time_utils.h"

/*
 * 밀리초
 */
unsigned long time_utils_millis(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000UL + ts.tv_nsec / 1000000UL;
}

/*
 * 마이크로초
 */
unsigned long time_utils_micros(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000UL + ts.tv_nsec / 1000UL;
}

/*
 * 밀리초 대기
 */
void time_utils_delay_ms(unsigned long ms) {
    usleep(ms * 1000);
}

/*
 * 마이크로초 대기
 */
void time_utils_delay_us(unsigned long us) {
    usleep(us);
}