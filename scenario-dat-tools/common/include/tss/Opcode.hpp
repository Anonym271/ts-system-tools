#pragma once
#include <iostream>

namespace tss
{
	enum class Opcode
	{
		META = 0x0003,
		SELECT = 0x0004,
		SPEAKER = 0x0007,
		TEXT = 0x0032,
		RETURN = 0x0045,
		BEGIN = 0x0048,
		SCENE = 0x005A,
		EVENT_CG = 0x0065,
		EVENT_CG_ = 0x0066,
		BG = 0x0067,
		X_0069 = 0x0069,
		X_006B = 0x006B,
		X_006C = 0x006C,
		X_006D = 0x006D,
		IF = 0x006E,
		SET = 0x0071,
		JUMP = 0x0072,
		CHOOSE = 0x0073,
		X_0075 = 0x0075,
		X_0076 = 0x0076,
		SOUND = 0x0077,
		X_0078 = 0x0078,
		CHAR_SPRITE = 0x0079,
		CHAR_HIDE = 0x007A,
		CHAR_SPRITE_ = 0x007B,
		CALL = 0x007C,
		X_0099 = 0x0099,
		X_009A = 0x009A,
		EXIT = 0x00A0,
		VOICE = 0x00A3,
		CHAR_MOVE = 0x00B4,
		X_00BB = 0x00BB,
		VIDEO = 0x00E7,
		SOUND_ = 0x00E9,
		X_00EA = 0x00EA,
		X_00EB = 0x00EB,
		X_00EC = 0x00EC,
		X_00ED = 0x00ED,
		X_00EF = 0x00EF,
		X_00F0 = 0x00F0,
		X_00F1 = 0x00F1,
		CAMERA = 0x00F5,
		X_00F6 = 0x00F6,
		X_00F7 = 0x00F7,
		X_00F8 = 0x00F8,
		DATE_A = 0x00FE,
		DATE_B = 0x00FF,
		DATE_C = 0x0101,
		X_0102 = 0x0102,
		X_0103 = 0x0103,
		X_0104 = 0x0104,
		X_0105 = 0x0105,
		X_0106 = 0x0106,
		X_0107 = 0x0107,
		X_0108 = 0x0108,
		X_0109 = 0x0109,
		X_010A = 0x010A,
		TITLE = 0x010B,
		X_010C = 0x010C,
	};

	void write_opcode(std::ostream& os, Opcode opcode);
	const char* opcode_str(Opcode opcode);
}