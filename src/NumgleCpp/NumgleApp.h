#pragma once

#include "DatasetRepository.h"
#include <memory>
#include <unordered_map>

const static std::unordered_map<char, int> SPECIAL_CHARATERS = {
    { '?', 0 }, { '!', 1 }, { '.', 2 }, { '^', 3 }, { '-', 4 }
};

class NumgleApp {
public:
    NumgleApp(std::unique_ptr<DatasetRepository> repo)
        : repo(std::move(repo)) {}

    std::string convertToNumgle(const std::vector<uint32_t>& input) {
        std::ostringstream ss{};
        const ConvertTables &tables = repo->getConvertTables();
        for (auto &code : input) {
            convertOneChar(ss, code, tables);
            ss << '\n';
        }
        return ss.str();
    }

private:
    std::unique_ptr<DatasetRepository> repo;

    void convertOneChar(std::ostringstream &stream, uint32_t code,
        const ConvertTables &tables) {
        const auto letterType = getLetterType(code);
        switch (letterType) {
        case LetterType::Empty:
            break;
        case LetterType::CompleteHangul: {
            const auto seperatedHan = seperateHan(code);

            if (seperatedHan.jung >= 8 && seperatedHan.jung != 20) {
                stream << tables.jongTable.at(seperatedHan.jong);
                stream << tables.jungTable.at(seperatedHan.jung - 8);
                stream << tables.choTable.at(seperatedHan.cho);
                break;
            }

            stream << tables.jongTable.at(seperatedHan.jong);
            stream << tables.cjTable.at(std::min((uint32_t)8, seperatedHan.jung))
                          .at(seperatedHan.cho);
            break;
        }
        case LetterType::NotCompleteHangul:
            stream << tables.hanTable.at(code - 0x3131);
            break;
        case LetterType::Number:
            stream << tables.numberTable.at(code - 48);
            break;
        case LetterType::SpecialLetter:
            stream << tables.specialTable.at(SPECIAL_CHARATERS.at(code));
            break;
        case LetterType::EnglishLower:
            stream << tables.engLowerTable.at(code - 97);
            break;
        case LetterType::EnglishUpper:
            stream << tables.engUpperTable.at(code - 65);
            break;
        }
    }

    enum class LetterType {
        Empty,
        CompleteHangul,
        NotCompleteHangul,
        EnglishUpper,
        EnglishLower,
        Number,
        SpecialLetter,
        Unknown
    };

    static LetterType getLetterType(uint32_t letter) {
        if (letter == ' ' || letter == '\r' || letter == '\n')
            return LetterType::Empty;
        else if (letter >= 44032 && letter <= 55203)
            return LetterType::CompleteHangul;
        else if (letter >= 0x3131 && letter <= 0x3163)
            return LetterType::NotCompleteHangul;
        else if (letter >= 65 && letter <= 90)
            return LetterType::EnglishUpper;
        else if (letter >= 97 && letter <= 122)
            return LetterType::EnglishLower;
        else if (letter >= 48 && letter <= 57)
            return LetterType::Number;
        else if (SPECIAL_CHARATERS.find(letter) != SPECIAL_CHARATERS.end())
            return LetterType::SpecialLetter;
        else
            return LetterType::Unknown;
    }

    struct Hangul {
        uint32_t cho;
        uint32_t jung;
        uint32_t jong;
    };

    static Hangul seperateHan(uint32_t han) {
        return { (han - 44032) / 28 / 21, (han - 44032) / 28 % 21,
            (han - 44032) % 28 };
    }
};
