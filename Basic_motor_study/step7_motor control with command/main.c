/*
 *7단계: 속도 프로파일 (감속/감속) - 절대좌표
 *
 *학습 목표:
 * - 가속/감속의 필요성 이해
 * - 반복문 안에서 변수 값 동적으로 변경하기
 * - 절대좌표 이동에 감속/감속 적용
 * - Y축을 추가해서 제작 
 * main.c
 */
 #include "configuration.h"
 #include "accel.h"
 #include "command.h" // <- 추가
 #include <wiringPi.h>
 #include <stdio.h>
 #include <stdbool.h>
 #include <math.h>
 
  
  int main(void) {
	  //wiringPi 초기화
	  if (wiringPiSetupGpio() == -1) {
		  printf("wiringPi 초기화 실패\n");
		  return 1;
	  }
	  
	  // X축 핀 설정
	  
	  pinMode(x_axis.step_pin, OUTPUT);
	  pinMode(x_axis.dir_pin, OUTPUT);
	  pinMode(x_axis.enable_pin, OUTPUT);

	  // Y축 핀 설정

	  pinMode(y_axis.step_pin, OUTPUT);
	  pinMode(y_axis.dir_pin, OUTPUT);
	  pinMode(y_axis.enable_pin, OUTPUT);
	  
	  //모터 활성화
	  digitalWrite(x_axis.enable_pin, LOW);
	  printf("X축 모터 활성화\n");
	  delay(2000);

	  digitalWrite(y_axis.enable_pin, LOW);
	  printf("Y축 모터 활성화\n");
	  delay(2000);
	  
    printf("\n=== 2축 제어 테스트 ===\n");
    printf("X축 설정: %.1f steps/mm\n", x_axis.steps_per_mm);
    printf("Y축 설정: %.1f steps/mm\n", y_axis.steps_per_mm);

	// ========= 명령어 시스템 시작 =============
	initCommandSystem();
	runCommandLoop();
	//========================================

    
    // 모터 비활성화
    digitalWrite(x_axis.enable_pin, HIGH);
    digitalWrite(y_axis.enable_pin, HIGH);
    
    printf("\n완료!\n");
	  
	  return 0;
  }
	  
	  