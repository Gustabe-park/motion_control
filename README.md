stepper_refactored/
│
├── config/ (설정 파일)
│ ├── hardware_config.h - 하드웨어 설정 (GPIO, 모터 사양)
│ └── motion_config.h - 모션 파라미터 (속도, 제한값)
│
├── include/ (공통 헤더)
│ └── common_types.h - 공통 타입 정의
│
├── drivers/ (하드웨어 드라이버)
│ ├── stepper_driver.h
│ └── stepper_driver.c - GPIO 제어
│
├── motion/ (모션 제어 - 세분화!)
│ ├── calibration.h/c - Step 캘리브레이션, 위치 추적
│ ├── motion_state.h/c - 상태 관리
│ ├── motion_planner.h/c - 경로 계획, 가감속 계산
│ └── motion_executor.h/c - 모션 실행, update()
│
├── utils/ (유틸리티)
│ ├── time_utils.h
│ └── time_utils.c - 시간 관련 함수
│
├── app/ (응용 프로그램)
│ └── app_sequence.c - Eric의 시퀀스 제어
│
├── Makefile - 빌드 자동화
└── README.md - 이 파일
