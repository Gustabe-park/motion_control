/*
 * interpolate.c
 * Step 13: 선형 보간 (Linear Interpolation)
 *
 * Bresenham 알고리즘으로 2축 동기화 직선 이동 구현
 */
 
#include "interpolate.h"
#include "accel.h"
#include <stdio.h>
#include <wiringPi.h>
#include <math.h>

/* ===========================================================
 * 경로 거리 계산
 * ===========================================================
 * 피타고라스 정리로 실제 이동 거리를 구한다.
 *
 *   거리 = sqrt( (목표X - 현재X)² + (목표Y - 현재Y)² )
 *
 * 예: X를 30mm, Y를 40mm 이동 → sqrt(900+1600) = 50mm
 */
 float calcLinearDistance(float x_target, float y_target) {
	 float dx = x_target - x_axis.current_mm;
	 float dy = y_target - y_axis.current_mm;
	 return sqrtf(dx*dx + dy*dy);
 }
 
/* ===========================================================
 * 선형 보간 이동 (핵심 함수)
 * ===========================================================
 */ 
 void moveLinear(float x_target, float y_target,
				 float speed_mm_s, float accel_mm_s2) {
					 
    /* ---- 1단계: 이동 거리 및 방향 계산 ---- */
	float dx = x_target - x_axis.current_mm; // X 이동 거리 (부호 포함)
	float dy = y_target - y_axis.current_mm; // X 이동 거리 (부호 포함)
	
	float distance = sqrtf(dx * dx + dy * dy); // 실제 경로 거리 (mm)
	
	/* 이동 거리가 0에 가까우면 종료 */
	if (distance < 0.01f) {
		printf("이미 목표 위치 입니다.\n");
		return;
	}
	
    printf("선형 보간 시작:\n");
    printf("  현재: X=%.2f, Y=%.2f\n",
           x_axis.current_mm, y_axis.current_mm);
    printf("  목표: X=%.2f, Y=%.2f\n", x_target, y_target);
    printf("  경로 거리: %.2fmm\n", distance);
	
	/* --- 2단계 step 수 계산 ----*/
    /*
     * fabs(): 절댓값 함수 → 방향은 따로 처리하고, 스텝 수는 항상 양수
     */	
	 int x_steps = (int)(fabsf(dx) * x_axis.steps_per_mm);
	 int y_steps = (int)(fabsf(dy) * y_axis.steps_per_mm);
	 
    printf("  X: %d steps, Y: %d steps\n", x_steps, y_steps);
	
    /* ---- 3단계: 방향 설정 ---- */
    /*
     * dx > 0 이면 + 방향 (HIGH), dx < 0 이면 - 방향 (LOW)
     */
	 digitalWrite(x_axis.dir_pin, dx >= 0 ? HIGH : LOW);
	 digitalWrite(y_axis.dir_pin, dy >= 0 ? HIGH : LOW);
	 
    /* ---- 4단계: dominant(우세) 축 결정 ---- */
    /*
     * step 수가 더 많은 축이 dominant
     * dominant 축은 매 루프마다 반드시 1 step
     * sub 축은 Bresenham 오차가 쌓이면 step
     */	 
	 bool x_is_dominant = (x_steps >= y_steps);
	 int dominant_steps = x_is_dominant ? x_steps : y_steps;
	 int sub_steps		= x_is_dominant ? y_steps : x_steps; 
	 
    printf("  우세 축: %s (%d steps)\n",
           x_is_dominant ? "X" : "Y", dominant_steps);
		   
    /* ---- 5단계: 속도 → delay 변환 ---- */
    /*
     * 사용자가 지정한 speed_mm_s는 경로(벡터) 기준 속도
     * dominant 축의 실제 속도를 계산해야 한다.
     *
     * dominant 축의 거리 비율 = dominant 거리 / 전체 경로 거리
     * dominant 축 속도 = 전체 속도 × 비율
     *
     * 예: 전체 50mm/s, X가 dominant(30mm/50mm = 0.6 비율)
     *     → X 축 속도 = 50 × 0.6 = 30mm/s
     */
	 float dominant_mm		= x_is_dominant ? fabsf(dx) : fabsf(dy); // fabsf 함수는 C/C++ 언어에서 float 타입 부동 소수점 수의 절댓값을 계산하여 반환하는 함수입니다. <math.h> 또는 <cmath> 헤더에 정의되어 있으며, 음수와 양수 모두 양수(또는 0)로 변환합니다. 
	 float dominant_ratio	= dominant_mm / distance;				 // 비율 (0~1)
	 float dominant_speed	= speed_mm_s * dominant_ratio;			 // dominant 축 실제 속도
	 
	 /* dominant 측 steps_per_mm 선택 */
	 float dom_steps_per_mm = x_is_dominant 
							  ? x_axis.steps_per_mm
							  : y_axis.steps_per_mm;
							  
    /* delay 계산 (accel.c의 함수 재사용) */
	int maxDelay	= speedToDelay(dominant_speed, dom_steps_per_mm);
	int accelSteps	= accelToSteps(dominant_speed, accel_mm_s2, dom_steps_per_mm);
	
    printf("  dominant 속도: %.1f mm/s → delay: %d us\n",
           dominant_speed, maxDelay);
    printf("  가속 구간: %d steps\n", accelSteps);	
	
    /* ---- 6단계: Bresenham 오차 초기화 ---- */
    /*
     * error는 sub 축을 step해야 할 시점을 판단하는 누적 오차
     * 초기값을 dominant/2로 설정하면 반올림 효과가 생겨서 더 균일한 분포가 됨
     *
     * [동작 원리]
     *   매 루프에서 error -= sub_steps
     *   error가 0 미만이 되면 → sub 축 1 step 실행
     *                         → error += dominant_steps (오차 보정)
     *
     * 예: dominant=4, sub=3 일 때
     *   초기: error = 2 (=4/2)
     *   i=0: error = 2-3 = -1 < 0 → sub step!  error = -1+4 = 3
     *   i=1: error = 3-3 =  0     → no step    error = 0
     *   i=2: error = 0-3 = -3 < 0 → sub step!  error = -3+4 = 1
     *   i=3: error = 1-3 = -2 < 0 → sub step!  error = -2+4 = 2
     *   결과: 4번 중 3번 sub step → 비율 3/4 = 0.75 ✓
     */
	 int error = dominant_steps / 2;
	 
	 /* 가속 시작 DELAY 최대 delay 5배, 느린 시작 */
	 int startDelay = maxDelay * 5;
	 if (startDelay < 500) {
		 startDelay = 500;
	 }
	 
	 /* 7단계 메인 루프 Bresenham + 가속/감속 */
	 for (int i = 0 ; i < dominant_steps; i++){
		 
		 /* accel.c와 동일한 방식 가속, 감속 계산*/
		 int delayUs;
		 
		 if (i < accelSteps){
			 // 가속구간 점점 빨라짐
			 float ratio	= (float)i / accelSteps;
			 float sqRatio	= ratio * ratio;
			 delayUs = startDelay - (int)(sqRatio * (startDelay - maxDelay));
		 }
		 else if (i >= dominant_steps - accelSteps){
			 // 감속구간 점점 느려짐
			 int remaining 	= dominant_steps - i;
			 float ratio 	= (float)remaining / accelSteps;
			 float sqRatio 	= ratio * ratio;
			 delayUs = startDelay - (int)(sqRatio * (startDelay - maxDelay));
		 }
		 else {
			 // 등속 구간
			 delayUs = maxDelay;
		 }
		 
		 // dominant 축은 반드시 step
		 if (x_is_dominant) {
			 digitalWrite(x_axis.step_pin, HIGH);
		 } else {
			 digitalWrite(y_axis.step_pin, HIGH);
		 }
		 
		 /* Bresenham: sub 축 step 여부 결정 */
		 error -= sub_steps;
		 if (error < 0) {
			 // 오차가 누적되어 sub 축도 step
			 if (x_is_dominant) {
				 digitalWrite(y_axis.step_pin, HIGH);
			 } else {
				 digitalWrite(x_axis.step_pin, HIGH);
			 }
			 error += dominant_steps; // 오차 보정
		 }
		 
		 // HIGH 유지 후 LOW로, 펄스 생성
		 delayMicroseconds(delayUs);
		 digitalWrite(x_axis.step_pin, LOW);
		 digitalWrite(y_axis.step_pin, LOW);		 
		 delayMicroseconds(delayUs);
	 }

	 // 8단계 위치 정보 업데이트
	 x_axis.current_mm		= x_target;
	 y_axis.current_mm		= y_target;
	 x_axis.current_steps += (dx >= 0) ? x_steps : -x_steps;
	 y_axis.current_steps += (dy >= 0) ? y_steps : -y_steps;

	 printf(" 도착 완료: X=%.2f, Y=%.2f\n\n", 
	 x_axis.current_mm, y_axis.current_mm);
				 }
			 
			 
/*
 * 삼항 연산자: x += (dx >= 0) ? x_steps : -x_steps;
 *if문
	if (dx >= 0) {
		x += x_steps;
	} else {
		x += -x_steps;
	}
	성능은 비슷, 가독성에 차이 잇을 수 잇음
	*/