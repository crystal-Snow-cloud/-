#include "PinyinConverter.h"
#include <windows.h>
#include <cctype>
#include <fstream>
#include <iostream>
//这个文件用来实现汉字到拼音的转换

//initialized函数：标记字典是否已加载，避免重复初始化。
bool PinyinConverter::initialized = false;
//charToPinyin：汉字(GBK字节串) → 拼音(如"liu")的映射表，是转换的核心数据。
std::unordered_map<std::string, std::string> PinyinConverter::charToPinyin;


//需要下面这两个函数：
// 原因：std::string 中的汉字占2个字节，直接按字节遍历会拆散汉字。
// 转换成 wstring（每个汉字一个 wchar_t）后才能逐个汉字处理。
// GBKToWStr：GBK字符串转宽字符，用于拆分单个汉字，然后用于遍历。
static std::wstring GBKToWStr(const std::string& gbkStr) {
    if (gbkStr.empty()) return L"";
    int len = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, NULL, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, &wstr[0], len);
    wstr.pop_back();
    return wstr;
}

// WStrToGBK：宽字符转回GBK字符串，用于查表。
static std::string WStrToGBK(const std::wstring& wstr) {
    if (wstr.empty()) return "";
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    std::string gbkStr(len, '\0');
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &gbkStr[0], len, NULL, NULL);
    gbkStr.pop_back();
    return gbkStr;
}


//这个initialize()函数的功能：加载汉字对应拼音字典。
//执行过程如下：
//1.先插入少量（我插了6个）内置汉字兜底（防止字典文件缺失时完全不可用，依旧能成功转化少量姓名）。
//2.打开外部字典文件 D:/Phone/Phone/data/pinyin_dict.txt（格式：汉字, 拼音）。
//3.逐行解析，存入 charToPinyin 映射表。
//4，设置 initialized = true，只加载一次。
//这是整个拼音转换的数据基础，提供汉字到全拼的映射。
void PinyinConverter::initialize() {
    if (initialized) return;

    // 内置兜底汉字（文件为GBK编码时，key天然就是GBK格式）
    charToPinyin["王"] = "wang";
    charToPinyin["李"] = "li";
    charToPinyin["张"] = "zhang";
    charToPinyin["刘"] = "liu";
    charToPinyin["阿"] = "a";
    charToPinyin["安"] = "an";

    // 加载外部字典文件
    std::ifstream dictFile("D:/Phone/Phone/data/pinyin_dict.txt");
    if (dictFile.is_open()) {
        std::string line;
        int count = 0;
        while (std::getline(dictFile, line)) {
            if (line.empty()) continue;
            size_t pos = line.find(',');
            if (pos == std::string::npos) continue;

            std::string hanzi = line.substr(0, pos);
            std::string pinyin = line.substr(pos + 1);
            charToPinyin[hanzi] = pinyin;
            count++;
        }
        dictFile.close();
    }
    initialized = true;
}


//这个getPinyin函数的功能：返回中文姓名的全拼。
//比如说刘德华会返回“liu de hua”
//执行过程如下：
//1.调用 initialize() 确保字典已加载。
//2,用 GBKToWStr (一开始就写了的辅助函数）将输入转为宽字符串，然后逐个汉字处理。
//3.每个汉字单独转回 GBK 查表，得到拼音并拼接。
//4.最后可以返回完整拼音串。
//是用来保存联系人信息时填充 Contact 的 pinyin 字段的。
std::string PinyinConverter::getPinyin(const std::string& chinese) {
    initialize();
    std::string result;

    std::wstring wstr = GBKToWStr(chinese);
    for (wchar_t wc : wstr) {
        std::string singleChar = WStrToGBK(std::wstring(1, wc));
        auto it = charToPinyin.find(singleChar);
        if (it != charToPinyin.end()) {
            if (!result.empty()) result += " ";
            result += it->second;
        }
    }
    return result;
}


//这个getInitials函数的功能：返回中文姓名的拼音首字母大写串。例如输入 "刘德华" 会 输出 "LDH"。
//执行过程：
//1.和上面的 getPinyin 一样拆分汉字。
//2.查表得到拼音后，只取首字母并转为大写。
//3.将所有首字母拼接返回。
//这是 Trie 树插入、查找、删除时用的 key
std::string PinyinConverter::getInitials(const std::string& chinese) {
    initialize();
    std::string result;

    std::wstring wstr = GBKToWStr(chinese);
    for (wchar_t wc : wstr) {
        std::string singleChar = WStrToGBK(std::wstring(1, wc));
        auto it = charToPinyin.find(singleChar);
        if (it != charToPinyin.end()) {
            result += (char)toupper(it->second[0]);
        }
    }
    return result;
}