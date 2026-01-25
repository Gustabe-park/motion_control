/*
 * app_gcode_runner.c
 * 
 * 역할: G-code 파일 실행 애플리케이션
 * 
 * 사용법:
 *   ./app_gcode_runner program.gcode
 */
 
 #include <stdio.h>
 #include <signal.h>
 #include <string.h>
 #include "../motion/motion_executor_xy.h"
 #include "../motion/calibration_xy.h"
 #include "../drivers/stepper_driver_xy.h"
 #include "../parser/gcode_parser.h"
 #include "../utils/time_utils.h"
 
 volatile int running = 1;
 
 void signal_handler(int signum) {
	 printf("\n[INFO]종료 요청\n");
	 running = 0;
 }
 
 int main(int argc, char* argv[]) {
    printf("========================================\n");
    printf("  G-code Runner\n");
    printf("========================================\n\n");	 
	
	// 인자 확인
	if(argc < 2) {
		printf("사용법: %s <gcode_file>\n", argv[0]);
		printf("예시: %s program.gcode\n\n", argv[0]);
		return 1;
	}
	
	const char* filename = argv[1];
	
	signal(SIGINT, signal_handler);
	
    // ========================================
    // 초기화
    // ========================================
	printf("[1] 모션 시스템 초기화...\n");
	if(motion_executor_init() != ERR_OK) {
		printf("오류: 초기화 실패!\n");
		return 1;
	}
	
	// 캘리브레이션 설정
	calibration_set(AXIS_X, 25.0); // 25 steps/mm
	calibration_set(AXIS_Y, 25.0);
	
	printf("\n[2]원점 설정\n");
    printf("    현재 위치를 (0, 0)으로 설정합니다.\n");
    printf("    모터를 수동으로 원점에 위치시키세요!\n");
    printf("    준비되면 Enter를 누르세요...\n");
	getchar();
	
	motion_executor_set_home();
	//모터 활성화
	printf("\n[3]모터 활성화...\n");
	stepper_driver_enable_all();
	time_utils_delay_ms(2000);
	
    // ========================================
    // G-code 파일 실행
    // ========================================
	printf("\n[4]G-code 파일 실행 시작\n");
	
	error_code_t err = gcode_execute_file(filename);
	
	if(err != ERR_OK) {
		printf("\n오류: 실행 실패 (코드: %d)\n", err);
	} else {
		printf("\n성공: 모든 명령 완료\n");
	}
	
    // ========================================
    // 최종 위치 확인
    // ========================================	
	float final_x, final_y;
	motion_executor_get_xy(&final_x, &final_y);
	
    printf("\n========================================\n");
    printf("  최종 위치: (%.2f, %.2f) mm\n", final_x, final_y);
    printf("========================================\n");	
	
    // ========================================
    // 종료
    // ========================================	
	time_utils_delay_ms(500);
	stepper_driver_disable_all();
	
	printf("\n프로그램 종료\n");
	return 0;
 }
 
	
	
	