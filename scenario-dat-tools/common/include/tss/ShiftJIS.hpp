#pragma once
#include <string>
#include <vector>

#include <unicode/unistr.h>
#include <unicode/schriter.h>

namespace tss
{
	class ShiftJIS
	{
	public:
		static std::vector<char> from_utf8(const std::vector<char>& text)
		{
			return std::move(from_utf8(text.data(), text.size()));
		}
		static std::vector<char> from_utf8(const char* text, size_t size)
		{
			icu::UnicodeString ucs(text, size, "utf8");
			std::vector<char> result(size * 4);
			auto length = ucs.extract(0, ucs.length(), result.data(), "shift_jis");
			result.resize(length - 1);
			return std::move(result);
		}
		static std::vector<char> from_utf8_esc(const std::vector<char>& text)
		{
			return std::move(from_utf8_esc(text.data(), text.size()));
		}
		static std::vector<char> from_utf8_esc(const char* text, size_t size)
		{
			icu::UnicodeString ucs(text, size, "utf8");
			icu::UnicodeString tmp(ucs.length(), 0, 0);
			icu::StringCharacterIterator it(ucs);
			UChar32 c;
			while (it.hasNext())
			{
				c = it.next32PostInc();
				switch (c)
				{
				case '\n':
					tmp.append("\\n");
					break;
				case '\t':
					tmp.append("\\t");
					break;
				case '\a':
					tmp.append("\\a");
					break;
				case '\\':
					tmp.append("\\\\");
					break;
				case '\"':
					tmp.append("\\\"");
					break;
				default:
					tmp.append(c);
					break;
				}
			}
			int32_t tmp_len = tmp.length();
			std::vector<char> result(tmp_len * 4);
			auto length = tmp.extract(0, tmp_len, result.data(), "shift-jis");
			result.resize(length);
			return std::move(result);
		}

		static std::vector<char> to_utf8(const std::vector<char>& text)
		{
			return std::move(to_utf8(text.data(), text.size()));
		}
		static std::vector<char> to_utf8(const char* text, size_t size)
		{
			icu::UnicodeString ucs(text, size, "shift_jis");
			std::vector<char> result(size * 4);
			auto length = ucs.extract(0, ucs.length(), result.data(), "utf8");
			result.resize(length);
			return std::move(result);
		}
		static std::vector<char> to_utf8_esc(const std::vector<char>& text)
		{
			return std::move(to_utf8_esc(text.data(), text.size()));
		}
		static std::vector<char> to_utf8_esc(const char* text, size_t size)
		{
			icu::UnicodeString ucs(text, size, "shift_jis");
			icu::UnicodeString tmp(ucs.length(), 0, 0);
			icu::StringCharacterIterator it(ucs);
			UChar32 c;
			while (it.hasNext())
			{
				c = it.next32PostInc();
				switch (c)
				{
				case '\n':
					tmp.append("\\n");
					break;
				case '\t':
					tmp.append("\\t");
					break;
				case '\a':
					tmp.append("\\a");
					break;
				case '\\':
					tmp.append("\\\\");
					break;
				case '\"':
					tmp.append("\\\"");
					break;
				default:
					tmp.append(c);
					break;
				}
			}
			int32_t tmp_len = tmp.length();
			std::vector<char> result(tmp_len * 4);
			auto length = tmp.extract(0, tmp_len, result.data(), "utf8");
			result.resize(length);
			return std::move(result);
		}
	};
}