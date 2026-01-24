/*
 * stepper_driver.c
 * 
 * 역할: 스텝모터 하드웨어 제어 구현
 */

#include <stdio.h>
#include <wiringPi.h>
#include "stepper_driver.h"

/*
 * 드라이버 초기화
 */
int stepper_init(void) {
    // wiringPi 초기화 (BCM 핀 번호 사용)
    if(wiringPiSetupGpio() == -1) {
        printf("[ERROR] wiringPi 초기화 실패\n");
        return -1;
    }
    
    printf("[INFO] wiringPi 초기화 완료\n");
    
    // GPIO 핀 모드 설정
    pinMode(STEP_PIN, OUTPUT);
    pinMode(DIR_PIN, OUTPUT);
    pinMode(ENABLE_PIN, OUTPUT);
    
    // 초기 상태
    digitalWrite(STEP_PIN, LOW);
    digitalWrite(DIR_PIN, DIR_FORWARD);
    digitalWrite(ENABLE_PIN, HIGH);  // 비활성화 상태로 시작
    
    printf("[INFO] GPIO 핀 설정 완료\n");
    printf("       STEP_PIN=%d, DIR_PIN=%d, ENABLE_PIN=%d\n", 
           STEP_PIN, DIR_PIN, ENABLE_PIN);
    
    return 0;
}

/*
 * 모터 활성화
 */
void stepper_enable(void) {
    digitalWrite(ENABLE_PIN, LOW);  // LOW = 활성화
    printf("[INFO] 모터 활성화\n");
}

/*
 * 모터 비활성화
 */
void stepper_disable(void) {
    digitalWrite(ENABLE_PIN, HIGH);  // HIGH = 비활성화
    printf("[INFO] 모터 비활성화\n");
}

/*
 * 방향 설정
 */
void stepper_set_direction(int dir) {
    digitalWrite(DIR_PIN, dir);
}

/*
 * 1 스텝 생성
 */
void stepper_make_step(void) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(5);  // 5us 펄스 폭
    digitalWrite(STEP_PIN, LOW);
}

/*
 * 마이크로초 단위 지연
 */
void stepper_delay_us(int us) {
    delayMicroseconds(us);
}