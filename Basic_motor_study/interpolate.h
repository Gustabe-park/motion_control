/*
 * interpolate.h
 * Step 13: 선형 보간 (Linear Interpolation)
 *
 * ======== 이 모듈의 목적 ========
 * X, Y 두 축을 동시에 움직여서 직선 경로를 만든다.
 *
 * ======== 핵심 알고리즘: Bresenham ========
 *
 * 예시: X를 30 step, Y를 40 step 이동해야 할 때
 *
 *   dominant(우세) 축 = Y (40 step으로 더 많음)
 *   sub(종속)     축 = X (30 step)
 *
 *   루프를 40번 돌면서:
 *     - 매 루프마다 Y는 반드시 1 step
 *     - X는 오차(error)가 쌓이면 step
 *       → 30/40 = 0.75 비율로 step 발생
 *
 *   결과: 두 축이 동시에 도착! 직선 경로!
 *
 * ======== 속도 계산 ========
 *   - 사용자가 지정한 speed_mm_s는 경로(벡터) 기준
 *   - 실제 거리 = sqrt(dx² + dy²)  ← 피타고라스
 *   - dominant 축의 실제 delay는 이 벡터 속도를 기준으로 계산
 */
 
#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include <stdbool.h>

/* ========= 함수 선언 =============*/

/*
 * 두 축을 동시에 직선으로 이동
 *
 * 매개변수:
 *   x_target   : X 목표 위치 (mm)
 *   y_target   : Y 목표 위치 (mm)
 *   speed_mm_s : 경로 기준 이동 속도 (mm/s)
 *   accel_mm_s2: 가속도 (mm/s²)
 */
 void moveLinear(float x_target, float y_target,
				 float speed_mm_s, float accel_mm_s2);
				 
/*
 * 현재 위치에서 직선 이동 가능한지 확인 (거리 계산)
 * 반환값: 이동할 경로 거리 (mm), 0이면 이동 불필요
 */
 float calcLinearDistance(float x_target, float y_target);
 
 #endif //INTERPOLATE_H