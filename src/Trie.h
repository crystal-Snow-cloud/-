#ifndef TRIE_H
#define TRIE_H

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include "Contact.h"

// TrieNode 类：Trie 树的节点
// 每个节点包含：
//   - children：子节点映射（字符 → 子节点）
//   - contacts：存储到达该节点时的联系人列表（仅 isEnd=true 时有意义）
//   - isEnd：标记该节点是否为一个完整拼音首字母串的结尾
class TrieNode {
public:
    std::unordered_map<char, std::shared_ptr<TrieNode>> children; // 子节点，键为拼音首字母（大写字母）
    std::vector<std::shared_ptr<Contact>> contacts;               // 存储该节点对应的联系人（仅 isEnd=true 时有效）
    bool isEnd;                                                    // 标记是否为词尾节点

    TrieNode() : isEnd(false) {}                                   // 构造函数，初始化为非词尾
};

// Trie 类：基于拼音首字母的前缀树
// 用于高效地进行联系人前缀匹配、精确查找、插入和删除
class Trie {
private:
    std::shared_ptr<TrieNode> root; // 根节点，不存储字符

    // 递归收集指定节点及其所有子树中的联系人
    // 用于前缀查询和获取全部联系人
    void collectContacts(std::shared_ptr<TrieNode> node,
        std::vector<std::shared_ptr<Contact>>& result);

    // 递归输出 Trie 树结构到文件（用于 saveToFile）
    void displayTrieNode(std::shared_ptr<TrieNode> node,
        const std::string& prefix,
        std::ofstream& file,
        int level);

public:
    Trie();                                    // 构造函数，创建空 Trie 树
    ~Trie();                                   // 析构函数，释放内存（智能指针自动管理）

    std::shared_ptr<TrieNode> getRoot();       // 获取根节点（供外部遍历使用）
    void insert(std::shared_ptr<Contact> contact, const std::string& key);  // 插入联系人，key 为拼音首字母串
    void remove(const std::string& key, const std::string& name);           // 删除指定姓名的联系人
    std::vector<std::shared_ptr<Contact>> searchByPrefix(const std::string& prefix); // 前缀模糊查询
    std::shared_ptr<Contact> searchByName(const std::string& name);                   // 按中文姓名精确查找
    void saveToFile(const std::string& filename);   // 将 Trie 树结构保存到文本文件
    void loadFromFile(const std::string& filename); // 从 CSV 格式文件加载并重建 Trie 树
    size_t getSize();                               // 返回 Trie 树中存储的联系人总数
};

#endif