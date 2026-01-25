/*
 * app_xy_sequence.c
 * 
 * 역할: 2축 시퀀스 제어 (실무형)
 * - State Machine
 * - 가감속 지원
 * - 1축 구조와 동일
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "../motion/motion_executor_xy.h"
#include "../motion/motion_state_xy.h"
#include "../motion/motion_planner_xy.h"
#include "../motion/calibration_xy.h"
#include "../drivers/stepper_driver_xy.h"
#include "../utils/time_utils.h"

// 위치 정의 (mm)
#define POS_P1_X    50.0
#define POS_P1_Y    0.0

#define POS_P2_X    50.0
#define POS_P2_Y    50.0

#define POS_P3_X    0.0
#define POS_P3_Y    50.0

// 시퀀스 상태
typedef enum {
    SEQ_INIT,
    SEQ_MOVE_P1,
    SEQ_WORK_P1,
    SEQ_MOVE_P2,
    SEQ_WORK_P2,
    SEQ_MOVE_P3,
    SEQ_WORK_P3,
    SEQ_HOME,
    SEQ_DONE
} seq_state_t;

volatile int running = 1;

void signal_handler(int signum) {
    printf("\n[INFO] 종료 요청\n");
    running = 0;
}

int main(void) {
    printf("========================================\n");
    printf("  2축 시퀀스 제어 (실무형)\n");
    printf("========================================\n\n");
    
    signal(SIGINT, signal_handler);
    
    // ========================================
    // 초기화
    // ========================================
    if(motion_executor_init() != ERR_OK) {
        printf("초기화 실패!\n");
        return 1;
    }
    
    calibration_set(AXIS_X, 25.0);  // 25 steps/mm
    calibration_set(AXIS_Y, 25.0);
    motion_executor_set_home();
    
    printf("초기 위치를 원점으로 설정하세요!\n\n");
    time_utils_delay_ms(2000);

    printf("enable 진행 중!\n\n");
    stepper_driver_enable_all();
    time_utils_delay_ms(5000);

    // ========================================
    // State Machine
    // ========================================
    seq_state_t state = SEQ_INIT;
    
    while(running) {
        // Executor 업데이트 (항상!)
        motion_executor_update();
        
        // State Machine
        switch(state) {
            case SEQ_INIT:
                printf("[시퀀스] 시작...\n");
                time_utils_delay_ms(1000);
                state = SEQ_MOVE_P1;
                break;
                
            case SEQ_MOVE_P1:
                printf("\n[이동] P1 (%.1f, %.1f) - 빠른 속도\n",
                       POS_P1_X, POS_P1_Y);
                motion_planner_set_preset(3);  // 빠름
                motion_executor_move_xy(POS_P1_X, POS_P1_Y);
                state = SEQ_WORK_P1;
                break;
                
            case SEQ_WORK_P1:
                if(!motion_state_is_busy()) {
                    printf("[작업] P1에서 2초 작업...\n");
                    time_utils_delay_ms(2000);
                    state = SEQ_MOVE_P2;
                }
                break;
                
            case SEQ_MOVE_P2:
                printf("\n[이동] P2 (%.1f, %.1f) - 보통 속도\n",
                       POS_P2_X, POS_P2_Y);
                motion_planner_set_preset(2);  // 보통
                motion_executor_move_xy(POS_P2_X, POS_P2_Y);
                state = SEQ_WORK_P2;
                break;
                
            case SEQ_WORK_P2:
                if(!motion_state_is_busy()) {
                    printf("[작업] P2에서 1초 작업...\n");
                    time_utils_delay_ms(1000);
                    state = SEQ_MOVE_P3;
                }
                break;
                
            case SEQ_MOVE_P3:
                printf("\n[이동] P3 (%.1f, %.1f) - 느린 속도\n",
                       POS_P3_X, POS_P3_Y);
                motion_planner_set_preset(1);  // 느림
                motion_executor_move_xy(POS_P3_X, POS_P3_Y);
                state = SEQ_WORK_P3;
                break;
                
            case SEQ_WORK_P3:
                if(!motion_state_is_busy()) {
                    printf("[작업] P3에서 1초 작업...\n");
                    time_utils_delay_ms(1000);
                    state = SEQ_HOME;
                }
                break;
                
            case SEQ_HOME:
                printf("\n[이동] 원점 복귀 - 보통 속도\n");
                motion_planner_set_preset(2);  // 보통
                motion_executor_move_xy(0.0, 0.0);
                state = SEQ_DONE;
                break;
                
            case SEQ_DONE:
                if(!motion_state_is_busy()) {
                    float x, y;
                    motion_executor_get_xy(&x, &y);
                    
                    printf("\n========================================\n");
                    printf("  시퀀스 완료!\n");
                    printf("  최종 위치: (%.2f, %.2f) mm\n", x, y);
                    printf("========================================\n");
                    running = 0;
                }
                break;
        }
        
        time_utils_delay_us(100);
    }
    
    // ========================================
    // 종료
    // ========================================
    time_utils_delay_ms(500);
    stepper_driver_disable_all();
    
    printf("\n프로그램 종료\n");
    return 0;
}