# Makefile for Stepper Control (Refactored)

CC = gcc
CFLAGS = -Wall -O2 -I.
LDFLAGS = -lwiringPi -lm -lrt

# 디렉토리
DRIVER_DIR = drivers
MOTION_DIR = motion
UTILS_DIR = utils
APP_DIR = app

# 소스 파일
DRIVER_SRC = $(DRIVER_DIR)/stepper_driver.c
MOTION_SRC = $(MOTION_DIR)/calibration.c \
             $(MOTION_DIR)/motion_state.c \
             $(MOTION_DIR)/motion_planner.c \
             $(MOTION_DIR)/motion_executor.c
UTILS_SRC = $(UTILS_DIR)/time_utils.c
APP_SRC = $(APP_DIR)/app_sequence.c

# 오브젝트 파일
DRIVER_OBJ = $(DRIVER_SRC:.c=.o)
MOTION_OBJ = $(MOTION_SRC:.c=.o)
UTILS_OBJ = $(UTILS_SRC:.c=.o)
APP_OBJ = $(APP_SRC:.c=.o)

ALL_OBJ = $(DRIVER_OBJ) $(MOTION_OBJ) $(UTILS_OBJ)

# 타겟
TARGET = app_sequence

# 기본 타겟
all: $(TARGET)

# 실행 파일 생성
$(TARGET): $(ALL_OBJ) $(APP_OBJ)
	@echo "링킹: $(TARGET)"
	$(CC) $(ALL_OBJ) $(APP_OBJ) -o $(TARGET) $(LDFLAGS)
	@echo "빌드 완료!"

# .c -> .o 컴파일
%.o: %.c
	@echo "컴파일: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# 정리
clean:
	@echo "정리 중..."
	rm -f $(DRIVER_OBJ) $(MOTION_OBJ) $(UTILS_OBJ) $(APP_OBJ) $(TARGET)
	@echo "정리 완료!"

# 실행
run: $(TARGET)
	@echo "실행 중..."
	sudo ./$(TARGET)

# 도움말
help:
	@echo "사용법:"
	@echo "  make        - 빌드"
	@echo "  make clean  - 정리"
	@echo "  make run    - 실행"
	@echo "  make help   - 도움말"

.PHONY: all clean run help