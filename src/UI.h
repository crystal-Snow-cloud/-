#ifndef UI_H
#define UI_H

#include "Trie.h"
#include "Contact.h"
#include <memory>
#include <vector>
#include <string>

// UI 类：负责控制台交互界面
// 提供菜单显示、用户输入处理和功能调度
class UI {
private:
    std::shared_ptr<Trie> trie;                     // Trie 树对象，用于高效前缀查询
    std::string dataFile;                           // 当前加载的数据文件路径
    std::vector<std::shared_ptr<Contact>> contactList; // 统一存储所有联系人（与 Trie 树同步）

    void displayMenu();                             // 显示主菜单选项
    void handleImport();                            // 处理“导入通讯录”功能
    void handleSearchByName();                      // 处理“按姓名查询”功能
    void handleInsert();                            // 处理“插入联系人”功能
    void handleSearchByPrefix();                    // 处理“按拼音前缀查询”功能
    void handleDelete();                            // 处理“删除联系人”功能
    void handlePerformanceTest();                   // 处理“性能测试”功能
    void handleDuplicateDetection();                // 处理“重复联系人检测”功能

public:
    UI();                                           // 构造函数，初始化成员变量
    void run();                                     // 启动主循环，显示菜单并响应用户操作
};

#endif