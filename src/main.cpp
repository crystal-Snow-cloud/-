#include "UI.h"
#include "PinyinConverter.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <clocale>
//这个main.cpp 是程序的入口点，负责环境初始化、控制台编码设置、拼音字典加载，然后启动 UI 主循环。



int main() {
    //下面这两行将控制台输出和输入的代码页设为 936（GBK 简体中文），确保中文在控制台正确显示和输入。
    SetConsoleOutputCP(936);
    SetConsoleCP(936);

    // 设置中文环境（设置 C 运行库的本地环境为中文简体），确保中文输出解析正常
    setlocale(LC_ALL, "chs");


    //下面都是用来优化的
    // 虚拟终端设置，用来性能拓展的
    //启用 Windows 控制台的虚拟终端序列
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }

    //提前加载汉字和拼音映射表（内置兜底 + 外部字典文件），防止首次查询时加载延迟。
    PinyinConverter::initialize();

    //下面这些都是启动信息
    std::cout << "========================================\n";
    std::cout << "    手机通讯录管理系统启动\n";
    std::cout << "    数据结构: Trie树 (前缀树)\n";
    std::cout << "    查询复杂度: O(L) L为前缀长度\n";
    std::cout << "========================================\n";

    //启动界面实现函数UI.cpp
    UI ui;
    ui.run();

    return 0;
}
