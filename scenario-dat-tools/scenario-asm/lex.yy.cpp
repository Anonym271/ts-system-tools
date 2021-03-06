// lex.yy.cpp generated by reflex 3.0.2 from C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l

#define REFLEX_VERSION "3.0.2"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  OPTIONS USED                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define REFLEX_OPTION_header_file         "lex.yy.h"
#define REFLEX_OPTION_lex                 lex
#define REFLEX_OPTION_lexer               Lexer
#define REFLEX_OPTION_outfile             "lex.yy.cpp"
#define REFLEX_OPTION_unicode             true

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 1: %top user code                                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#line 1 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"

#include <string>
#include <unordered_set>
#include <vector>
#include <algorithm>

#include <unicode/unistr.h>
#include <tss/Opcode.hpp>



////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  REGEX MATCHER                                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <reflex/matcher.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  ABSTRACT LEXER CLASS                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <reflex/abslexer.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  LEXER CLASS                                                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

class Lexer : public reflex::AbstractLexer<reflex::Matcher> {
#line 12 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"

	using Opcode = tss::Opcode;
	icu::UnicodeString _string;

public:
	Opcode opcode;
	std::vector<char> var_data;
	std::unordered_set<std::string> variables;

	std::vector<char> string()
	{
		std::vector<char> result(_string.length() * 4);
		auto length = _string.extract(0, _string.length(), result.data(), "shift_jis");
		result.resize(length);
		return std::move(result);
	}
private:
	void append_c_string()
	{
		const char* pos = text();
		const char* end = pos + std::min<int>(19, size());
		int count = 0;
		while (pos < end)
		{
			if (*pos == 0)
				break;
			var_data.push_back(*pos);
			pos++;
			count++;
		}
		var_data.resize(var_data.size() + 20 - count, 0);
	}

	std::string error_head()
	{
		return "ERROR (" + std::to_string(lineno()) + ":" + std::to_string(columno()) + "): ";
	}

 public:
  typedef reflex::AbstractLexer<reflex::Matcher> AbstractBaseLexer;
  Lexer(
      const reflex::Input& input = reflex::Input(),
      std::ostream&        os    = std::cout)
    :
      AbstractBaseLexer(input, os)
  {
  }
  static const int INITIAL = 0;
  static const int STRING = 1;
  static const int VARIABLE = 2;
  virtual int lex(void);
  int lex(const reflex::Input& input)
  {
    in(input);
    return lex();
  }
  int lex(const reflex::Input& input, std::ostream *os)
  {
    in(input);
    if (os)
      out(*os);
    return lex();
  }
};

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 2: rules                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

int Lexer::lex(void)
{
  static const char *REGEX_INITIAL = "(?m)(\")|([\\x09\\x0a\\x0d\\x20])|(#.*\\n)|(\\(.*\\))|((?:(?:[0-9A-Fa-f])+))|((?:\\Qmeta\\E))|((?:\\Qselect\\E))|((?:\\Qspeaker\\E))|((?:\\Qtext\\E))|((?:\\Qreturn\\E))|((?:\\Qbegin\\E))|((?:\\Qscene\\E))|((?:\\Qevent_cg\\E))|((?:\\Qevent_cg_\\E))|((?:\\Qbg\\E))|((?:\\Qif\\E))|((?:\\Qset\\E))|((?:\\Qjump\\E))|((?:\\Qchoose\\E))|((?:\\Qsound\\E))|((?:\\Qchar_sprite\\E))|((?:\\Qchar_hide\\E))|((?:\\Qchar_sprite_\\E))|((?:\\Qcall\\E))|((?:\\Qexit\\E))|((?:\\Qvoice\\E))|((?:\\Qchar_move\\E))|((?:\\Qvideo\\E))|((?:\\Qsound_\\E))|((?:\\Qcamera\\E))|((?:\\Qdate_a\\E))|((?:\\Qdate_b\\E))|((?:\\Qdate_c\\E))|((?:\\Qtitle\\E))|(.)";
  static const reflex::Pattern PATTERN_INITIAL(REGEX_INITIAL);
  static const char *REGEX_STRING = "(?m)(\")|(\\\\\\\\)|(\\\\\")|(\\\\/)|(\\\\b)|(\\\\f)|(\\\\n)|(\\\\r)|(\\\\t)|(\\\\f)|(\\\\x(?:(?:[0-9A-Fa-f])(?:[0-9A-Fa-f])?))|((?:[\\x00-\\x7f]|[\\xc2-\\xdf][\\x80-\\xbf]|\\xe0[\\xa0-\\xbf][\\x80-\\xbf]|[\\xe1-\\xec][\\x80-\\xbf][\\x80-\\xbf]|\\xed[\\x80-\\x9f][\\x80-\\xbf]|[\\xee\\xef][\\x80-\\xbf][\\x80-\\xbf]|\\xf0[\\x90-\\xbf][\\x80-\\xbf][\\x80-\\xbf]|[\\xf1-\\xf3][\\x80-\\xbf][\\x80-\\xbf][\\x80-\\xbf]|\\xf4[\\x80-\\x8f][\\x80-\\xbf][\\x80-\\xbf]))";
  static const reflex::Pattern PATTERN_STRING(REGEX_STRING);
  static const char *REGEX_VARIABLE = "(?m)(\")|((?:[A-Z_a-z][0-9A-Z_a-z]*))|((?:[*+\\x2d/=]|[<->]=|(?:\\Q&&\\E)|(?:\\Q||\\E))|[0-9]+)|([\\x09\\x20])|(.)";
  static const reflex::Pattern PATTERN_VARIABLE(REGEX_VARIABLE);
  if (!has_matcher())
  {
    matcher(new Matcher(PATTERN_INITIAL, stdinit(), this));
  }
  while (true)
  {
    switch (start())
    {
      case INITIAL:
        matcher().pattern(PATTERN_INITIAL);
        switch (matcher().scan())
        {
          case 0:
            if (matcher().at_end())
            {
#line 140 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ return '$'; }

            }
            else
            {
              out().put(matcher().input());
            }
            break;
          case 1: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:64: \" :
#line 64 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{
					if (opcode == Opcode::SET || opcode == Opcode::IF)
					{
						var_data.clear();
						start(VARIABLE);
					}
					else
					{
						_string.remove();
						start(STRING);
					}
				}
            break;
          case 2: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:105: [ \t\n\r] :
#line 105 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{}
            break;
          case 3: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:106: #.*\n :
#line 106 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{}
            break;
          case 4: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:107: \(.*\) :
#line 107 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{}

            break;
          case 5: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:109: {hexstr} :
#line 109 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = (Opcode)std::stoi(text(), 0, 16); return 'o'; }
            break;
          case 6: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:110: "meta" :
#line 110 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::META; return 'o'; }
            break;
          case 7: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:111: "select" :
#line 111 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::SELECT; return 'o'; }
            break;
          case 8: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:112: "speaker" :
#line 112 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::SPEAKER; return 'o'; }
            break;
          case 9: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:113: "text" :
#line 113 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::TEXT; return 'o'; }
            break;
          case 10: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:114: "return" :
#line 114 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::RETURN; return 'o'; }
            break;
          case 11: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:115: "begin" :
#line 115 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::BEGIN; return 'o'; }
            break;
          case 12: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:116: "scene" :
#line 116 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::SCENE; return 'o'; }
            break;
          case 13: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:117: "event_cg" :
#line 117 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::EVENT_CG; return 'o'; }
            break;
          case 14: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:118: "event_cg_" :
#line 118 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::EVENT_CG_; return 'o'; }
            break;
          case 15: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:119: "bg" :
#line 119 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::BG; return 'o'; }
            break;
          case 16: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:120: "if" :
#line 120 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::IF; return 'o'; }
            break;
          case 17: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:121: "set" :
#line 121 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::SET; return 'o'; }
            break;
          case 18: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:122: "jump" :
#line 122 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::JUMP; return 'o'; }
            break;
          case 19: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:123: "choose" :
#line 123 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::CHOOSE; return 'o'; }
            break;
          case 20: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:124: "sound" :
#line 124 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::SOUND; return 'o'; }
            break;
          case 21: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:125: "char_sprite" :
#line 125 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::CHAR_SPRITE; return 'o'; }
            break;
          case 22: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:126: "char_hide" :
#line 126 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::CHAR_HIDE; return 'o'; }
            break;
          case 23: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:127: "char_sprite_" :
#line 127 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::CHAR_SPRITE_; return 'o'; }
            break;
          case 24: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:128: "call" :
#line 128 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::CALL; return 'o'; }
            break;
          case 25: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:129: "exit" :
#line 129 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::EXIT; return 'o'; }
            break;
          case 26: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:130: "voice" :
#line 130 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::VOICE; return 'o'; }
            break;
          case 27: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:131: "char_move" :
#line 131 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::CHAR_MOVE; return 'o'; }
            break;
          case 28: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:132: "video" :
#line 132 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::VIDEO; return 'o'; }
            break;
          case 29: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:133: "sound_" :
#line 133 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::SOUND_; return 'o'; }
            break;
          case 30: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:134: "camera" :
#line 134 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::CAMERA; return 'o'; }
            break;
          case 31: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:135: "date_a" :
#line 135 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::DATE_A; return 'o'; }
            break;
          case 32: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:136: "date_b" :
#line 136 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::DATE_B; return 'o'; }
            break;
          case 33: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:137: "date_c" :
#line 137 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::DATE_C; return 'o'; }
            break;
          case 34: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:138: "title" :
#line 138 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ opcode = Opcode::TITLE; return 'o'; }

            break;
          case 35: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:142: . :
#line 142 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ throw std::runtime_error(error_head() + "unexpected character: " + str()); }

            break;
        }
        break;
      case STRING:
        matcher().pattern(PATTERN_STRING);
        switch (matcher().scan())
        {
          case 0:
            if (matcher().at_end())
            {
#line 140 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ return '$'; }

            }
            else
            {
              out().put(matcher().input());
            }
            break;
          case 1: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:77: \" :
#line 77 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ start(INITIAL); return 's'; }
            break;
          case 2: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:78: \\\\ :
#line 78 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ _string.append('\\'); }
            break;
          case 3: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:79: \\\" :
#line 79 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ _string.append('\"'); }
            break;
          case 4: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:80: \\\/ :
#line 80 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ _string.append('/'); }
            break;
          case 5: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:81: \\b :
#line 81 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ _string.append('\b'); }
            break;
          case 6: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:82: \\f :
#line 82 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ _string.append('\f'); }
            break;
          case 7: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:83: \\n :
#line 83 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ _string.append('\n'); }
            break;
          case 8: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:84: \\r :
#line 84 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ _string.append('\r'); }
            break;
          case 9: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:85: \\t :
#line 85 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ _string.append('\t'); }
            break;
          case 10: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:86: \\f :
#line 86 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ _string.append('\f'); }
            break;
          case 11: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:87: \\x{hexchar} :
#line 87 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ _string.append(std::stoi(text() + 2, 0, 16)); }
 //.						{ _string.append(icu::UnicodeString(text(), size(), "utf8")); }
            break;
          case 12: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:89: \p{Unicode} :
#line 89 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ _string.append(icu::UnicodeString(text(), size(), "utf8")); }
            break;
        }
        break;
      case VARIABLE:
        matcher().pattern(PATTERN_VARIABLE);
        switch (matcher().scan())
        {
          case 0:
            if (matcher().at_end())
            {
#line 140 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ return '$'; }

            }
            else
            {
              out().put(matcher().input());
            }
            break;
          case 1: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:93: \" :
#line 93 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ start(INITIAL); return 'v';}
            break;
          case 2: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:94: {ident} :
#line 94 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ /* identifiers */
							if (size() > 20)
								std::cout << "WARNING: variable " << text() << " too long, trucating.\n";
							variables.emplace(text());
							append_c_string();
						}
            break;
          case 3: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:100: {operator}|[0-9]+ :
#line 100 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ append_c_string(); } /* operators and numbers */
            break;
          case 4: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:101: [ \t] :
#line 101 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{} /* Delim */
            break;
          case 5: // rule C:\Users\Jonas\source\ts-system-tools\scenario-dat-tools\scenario-asm\tsa.l:102: . :
#line 102 "C:\\Users\\Jonas\\source\\ts-system-tools\\scenario-dat-tools\\scenario-asm\\tsa.l"
{ throw std::runtime_error(error_head() + "unexpected character: " + str()); }
            break;
        }
        break;
      default:
        start(0);
    }
  }
}
