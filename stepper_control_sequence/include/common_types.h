/*
 * common_types.h
 * 
 * 역할: 프로젝트 전체에서 사용하는 공통 타입
 * - 상태 enum
 * - 에러 코드
 * - 공통 구조체
 */
 
 #ifndef COMMON_TYPES_H
 #define COMMON_TYPES_H
 
 #include <stdint.h>
 #include <stdbool.h>
 
// ========================================
// 에러 코드
// ========================================
 typedef enum {
	 ERR_OK = 0,						//성공
	 ERR_BUSY = -1,						//이미 실행 중
	 ERR_INVALID_PARAM = -2,			//잘못된 파라미터
	 ERR_OUT_OF_RANGE = -3,				//범위 초과
	 ERR_TIMEOUT = -4,					//타임아웃
	 ERR_HARDWARE = -5					//하드웨어 오류
 } error_code_t;
 
// ========================================
// 방향
// ======================================== 
 typedef enum {
	 DIR_BACKWARD = 0,
	 DIR_FORWARD = 1
 } direction_t;
 
// ========================================
// 모션 상태
// ========================================
 typedef enum {
	 MOTION_IDLE,						//대기 중
	 MOTION_RUNNING,					//실행 중
	 MOTION_PAUSED,						//일시 정지
	 MOTION_COMPLETED,					//완료
	 MOTION_ERROR						//에러
 } motion_state_t;
 
// ========================================
// 모션 구간
// ========================================
 typedef enum {
	 PHASE_ACCEL,				//가속
	 PHASE_CRUISE,				//등속
	 PHASE_DECEL,				//감속
 } motion_phase_t;
 
// ========================================
// 속도 프로파일
// ======================================== 
 typedef struct {
	 int speed_min;				//최저속 delay (us)
	 int speed_max;				//최고속 delay (us)
	 int accel_steps;			//가속 스텝 수
 } speed_profile_t;
 
// ========================================
// 위치 정보
// ========================================
 typedef struct {
	 long steps;			//위치 (step)
	 float mm;				//위치 (mm)
 } position_t;
 
// ========================================
// 모션 명령
// ========================================
 typedef struct {
	 long target_steps;		//목표 스텝
	 direction_t dir;		//방향
	 speed_profile_t speed;	//속도 프로파일
 } motion_command_t;
 
 #endif //COMMON_TYPES_H