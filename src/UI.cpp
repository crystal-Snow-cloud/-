#include "UI.h"
#include "FileManager.h"
#include "PinyinConverter.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <map>
#include <cstdlib>
#include <ctime>
//这个UI.cpp 是整个程序的用户交互层，负责显示菜单、接收用户输入，
// 并调用 Trie、FileManager、PinyinConverter 等模块完成各项功能。

//先写构造函数 UI()
//功能是初始化 Trie 树对象（智能指针）、默认通讯录文件名。
//设置随机数种子，用于性能测试的随机查询，功能7就是用随机生成的数据来查询的。
UI::UI() : trie(std::make_shared<Trie>()), dataFile("contacts.txt") {
    // 初始化随机数种子
    srand(static_cast<unsigned int>(time(nullptr)));
}

//这个displayMenu函数：在屏幕上打印操作菜单。
void UI::displayMenu() {
    std::cout << "\n========================================\n";
    std::cout << "     手机通讯录管理系统 (基于Trie树)\n";
    std::cout << "========================================\n";
    std::cout << "1. 导入通讯录文件\n";
    std::cout << "2. 展示Trie树结构\n";
    std::cout << "3. 按姓名查询联系人\n";
    std::cout << "4. 插入新联系人\n";
    std::cout << "5. 按拼音首字母前缀查询\n";
    std::cout << "6. 删除联系人\n";
    std::cout << "7. 批量查询性能测试\n";
    std::cout << "8. 重复联系人检测\n";
    std::cout << "9. 保存Trie树到文件\n";
    std::cout << "0. 退出程序\n";
    std::cout << "========================================\n";
    std::cout << "请选择操作: ";
}


//下面这个handleImport函数对应选项 1（导入通讯录文件，展示前100条）
//功能一：询问用户文件名（默认 data / contacts.txt）。
//功能二：清空旧数据：重建 contactList 和 Trie 对象，避免重复导入导致数据叠加。
//功能三：调用 FileManager::loadContacts() 读取文件，得到联系人列表。
//功能四：将所有联系人插入 Trie 树，并计算构建耗时。
//功能五：调用 FileManager::displayFirstN(contactList, 100) 展示前 100 条。
//还能输出总记录数和构建时间。
void UI::handleImport() {
    std::cout << "\n请输入通讯录文件名（测试数据为data/contacts.txt） ";
    std::string filename;
    std::getline(std::cin, filename);
    if (filename.empty()) filename = dataFile;

    std::cout << "正在加载联系人...\n";

    // 1. 清空旧数据，避免重复导入叠加
    contactList.clear();
    // 直接重建一棵全新的Trie树，替代有风险的 clear() 方法
    trie = std::make_shared<Trie>();

    // 2. 加载文件到内存
    contactList = FileManager::loadContacts(filename);

    // 先打印加载数量，确认数据是否正常读取
    std::cout << "文件读取完成，共 " << contactList.size() << " 条数据\n";

    // 3. 构建 Trie 树
    auto start = std::chrono::high_resolution_clock::now();
    for (auto& contact : contactList) {
        trie->insert(contact, contact->pinyinInitials);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "成功导入 " << contactList.size() << " 条联系人\n";
    std::cout << "Trie树构建用时: " << duration.count() << " 毫秒\n";

    // 4. 显示前100条
    FileManager::displayFirstN(contactList, 100);
}

//下面这个handleSearchByName对应选项 3（按姓名查询）
//功能一：获取用户输入的中文姓名。
//功能二：调用 trie->searchByName(name) 执行查找（内部会转拼音首字母 + 沿路径搜索 + 节点内精确匹配）。
//功能三：显示查询结果或“未找到”提示。
void UI::handleSearchByName() {
    std::cout << "\n请输入要查询的姓名: ";
    std::string name;
    std::getline(std::cin, name);

    auto contact = trie->searchByName(name);
    if (contact) {
        std::cout << "\n查询结果:\n";
        contact->display();
    }
    else {
        std::cout << "未找到该联系人\n";
    }
}

//handleInsert函数对应选项4（插入联系人并展示位置）
//功能一：让输入姓名、英文名、电话号码。
//功能二：调用 PinyinConverter 中的函数生成全拼和首字母。
//功能三：创建 Contact 对象，插入 Trie 树。
//功能四：输出插入路径（如 根节点->L->D->H）和联系人信息，满足“展示在 Trie 树中的位置”的要求。
void UI::handleInsert() {
    std::cout << "\n=== 插入新联系人 ===\n";
    std::string name, englishName, phone;

    std::cout << "姓名: ";
    std::getline(std::cin, name);
    std::cout << "英文名: ";
    std::getline(std::cin, englishName);
    std::cout << "电话: ";
    std::getline(std::cin, phone);

    std::string pinyin = PinyinConverter::getPinyin(name);
    std::string initials = PinyinConverter::getInitials(name);

    auto contact = std::make_shared<Contact>(name, englishName, phone, pinyin, initials);
    trie->insert(contact, initials);

    std::cout << "\n联系人已成功插入!\n";
    std::cout << "存储路径: 根节点";
    for (char ch : initials) {
        std::cout << " -> " << ch;
    }
    std::cout << "\n";
    contact->display();
}


//这个handleSearchByPrefix函数对应选项 5（按前缀查询）
//功能一：获取用户输入的拼音首字母前缀，转为大写。
//功能二：调用 trie->searchByPrefix(prefix) 收集所有匹配的联系人。
//功能三：显示全部结果。
//功能四：最后输出匹配总数。
void UI::handleSearchByPrefix() {
    std::cout << "\n请输入拼音首字母前缀: ";
    std::string prefix;
    std::getline(std::cin, prefix);

    // 转换为大写
    std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper);

    auto results = trie->searchByPrefix(prefix);

    std::cout << "\n找到 " << results.size() << " 个匹配的联系人:\n";
    std::cout << std::string(60, '-') << std::endl;

    // 遍历所有结果，不做数量限制
    for (auto& contact : results) {
        contact->display();
    }

    std::cout << std::string(60, '-') << std::endl;
    std::cout << "共显示 " << results.size() << " 条记录。" << std::endl;
}

//对应选项 6（删除联系人）
//功能一：输入姓名，先通过 searchByName 找到联系人并显示。
//功能二：让用户确认（y / n）。
//功能三：确认后调用 trie->remove(initials, name) 执行删除。
//功能四：输出删除结果。
void UI::handleDelete() {
    std::cout << "\n请输入要删除的联系人姓名: ";
    std::string name;
    std::getline(std::cin, name);

    auto contact = trie->searchByName(name);
    if (contact) {
        std::cout << "确认删除以下联系人?\n";
        contact->display();
        std::cout << "确认删除 (y/n): ";

        std::string confirm;
        std::getline(std::cin, confirm);

        if (confirm == "y" || confirm == "Y") {
            trie->remove(contact->pinyinInitials, name);
            std::cout << "联系人已删除\n";
        }
    }
    else {
        std::cout << "未找到该联系人\n";
    }
}


//下面这个handlePerformanceTest函数对应要选项7，功能有：
//功能一：让用户输入测试次数。
//功能二：从 Trie 获取全部联系人，随机选取姓名作为测试查询。
//功能三：依次执行 searchByName，使用 <chrono> 记录每次的微秒级耗时。
//功能四：统计总时间、平均时间、最大时间并输出。
void UI::handlePerformanceTest() {
    std::cout << "\n请输入测试次数: ";
    int times;
    std::cin >> times;
    std::cin.ignore();  // 清除输入缓冲区

    // 获取所有联系人姓名
    auto allContacts = trie->searchByPrefix("");
    if (allContacts.empty()) {
        std::cout << "通讯录为空，无法测试\n";
        return;
    }

    // 随机生成测试查询
    std::vector<std::string> testQueries;
    for (int i = 0; i < times; i++) {
        int index = rand() % allContacts.size();
        testQueries.push_back(allContacts[index]->name);
    }

    double totalTime = 0;
    double maxTime = 0;

    for (auto& name : testQueries) {
        auto start = std::chrono::high_resolution_clock::now();
        auto result = trie->searchByName(name);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double timeMs = duration.count() / 1000.0;

        totalTime += timeMs;
        if (timeMs > maxTime) {
            maxTime = timeMs;
        }
    }

    std::cout << "\n=== 性能测试结果 ===\n";
    std::cout << "测试次数: " << times << "\n";
    if (times > 0) {
        std::cout << "平均查询时间: " << (totalTime / times) << " 毫秒\n";
    }
    std::cout << "最大查询时间: " << maxTime << " 毫秒\n";
}


//下面这个handleDuplicateDetection函数对应选项八（重复联系人检测）
//功能一：获取所有联系人，用 std::map<string, vector<Contact>> 按姓名分组。
//功能二：输出出现次数 > 1 的姓名，并列出各重复项的电话和英文名。
//功能三：若无重复则提示。
void UI::handleDuplicateDetection() {
    auto allContacts = trie->searchByPrefix("");

    // 使用map来检测重复（按姓名分组）
    std::map<std::string, std::vector<std::shared_ptr<Contact>>> nameMap;

    for (auto& contact : allContacts) {
        nameMap[contact->name].push_back(contact);
    }

    std::cout << "\n=== 重复联系人检测 ===\n";
    bool found = false;

    for (auto& pair : nameMap) {
        if (pair.second.size() > 1) {
            found = true;
            std::cout << "\n姓名: " << pair.first << " 出现 "
                << pair.second.size() << " 次\n";
            for (auto& contact : pair.second) {
                std::cout << "  电话: " << contact->phone;
                if (!contact->englishName.empty()) {
                    std::cout << " | 英文名: " << contact->englishName;
                }
                std::cout << "\n";
            }
        }
    }

    if (!found) {
        std::cout << "未发现重复联系人\n";
    }
}

//程序主循环：
//先显示菜单，然后读取选项，接着根据选择调用对应处理函数。
//选择 0 时退出循环，结束程序（智能指针shared_ptr 自动释放内存）。
//每次操作后暂停，等待用户按回车继续。
void UI::run() {
    bool running = true;

    while (running) {
        displayMenu();
        int choice;
        std::cin >> choice;
        std::cin.ignore();  // 清除输入缓冲区中的换行符

        switch (choice) {
        case 1:
            handleImport();
            break;
        case 2: {
            std::cout << "\nTrie树结构将保存到 trie_structure.txt\n";
            trie->saveToFile("trie_structure.txt");
            std::cout << "保存完成！\n";
            std::cout << "当前Trie树大小: " << trie->getSize() << " 个联系人\n";
            break;
        }
        case 3:
            handleSearchByName();
            break;
        case 4:
            handleInsert();
            break;
        case 5:
            handleSearchByPrefix();
            break;
        case 6:
            handleDelete();
            break;
        case 7:
            handlePerformanceTest();
            break;
        case 8:
            handleDuplicateDetection();
            break;
        case 9: {
            trie->saveToFile("trie_output.txt");
            std::cout << "\nTrie树已保存到 trie_output.txt\n";
            break;
        }
        case 0:
            running = false;
            std::cout << "\n程序退出，释放内存...\n";
            break;
        default:
            std::cout << "\n无效选择，请重试\n";
        }

        if (running) {
            std::cout << "\n按回车键继续...";
            std::cin.get();
        }
    }
}