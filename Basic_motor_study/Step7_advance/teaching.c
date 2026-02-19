/*
 * teaching.c
 * 티칭 팬던트 기능 구현
 * step 12: 모션 버퍼 및 큐 관리 추가
 */
 
 #include "teaching.h"
 #include "accel.h"
 #include "command.h"
 #include <stdio.h>
 #include <string.h>
 
 // ============ 전역 변수 생성 ==============
 
 TeachPoint g_teach_points[MAX_TEACH_POINTS];	// 티칭 포인트 저장 배열
 int g_teach_count = 0; 						// 현재 저장된 포인트 수
 
 MotionBuffer g_motion_buffer;					// 모션 버퍼 (Step 12 추가)
 
 // ============ 티칭 팬던트 기본 함수 (Step 11까지) =============
 
 void teachCurrentPosition(void) {
	 
	 /* 배열 범위 초과 방지 */
	 if (g_teach_count >= MAX_TEACH_POINTS) {
		 printf("오류: 최대 저장 개수(%d)에 도달\n",MAX_TEACH_POINTS);
		 printf("-> CLEAR 또는 DELETE 로 공간 확보 요청\n");
		 return;
	 }
	 
	 // 현재 위치와 설정값을 구조체에 저장
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
	printf("===================================================\n");
 }
 
 void deleteTeachPoints(int index) {
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
	 
	 // 파일명이 NULL이면 기본 파일명 사용
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
	 printf("===== 티칭 포인트 연속 실행 완료 ======\n");
 }
		
	// ========== Step 12: 모션 버퍼 관리 함수 =============
 /*
  * 모션 버퍼 초기화
  * 프로그램 시작 시 한 번 호출
  */
 void initMotionBuffer(void) {
	g_motion_buffer.front = 0;
	g_motion_buffer.rear = 0;
	g_motion_buffer.count = 0;
	
	printf("모션 버퍼 초기화 완료 (크기: %d)\n", MOTION_BUFFER_SIZE);
 }
 
 /*
  * 버퍼에 포인트 추가 (Enqueue)
  * 환형 버퍼 방식: rear가 끝에 도달하면 다시 처음으로 
  */
  int addToMotionBuffer(TeachPoint point) {
	  
	  // 버퍼가 가득 찼는지 확인
	  if (g_motion_buffer.count >= MOTION_BUFFER_SIZE) {
		  printf("오류: 모션 버퍼가 가득 찼습니다. (%d/%d)\n",
					g_motion_buffer.count, MOTION_BUFFER_SIZE);
			return -1;
	  }
	  
	  // rear 위치에 포인트 저장
	  g_motion_buffer.buffer[g_motion_buffer.rear] = point;
	  
	  // rear 위치 증가 (환형 구조)
	  g_motion_buffer.rear = (g_motion_buffer.rear + 1) % MOTION_BUFFER_SIZE;
	  
	  // 개수 증가
	  g_motion_buffer.count++;
	  
	  return 0; // 성공
  }
  
  /*
   * 버퍼에서 포인트 꺼내기 (Dequeue)
   * 환형 버퍼 방식: front가 끝에 도달하면 다시 처음으로
   */
   int getFromMotionBuffer(TeachPoint *point) {
	   
	   // 버퍼가 비어있는지 확인
	   if (g_motion_buffer.count == 0) {
		   return -1;
	   }
	   
	   // front 위치의 포인트를 복사
	   *point = g_motion_buffer.buffer[g_motion_buffer.front];
	   
	   // front 위치 증가 (환형 구조)
	   g_motion_buffer.front = (g_motion_buffer.front + 1) % MOTION_BUFFER_SIZE;
	   
	   // 개수 감소
	   g_motion_buffer.count--;
	   
	   return 0;
   }
   
  /*
   * 버퍼 상태 출력
   * 실시간 모니터링 용
   */  
  void showBufferStatus(void) {
	  printf("\n===== 모션 버퍼 상태 ======\n");
	  printf("사용 중: %d/%d (%.1f%%)\n",
			g_motion_buffer.count,
			MOTION_BUFFER_SIZE,
			(float)g_motion_buffer.count / MOTION_BUFFER_SIZE * 100.0);
	  printf("여유 공간: %d\n", MOTION_BUFFER_SIZE - g_motion_buffer.count);
	  printf("front: %d, rear: %d\n",
				g_motion_buffer.front,
				g_motion_buffer.rear);
				
	  // 상태 판단
	  if (g_motion_buffer.count == 0) {
		  printf("상태: 비어 있음(empty)\n");
	  } else if (g_motion_buffer.count >= MOTION_BUFFER_SIZE * 0.9) {
		  printf("상태: 거의 가득참, 90%% 이상\n");
	  } else if (g_motion_buffer.count >= MOTION_BUFFER_SIZE * 0.5) {
		  printf("상태: 절반 이상 사용 중\n");
	  } else {
		  printf("상태: 정상, 50%% 미만\n");
	  }
	  printf("=========================\n");
  }
  
 /*
  * 버퍼 비우기
  * 긴급 정지 또는 초기화 시 사용자가
  */
  void clearMotionBuffer(void) {
	  g_motion_buffer.front = 0;
	  g_motion_buffer.rear = 0;
	  g_motion_buffer.count = 0;
	  
	  printf("모든 버퍼 비움 완료\n");
  }
  
 /*
  * 티칭 포인트 -> 모션 버퍼 전송
  * g_teach_points 배열의 모든 포인트를 모션 버퍼로 복사
  */
  void sendToMotionBuffer(void) {
	  
	  if (g_teach_count == 0) {
		  printf("전송할 티칭 포인트가 없습니다.\n");
		  printf("-> TEACH로 포인트를 저장하거나 LOAD로 파일을 불러오세요.\n");
		  return;
	  }
	  
	  printf("\n티칭 포인트-> 모션 버퍼 전송 중...\n");
	  
	  int sent_count = 0;
	  
	  for (int i = 0; i < g_teach_count; i++) {
		  
		  // 버퍼에 추가 시도
		  if (addToMotionBuffer(g_teach_points[i]) != 0) {
			  // 버퍼에 가득 참
			  printf("경고: %d번 포인트부터 버퍼 가득참\n", i);
			  printf("		전송 중단 (%d개만 전송됨)\n", sent_count);
			  break;
		  }
		  
		  sent_count++;
	  }
	  
	  printf("-> 전송 완료: %d개 포인트\n", sent_count);
	  printf(" 버퍼 사용량: %d/%d\n",
			g_motion_buffer.count, MOTION_BUFFER_SIZE);
  }
  
  /*
   * 모션 버퍼 실행
   * 버퍼에서 포인트를 하나씩 꺼내면서 모터 이동 실행
   */  
   void runFromMotionBuffer(void) {
	   
	   if (g_motion_buffer.count == 0) {
		   printf("모션 버퍼가 비어있습니다.\n");
		   printf("-> SEND로 포인트를 버퍼로 전송하세요.\n");
		   return;
	   }
  
  printf("\n ========== 모션 버퍼 실행 시작 ============ \n");
  printf("대기 중인 명령: %d개 \n\n", g_motion_buffer.count);
	  
  TeachPoint point;
  int executed = 0;
  
  // 버퍼가 빌 떄까지 계속
  while (getFromMotionBuffer(&point) == 0) { // while문 조건안에 함수를 넣으면, 함수가 실행 된다.!
											 // for문으로 써도 될거 같은데, while을 사용한 이유? 버퍼가 비워 질 때 까지 계속 반복해야 함으로
	  executed++;
	  
	  printf("[%d/%d] X=%.2f,Y=%.2f\n",
			   executed,
			   executed + g_motion_buffer.count,
			   point.x_mm,
			   point.y_mm);
	  printf("		속도=%.1f, 가속=%.1f\n",
				point.speed_mm_s,
				point.accel_mm_s2);
				
	 // X축 이동
	 moveAxisBySpeed(&x_axis,
					 point.x_mm,
					 point.speed_mm_s,
					 point.accel_mm_s2);
	 
	 // Y축 이동
	 moveAxisBySpeed(&y_axis,
					 point.y_mm,
					 point.speed_mm_s,
					 point.accel_mm_s2);	 
	  
	 printf("남은 명령: %d \n",g_motion_buffer.count);
  }
  
  printf("=====실행 완료 (%d개)=======\n",executed);
   }