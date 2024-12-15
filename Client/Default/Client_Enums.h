#pragma once

namespace Client
{
	enum ANITYPE
	{
		ANI_STOP
		, ANI_LOOP
		, ANI_BACKTOIDLE
		, ANI_CHAIN
		, ANI_BACKTOANOTHER

		, ANI_END
	};

	enum OBJ_STATE // b 붙어있는건 뒷걸음질치면서 하는 모션 or 같은행동인데 모션만다른거
	{
		//S = START, L = LOOP, E = END (리소스에도 이니셜 붙어있음)
		OBJSTATE_IDLE
		, OBJSTATE_ATT1
		, OBJSTATE_ATT2
		, OBJSTATE_ATT3
		, OBJSTATE_ATT4
		, OBJSTATE_ATT5
		, OBJSTATE_ATT6

		, OBJSTATE_SKILL1
		, OBJSTATE_SKILL2
		, OBJSTATE_SKILL3
		, OBJSTATE_SKILL4
		, OBJSTATE_SKILL5
		, OBJSTATE_SKILL6
		, OBJSTATE_SKILL7
		, OBJSTATE_SKILL8


		, OBJSTATE_STOP

		, OBJSTATE_RUSH

		, OBJSTATE_BACKROLL
		, OBJSTATE_WEAPON_DRAW // 전투준비
		, OBJSTATE_WEAPON_UNDRAW //전투해제
		, OBJSTATE_SH_DRAW // 검방들기
		, OBJSTATE_SH_UNDRAW // 검방내리기
		, OBJSTATE_ROLL
		, OBJSTATE_ROLL_B
		, OBJSTATE_RIDEROLL
		, OBJSTATE_RUN
		, OBJSTATE_WALK
		, OBJSTATE_WALK_B
		, OBJSTATE_RUN_B
		, OBJSTATE_RUN_S
		, OBJSTATE_RUN_TURN90R
		, OBJSTATE_RUN_TURN180R
		, OBJSTATE_RUN_TURN90L
		, OBJSTATE_RUN_TURN180L
		, OBJSTATE_FALL

		, OBJSTATE_THROW
		, OBJSTATE_TUMBLING // 끼얏호우
		, OBJSTATE_TUMBLEB	// 자빠짐
		, OBJSTATE_TUMBLEF
		, OBJSTATE_TUMBLEE
		, OBJSTATE_TUMBLEE_F // 엎어진거 일어남
		, OBJSTATE_TUMBLEL // 옆에 맞고 엎어짐
		, OBJSTATE_TUMBLER

		, OBJSTATE_TEST1
		, OBJSTATE_TEST2

		, OBJSTATE_END
	};

	enum KeyType
	{
		ESC = 0x01
		, Key1 = 0x02
		, Key2 = 0x03
		, Key3 = 0x04
		, Key4 = 0x05
		, Key5 = 0x06
		, Key6 = 0x07
		, Key7 = 0x08
		, Key8 = 0x09
		, Key9 = 0x0A
		, Key0 = 0x0B
		, BACKSPACE = 0x0E
		, TAB = 0x0F
		, Q = 0x10
		, W = 0x11
		, E = 0x12
		, R = 0x13
		, T = 0x14
		, Y = 0x15
		, U = 0x16
		, I = 0x17
		, O = 0x18
		, P = 0x19
		, ENTER = 0x1C
		, LCTRL = 0x1D
		, A = 0x1E
		, S = 0x1F
		, D = 0x20
		, F = 0x21
		, G = 0x22
		, LSHIFT = 0x2A
		, Z = 0x2C
		, X = 0x2D
		, C = 0x2E
		, V = 0x2F
		, SPACE = 0x39
		, F1 = 0x3B
		, F2 = 0x3C
		, F3 = 0x3D
		, F4 = 0x3E
		, F5 = 0x3F
		, UP = 0xC8
		, LEFT = 0xCB
		, RIGHT = 0xCD
		, DOWN = 0xD0

		, Key_End
	};
}