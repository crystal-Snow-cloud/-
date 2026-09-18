#include "FileManager.h"
#include "Contact.h"
#include "PinyinConverter.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
//这个FileManager 文件负责从导入的联系人信息文件读取通讯录数据，为每条记录生成拼音和首字母字段，并返回 Contact 对象列表。






//这个loadContacts函数的功能是：从文件加载通讯录数据，返回一个 vector，每个元素都是一个 Contact对象的智能指针。
// 执行过程如下：
//1.先打开文件，如果失败则输出错误并返回空向量。
//2.然后逐行读取，用逗号分隔出 姓名、英文名、电话号码（和测试数据格式一样）。
//3.只要姓名非空就视为有效记录（不管英文名或电话空不空），这样可以兼容不完整信息的情况。
//4.为每个联系人调用 PinyinConverter::getPinyin 和 getInitials两个函数，自动生成全拼和首字母字段，并存入 Contact 对象。
//最多加载 3 万条（if(count >= 30000) break），符合题目要求。
//只要姓名非空就加载，兼容电话/英文名空的情况
std::vector<std::shared_ptr<Contact>> FileManager::loadContacts(const std::string& filename) {
    std::vector<std::shared_ptr<Contact>> contacts;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return contacts;
    }

    std::string line;
    int count = 0;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string name, englishName, phone;

        std::getline(ss, name, ',');
        std::getline(ss, englishName, ',');
        std::getline(ss, phone, ',');

        // 修复点：原来要求姓名+电话都非空，现在只要姓名非空就加载
        if (!name.empty()) {
            std::string pinyin = PinyinConverter::getPinyin(name);
            std::string initials = PinyinConverter::getInitials(name);

            auto contact = std::make_shared<Contact>(name, englishName, phone, pinyin, initials);
            contacts.push_back(contact);
            count++;

            if (count >= 30000) break; // 限制加载3万条
        }
    }

    file.close();
    return contacts;
}

//下面是第一个重载函数，功能为：直接读取文件，在屏幕上显示前 n 条记录，不依赖内存中的联系人列表。
//执行过程如下：
//1.打开文件，打印表头（序号、姓名、英文名、电话）。
//2.逐行解析文件内容，然后按固定宽度左对齐输出前 n 条。
//可以独立于加载流程查看原始文件内容，用于调试或验证文件格式。
void FileManager::displayFirstN(const std::string& filename, int n) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return;
    }

    std::cout << "\n=== 通讯录前 " << n << " 条记录 ===\n" << std::endl;
    std::cout << std::left << std::setw(5) << "序号"
        << std::setw(15) << "姓名"
        << std::setw(15) << "英文名"
        << std::setw(15) << "电话" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    std::string line;
    int count = 0;

    while (std::getline(file, line) && count < n) {
        std::stringstream ss(line);
        std::string name, englishName, phone;

        std::getline(ss, name, ',');
        std::getline(ss, englishName, ',');
        std::getline(ss, phone, ',');

        std::cout << std::left << std::setw(5) << (++count)
            << std::setw(15) << name
            << std::setw(15) << englishName
            << std::setw(15) << phone << std::endl;
    }

    file.close();
}

// 下面是第二个重载，功能是：根据已加载到内存的联系人向量，显示前 n 条记录。
//执行过程如下：
//1打印表头。
//2.遍历向量（最多 n 条或向量实际大小），输出每个 Contact 的 name、englishName、phone。
//这是展示前100条的典型实现，可以确保显示的结果与导入 Trie 树的数据完全同步。
void FileManager::displayFirstN(const std::vector<std::shared_ptr<Contact>>& contacts, int n) {
    std::cout << "\n=== 通讯录前 " << n << " 条记录 ===\n" << std::endl;
    std::cout << std::left 
        << std::setw(5) << "序号"
        << std::setw(15) << "姓名"
        << std::setw(15) << "英文名"
        << std::setw(15) << "电话" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    int showCount = std::min((int)contacts.size(), n);
    for (int i = 0; i < showCount; ++i) {
        std::cout << std::left << std::setw(5) << (i + 1)
            << std::setw(15) << contacts[i]->name
            << std::setw(15) << contacts[i]->englishName
            << std::setw(15) << contacts[i]->phone << std::endl;
    }
}
//上面这两个 displayFirstN 重载，
// 一个直接读文件展示前几条记录，
// 另一个根据内存中的数据展示，用来满足题目‘展示前100条’的要求，并保证展示的东西和程序实际使用的数据完全一样。
