/*
 * gcode_parser.c
 * 
 * 역할: G-code 파싱 구현
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gcode_parser.h"
#include "../motion/motion_executor_xy.h"
#include "../motion/motion_planner_xy.h"

// ========================================
// 내부 헬퍼 함수
// ========================================

/*
 * 문자열에서 숫자 추출
 * 
 * 예: "X50.5" → 50.5 추출
 */
static bool parse_float(const char* str, char prefix, float* value) {
    // prefix 문자 찾기 (대소문자 무시)
    const char* ptr = str;
    while(*ptr) {
        if(toupper(*ptr) == toupper(prefix)) {
            // 숫자 변환
            char* end;
            *value = strtof(ptr + 1, &end);
            
            // 변환 성공 확인
            if(end != ptr + 1) {
                return true;
            }
        }
        ptr++;
    }
    return false;
}

/*
 * 문자열에서 정수 추출
 */
static bool parse_int(const char* str, char prefix, int* value) {
    const char* ptr = str;
    while(*ptr) {
        if(toupper(*ptr) == toupper(prefix)) {
            char* end;
            *value = strtol(ptr + 1, &end, 10);
            
            if(end != ptr + 1) {
                return true;
            }
        }
        ptr++;
    }
    return false;
}

/*
 * 주석 제거 및 공백 정리
 * 
 * G-code 주석: 
 *   - (괄호 안)
 *   - ; 세미콜론 뒤
 */
static void remove_comments(char* line) {
    char* ptr = line;
    
    // 괄호 주석 제거
    while(*ptr) {
        if(*ptr == '(') {
            char* end = strchr(ptr, ')');
            if(end) {
                // 주석 부분을 공백으로 대체
                while(ptr <= end) {
                    *ptr++ = ' ';
                }
            } else {
                // 닫는 괄호 없으면 끝까지 주석
                *ptr = '\0';
                break;
            }
        } else {
            ptr++;
        }
    }
    
    // 세미콜론 주석 제거
    ptr = strchr(line, ';');
    if(ptr) {
        *ptr = '\0';
    }
}

/*
 * 속도 값 → 프리셋 변환
 * 
 * F 값 범위에 따라 프리셋 선택
 */
static int feedrate_to_preset(int feedrate) {
    if(feedrate <= 300) {
        return 1;  // 느림
    } else if(feedrate <= 800) {
        return 2;  // 보통
    } else {
        return 3;  // 빠름
    }
}

// ========================================
// 파서 메인 함수
// ========================================

/*
 * 한 줄 파싱
 */
error_code_t gcode_parse_line(const char* line, gcode_result_t* result) {
    if(!line || !result) {
        return ERR_INVALID_PARAM;
    }
    
    // 결과 초기화
    memset(result, 0, sizeof(gcode_result_t));
    
    // 원본 라인 저장
    strncpy(result->original_line, line, sizeof(result->original_line) - 1);
    
    // 작업용 버퍼 (원본 보존)
    char buffer[128];
    strncpy(buffer, line, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    // 주석 제거
    remove_comments(buffer);
    
    // 빈 줄 확인
    char* ptr = buffer;
    while(*ptr && isspace(*ptr)) ptr++;
    if(*ptr == '\0') {
        result->command = GCODE_NONE;
        return ERR_OK;
    }
    
    // ========================================
    // G 명령 파싱
    // ========================================
    int g_code = -1;
    if(parse_int(buffer, 'G', &g_code)) {
        switch(g_code) {
            case 0:
                result->command = GCODE_G0;
                break;
            case 1:
                result->command = GCODE_G1;
                break;
            case 28:
                result->command = GCODE_G28;
                return ERR_OK;  // G28은 좌표 필요 없음
            case 92:
                result->command = GCODE_G92;
                break;
            default:
                printf("[GCODE] 지원하지 않는 G 명령: G%d\n", g_code);
                result->command = GCODE_ERROR;
                return ERR_INVALID_PARAM;
        }
    }
    
    // ========================================
    // M 명령 파싱
    // ========================================
    int m_code = -1;
    if(parse_int(buffer, 'M', &m_code)) {
        switch(m_code) {
            case 2:
                result->command = GCODE_M2;
                return ERR_OK;
            default:
                printf("[GCODE] 지원하지 않는 M 명령: M%d\n", m_code);
                result->command = GCODE_ERROR;
                return ERR_INVALID_PARAM;
        }
    }
    
    // G/M 명령이 없으면 오류
    if(result->command == GCODE_NONE) {
        result->command = GCODE_ERROR;
        return ERR_INVALID_PARAM;
    }
    
    // ========================================
    // X, Y 좌표 파싱
    // ========================================
    result->has_x = parse_float(buffer, 'X', &result->x);
    result->has_y = parse_float(buffer, 'Y', &result->y);
    
    // ========================================
    // F (속도) 파싱
    // ========================================
    if(parse_int(buffer, 'F', &result->feed_rate)) {
        result->has_f = true;
        // 프리셋으로 변환
        result->feed_rate = feedrate_to_preset(result->feed_rate);
    }
    
    return ERR_OK;
}

/*
 * 파싱 결과 출력
 */
void gcode_print_result(const gcode_result_t* result) {
    if(!result) return;
    
    printf("[GCODE] 명령: ");
    
    switch(result->command) {
        case GCODE_NONE:
            printf("빈 줄\n");
            return;
        case GCODE_G0:
            printf("G0 (빠른 이동)");
            break;
        case GCODE_G1:
            printf("G1 (직선 이동)");
            break;
        case GCODE_G28:
            printf("G28 (홈 복귀)");
            break;
        case GCODE_G92:
            printf("G92 (좌표 설정)");
            break;
        case GCODE_M2:
            printf("M2 (프로그램 종료)");
            break;
        case GCODE_ERROR:
            printf("오류");
            break;
        default:
            printf("알 수 없음");
            break;
    }
    
    if(result->has_x) printf(" X=%.2f", result->x);
    if(result->has_y) printf(" Y=%.2f", result->y);
    if(result->has_f) printf(" F=%d", result->feed_rate);
    
    printf("\n");
}

/*
 * 파일 실행
 */
error_code_t gcode_execute_file(const char* filename) {
    if(!filename) {
        return ERR_INVALID_PARAM;
    }
    
    // 파일 열기
    FILE* fp = fopen(filename, "r");
    if(!fp) {
        printf("[GCODE] 오류: 파일을 열 수 없습니다 - %s\n", filename);
        return ERR_HARDWARE;
    }
    
    printf("\n========================================\n");
    printf("  G-code 파일 실행: %s\n", filename);
    printf("========================================\n\n");
    
    char line[128];
    int line_number = 0;
    error_code_t result = ERR_OK;
    
    // 라인별 읽기 및 실행
    while(fgets(line, sizeof(line), fp)) {
        line_number++;
        
        // 개행 문자 제거
        size_t len = strlen(line);
        if(len > 0 && line[len-1] == '\n') {
            line[len-1] = '\0';
        }
        
        // 파싱
        gcode_result_t gcode;
        error_code_t err = gcode_parse_line(line, &gcode);
        
        if(err != ERR_OK) {
            printf("[라인 %d] 파싱 오류: %s\n", line_number, line);
            continue;
        }
        
        // 빈 줄 건너뛰기
        if(gcode.command == GCODE_NONE) {
            continue;
        }
        
        // 결과 출력
        printf("[라인 %d] ", line_number);
        gcode_print_result(&gcode);
        
        // ========================================
        // 명령 실행
        // ========================================
        switch(gcode.command) {
            case GCODE_G0:  // 빠른 이동
                // 속도 설정
                if(gcode.has_f) {
                    motion_planner_set_preset(gcode.feed_rate);
                } else {
                    motion_planner_set_preset(3);  // G0 기본: 빠름
                }
                
                // 이동
                if(gcode.has_x && gcode.has_y) {
                    motion_executor_move_xy(gcode.x, gcode.y);
                } else if(gcode.has_x) {
                    motion_executor_move_x(gcode.x);
                } else if(gcode.has_y) {
                    motion_executor_move_y(gcode.y);
                }
                
                // 완료 대기
                motion_executor_wait_complete();
                break;
                
            case GCODE_G1:  // 직선 이동
                // 속도 설정
                if(gcode.has_f) {
                    motion_planner_set_preset(gcode.feed_rate);
                } else {
                    motion_planner_set_preset(2);  // G1 기본: 보통
                }
                
                // 이동
                if(gcode.has_x && gcode.has_y) {
                    motion_executor_move_xy(gcode.x, gcode.y);
                } else if(gcode.has_x) {
                    motion_executor_move_x(gcode.x);
                } else if(gcode.has_y) {
                    motion_executor_move_y(gcode.y);
                }
                
                // 완료 대기
                motion_executor_wait_complete();
                break;
                
            case GCODE_G28:  // 홈 복귀
                motion_planner_set_preset(2);  // 보통 속도
                motion_executor_move_xy(0.0, 0.0);
                motion_executor_wait_complete();
                break;
                
            case GCODE_G92:  // 좌표 설정
                motion_executor_set_home();
                printf("         → 현재 위치를 원점으로 설정\n");
                break;
                
            case GCODE_M2:  // 프로그램 종료
                printf("\n[프로그램 종료]\n");
                result = ERR_OK;
                goto cleanup;
                
            default:
                break;
        }
    }
    
cleanup:
    fclose(fp);
    
    printf("\n========================================\n");
    printf("  실행 완료: 총 %d 라인\n", line_number);
    printf("========================================\n");
    
    return result;
}