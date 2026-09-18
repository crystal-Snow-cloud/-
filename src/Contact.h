#ifndef CONTACT_H
#define CONTACT_H

#include <string>
#include <iostream>

class Contact {
public:
    std::string name;        // 中文姓名
    std::string englishName; // 英文名
    std::string phone;       // 电话号码
    std::string pinyin;      // 拼音全拼
    std::string pinyinInitials; // 拼音首字母

    // 构造函数声明
    Contact();
    Contact(std::string n, std::string en, std::string p,std::string py, std::string pi);

    // 显示联系人信息
    void display() const;

    // 获取格式化的字符串表示
    std::string toString() const;

    // 运算符重载
    bool operator==(const Contact& other) const;
    bool operator<(const Contact& other) const;
};

#endif 