# Makefile
CC = gcc
CFLAGS = -Wall -g
LIBS = -lwiringPi -lm

# 타겟: step7 실행 파일
TARGET = step7

# 소스 파일들
SRCS = main.c accel.c command.c teaching.c

# 오브젝트 파일들 (.c를 .o로 변경)
OBJS = $(SRCS:.c=.o)

# 기본 타겟
all: $(TARGET)

# 실행 파일 생성
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

# .c 파일을 .o 파일로 컴파일
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 클린업
clean:
	rm -f $(OBJS) $(TARGET)

# 재컴파일
rebuild: clean all

.PHONY: all clean rebuild