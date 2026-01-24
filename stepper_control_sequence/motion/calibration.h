/*
 * calibration.h
 * 
 * 역할: Step 캘리브레이션 관리
 * - steps/mm 변환
 * - 위치 추적
 */
 
 #ifndef CALIBRATION_H //ifndef 가 무엇인가? 그리고 항상 첫번쨰 헤더파일은 이런식으로 정의하는데 이유가 잇나?
 #define CALIBRATION_H
 
 #include "../include/common_types.h"
 #include "../config/hardware_config.h"
 
 /*
  * 캘리브레이션 초기화
  */
 void calibration_init(void);
 
 /*
  * 캘리브레이션 설정
  * steps_per_mm: 1mm당 스텝 수
  */ 
 void calibration_set(float steps_per_mm);
 
 /*
  * 캘리브레이션 값 가져오기
  */
 float calibration_get(void);
 
 /*
  * 단위 변환: mm → steps
  */
 long calibration_mm_to_steps(float mm);
 
 /*
  * 단위 변환: steps → mm
  */
 float calibration_steps_to_mm(long steps);
 
 /*
  * 현재 절대 위치 설정
  */
 void calibration_set_position_steps(long steps);
 void calibration_set_position_mm(float mm);
 
 /*
  * 현재 절대 위치 가져오기
  */
 long calibration_get_position_steps(void);
 float calibration_get_position_mm(void);
 
 /*
  * 원점 리셋
  */
 void calibration_reset_position(void);
 
 /*
  * 위치 업데이트 (내부용)
  */
 void calibration_update_position(direction_t dir);
 
 #endif //CALIBRATION_H