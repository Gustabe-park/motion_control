/*
 * app_sequence.c
 * 
 * 역할: 시퀀스 제어 응용 프로그램
 * - 리팩토링된 모듈 사용
 */

#include <stdio.h>
#include <signal.h>
#include "../motion/motion_executor.h"
#include "../motion/motion_state.h"
#include "../motion/motion_planner.h"
#include "../motion/calibration.h"
#include "../drivers/stepper_driver.h"
#include "../utils/time_utils.h"

volatile int running = 1;

void signal_handler(int signum) {
    printf("\n[APP] 종료 요청\n");
    running = 0;
}

// 시퀀스 상태
typedef enum {
    SEQ_INIT,
    SEQ_MOVE_P1,
    SEQ_WORK_P1,
    SEQ_MOVE_P2,
    SEQ_WORK_P2,
    SEQ_HOME,
    SEQ_DONE
} seq_state_t;

// 위치 (mm)
#define POS_P1  20.0
#define POS_P2  40.0

int main(void) {
    printf("========================================\n");
    printf("  시퀀스 제어 (리팩토링)\n");
    printf("========================================\n\n");
    
    signal(SIGINT, signal_handler);
    
    // 초기화
    if(motion_executor_init() != ERR_OK) {
        return 1;
    }
    
    // 캘리브레이션 (200 step/rev, 8mm/rev)
    calibration_set(25.0);
    calibration_reset_position();
    
    printf("시퀀스: HOME → P1(20mm) → P2(40mm) → HOME\n\n");
    
    seq_state_t state = SEQ_INIT;
    
    // 메인 루프
    while(running) {  // state 조건 제거!
        // Executor 업데이트 (항상!)
        motion_executor_update();
        
        // 상태 머신
        switch(state) {
            case SEQ_INIT:
                printf("[시퀀스] 시작...\n");
                time_utils_delay_ms(1000);
                state = SEQ_MOVE_P1;
                break;
                
            case SEQ_MOVE_P1:
                printf("\n[이동] P1 (20mm) - 보통 속도\n");
                motion_planner_set_preset(2);  // 보통
                motion_executor_move_absolute_mm(POS_P1);
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
                printf("\n[이동] P2 (40mm) - 빠른 속도\n");
                motion_planner_set_preset(3);  // 빠름
                motion_executor_move_absolute_mm(POS_P2);
                state = SEQ_WORK_P2;
                break;
                
            case SEQ_WORK_P2:
                if(!motion_state_is_busy()) {
                    printf("[작업] P2에서 1초 작업...\n");
                    time_utils_delay_ms(1000);
                    state = SEQ_HOME;
                }
                break;
                
            case SEQ_HOME:
                printf("\n[이동] 원점 복귀 - 느린 속도\n");
                motion_planner_set_preset(1);  // 느림
                motion_executor_move_absolute_mm(0.0);
                state = SEQ_DONE;
                break;
                
            case SEQ_DONE:
                if(!motion_state_is_busy()) {
                    printf("\n========================================\n");
                    printf("  시퀀스 완료!\n");
                    printf("  최종 위치: %.2f mm\n",
                           calibration_get_position_mm());
                    printf("========================================\n");
                    running = 0;  // 루프 종료!
                }
                break;
        }
        
        time_utils_delay_us(100);
    }
    
    // 안전을 위한 추가 대기
    printf("\n[INFO] 모터 비활성화 준비...\n");
    time_utils_delay_ms(500);
    
    stepper_disable();
    printf("[INFO] 프로그램 종료\n");
    return 0;
}