/*
 * teaching.h
 * 티칭 펜던트 기능 - 좌표 저장, 파일 저장/불러오기, 연속 실행
 * Step 12: 모션 버퍼 및 큐 관리 추가
 * 
 * ======== 이 모듈의 기능 ==========
 * 1. TEACH 	: 현재 X,Y 좌표를 메모리에 저장 (포인트 추가)
 * 2. LIST		: 저장된 포인트 목록 출력
 * 3. DELETE	: 특정 포인트 삭제
 * 4. CLEAR		: 저장된 포인트 전부 삭제
 * 5. SAVE		: 메모리의 포인트들을 텍스트 파일로 저장
 * 6. LOAD		: 텍스트 파일에서 포인트들을 불러오기
 * 7. RUN		: 저장된 포인트들을 순서대로 연속 실행
 * 
 * ======== Step 12 추가 기능 ==========
 * 8. SEND		: 티칭 포인트 → 모션 버퍼로 전송
 * 9. EXEC		: 모션 버퍼에서 꺼내면서 실행
 * 10. BUFFER	: 버퍼 상태 확인
 * 11. FLUSH	: 버퍼 비우기
 */
 
 #ifndef TEACHING_H
 #define TEACHING_H
  
 /* =========== 상수 정의 ============ */
 
 // 티칭 포인트 배열 크기
 #define MAX_TEACH_POINTS 100
 
 // 모션 버퍼 크기 (환형 버퍼)
 #define MOTION_BUFFER_SIZE 50
 
 // 기본 저장 파일명
 #define DEFAULT_TEACH_FILE "teach_points.txt"
 
 /* ========== 구조체 정의 ============ */
 
 /*
  * TeachPoint: 하나의 티칭 포인트를 나타내는 구조체
  *
  * 각 포인트에는 X좌표, Y좌표 그리고 해당 포인트의 속도와 가속도 설정값이 함께 저장됨
  *
  * 예시:
  *		포인트 1: X=10.0, Y=20.0, 속도=50, 가속도=200	(느리게 접근)
  *		포인트 2: X=50.0, Y=20.0, 속도=100, 가속도=500	(빠르게 접근)
  */
  
  typedef struct {
	  float x_mm;
	  float y_mm;
	  float speed_mm_s;
	  float accel_mm_s2;
  } TeachPoint;
  
 /*
  * MotionBuffer: 환형 버퍼 (Circular Buffer)
  * 
  * 실시간으로 명령을 추가하고 실행하기 위한 큐 구조
  * 
  * 동작 원리:
  *   - front: 다음에 실행할 명령의 위치
  *   - rear: 다음에 추가할 명령의 위치  
  *   - count: 현재 버퍼에 있는 명령 개수
  * 
  * 환형 구조:
  *   인덱스가 끝(49)에 도달하면 다시 처음(0)으로 돌아감
  *   (rear + 1) % MOTION_BUFFER_SIZE 연산으로 구현
  */
 typedef struct {
	TeachPoint buffer[MOTION_BUFFER_SIZE]; // 50개 포인트 저장 공간
	int front; 	// 다음 실행 위치(dequeue)
	int rear; 	// 다음 추가 위치(enqueue)
	int count;	// 현재 저장된 개수
 } MotionBuffer;
  
 /* ============= 전역 변수 (extern 선언) ================= */
 
 // 티칭 포인트 배열
 extern TeachPoint g_teach_points[MAX_TEACH_POINTS];
 extern int g_teach_count;
 
 // 모션 버퍼 (Step 12 추가)
 extern MotionBuffer g_motion_buffer;
 
 /* ============= 티칭 펜던트 기본 함수 ================= */
 
 // 현재 모터의 X,Y 위치를 티칭 포인트로 저장
 void teachCurrentPosition(void);

 // 저장된 모든 티칭 포인트를 번호와 함께 화면에 출력
 void listTeachPoints(void);
 
 // 지정한 번호의 티칭 포인트를 삭제
 void deleteTeachPoints(int index);

 // 저장된 모든 티칭 포인트들을 삭제
 void clearTeachPoints(void); 
 
 // 메모리에 저장된 티칭 포인트들을 텍스트 파일로 저장
 int saveTeachPoints(const char *filename);
 
 // 텍스트 파일에서 티칭 포인트들을 읽어와 메모리에 저장
 int loadTeachPoints(const char *filename);
 
 // 저장된 포인트들을 0번부터 순서대로 연속 실행하기
 void runTeachPoints(void);
 
 /* ============= Step 12: 모션 버퍼 관리 함수 ================= */
 
 /*
  * 모션 버퍼 초기화
  * - front, rear, count를 모두 0으로 설정
  * - 프로그램 시작 시 한 번 호출
  */
 void initMotionBuffer(void);

 /*
  * 버퍼에 포인트 추가 (Enqueue)
  * - 버퍼가 가득 차면 -1 반환
  * - 성공하면 0 반환
  */ 
  int addToMotionBuffer(TeachPoint point);
  
 /*
  * 버퍼에서 포인트 꺼내기 (Dequeue)
  * - 버퍼가 비어있으면 -1 반환
  * - 성공하면 0 반환, point에 데이터 복사
  */
 int getFromMotionBuffer(TeachPoint *point);  
 
  /*
  * 버퍼 상태 확인
  * - 현재 사용량, 여유 공간, front/rear 위치 출력
  */
  void showBufferStatus(void);
 
 /*
  * 버퍼 비우기
  * - front, rear, count를 모두 0으로 리셋
  */ 
  void clearMotionBuffer(void);
 
 /*
  * 티칭 포인트 → 모션 버퍼 전송
  * - g_teach_points 배열의 모든 포인트를 모션 버퍼로 복사
  * - 버퍼가 가득 차면 중단
  */
 void sendToMotionBuffer(void);

 /*
  * 모션 버퍼 실행
  * - 버퍼에서 포인트를 하나씩 꺼내면서 모터 이동 실행
  * - 버퍼가 빌 때까지 계속
  */
 void runFromMotionBuffer(void);

 #endif //TEACHING_H 