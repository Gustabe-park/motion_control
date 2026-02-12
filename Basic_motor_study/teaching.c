/*
 * teaching.c
 * 티칭 팬던트 기능 구현
 */
 
 #include "teaching.h"
 #include "accel.h"
 #include "command.h"
 #include <stdio.h>
 #include <string.h>
 
 TeachPoint g_teach_points[MAX_TEACH_POINTS];	 // 티칭 포인트 저장 배열
 int g_teach_count = 0; 						// 현재 저장된 포인트 수
 
 void teachCurrentPosition(void) {
	 
	 /* 배열 범위 초과 방지 */
	 if (g_teach_count >= MAX_TEACH_POINTS) {
		 printf("오류: 최대 저장 개수(%d)에 도달\n",MAX_TEACH_POINTS);
		 printf("-> CLEAR 또는 DELETE 로 공간 확보 요청\n");
		 return;
	 }
	 
	 // 현재 위치와 설정값을 구조체에 저장된
	 g_teach_points[g_teach_count].x_mm			= x_axis.current_mm;
	 g_teach_points[g_teach_count].y_mm			= y_axis.current_mm;
	 g_teach_points[g_teach_count].speed_mm_s	= g_speed_mm_s;
	 g_teach_points[g_teach_count].accel_mm_s2	= g_accel_mm_s2;

	// 저장 결과 출력
	printf("-> 포인트 %d 저장 완료\n", g_teach_count);
	printf(" X = %.2fmm, Y=%.2fmm\n",
	 g_teach_points[g_teach_count].x_mm,	
	 g_teach_points[g_teach_count].y_mm);
	printf("속도 = %.1fmm/s, 가속도 = %.1fmm/s2\n",
	 g_teach_points[g_teach_count].speed_mm_s,
	 g_teach_points[g_teach_count].accel_mm_s2);
	 
	// 포인트 개수 증가
	g_teach_count++;
	
	printf("(총 %d개 포인트 저장됨)\n", g_teach_count);
 }
 
 void listTeachPoints(void) {
	 
	 if (g_teach_count == 0) {
		 printf("저장된 포인트가 없습니다.\n");
		 printf("-> TEACH 명령으로 현재 위치를 저장하세요.\n");
		 return;
	 }
	 
    /* 헤더 출력 */
    printf("\n===== 티칭 포인트 목록 (총 %d개) =====\n", g_teach_count);
    printf("번호 |   X(mm)   |   Y(mm)   | 속도(mm/s) | 가속(mm/s²)\n");
    printf("-----+-----------+-----------+------------+------------\n");	 
	
	for (int i = 0; i < g_teach_count; i++) {
		printf(" %3d | %9.2f | %9.2f | %10.1f | %10.1f\n",
				i,
				g_teach_points[i].x_mm,
				g_teach_points[i].y_mm,
				g_teach_points[i].speed_mm_s,	
				g_teach_points[i].accel_mm_s2);
	}
	printf("==================================================="\n);
 }
 
 void deleteTeachPoint(int index) {
	 // 유효 범위 확인
	 if (index < 0 || index >= g_teach_count) {
		 printf("오류: 유효하지 않은 번호입니다. (0~%d)\n", g_teach_count - 1);
		 return;
	 }
	 
	 // 삭제 전 해당 포인트 정보 출력
	 printf("->포인트 %d 삭제: X = %.2f, Y=%.2f\n",
			index,
			g_teach_points[index].x_mm,
			g_teach_points[index].y_mm);
			
	// 배열 재정렬: 삭제된 위치 뒤의 요소들을 앞으로 한 칸씩 복사
	for (int i = index; i < g_teach_count - 1; i++) {
		g_teach_points[i] = g_teach_points[i + 1];
	}
	
	// 포인트 개수 감소
	g_teach_count--;
	
	printf(" 남은 포인트 개수: %d개 \n",g_teach_count);
 }
 
 // 배열 데이터를 실제로 지우지는 않고, 카운터만 0으로 리셋한다.
 // 새 데이터가 덮어쓰므로 실제 삭제는 불필요
 
 void clearTeachPoints(void) {
	 g_teach_count = 0;
	 printf("-> 모든 티칭 포인트가 삭제 되었습니다.\n");
 }
 
 // 메모리의 티칭 포인트들을 텍스트 파일로 저장하는 함수
 int saveTeachPoints(const char *filename) {
	 
	 // 저장할 포인트가 없으면 중단
	 if (g_teach_count == 0) {
		 printf("저장할 포인트가 없습니다.\n");
		 return -1;
	 }
	 
	 // 파일명이 NULL이면 기본 파일명 사용
	 // 사용자가 SAVE만 입력하면 "teach_points.txt"를 사용
	 
	 if (filename == NULL) {
		 filename = DEFAULT_TEACH_FILE;
	 }
	 
	 // 파일 열기 (쓰기 모드 "w")
	 FILE *fp = fopen(filename, "w");
	 if (fp == NULL) {
		 printf("오류: 파일 '%s'을 열 수 없습니다.\n", filename);
		 return -1;
	 }
	// 파일 첫 줄에 주석(헤더) 작성 
	//'#'으로 시작하는 줄은 읽을 때 건너 뜀, 내용 이해 목적 줄
	fprintf(fp, "# Teaching Points (X_mm, Y_mm, Speed_mm_s, Accel_mm_s2)\n");
	
	// 각 포인트를 한 줄씩 파일에 기록
	// 쉼표(,)로 값을 구분하는 csv 형식
	// \n으로 줄바꿈하여 다음 포인트로 이동
	
	for (int i = 0;i < g_teach_count; i++) {
		fprintf(fp, "%.2f,%.2f,%.1f,%.1f\n",
				g_teach_points[i].x_mm,
				g_teach_points[i].y_mm,
				g_teach_points[i].speed_mm_s,
				g_teach_points[i].accel_mm_s2);
	}
	
	// 파일 닫기 
	// 버퍼에 남아있는 데이터를 디스크에 확실히 기록
	// fclose 안하면 데이터 손실 발생 가능성 
	fclose(fp);
	
	printf("-> %d개 포인트를 '%s' 파일에 저장했습니다.\n", g_teach_count, filename);
	return 0;
	 }

 int loadTeachPoints(const char *filename) {
	 
	 // 파일명이 NULL이면 기본 파일명 사용자가
	 if (filename == NULL) {
		 filename = DEFAULT_TEACH_FILE;
	 }

	 FILE *fp = fopen(filename, "r");
	 if (fp == NULL) {
		 printf("오류: 파일 '%s'을 열 수 없습니다.\n", filename);
		 return -1;	 
	 }
	 // 기존 데이터 초기화
	 g_teach_count = 0;
	 
	 // 한 줄씩 읽어서 파싱
	 char line[256];
	 
	 while (fgets(line, sizeof(line), fp) != NULL) {
		 
		 //주석 줄 건너뛰기
		 if (line[0] == '#') {
			 continue;
		 }
		 
		 // 빈 줄 건너뛰기 및 줄 바꿈 문자만 있는 빈 줄도 무시
		 if (line[0] == '\n' || line[0] == '\r') {
			 continue;
		 }
		 
		 // 배열이 가득 찼으면 더 이상 읽지 않음
		 if (g_teach_count >= MAX_TEACH_POINTS) {
			 printf("경고: 최대 저장 개수(%d)에 도달하여 나머지는 무시합니다.\n",
					MAX_TEACH_POINTS);
			break;
		 }
		 
		 // sscanf로 csv 형식 파싱
		 float x, y, spd, acc;
		 int parsed = sscanf(line, "%f,%f,%f,%f",&x,&y,&spd,&acc);
		 
		 if (parsed == 4) {
			 // 4개 값 모두 정상적으로 읽힌 경우에만 저장
			 g_teach_points[g_teach_count].x_mm				=x;
			 g_teach_points[g_teach_count].y_mm				=y;
			 g_teach_points[g_teach_count].speed_mm_s		=spd;
			 g_teach_points[g_teach_count].accel_mm_s2		=acc;
			 g_teach_count++;
		 } else {
			 // 파싱 실패, 사용자가 수동 편집 중 실수하면 발생 가능성
			 // 해당 줄만 무시하고 계속 읽기
			 
			 printf("경고: 잘못된 형식의 줄 무시: %s",line);
		 }
	 }
		 
	 //파일 닫기
	 fclose(fp);
	 
	 printf("-> '%s'에서 %d개 포인트를 불러왔습니다.\n", filename, g_teach_count);
	 
	 //불러온 포인트 목록 바로 출력
	 listTeachPoints();
	 
	 return 0;
}




 //저장된 포인트들을 순서대로 연속 실행하는 함수	
 void runTeachPoints(void) {

 //실행할 포인트가 없는 경우	 
	 if (g_teach_count == 0) {
		 printf("실행할 포인트가 없습니다.\n");
		 printf("-> teach로 포인트를 저장하거나 load로 파일을 불러오세요.\n");
		 return;
	 }
	 
	 printf("\n ====== 티칭 포인트 연속 실행 시작 ========\n");
	 printf("총 %d개 포인트를 순서대로 실행합니다.\n\n",g_teach_count);
 
	 for (int i = 0; i < g_teach_count; i++) {
		 
		 printf("- 포인트 %d/%d \n", i+1, g_teach_count);
		 printf("	목표: X=%.2fmm, y=%.2fmm\n",
				g_teach_points[i].x_mm,
				g_teach_points[i].y_mm);
		 printf("	속도=%.1fmm/s, 가속도=%.1fmm/s2\n",
				g_teach_points[i].speed_mm_s,
				g_teach_points[i].accel_mm_s2);
				
		// X축 이동
		moveAxisBySpeed(&x_axis,
						g_teach_points[i].x_mm,
						g_teach_points[i].speed_mm_s,
						g_teach_points[i].accel_mm_s2);

		// y축 이동
		moveAxisBySpeed(&y_axis,
						g_teach_points[i].y_mm,
						g_teach_points[i].speed_mm_s,
						g_teach_points[i].accel_mm_s2);
						
		printf("-> 포인트 %d 도착 완료 \n\n", i + 1);
	 }
		
						