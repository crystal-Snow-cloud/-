#include "Trie.h"
#include "PinyinConverter.h"
#include <fstream>
#include <algorithm>
//
//这个文件实现了 Trie 树的全部核心操作。

//一、构造函数：创建一个空的 Trie 树，根节点用 shared_ptr 管理。
Trie::Trie() : root(std::make_shared<TrieNode>()) {}

//二、析构函数：以下这个函数释放整棵 Trie 树的内存。
//因为所有节点都用 shared_ptr 相互引用，root.reset() 后子节点也会自动递归释放，无需手动遍历
Trie::~Trie() {
    // 智能指针会自动管理内存，不需要手动删除
    // 但为了确保正确释放，可以显式重置
    root.reset();
}

//三、返回根节点的 shared_ptr，      用于 UI 层或测试代码直接访问树结构（例如做遍历展示）
std::shared_ptr<TrieNode> Trie::getRoot() {
    return root;
}

//四、以下是核心插入函数。
//1.接收一个 Contact 智能指针和对应的“拼音首字母串”key。
//2.逐字符在 Trie 中创建 / 延伸路径，在终点节点标记 isEnd = true，并将联系人加入 contacts 列表。
//如果多个联系人共享同一拼音前缀，那么它们会共用路径，最终只在终点节点用列表区分。
void Trie::insert(std::shared_ptr<Contact> contact, const std::string& key) {
    if (!contact || key.empty()) return;

    auto node = root;

    for (char ch : key) {
        if (node->children.find(ch) == node->children.end()) {
            node->children[ch] = std::make_shared<TrieNode>();
        }
        node = node->children[ch];
    }

    node->isEnd = true;
    node->contacts.push_back(contact);
}


//五、按中文姓名精确查找。
//1.调用 PinyinConverter::getInitials(name) 得到首字母串（如 "LDH"）。
//2.沿路径走到对应节点。
//3.在该节点的 contacts 列表中逐一比对姓名，返回匹配的那一个。
//返回 shared_ptr<Contact>（联系人完整信息），未找到则返回 nullptr。
std::shared_ptr<Contact> Trie::searchByName(const std::string& name) {
    if (name.empty()) return nullptr;

    // 获取拼音首字母
    std::string initials = PinyinConverter::getInitials(name);
    auto node = root;

    // 遍历Trie树找到对应节点
    for (char ch : initials) {
        if (node->children.find(ch) == node->children.end()) {
            return nullptr; // 没有匹配的节点
        }
        node = node->children[ch];
    }

    // 在找到的节点中搜索具体联系人
    if (node->isEnd) {
        for (auto& contact : node->contacts) {
            if (contact && contact->name == name) {
                return contact;
            }
        }
    }
    return nullptr;
}


//六、按拼音首字母前缀模糊查找。
//1.先沿 prefix 走到目标节点。
//2.调用 collectContacts 递归收集该节点及其所有子树中的联系人。
//返回 vector<shared_ptr<Contact>>，包含所有匹配的联系人（例如输入 "L"，会得到所有姓为L的人）。
std::vector<std::shared_ptr<Contact>> Trie::searchByPrefix(const std::string& prefix) {
    std::vector<std::shared_ptr<Contact>> result;
    
    if (prefix.empty()) {
        // 如果前缀为空，返回所有联系人
        collectContacts(root, result);
        return result;
    }

    auto node = root;

    // 找到前缀对应的节点
    for (char ch : prefix) {
        if (node->children.find(ch) == node->children.end()) {
            return result; // 未找到匹配的前缀
        }
        node = node->children[ch];
    }

    // 收集该节点下的所有联系人
    collectContacts(node, result);
    return result;
}


//七、以下是一个递归辅助函数，用来收集子树中所有 isEnd 节点中的联系人。
//是用于前缀查询和统计总数的。
void Trie::collectContacts(std::shared_ptr<TrieNode> node,
    std::vector<std::shared_ptr<Contact>>& result) {
    if (!node) return;

    // 如果是终止节点，添加所有联系人
    if (node->isEnd) {
        for (auto& contact : node->contacts) {
            if (contact) {
                result.push_back(contact);
            }
        }
    }

    // 递归收集子节点的联系人
    for (auto& child : node->children) {
        if (child.second) {
            collectContacts(child.second, result);
        }
    }
}


//八、以下是删除联系人函数，步骤如下：
//1.沿 key 找到对应节点。
//2.使用 remove_if 删除 contacts 列表中姓名匹配的联系人。
//3.若列表变空，则将 isEnd 设为 false。
//没实现的一点：当前实现不回收空节点（因为用了智能指针，直接删掉节点较麻烦），智能指针自动释放内存，但功能上已正确移除联系人，不影响查询。
void Trie::remove(const std::string& key, const std::string& name) {
    if (key.empty() || name.empty()) return;

    auto node = root;

    // 找到对应节点
    for (char ch : key) {
        if (node->children.find(ch) == node->children.end()) {
            return; // 未找到
        }
        node = node->children[ch];
    }

    // 删除联系人
    if (node->isEnd) {
        auto& contacts = node->contacts;

        // 使用remove_if和erase删除指定姓名的联系人
        auto it = std::remove_if(contacts.begin(), contacts.end(),
            [&name](const std::shared_ptr<Contact>& c) {
                return c && c->name == name;
            });

        if (it != contacts.end()) {
            contacts.erase(it, contacts.end());
        }

        // 如果没有联系人了，标记为非终止节点
        if (contacts.empty()) {
            node->isEnd = false;
        }
    }
}

//九、以下这个函数将整棵 Trie 树的结构可视化写入文本文件。
//输入的文件包含根节点、层级、路径、词尾标记、联系人数量及具体姓名和电话。
//方便调试和展示（完全对应方案A的缩进层级文本）。
void Trie::saveToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件进行写入: " << filename << std::endl;
        return;
    }

    // 写入文件头
    file << "=== Trie树结构 ===\n";
    file << "格式: 节点层级 | 字符 | 是否为词尾 | 联系人列表\n";
    file << std::string(60, '=') << "\n\n";

    // 递归展示Trie树
    displayTrieNode(root, "", file, 0);

    file.close();
}

//十、以下这个是递归输出函数，被 saveToFile 调用。
//按缩进格式输出每个节点的路径、层级、联系人详情。（就像trie_structure.txt和trie_output.txt中的格式）
void Trie::displayTrieNode(std::shared_ptr<TrieNode> node,
    const std::string& prefix,
    std::ofstream& file,
    int level) {
    if (!node) return;

    // 显示当前节点信息
    if (!prefix.empty() || level == 0) {
        file << std::string(level * 2, ' ')
            << "层级: " << level
            << " | 路径: " << (prefix.empty() ? "根节点" : prefix);

        if (node->isEnd) {
            file << " | [词尾]";
            if (!node->contacts.empty()) {
                file << " | 联系人数量: " << node->contacts.size();
                for (auto& contact : node->contacts) {
                    if (contact) {
                        file << "\n" << std::string(level * 2 + 2, ' ')
                            << "- " << contact->name
                            << " (" << contact->phone << ")";
                    }
                }
            }
        }
        file << "\n";
    }

    // 递归显示子节点
    for (auto& child : node->children) {
        if (child.second) {
            displayTrieNode(child.second, prefix + child.first, file, level + 1);
        }
    }
}

//十一、下面这个函数从 CSV 格式的文件“重新构建” Trie 树。
//会清空当前树，逐行读取 姓名, 英文名, 电话, 拼音, 首字母 格式的记录，并重新插入。
//是用来程序启动时快速恢复之前保存的通讯录数据的。
void Trie::loadFromFile(const std::string& filename) {
    // 这个方法用于从之前保存的文件重新加载Trie树
    // 实际实现中，通常会直接重新构建Trie树
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return;
    }

    // 清空当前Trie树
    root = std::make_shared<TrieNode>();

    std::string line;
    while (std::getline(file, line)) {
        // 跳过注释行和分隔线
        if (line.empty() || line[0] == '=' || line.substr(0, 2) == "格式") {
            continue;
        }

        // 解析联系人信息
        std::stringstream ss(line);
        std::string name, englishName, phone, pinyin, initials;

        std::getline(ss, name, ',');
        std::getline(ss, englishName, ',');
        std::getline(ss, phone, ',');
        std::getline(ss, pinyin, ',');
        std::getline(ss, initials, ',');

        if (!name.empty() && !phone.empty()) {
            auto contact = std::make_shared<Contact>(name, englishName, phone, pinyin, initials);
            insert(contact, initials);
        }
    }

    file.close();
}


//十二、最后这个函数是用来收集全部联系人并返回总数的。
//可以用来显示当前通讯录规模。
size_t Trie::getSize() {
    std::vector<std::shared_ptr<Contact>> allContacts;
    collectContacts(root, allContacts);
    return allContacts.size();
}