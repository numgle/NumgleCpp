#pragma once

#include <cstdint>
#include <sstream>
#include <vector>
#include <string>

constexpr static uint32_t UTF8_ACCEPT = 0;
constexpr static uint32_t UTF8_REJECT = 1;

static const uint8_t utf8d[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
  8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
  0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
  0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
  0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
  1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
  1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
  1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

struct Utf8Decoder {
    uint32_t state{ 0 };
    uint32_t codep;

    uint32_t decode(uint32_t byte) {
        uint32_t type = utf8d[byte];

        codep = (state != UTF8_ACCEPT) ?
            (byte & 0x3fu) | (codep << 6) :
            (0xff >> type) & (byte);

        state = utf8d[256 + state * 16 + type];
        return state;
    }
};

inline static std::vector<uint32_t> splitUnicodes(const std::string& str) {
    std::vector<uint32_t> codepoints;
    codepoints.reserve(str.size());
    Utf8Decoder decoder = {};
    for (size_t i = 0; i < str.size(); ++i) {
        if (decoder.decode(*reinterpret_cast<const uint8_t*>(&str[i])) == UTF8_ACCEPT) {
            codepoints.push_back(decoder.codep);
        }
    }
    return std::move(codepoints);
}

inline static std::string uriDecode(const std::string& str) {
    std::ostringstream ret;
    char ch;
    int i, ii, len = str.length();

    for (i = 0; i < len; i++) {
        if (str[i] != '%') {
            if (str[i] == '+')
                ret << ' ';
            else
                ret << str[i];
        } else {
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret << ch;
            i += 2;
        }
    }
    return ret.str();
}