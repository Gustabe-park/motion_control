/*
 * configuration.h
 * 기계 설정
 */
 #ifndef CONFIGURATION_H
 #define CONFIGURATION_H
 
 //핀 번호 설정 ========
 // X축
 #define X_STEP			4
 #define X_DIR			3
 #define X_ENABLE		2
 
 // Y축
 #define Y_ENABLE		22
 #define Y_DIR			27
 #define Y_STEP			17
 
 // 리미트 스위치 핀 (Step 15 추가) ========
 // 각 축의 원점(최소) 방향 리미트 스위치
 // NC(Normally Closed) 방식: 평소 HIGH, 스위치 눌리면 LOW 
 #define X_LIMIT_MIN	5	// X축 원점 방향 리미트 스위치
 #define Y_LIMIT_MIN	6	// Y축 원점 방향 리미트 스위치 
 
 // 호밍 설정
 #define HOMING_SPEED_MM_S 		5.0f		// 호밍 이동 속도 (느리게 = 정확하게)
 #define HOMING_ACCEL_MM_S2 	5.0f		// 호밍 가속도
 #define HOMING_MAX_MM		 	200.0f		// 호밍 최대 탐색 거리 
 #define HOMING_BACKOFF_MM		2.0f		// 스위치 감지 후 살짝 후퇴 거리 (mm)
 
 #endif //CONFIGURATION_H
