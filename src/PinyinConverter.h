#ifndef PINYIN_CONVERTER_H
#define PINYIN_CONVERTER_H

#include <string>
#include <unordered_map>
#include <vector>

class PinyinConverter {
private:
    static std::unordered_map<std::string, std::string> charToPinyin;
    static bool initialized;

public:
    static void initialize();
    static std::string getPinyin(const std::string& chinese);
    static std::string getInitials(const std::string& chinese);
    
};

#endif