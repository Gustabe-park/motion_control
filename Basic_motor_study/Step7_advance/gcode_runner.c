/*
 * gcode_runner.c
 * G-code 파일 읽기 및 실행 흐름 제어 (Step 17)
 *
 * 이 파일이 하는 일:
 *   - 파일 열기 / 닫기
 *   - 한 줄씩 읽기 (fgets)
 *   - 읽은 줄을 gcode_parser에 전달
 *   - 진행 상황 출력
 *
 * 이 파일이 하지 않는 일:
 *   - 문자열 파싱        (→ gcode_parser.c 담당)
 *   - 모터 직접 제어     (→ gcode_parser.c → accel.c 담당)
 */
 
 #include "gcode_runner.h"
 #include "gcode_parser.h"
 #include "accel.h"
 #include <stdio.h>
 
/* ============================================================
 * executeGcodeFile: .gcode 파일 전체를 읽어 순차 실행
 * ============================================================
 *
 * 실행 흐름:
 *   ① 파일 열기
 *   ② 한 줄씩 읽기 → runGcodeLine() 전달
 *       빈 줄 / 주석 → runGcodeLine() 내부에서 자동 건너뜀
 *       유효한 줄    → 파싱 + 실행까지 runGcodeLine()이 처리
 *   ③ 파일 닫기 + 완료 출력
 */
 int executeGcodeFile(const char *filename) {
	 
	 // 파일 열기
	 FILE *fp = fopen(filename, "r");
	 if (fp == NULL){
		 printf("오류: 파일 '%s'을 열 수 없습니다.\n",filename);
		 return -1;
	 }
	 
	 printf("\n=======g-code 파일 실행 시작========\n");
	 printf("파일: %s\n\n",filename);
	 
	 // 한줄 씩 읽어 runGcodeLine()에 전달 
	 char line[256];
	 int exec_count = 0;
	 
	 while (fgets(line, sizeof(line), fp) != NULL){
        /*
         * runGcodeLine() 하나로 파싱 + 빈 줄 건너뜀 + 실행 처리
         * 반환값: 0 = 실행됨, -1 = 건너뜀
         */
		if (runGcodeLine(line) == 0){
			exec_count++;
		}
	 }
	 
	 // 완료
	 fclose(fp);
	 
    printf("\n====== G-code 파일 실행 완료 ======\n");
    printf("실행된 명령: %d\n", exec_count);
    printf("최종 위치: X=%.2fmm, Y=%.2fmm\n",
           x_axis.current_mm, y_axis.current_mm);
		   
    return 0;
 }
	 