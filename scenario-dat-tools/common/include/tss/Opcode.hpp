#include <string>
#include <iostream>
#include <exception>
#include <iomanip>

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
		JUMP = 0x005A,
		EVENT_CG = 0x0065,
		EVENT_CG_ = 0x0066,
		BG = 0x0067,
		X_0069 = 0x0069,
		X_006B = 0x006B,
		X_006C = 0x006C,
		X_006D = 0x006D,
		IF = 0x006E,
		SET = 0x0071,
		X_0072 = 0x0072,
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

	void write_opcode(std::ostream& os, Opcode opcode)
	{
		os << std::setw(16);
		switch (opcode)
		{
		case Opcode::META:
			os << "meta"; break;
		case Opcode::SELECT:
			os << "select"; break;
		case Opcode::SPEAKER:
			os << "speaker"; break;
		case Opcode::TEXT:
			os << "text"; break;
		case Opcode::RETURN:
			os << "return"; break;
		case Opcode::BEGIN:
			os << "begin"; break;
		case Opcode::JUMP:
			os << "jump"; break;
		case Opcode::EVENT_CG:
			os << "event_cg"; break;
		case Opcode::EVENT_CG_:
			os << "event_cg_"; break;
		case Opcode::BG:
			os << "bg"; break;
		case Opcode::X_0069:
			os << "0069"; break;
		case Opcode::X_006B:
			os << "006B"; break;
		case Opcode::X_006C:
			os << "006C"; break;
		case Opcode::X_006D:
			os << "006D"; break;
		case Opcode::IF:
			os << "if"; break;
		case Opcode::SET:
			os << "set"; break;
		case Opcode::X_0072:
			os << "0072"; break;
		case Opcode::CHOOSE:
			os << "choose"; break;
		case Opcode::X_0075:
			os << "0075"; break;
		case Opcode::X_0076:
			os << "0076"; break;
		case Opcode::SOUND:
			os << "sound"; break;
		case Opcode::X_0078:
			os << "0078"; break;
		case Opcode::CHAR_SPRITE:
			os << "char_sprite"; break;
		case Opcode::CHAR_HIDE:
			os << "char_hide"; break;
		case Opcode::CHAR_SPRITE_:
			os << "char_sprite_"; break;
		case Opcode::CALL:
			os << "call"; break;
		case Opcode::X_0099:
			os << "0099"; break;
		case Opcode::X_009A:
			os << "009A"; break;
		case Opcode::EXIT:
			os << "exit"; break;
		case Opcode::VOICE:
			os << "voice"; break;
		case Opcode::CHAR_MOVE:
			os << "char_move"; break;
		case Opcode::X_00BB:
			os << "00BB"; break;
		case Opcode::VIDEO:
			os << "video"; break;
		case Opcode::SOUND_:
			os << "sound_"; break;
		case Opcode::X_00EA:
			os << "00EA"; break;
		case Opcode::X_00EB:
			os << "00EB"; break;
		case Opcode::X_00EC:
			os << "00EC"; break;
		case Opcode::X_00ED:
			os << "00ED"; break;
		case Opcode::X_00EF:
			os << "00EF"; break;
		case Opcode::X_00F0:
			os << "00F0"; break;
		case Opcode::X_00F1:
			os << "00F1"; break;
		case Opcode::CAMERA:
			os << "camera"; break;
		case Opcode::X_00F6:
			os << "00F6"; break;
		case Opcode::X_00F7:
			os << "00F7"; break;
		case Opcode::X_00F8:
			os << "00F8"; break;
		case Opcode::DATE_A:
			os << "date_a"; break;
		case Opcode::DATE_B:
			os << "date_b"; break;
		case Opcode::DATE_C:
			os << "date_c"; break;
		case Opcode::X_0102:
			os << "0102"; break;
		case Opcode::X_0103:
			os << "0103"; break;
		case Opcode::X_0104:
			os << "0104"; break;
		case Opcode::X_0105:
			os << "0105"; break;
		case Opcode::X_0106:
			os << "0106"; break;
		case Opcode::X_0107:
			os << "0107"; break;
		case Opcode::X_0108:
			os << "0108"; break;
		case Opcode::X_0109:
			os << "0109"; break;
		case Opcode::X_010A:
			os << "010A"; break;
		case Opcode::TITLE:
			os << "title"; break;
		case Opcode::X_010C:
			os << "010C"; break;
		default:
			throw std::runtime_error("Unknown Opcode: " + std::to_string((uint32_t)opcode));
		}
	}
}