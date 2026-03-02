/*
 * gcode_runner.h
 * G-code 파일 읽기 및 실행 흐름 제어 (Step 17)
 *
 * 역할: 파일 I/O + 실행 흐름만 담당
 *   - .gcode 파일을 열어 한 줄씩 읽기
 *   - 읽은 줄을 gcode_parser에 전달
 *   - 진행 상황 출력
 *
 * 나중에 파일 대신 시리얼/네트워크로 G-code를 받아도
 * gcode_parser.c는 그대로 재사용 가능
 */

#ifndef GCODE_RUNNER_H
#define GCODE_RUNNER_H

/*
 * .gcode 파일 전체 실행
 *
 * 파일을 한 줄씩 읽어 parseGcode() → executeGcode() 로 전달
 * 주석, 빈 줄은 건너뜀
 * 진행 상황을 [현재줄/전체줄] 형식으로 출력
 *
 * 반환값:
 *   0  = 성공
 *  -1  = 파일 열기 실패
 */
int executeGcodeFile(const char *filename);

#endif /* GCODE_RUNNER_H */