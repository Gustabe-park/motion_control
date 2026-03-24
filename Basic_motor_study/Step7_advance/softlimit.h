/*
 * softlimit.h
 * Step 20: 소프트 리미트 구현
 *
 * ======== 소프트 리미트란? ========
 *
 * 하드 리미트 (물리 스위치, Step 15/19):
 *   모터가 실제로 끝까지 가서 스위치에 부딪혀야 감지
 *   → 충돌 에너지가 이미 발생한 후 정지
 *
 * 소프트 리미트 (소프트웨어 경계, Step 20):
 *   이동 명령이 들어오면 실행 전에 좌표를 검사
 *   범위 초과 → 명령 자체를 취소 → 모터는 움직이지 않음
 *
 *   G1 X150 → checkSoftLimit(150, ...) → false → 이동 취소!
 *
 * ======== 왜 호밍 후에만 활성화하는가? ========
 *
 *   전원 켤 때 모터는 자신의 위치를 모른다 (current_mm = 0 이지만 실제 위치는?)
 *   호밍 완료 후에야 current_mm이 실제 위치와 일치
 *   → 그 전에 소프트 리미트를 켜면 정상 이동도 잘못 차단할 수 있음
 *
 *   호밍 성공 → enableSoftLimit() 자동 호출 → 이후부터 검사 시작
 *
 * ======== 검증 시점 ========
 *
 *   이동 전 검증 (올바름):
 *     명령 수신 → 좌표 검사 → 안전하면 이동
 *
 *   이동 후 검증 (의미 없음):
 *     명령 수신 → 이동 → 충돌 → 그제서야 검사...
 */
 
 #ifndef SOFTLIMIT_H
 #define SOFTLIMIT_H
 
 /* ========== 전역 변수 (extern 선언) ========== */

/*
 * g_soft_limit_enabled: 소프트 리미트 활성화 여부
 *
 * false: 검사 안 함 (초기값, 호밍 전)
 * true : 모든 이동 전에 좌표 검사
 */
 extern bool g_soft_limit_enabled;
 
 /* ========== 함수 선언 ========== */

/*
 * 초기화: 비활성화 상태로 시작
 * main()에서 호출
 */
 void initSoftLimit(void);
 
 /*
 * 소프트 리미트 활성화
 * 호밍 성공 후 command.c에서 자동 호출
 */
 void enableSoftLimit(void);
 
/*
 * 소프트 리미트 비활성화
 * SLDISABLE 명령어로 수동 해제 (주의 필요)
 */
void disableSoftLimit(void);

/*
 * 좌표 유효성 검증 ← 핵심 함수
 *
 * 매개변수:
 *   x, y    : 검증할 목표 좌표 (mm)
 *   check_x : X축 검사 여부
 *             "G1 Y50" 처럼 X 없는 명령은 false
 *   check_y : Y축 검사 여부
 *
 * 반환값:
 *   true  = 안전, 이동 가능
 *   false = 범위 초과, 이동 취소
 *
 * 사용 예시:
 *   if (!checkSoftLimit(x_target, 0.0f, true, false)) continue;
 */
 bool checkSoftLimit(float x, float y, bool check_x, bool check_y);

/*
 * 현재 소프트 리미트 설정 출력
 * SLIMIT 명령어에서 호출
 */
void printSoftLimitStatus(void);

#endif /* SOFTLIMIT_H */