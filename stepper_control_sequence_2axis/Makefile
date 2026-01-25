# ========================================
# Makefile - G-code Runner 프로젝트
# ========================================

CC = gcc
CFLAGS = -Wall -O2
LIBS = -lwiringPi -lm

# 디렉토리
SRC_DIR = .
BUILD_DIR = build

# 소스 파일들
SOURCES = \
	app/app_gcode_runner.c \
	parser/gcode_parser.c \
	motion/motion_executor_xy.c \
	motion/motion_state_xy.c \
	motion/motion_planner_xy.c \
	motion/calibration_xy.c \
	drivers/stepper_driver_xy.c \
	utils/time_utils.c

# 오브젝트 파일들
OBJECTS = $(SOURCES:.c=.o)

# 실행 파일
TARGET = app_gcode_runner

# ========================================
# 빌드 규칙
# ========================================

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "링킹: $@"
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo "빌드 완료!"

%.o: %.c
	@echo "컴파일: $<"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "정리 중..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "정리 완료!"

# ========================================
# 실행 테스트
# ========================================

test-square: $(TARGET)
	@echo "사각형 테스트 실행..."
	sudo ./$(TARGET) test_square.gcode

test-star: $(TARGET)
	@echo "별 모양 테스트 실행..."
	sudo ./$(TARGET) test_star.gcode

test-comp: $(TARGET)
	@echo "종합 테스트 실행..."
	sudo ./$(TARGET) test_comprehensive.gcode

.PHONY: all clean test-square test-star test-comp
