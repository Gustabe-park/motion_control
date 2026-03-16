/*
 * gcode_parser.h
 * G-code 파싱 및 명령 실행 (Step 16)
 *
 * 역할: 문자열 해석만 담당
 *   - 문자열 → GcodeCommand 구조체 변환
 *   - GcodeCommand → 모터 동작 변환
 *
 * 이 모듈은 데이터 출처를 모름
 * (파일인지, 시리얼인지, 버퍼인지 상관없이 동작)
 */

#ifndef GCODE_PARSER_H
#define GCODE_PARSER_H

#include <stdbool.h>

/* ========== 상수 정의 ========== */

/* 기본 피드레이트: 파일에 F값 없을 때 사용 (mm/min) */
#define GCODE_DEFAULT_FEEDRATE  1000.0f

/* ========== 구조체 정의 ========== */

/*
 * GcodeCommand: 파싱된 G-code 한 줄의 데이터
 *
 * feedrate에 항상 기본값이 들어있으므로 has_f 불필요
 *   → 기본값: GCODE_DEFAULT_FEEDRATE
 *   → F값이 줄에 있으면 해당 값으로 덮어씀
 *
 * 예시:
 *   "G1 X50 Y30 F3000" 파싱 결과
 *     g_num    = 1
 *     x        = 50.0,  has_x = true
 *     y        = 30.0,  has_y = true
 *     feedrate = 3000.0
 *
 *   "G1 X10" 파싱 결과 (F 없음)
 *     g_num    = 1
 *     x        = 10.0,  has_x = true
 *     y        = 0.0,   has_y = false
 *     feedrate = 1000.0  ← 기본값 유지
 */
typedef struct {
    int   g_num;      /* G 번호 (-1이면 G 없음) */
    float x;          /* X 좌표 (mm) */
    float y;          /* Y 좌표 (mm) */
    float feedrate;   /* 피드레이트 (mm/min) */
    bool  has_x;      /* X 값이 줄에 있었는가 */
    bool  has_y;      /* Y 값이 줄에 있었는가 */
} GcodeCommand;

/* ========== 함수 선언 ========== */

/*
 * G-code 한 줄 파싱
 *
 * 입력: "G1 X50 Y30 F3000"
 * 출력: GcodeCommand 구조체에 결과 저장
 *
 * 반환값:
 *   0  = 파싱 성공
 *  -1  = 주석, 빈 줄, 지원 안 되는 명령어 (건너뜀)
 */
int parseGcode(const char *line, GcodeCommand *cmd);

/*
 * 파싱된 명령 실행
 *
 * parseGcode() 결과를 받아 모터를 실제로 움직임
 *
 * G0/G1: moveLinear() 또는 moveAxisBySpeed() 호출
 * G28  : runHomingSequence() 호출
 */
void executeGcode(const GcodeCommand *cmd);

/*
 * 한 줄 파싱 + 실행을 한 번에 처리하는 편의 함수
 *
 * command.c에서 "GCODE G1 X50 Y30 F3000" 직접 입력 시 사용
 *
 * 반환값:
 *   0  = 성공
 *  -1  = 파싱 실패 (주석, 빈 줄, 지원 안 되는 명령)
 */
int runGcodeLine(const char *line);

#endif /* GCODE_PARSER_H */