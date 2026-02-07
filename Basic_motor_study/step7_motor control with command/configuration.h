/*
 *
 *configuration.h
 *기계 설정
 */ 
 #ifndef CONFIGURATION_H
 #define CONFIGURATION_H
 
 //핀 번호 설정 ======================
 // X축
 #define X_STEP				4
 #define X_DIR				3
 #define X_ENABLE			2
 
 // Y축
 #define Y_ENABLE			22
 #define Y_DIR				27
 #define Y_STEP				17 
 
 //기계적 설정=========================
 //#define MM_PER_REV			10.0
 //#define STEPS_PER_MM		(STEPS_PER_REV / MM_PER_REV)
 
 #endif //CONFIGURATION_H