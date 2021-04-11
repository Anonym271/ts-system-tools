%top{
#include <string>
#include <unordered_set>
#include <vector>
#include <algorithm>

#include <unicode/unistr.h>
#include <tss/Opcode.hpp>

%}

%class{
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
%}

%option unicode

hexdig		[a-fA-F0-9]
hexchar		{hexdig}{hexdig}?
hexstr		{hexdig}+
ident		[a-zA-Z_][a-zA-Z0-9_]*
operator    [\+\-\*\/\=]|[\>\<\=]=|"&&"|"||"

%x STRING
%x VARIABLE

%%

\"				{ 
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
<STRING>{
\"						{ start(INITIAL); return 's'; }
\\\\					{ _string.append('\\'); }
\\\"					{ _string.append('\"'); }
\\\/ 					{ _string.append('/'); }
\\b						{ _string.append('\b'); }
\\f						{ _string.append('\f'); }
\\n						{ _string.append('\n'); }
\\r						{ _string.append('\r'); }
\\t						{ _string.append('\t'); }
\\f						{ _string.append('\f'); }
\\x{hexchar}			{ _string.append(std::stoi(text() + 2, 0, 16)); }
 //.						{ _string.append(icu::UnicodeString(text(), size(), "utf8")); }
\p{Unicode}     { _string.append(icu::UnicodeString(text(), size(), "utf8")); }
}

<VARIABLE>{
\"						{ start(INITIAL); return 'v';}
{ident}					{ /* identifiers */
							if (size() > 20) 
								std::cout << "WARNING: variable " << text() << " too long, trucating.\n";
							variables.emplace(text());
							append_c_string();
						}
{operator}|[0-9]+		{ append_c_string(); } /* operators and numbers */
[ \t]					{} /* Delim */
.						{ throw std::runtime_error(error_head() + "unexpected character: " + str()); }
}

[ \t\n\r]		{}
#.*\n			{}
\(.*\)			{}

{hexstr}        { opcode = (Opcode)std::stoi(text(), 0, 16); return 'o'; }
"meta"          { opcode = Opcode::META; return 'o'; }
"select"        { opcode = Opcode::SELECT; return 'o'; }
"speaker"       { opcode = Opcode::SPEAKER; return 'o'; }
"text"          { opcode = Opcode::TEXT; return 'o'; }
"return"        { opcode = Opcode::RETURN; return 'o'; }
"begin"         { opcode = Opcode::BEGIN; return 'o'; }
"scene"         { opcode = Opcode::SCENE; return 'o'; }
"event_cg"      { opcode = Opcode::EVENT_CG; return 'o'; }
"event_cg_"     { opcode = Opcode::EVENT_CG_; return 'o'; }
"bg"            { opcode = Opcode::BG; return 'o'; }
"if"            { opcode = Opcode::IF; return 'o'; }
"set"           { opcode = Opcode::SET; return 'o'; }
"jump"          { opcode = Opcode::JUMP; return 'o'; }
"choose"        { opcode = Opcode::CHOOSE; return 'o'; }
"sound"         { opcode = Opcode::SOUND; return 'o'; }
"char_sprite"   { opcode = Opcode::CHAR_SPRITE; return 'o'; }
"char_hide"     { opcode = Opcode::CHAR_HIDE; return 'o'; }
"char_sprite_"  { opcode = Opcode::CHAR_SPRITE_; return 'o'; }
"call"          { opcode = Opcode::CALL; return 'o'; }
"exit"          { opcode = Opcode::EXIT; return 'o'; }
"voice"         { opcode = Opcode::VOICE; return 'o'; }
"char_move"     { opcode = Opcode::CHAR_MOVE; return 'o'; }
"video"         { opcode = Opcode::VIDEO; return 'o'; }
"sound_"        { opcode = Opcode::SOUND_; return 'o'; }
"camera"        { opcode = Opcode::CAMERA; return 'o'; }
"date_a"        { opcode = Opcode::DATE_A; return 'o'; }
"date_b"        { opcode = Opcode::DATE_B; return 'o'; }
"date_c"        { opcode = Opcode::DATE_C; return 'o'; }
"title"         { opcode = Opcode::TITLE; return 'o'; }

<<EOF>>			{ return '$'; }

.				{ throw std::runtime_error(error_head() + "unexpected character: " + str()); }

%%