/*
 * motion_state.h
 * 
 * 역할: 모션 상태 관리
 * - 현재 상태 추적
 * - 진행률 계산
 */
 
 #ifndef MOTION_STATE_H
 #define MOTION_STATE_H
 
 #include "../include/common_types.h"
 
/*
 * 상태 초기화
 */
 void motion_state_init(void);

/*
 * 상태 설정
 */
 void motion_state_set(motion_state_t state);

/*
 * 상태 가져오기
 */ 
 motion_state_t motion_state_get(void);
 
/*
 * 구간 설정
 */ 
 void motion_state_set_phase(motion_phase_t phase);
 
/*
 * 구간 가져오기
 */ 
 motion_phase_t motion_state_get_phase(void);
 
/*
 * 진행률 계산 (0~100%)
 */
 int motion_state_get_progress(void);
 
/*
 * 현재/목표 스텝 설정
 */
 void motion_state_set_current_step(long step);
 void motion_state_set_target_steps(long steps);
 
/*
 * 현재/목표 스텝 가져오기
 */ 
 long motion_state_get_current_step(void);
 long motion_state_get_target_steps(void);
 
/*
 * Busy 확인
 */
 bool motion_state_is_busy(void);

#endif //MOTION_STATE_H 