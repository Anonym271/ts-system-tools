#include <tss/Opcode.hpp>
#include <string>
#include <exception>
#include <iomanip>

namespace tss
{
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
		case Opcode::SCENE:
			os << "scene"; break;
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
		case Opcode::JUMP:
			os << "jump"; break;
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
	const char* opcode_str(Opcode opcode)
	{
		switch (opcode)
		{

		case Opcode::META:
			return "meta"; break;
		case Opcode::SELECT:
			return "select"; break;
		case Opcode::SPEAKER:
			return "speaker"; break;
		case Opcode::TEXT:
			return "text"; break;
		case Opcode::RETURN:
			return "return"; break;
		case Opcode::BEGIN:
			return "begin"; break;
		case Opcode::SCENE:
			return "scene"; break;
		case Opcode::EVENT_CG:
			return "event_cg"; break;
		case Opcode::EVENT_CG_:
			return "event_cg_"; break;
		case Opcode::BG:
			return "bg"; break;
		case Opcode::X_0069:
			return "0069"; break;
		case Opcode::X_006B:
			return "006B"; break;
		case Opcode::X_006C:
			return "006C"; break;
		case Opcode::X_006D:
			return "006D"; break;
		case Opcode::IF:
			return "if"; break;
		case Opcode::SET:
			return "set"; break;
		case Opcode::JUMP:
			return "jump"; break;
		case Opcode::CHOOSE:
			return "choose"; break;
		case Opcode::X_0075:
			return "0075"; break;
		case Opcode::X_0076:
			return "0076"; break;
		case Opcode::SOUND:
			return "sound"; break;
		case Opcode::X_0078:
			return "0078"; break;
		case Opcode::CHAR_SPRITE:
			return "char_sprite"; break;
		case Opcode::CHAR_HIDE:
			return "char_hide"; break;
		case Opcode::CHAR_SPRITE_:
			return "char_sprite_"; break;
		case Opcode::CALL:
			return "call"; break;
		case Opcode::X_0099:
			return "0099"; break;
		case Opcode::X_009A:
			return "009A"; break;
		case Opcode::EXIT:
			return "exit"; break;
		case Opcode::VOICE:
			return "voice"; break;
		case Opcode::CHAR_MOVE:
			return "char_move"; break;
		case Opcode::X_00BB:
			return "00BB"; break;
		case Opcode::VIDEO:
			return "video"; break;
		case Opcode::SOUND_:
			return "sound_"; break;
		case Opcode::X_00EA:
			return "00EA"; break;
		case Opcode::X_00EB:
			return "00EB"; break;
		case Opcode::X_00EC:
			return "00EC"; break;
		case Opcode::X_00ED:
			return "00ED"; break;
		case Opcode::X_00EF:
			return "00EF"; break;
		case Opcode::X_00F0:
			return "00F0"; break;
		case Opcode::X_00F1:
			return "00F1"; break;
		case Opcode::CAMERA:
			return "camera"; break;
		case Opcode::X_00F6:
			return "00F6"; break;
		case Opcode::X_00F7:
			return "00F7"; break;
		case Opcode::X_00F8:
			return "00F8"; break;
		case Opcode::DATE_A:
			return "date_a"; break;
		case Opcode::DATE_B:
			return "date_b"; break;
		case Opcode::DATE_C:
			return "date_c"; break;
		case Opcode::X_0102:
			return "0102"; break;
		case Opcode::X_0103:
			return "0103"; break;
		case Opcode::X_0104:
			return "0104"; break;
		case Opcode::X_0105:
			return "0105"; break;
		case Opcode::X_0106:
			return "0106"; break;
		case Opcode::X_0107:
			return "0107"; break;
		case Opcode::X_0108:
			return "0108"; break;
		case Opcode::X_0109:
			return "0109"; break;
		case Opcode::X_010A:
			return "010A"; break;
		case Opcode::TITLE:
			return "title"; break;
		case Opcode::X_010C:
		return "010C"; break;		default:
			throw std::runtime_error("Unknown Opcode: " + std::to_string((uint32_t)opcode));
		}
	}
}