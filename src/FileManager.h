#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <vector>
#include <memory>
#include <string>
#include "Contact.h"
#include "Trie.h"

class FileManager {
public:
    //loadContacts— 从文件加载联系人
    static std::vector<std::shared_ptr<Contact>> loadContacts(const std::string& filename);
    //saveContacts— 保存联系人到文件
    static void saveContacts(const std::vector<std::shared_ptr<Contact>>& contacts,
        const std::string& filename);
    //按文件名读取并显示
    static void displayFirstN(const std::string& filename, int n = 100);
    //直接用内存数据显示
    static void displayFirstN(const std::vector<std::shared_ptr<Contact>>& contacts, int n);
};

#endif