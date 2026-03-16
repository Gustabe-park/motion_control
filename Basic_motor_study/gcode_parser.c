/*
 * gcode_parser.c
 * G-code 파싱 및 명령 실행 구현 (Step 16)
 *
 * 이 파일이 하는 일:
 *   1. 문자열 한 줄 → GcodeCommand 구조체로 변환 (parseGcode)
 *   2. GcodeCommand → 모터 실제 동작 (executeGcode)
 *   3. 파싱 + 실행 한 번에 처리 (runGcodeLine)
 *
 * 이 파일이 하지 않는 일:
 *   - 파일 열기/읽기 (→ gcode_runner.c 담당)
 *   - 진행 상황 관리 (→ gcode_runner.c 담당)
 *
 * ======== 버그 수정 내역 (원본 gcode.c 대비) ========
 * 1. isBlankLine(): 캐스트 괄호 누락 수정
 *      수정 전: if (!isspace(unsigned char)*line))
 *      수정 후: if (!isspace((unsigned char)*line))
 *
 * 2. executeGcode() 매개변수 오타 수정
 *      수정 전: const GcodeCommnad *cmd
 *      수정 후: const GcodeCommand *cmd
 */

#include "gcode_parser.h"
#include "accel.h"      /* x_axis, y_axis, moveAxisBySpeed() */
#include "command.h"    /* g_accel_mm_s2 */
#include "homing.h"      //→ runHomingSequence()
#include "interpolate.h" //→ moveLinear()
#include <stdio.h>
#include <string.h>
#include <ctype.h>      /* toupper(), isspace() */

/* ============================================================
 * 내부 도우미 함수 (static: 이 파일 안에서만 사용)
 * ============================================================ */

/*
 * preprocessLine: 주석 제거 + 대문자 변환
 *
 * 파싱 루프를 단순하게 유지하기 위해 사전 정리
 *
 * 예: "G1 X50 Y30 ; 이동 명령" → "G1 X50 Y30 "
 *     "g1 x50 y30"             → "G1 X50 Y30"  (소문자 → 대문자)
 *
 * 매개변수:
 *   src : 원본 문자열
 *   dst : 결과를 저장할 버퍼
 *   size: dst 버퍼 크기 (버퍼 오버플로우 방지용)
 */
static void preprocessLine(const char *src, char *dst, int size) {
    int i = 0;
    while (*src != '\0' && i < size - 1) {
        /* ';' 만나면 주석 시작 → 이후는 무시 */
        if (*src == ';') {
            break;
        }
        /* 소문자 → 대문자 변환 */
        dst[i++] = (char)toupper((unsigned char)*src);
        src++;
    }
    dst[i] = '\0';
}

/*
 * isBlankLine: 문자열이 공백만으로 이루어져 있는지 확인
 *
 * 반환값:
 *   true  = 빈 줄 (공백, 탭, 개행만 있음) → 파싱 불필요
 *   false = 내용 있음 → 파싱 필요
 *
 * [버그 수정] 원본: if (!isspace(unsigned char)*line))
 *                    캐스트 괄호가 빠져 컴파일 오류 발생
 *            수정: if (!isspace((unsigned char)*line))
 */
static bool isBlankLine(const char *line) {
    while (*line != '\0') {
        if (!isspace((unsigned char)*line)) {  /* ← 괄호 수정 */
            return false;
        }
        line++;
    }
    return true;
}

/* ============================================================
 * parseGcode: G-code 한 줄 문자열 → GcodeCommand 구조체
 * ============================================================
 *
 * 파싱 전략:
 *   sscanf("%s %s...")를 쓰지 않는 이유:
 *     G-code는 파라미터 순서가 고정되지 않음
 *     "G1 X50 Y30 F3000"과 "G1 F3000 Y30 X50" 모두 유효
 *
 *   해결책: 문자 단위 스캔
 *     G, X, Y, F 글자를 발견하면 → 바로 뒤 숫자를 sscanf로 읽기
 *
 * 단계별 추적 예시:
 *   입력: "G1 X50.5 Y-10.0 F3000"
 *         (preprocessLine 후: "G1 X50.5 Y-10.0 F3000")
 *
 *   i=0: 'G' 발견 → &clean[1]="1 X50.5..." → g_num = 1
 *   i=1: '1' → 숫자, 키워드 아님 → 무시
 *   i=2: ' ' → 공백 → 무시
 *   i=3: 'X' 발견 → &clean[4]="50.5..." → x = 50.5, has_x = true
 *   ...
 */
int parseGcode(const char *line, GcodeCommand *cmd) {

    /* ── ① 전처리: 주석 제거 + 대문자 변환 ── */
    char clean[256];
    preprocessLine(line, clean, sizeof(clean));

    /* ── ② 빈 줄이면 파싱 불필요 ── */
    if (isBlankLine(clean)) {
        return -1;
    }

    /* ── ③ 구조체 초기값 설정 ── */
    cmd->g_num    = -1;                    /* -1 = G 명령어 없음 */
    cmd->x        = 0.0f;
    cmd->y        = 0.0f;
    cmd->feedrate = GCODE_DEFAULT_FEEDRATE; /* F 없으면 기본값 사용 */
    cmd->has_x    = false;
    cmd->has_y    = false;

    /* ── ④ 문자 단위 스캔 ── */
    int i   = 0;
    int len = (int)strlen(clean);

    while (i < len) {
        char c = clean[i];

        if (c == 'G') {
            /* G 번호 읽기: &clean[i+1] = 'G' 바로 다음 위치 */
            int g_val;
            if (sscanf(&clean[i + 1], "%d", &g_val) == 1) {
                cmd->g_num = g_val;
            }
        }
        else if (c == 'X') {
            float x_val;
            if (sscanf(&clean[i + 1], "%f", &x_val) == 1) {
                cmd->x     = x_val;
                cmd->has_x = true;
            }
        }
        else if (c == 'Y') {
            float y_val;
            if (sscanf(&clean[i + 1], "%f", &y_val) == 1) {
                cmd->y     = y_val;
                cmd->has_y = true;
            }
        }
        else if (c == 'F') {
            float f_val;
            if (sscanf(&clean[i + 1], "%f", &f_val) == 1) {
                cmd->feedrate = f_val;
            }
        }

        i++;
    }

    /* ── ⑤ G 번호 없으면 무효 줄 ── */
    if (cmd->g_num < 0) {
        return -1;
    }

    /* ── ⑥ 지원 명령어 확인 ── */
    /*
     * 새 명령어 추가 시 case만 추가하면 됨
     * 예: case 2: case 3: (원호 보간, 나중에 추가 예정)
     */
    switch (cmd->g_num) {
        case 0:
        case 1:
        case 28:
            break;
        default:
            printf("경고: G%d는 지원하지 않는 명령어입니다.\n", cmd->g_num);
            return -1;
    }

    return 0;
}

/* ============================================================
 * executeGcode: 파싱된 명령 → 실제 모터 동작
 * ============================================================
 *
 * 피드레이트 단위 변환:
 *   G-code F값: mm/min
 *   moveAxisBySpeed() 입력: mm/s
 *   변환: mm/s = mm/min ÷ 60
 *   예: F3000 mm/min ÷ 60 = 50.0 mm/s
 *
 * G0 vs G1:
 *   실제 3D 프린터: G0 = 급속이동(압출 없음), G1 = 작업이동(압출)
 *   현재 단계에서는 모터 이동만 구현하므로 동일하게 처리
 *
 * [버그 수정] 원본: const GcodeCommnad *cmd  (오타)
 *            수정:  const GcodeCommand *cmd
 */
void executeGcode(const GcodeCommand *cmd) {  /* ← 오타 수정 */

    switch (cmd->g_num) {

        case 0:
        case 1: {
            /* 피드레이트 변환: mm/min → mm/s */
            float speed_mm_s = cmd->feedrate / 60.0f;

            printf("[G%d] 이동 | ", cmd->g_num);
            if (cmd->has_x) printf("X=%.2f ", cmd->x);
            if (cmd->has_y) printf("Y=%.2f ", cmd->y);
            printf("F=%.0f(%.1fmm/s)\n", cmd->feedrate, speed_mm_s);

            /*
             * 이동 분기:
             *   X, Y 둘 다 있음 → moveLinear() (Step 13 연결)
             *   X만 있음        → X축만 이동
             *   Y만 있음        → Y축만 이동
             *   둘 다 없음      → F값만 변경하는 줄
             */
            if (cmd->has_x && cmd->has_y) {
                moveLinear(cmd->x, cmd->y, speed_mm_s, g_accel_mm_s2);
            }
            else if (cmd->has_x) {
                moveAxisBySpeed(&x_axis, cmd->x, speed_mm_s, g_accel_mm_s2);
            }
            else if (cmd->has_y) {
                moveAxisBySpeed(&y_axis, cmd->y, speed_mm_s, g_accel_mm_s2);
            }
            else {
                printf("  → 이동 없음 (F값만 변경)\n");
            }
            break;
        }

        case 28:
            /* G28: 호밍 시퀀스 (Step 15 연결) */
            printf("[G28] 호밍 시작\n");
            runHomingSequence();
            printf("[G28] 호밍 완료\n");
            break;

        default:
            printf("알 수 없는 명령어: G%d\n", cmd->g_num);
            break;
    }
}

/* ============================================================
 * runGcodeLine: 파싱 + 실행 한 번에
 * ============================================================
 *
 * command.c에서 직접 G-code 입력 시 사용
 * 예: CMD> GCODE G1 X50 Y30 F3000
 */
int runGcodeLine(const char *line) {
    GcodeCommand cmd;

    if (parseGcode(line, &cmd) != 0) {
        /* 빈 줄/주석은 조용히 넘김, 경고는 parseGcode에서 출력 */
        return -1;
    }

    executeGcode(&cmd);
    return 0;
}