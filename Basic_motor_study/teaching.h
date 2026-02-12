/*
 * teaching.h
 * 티칭 팬던트 기능 - 좌표 저장, 파일 저장/불러오기, 연속 실행
 * ======== 이 모듈의 기능 ==========
 * 1. TEACH 	: 현재 X,Y 좌표를 메모리에 저장 (포인트 추가)
 * 2. LIST		: 저장된 포인트 목록 출력
 * 3. DELETE	: 특정 포인트 삭제
 * 4. CLEAR		: 저장된 포인트 전부 삭제
 * 5. SAVE		: 메모리의 포인트들을 텍스트 파일로 저장/불러오기
 * 6. LOAD		: 텍스트 파일에서 포인트들을 불러오기
 * 7. RUN		: 저장된 포인트들을 순서대로 연속 실행
 */
 
 #ifndef TEACH_H
 #define TEACH_H
 
 /* =========== 상수 정의 ============ */
 //저장할 수 있는 최대 좌표 개수, 
 //배열 크기를 미리 정해두는 정적 배열 방식 사용
 #define MAX_TEACH_POINTS 100

 //기본 저장 파일명
 //SAVE/LOAD 명령어에서 파일명을 생략하면 이 이름을 사용
 #define DEFAULT_TEACH_FILE "teach_points.txt"
 
 /* ========== 구조체 정의 ============ */
 
 /*
  *TeachPoint: 하나의 티칭 포인트를 나타내는 구조체
  *
  *각 포인트에는 X좌표, Y좌표 그리고 해당 포인트의 속도와 가속도 설정값이 함께 저장됨
  *
  *예시:
  *		포인트 1: X=10.0, Y=20.0, 속도=50, 가속도=200	(느리게 접근)
  *		포인트 2: X=50.0, Y=20.0, 속도=100, 가속도=500	(빠르게 접근)
  */
  
 typedef struct {
	 float x_mm;
	 float y_mm;
	 float speed_mm_s;
	 float accel_mm_s2;
 } TeachPoint;
 
 /*============= 전역 변수 (extern 선언) =================*/
 
 // g_teach_points[]: 티칭 포인트를 저장하는 배열
 // 최대 MAX TEACH POINTS개의 포인트를 저장할  수 있다.
 // 인덱스 0부터 순서대로 채워진다.
 
 extern TeachPoint g_teach_points[MAX_TEACH_POINTS];
 extern int g_teach_count;
 
 /*============= 함수 선언 =================*/
 
 // 현재 모터의 X,Y 위치를 티칭 포인트로 저장, 속도 및 가속도도 저장
 // 배열이 가득 차면 경고 메세지를 출력하고 저장하지 않음
 void teachCurrentPosition(void);
 
 // 저장된 모든 티칭 포인트를 번호와 함께 화면에 출력
 void listTeachPoints(void);
 
 // 지정한 번호의 티칭 포인트를 삭제
 // 삭제 후 뒤에 있는 포인트들이 앞으로 당겨짐
 void deleteTeachPoint(int index);
 
 // 저장된 모든 티칭 포인트를 삭제
 void clearTeachPoints(void);
 
 // 메모리에 저장된 티칭 포인트들을 텍스트 파일로 저장/불러오기
 int saveTeachPoints(const char *filename);
 
 // 텍스트 파일에서 티칭 포인트들을 읽어와 메모리에 저장/불러오기
 int loadTeachPoints(const char *filename);
 
 // 저장된 포인트들을 0번부터 순서대로 연속 실행
 void runTeachPoints(void);
 
 #endif // TEACHING_H