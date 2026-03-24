/*
 * softlimit.c
 * Step 20: 소프트 리미트 구현
 */
 
 #include "softlimit.h"
 #include "configuration.h"
 #include <stdio.h>
 
/* ========== 전역 변수 실제 생성 ========== */

/*
 * 초기값 false: 프로그램 시작 시 비활성화
 * 이유: 호밍 전에는 위치 정보가 불정확하므로 검사하면 오히려 해가 됨
 */
 bool g_soft_limit_enabled = false;
 
 /*========= 초기화 ==========*/
 
 void initSoftLimit(void) {
	 g_soft_limit_enabled = false;
    printf("[소프트 리미트] 초기화 완료 (비활성 상태)\n");
    printf("  -> 호밍 완료 후 자동으로 활성화됩니다.\n");
}

/* ========== 활성화 / 비활성화 ========== */

 void enableSoftLimit(void) {
	 g_soft_limit_enabled = true;
    printf("\n[소프트 리미트] 활성화!\n");
    printf("  X: %.1f ~ %.1fmm\n", SOFT_LIMIT_X_MIN, SOFT_LIMIT_X_MAX);
    printf("  Y: %.1f ~ %.1fmm\n", SOFT_LIMIT_Y_MIN, SOFT_LIMIT_Y_MAX);
    printf("  -> 이 범위를 벗어나는 이동 명령은 차단됩니다.\n\n");
}

 void disableSoftLimit(void) {
    g_soft_limit_enabled = false;
    printf("[소프트 리미트] 비활성화!\n");
    printf("  -> 경고: 작업 영역 제한이 없습니다. 충돌에 주의하세요!\n");
}

/* ========== 좌표 유효성 검증 ========== */

 bool checkSoftLimit(float x, float y, bool check_x, bool check_y) {
	 
	 // 비활성화 상태면 검사 없이 통과
	 if (!g_soft_limit_enabled) {
		 return true;
	 }
	 
	 bool ok = true;
    /*
     * check_x가 true일 때만 X축 검사
     *
     * 왜 check_x를 쓰는가?
     *   "G1 Y50" → X 좌표 없음 → cmd->has_x = false → check_x = false
     *   이때 x=0.0 (초기값)으로 검사하면 X=0이 범위 안이라 통과는 하지만
     *   "X는 검사 안 해도 된다"는 의도를 명확히 표현하기 위해 플래그 사용
     */
	 if (check_x) {
		 if (x < SOFT_LIMIT_X_MIN || x > SOFT_LIMIT_X_MAX) {
            printf("[소프트 리미트] X 범위 초과!\n");
            printf("  요청: X=%.2fmm  허용: %.1f ~ %.1fmm\n",
                   x, SOFT_LIMIT_X_MIN, SOFT_LIMIT_X_MAX);
            ok = false;
        }
	 }
	 
    if (check_y) {
        if (y < SOFT_LIMIT_Y_MIN || y > SOFT_LIMIT_Y_MAX) {
            printf("[소프트 리미트] Y 범위 초과!\n");
            printf("  요청: Y=%.2fmm  허용: %.1f ~ %.1fmm\n",
                   y, SOFT_LIMIT_Y_MIN, SOFT_LIMIT_Y_MAX);
            ok = false;
        }
    }

	if (!ok) {
		printf("-> 이동 취소, 좌표를 수정하세요.\n");
	}
	
	return ok;
 }
 
/* ========== 상태 출력 ========== */

void printSoftLimitStatus(void) {
    printf("\n===== 소프트 리미트 설정 =====\n");
    printf("상태  : %s\n", g_soft_limit_enabled ? "활성화 [보호 중]" : "비활성화 [주의]");
    printf("X 범위: %.1f ~ %.1fmm\n", SOFT_LIMIT_X_MIN, SOFT_LIMIT_X_MAX);
    printf("Y 범위: %.1f ~ %.1fmm\n", SOFT_LIMIT_Y_MIN, SOFT_LIMIT_Y_MAX);
    printf("명령어: SLENABLE / SLDISABLE 으로 전환\n");
    printf("==============================\n\n");
} 
		