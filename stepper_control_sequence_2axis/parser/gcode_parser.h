/*
 * gcode_parser.h
 * 
 * 역할: G-code 명령 파싱
 * - G0/G1: 이동 (빠름/직선)
 * - G28: 홈 복귀
 * - G92: 좌표계 설정
 * - F: 속도 설정
 * - M2: 프로그램 종료
 */

#ifndef GCODE_PARSER_H
#define GCODE_PARSER_H

#include "../include/common_types_xy.h"

// ========================================
// G-code 명령 타입
// ========================================
typedef enum {
    GCODE_NONE = 0,      // 빈 줄 또는 주석
    GCODE_G0 = 1,        // 빠른 이동 (Rapid)
    GCODE_G1 = 2,        // 직선 이동 (Feed)
    GCODE_G28 = 28,      // 홈 복귀
    GCODE_G92 = 92,      // 좌표계 설정
    GCODE_M2 = 1002,     // 프로그램 종료
    GCODE_ERROR = -1     // 파싱 오류
} gcode_command_t;

// ========================================
// 파싱 결과 구조체
// ========================================
typedef struct {
    gcode_command_t command;  // 명령 타입
    
    // 좌표 데이터
    bool has_x;
    bool has_y;
    float x;
    float y;
    
    // 속도 데이터
    bool has_f;
    int feed_rate;  // 속도 프리셋 (1~3)
    
    // 원본 라인 (디버깅용)
    char original_line[128];
    
} gcode_result_t;

// ========================================
// 파서 함수
// ========================================

/*
 * 한 줄 파싱
 * 
 * line: G-code 문자열 (예: "G1 X50 Y30 F500")
 * result: 파싱 결과 저장
 * 
 * 반환: ERR_OK (성공), ERR_INVALID_PARAM (파싱 오류)
 */
error_code_t gcode_parse_line(const char* line, gcode_result_t* result);

/*
 * 파일 파싱 및 실행
 * 
 * filename: G-code 파일 경로
 * 
 * 반환: ERR_OK (성공), ERR_HARDWARE (파일 열기 실패)
 */
error_code_t gcode_execute_file(const char* filename);

/*
 * 결과 출력 (디버깅)
 */
void gcode_print_result(const gcode_result_t* result);

#endif // GCODE_PARSER_H