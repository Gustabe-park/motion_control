/*
 * step8_coordinate_system.c
 * 좌표 변환 시스템 - mm 단위 2축 제어
 * 
 * 하드웨어 구성:
 * X축: GPIO 2(ENABLE), 3(DIR), 4(STEP)
 * Y축: GPIO 22(ENABLE), 27(DIR), 17(STEP)
 * 
 * 컴파일: gcc -o step8 step8_coordinate_system.c -lwiringPi -lm
 * 실행: sudo ./step8
 */

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

// ========== 모터 사양 설정 (실제 값으로 수정 필요!) ==========
#define STEPS_PER_REV 200           // Full step 기준
#define MICROSTEPS 8                // 드라이버 마이크로스테핑 설정
#define MM_PER_REV 5.0              // 1회전당 이동거리 (mm)

// ========== 계산값 ==========
#define STEPS_PER_MM ((STEPS_PER_REV * MICROSTEPS) / MM_PER_REV)

// ========== 작업 영역 설정 ==========
#define X_MIN 0.0
#define X_MAX 100.0
#define Y_MIN 0.0
#define Y_MAX 100.0

// ========== GPIO 핀 정의 ==========
// X축
#define X_ENABLE 2
#define X_DIR 3
#define X_STEP 4

// Y축
#define Y_STEP 17
#define Y_DIR 27
#define Y_ENABLE 22

// ========== 타이밍 설정 ==========
#define DEFAULT_DELAY 500           // 기본 펄스 간격 (us)
#define PULSE_WIDTH 5               // 펄스 폭 (us)

// ========== 구조체 정의 ==========

// mm 단위 좌표
typedef struct {
    double x;
    double y;
} Coordinate;

// steps 단위 위치
typedef struct {
    long x;
    long y;
} StepPosition;

// 모터 축 정보
typedef struct {
    int enable_pin;
    int dir_pin;
    int step_pin;
    long current_steps;     // 현재 위치 (steps)
    double current_mm;      // 현재 위치 (mm)
} Axis;

// ========== 전역 변수 ==========
Axis x_axis, y_axis;
int current_delay = DEFAULT_DELAY;

// ========== 좌표 변환 함수 ==========

// mm를 steps로 변환
long mm_to_steps(double mm) {
    return (long)round(mm * STEPS_PER_MM);
}

// steps를 mm로 변환
double steps_to_mm(long steps) {
    return (double)steps / STEPS_PER_MM;
}

// 좌표 유효성 검증
bool is_coordinate_valid(Coordinate coord) {
    if (coord.x < X_MIN || coord.x > X_MAX) {
        printf("오류: X 좌표 범위 초과 (%.2f mm, 허용: %.2f~%.2f)\n", 
               coord.x, X_MIN, X_MAX);
        return false;
    }
    if (coord.y < Y_MIN || coord.y > Y_MAX) {
        printf("오류: Y 좌표 범위 초과 (%.2f mm, 허용: %.2f~%.2f)\n", 
               coord.y, Y_MIN, Y_MAX);
        return false;
    }
    return true;
}

// ========== 모터 제어 함수 ==========

// 축 초기화
void init_axis(Axis *axis, int enable_pin, int dir_pin, int step_pin) {
    axis->enable_pin = enable_pin;
    axis->dir_pin = dir_pin;
    axis->step_pin = step_pin;
    axis->current_steps = 0;
    axis->current_mm = 0.0;
    
    pinMode(enable_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);
    pinMode(step_pin, OUTPUT);
    
    digitalWrite(enable_pin, HIGH);  // 비활성화
    digitalWrite(dir_pin, LOW);
    digitalWrite(step_pin, LOW);
}

// 축 활성화/비활성화
void set_axis_enable(Axis *axis, bool enable) {
    digitalWrite(axis->enable_pin, enable ? LOW : HIGH);
}

// 단일 축 이동 (상대 이동, steps 단위)
void move_axis_steps(Axis *axis, long steps, int delay_us) {
    if (steps == 0) return;
    
    // 방향 설정
    bool direction = (steps > 0);
    digitalWrite(axis->dir_pin, direction ? HIGH : LOW);
    delayMicroseconds(10);  // 방향 설정 후 안정화
    
    // 스텝 수행
    long abs_steps = labs(steps);
    for (long i = 0; i < abs_steps; i++) {
        digitalWrite(axis->step_pin, HIGH);
        delayMicroseconds(PULSE_WIDTH);
        digitalWrite(axis->step_pin, LOW);
        delayMicroseconds(delay_us);
    }
    
    // 위치 업데이트
    axis->current_steps += steps;
    axis->current_mm = steps_to_mm(axis->current_steps);
}

// 2축 동시 이동 (선형 보간 간단 버전)
void move_2axis_steps(long x_steps, long y_steps, int delay_us) {
    if (x_steps == 0 && y_steps == 0) return;
    
    // 방향 설정
    if (x_steps != 0) {
        digitalWrite(x_axis.dir_pin, (x_steps > 0) ? HIGH : LOW);
    }
    if (y_steps != 0) {
        digitalWrite(y_axis.dir_pin, (y_steps > 0) ? HIGH : LOW);
    }
    delayMicroseconds(10);
    
    // 절대값
    long abs_x = labs(x_steps);
    long abs_y = labs(y_steps);
    long max_steps = (abs_x > abs_y) ? abs_x : abs_y;
    
    // 간단한 선형 보간
    for (long i = 0; i < max_steps; i++) {
        bool step_x = (abs_x > 0) && ((i * abs_x) / max_steps > ((i-1) * abs_x) / max_steps);
        bool step_y = (abs_y > 0) && ((i * abs_y) / max_steps > ((i-1) * abs_y) / max_steps);
        
        if (step_x) digitalWrite(x_axis.step_pin, HIGH);
        if (step_y) digitalWrite(y_axis.step_pin, HIGH);
        
        delayMicroseconds(PULSE_WIDTH);
        
        if (step_x) digitalWrite(x_axis.step_pin, LOW);
        if (step_y) digitalWrite(y_axis.step_pin, LOW);
        
        delayMicroseconds(delay_us);
    }
    
    // 위치 업데이트
    x_axis.current_steps += x_steps;
    x_axis.current_mm = steps_to_mm(x_axis.current_steps);
    y_axis.current_steps += y_steps;
    y_axis.current_mm = steps_to_mm(y_axis.current_steps);
}

// ========== 좌표 기반 이동 함수 ==========

// 절대 좌표로 이동 (mm 단위)
void move_to_coordinate(Coordinate target) {
    // 유효성 검증
    if (!is_coordinate_valid(target)) {
        return;
    }
    
    // 현재 위치
    printf("현재 위치: (%.2f, %.2f) mm\n", x_axis.current_mm, y_axis.current_mm);
    printf("목표 위치: (%.2f, %.2f) mm\n", target.x, target.y);
    
    // 목표 steps 계산
    long target_x_steps = mm_to_steps(target.x);
    long target_y_steps = mm_to_steps(target.y);
    
    // 이동 거리 계산
    long delta_x = target_x_steps - x_axis.current_steps;
    long delta_y = target_y_steps - y_axis.current_steps;
    
    printf("이동 거리: X=%.2fmm(%ld steps), Y=%.2fmm(%ld steps)\n",
           steps_to_mm(delta_x), delta_x,
           steps_to_mm(delta_y), delta_y);
    
    // 이동 수행
    printf("이동 중...\n");
    move_2axis_steps(delta_x, delta_y, current_delay);
    
    printf("이동 완료: (%.2f, %.2f) mm\n\n", x_axis.current_mm, y_axis.current_mm);
}

// 상대 좌표로 이동 (mm 단위)
void move_relative(double dx, double dy) {
    Coordinate target;
    target.x = x_axis.current_mm + dx;
    target.y = y_axis.current_mm + dy;
    
    move_to_coordinate(target);
}

// 호밍 (원점 복귀)
void homing() {
    printf("=== 호밍 시작 ===\n");
    
    Coordinate home = {0.0, 0.0};
    move_to_coordinate(home);
    
    // 위치 리셋
    x_axis.current_steps = 0;
    x_axis.current_mm = 0.0;
    y_axis.current_steps = 0;
    y_axis.current_mm = 0.0;
    
    printf("호밍 완료\n\n");
}

// ========== 사용자 인터페이스 ==========

void print_menu() {
    printf("\n========== 좌표 변환 시스템 ==========\n");
    printf("모터 설정: %.2f steps/mm\n", STEPS_PER_MM);
    printf("작업 영역: X(%.0f~%.0f mm), Y(%.0f~%.0f mm)\n", 
           X_MIN, X_MAX, Y_MIN, Y_MAX);
    printf("현재 위치: (%.2f, %.2f) mm\n", x_axis.current_mm, y_axis.current_mm);
    printf("현재 속도: %d us/step\n", current_delay);
    printf("=====================================\n");
    printf("G - 절대 좌표 이동 (Go to)\n");
    printf("M - 상대 좌표 이동 (Move)\n");
    printf("H - 호밍 (Home)\n");
    printf("E - 모터 활성화/비활성화 (Enable)\n");
    printf("S - 속도 설정 (Speed)\n");
    printf("P - 현재 위치 표시 (Position)\n");
    printf("T - 변환 테스트 (Test)\n");
    printf("Q - 종료 (Quit)\n");
    printf("=====================================\n");
    printf("명령 입력: ");
}

void test_conversion() {
    printf("\n=== 좌표 변환 테스트 ===\n");
    
    double test_mm_values[] = {0.0, 10.0, 50.0, 100.0, 25.5};
    int num_tests = sizeof(test_mm_values) / sizeof(test_mm_values[0]);
    
    printf("Steps/mm: %.2f\n\n", STEPS_PER_MM);
    
    for (int i = 0; i < num_tests; i++) {
        double mm = test_mm_values[i];
        long steps = mm_to_steps(mm);
        double mm_back = steps_to_mm(steps);
        double error = fabs(mm - mm_back);
        
        printf("%.2f mm → %ld steps → %.4f mm (오차: %.6f mm)\n",
               mm, steps, mm_back, error);
    }
    printf("\n");
}

void show_position() {
    printf("\n=== 현재 위치 ===\n");
    printf("X: %.2f mm (%ld steps)\n", x_axis.current_mm, x_axis.current_steps);
    printf("Y: %.2f mm (%ld steps)\n", y_axis.current_mm, y_axis.current_steps);
    printf("\n");
}

// ========== 메인 함수 ==========

int main() {
    printf("Step 8: 좌표 변환 시스템\n\n");
    
    // wiringPi 초기화
    if (wiringPiSetupGpio() == -1) {
        printf("wiringPi 초기화 실패!\n");
        return 1;
    }
    
    // 축 초기화
    init_axis(&x_axis, X_ENABLE, X_DIR, X_STEP);
    init_axis(&y_axis, Y_ENABLE, Y_DIR, Y_STEP);
    
    printf("모터 초기화 완료\n");
    printf("현재 설정: %.2f steps/mm\n", STEPS_PER_MM);
    printf("(모터 사양에 맞게 코드 상단의 설정값을 수정하세요)\n");
    
    char command;
    bool motors_enabled = false;
    
    while (1) {
        print_menu();
        
        if (scanf(" %c", &command) != 1) {
            while (getchar() != '\n');  // 입력 버퍼 클리어
            continue;
        }
        
        command = toupper(command);
        
        switch (command) {
            case 'G': {  // 절대 좌표 이동
                double x, y;
                printf("목표 좌표 입력 (X Y): ");
                if (scanf("%lf %lf", &x, &y) == 2) {
                    Coordinate target = {x, y};
                    move_to_coordinate(target);
                } else {
                    printf("잘못된 입력입니다.\n");
                    while (getchar() != '\n');
                }
                break;
            }
            
            case 'M': {  // 상대 좌표 이동
                double dx, dy;
                printf("이동 거리 입력 (dX dY): ");
                if (scanf("%lf %lf", &dx, &dy) == 2) {
                    move_relative(dx, dy);
                } else {
                    printf("잘못된 입력입니다.\n");
                    while (getchar() != '\n');
                }
                break;
            }
            
            case 'H':  // 호밍
                homing();
                break;
            
            case 'E': {  // 모터 활성화/비활성화
                motors_enabled = !motors_enabled;
                set_axis_enable(&x_axis, motors_enabled);
                set_axis_enable(&y_axis, motors_enabled);
                printf("모터 %s\n\n", motors_enabled ? "활성화" : "비활성화");
                break;
            }
            
            case 'S': {  // 속도 설정
                int new_delay;
                printf("펄스 간격 입력 (us, 현재=%d): ", current_delay);
                if (scanf("%d", &new_delay) == 1 && new_delay > 0) {
                    current_delay = new_delay;
                    printf("속도 변경 완료: %d us\n\n", current_delay);
                } else {
                    printf("잘못된 입력입니다.\n");
                    while (getchar() != '\n');
                }
                break;
            }
            
            case 'P':  // 위치 표시
                show_position();
                break;
            
            case 'T':  // 변환 테스트
                test_conversion();
                break;
            
            case 'Q':  // 종료
                printf("\n프로그램을 종료합니다.\n");
                set_axis_enable(&x_axis, false);
                set_axis_enable(&y_axis, false);
                return 0;
            
            default:
                printf("알 수 없는 명령입니다.\n\n");
                break;
        }
    }
    
    return 0;
}