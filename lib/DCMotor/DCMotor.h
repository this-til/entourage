// DCMotor.h

#ifndef _DCMOTOR_h
#define _DCMOTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define TRACK_ADDR		0x6000

#define	R_F_M_PIN		8		/*控制 右 前 边电机的引脚，输出PWM*/
#define	R_B_M_PIN		7		/*控制 右 后 边电机的引脚，输出PWM*/

#define	L_F_M_PIN		3		/*控制 左 前 边电机的引脚，输出PWM*/
#define	L_B_M_PIN		2		/*控制 左 后 边电机的引脚，输出PWM*/

#define R_CONTROL_PIN	5
#define L_CONTROL_PIN	6

#define DEFAULT_SPEED	50

#define MOVE_RUNMODE_STOP	0
#define MOVE_RUNMODE_FF		1
#define MOVE_RUNMODE_BB		2
#define MOVE_RUNMODE_LIFT	3
#define MOVE_RUNMODE_RIGHT	4
#define MOVE_RUNMODE_TRACK	5

class _DCMotor
{
public:
	_DCMotor();
	~_DCMotor();

	void Initialization(uint32_t fHz = 30000);
	void Go(uint8_t speed);
	uint16_t Go(uint8_t speed, uint16_t distance);
	void Back(uint8_t speed);
	uint16_t Back(uint8_t speed, uint16_t distance);
	void TurnLeft(int8_t Lspeed, int8_t Rspeed);
	void TurnLeft(int8_t speed);
	void TurnRight(int8_t Lspeed, int8_t Rspeed);
	void TurnRight(int8_t speed);
	void Stop(void);
	void StartUp(void);
	void CarTrack(uint8_t Car_Spend);
	uint16_t _DCMotor::TurnRight_Code(uint8_t speed, uint16_t _distance);
	uint16_t _DCMotor::TurnLeft_Code(uint8_t speed, uint16_t _distance);
	void TurnLeftToRight(void);
	void TurnRightToLeft(void);

	void MotorSpeed(uint8_t runmode, int8_t l_speed, int8_t r_speed);
	void SpeedSetOne(int16_t s, uint16_t *c1, uint16_t *c2);
	void SpeedCtr(int16_t L_speed, int16_t R_speed);
	void ParameterInit(void);
	bool ClearCodeDisc(void);
	uint8_t SearchBit(uint8_t mode, uint8_t s);

private:
	boolean track_flag;
	void MotorFrequency(uint32_t _fHz);
	uint32_t fHz;
	uint8_t  RFSpeed, RBSpeed;

	uint32_t LfHz;
	uint8_t  LFSpeed, LBSpeed;

	uint8_t ShiftBitAdd(uint8_t);
	uint8_t JudgeAround(uint8_t);
	

	uint16_t speed[101];
	
};

extern _DCMotor DCMotor;

#endif

