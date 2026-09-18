#include "Contact.h"
//这个Contact 类是通讯录的数据模型，包含姓名、英文名、电话、拼音和首字母。
//同时也提供了 display 方法用于屏幕输出。
//同时重载了 == 和 < 运算符，分别用于重复检测和容器排序。
// Trie 树的节点中存储的就是 Contact 的智能指针。

// Contact类的构造函数实现如下：
Contact::Contact(): name(""), englishName(""), phone(""), pinyin(""), pinyinInitials("") {
}

Contact::Contact(std::string n, std::string en, std::string p, std::string py, std::string pi)
    : name(n), englishName(en), phone(p), pinyin(py), pinyinInitials(pi) {
}

// 显示联系人信息
void Contact::display() const {
    std::cout << "姓名: " << name
        << " | 英文名: " << englishName
        << " | 电话: " << phone
        << " | 拼音: " << pinyin
        << " | 首字母: " << pinyinInitials << std::endl;
}

// 获取格式化的联系人信息字符串
std::string Contact::toString() const {
    return name + "," + englishName + "," + phone + "," + pinyin + "," + pinyinInitials;
}

// 比较两个联系人是否相等（基于姓名和电话号码）
bool Contact::operator==(const Contact& other) const {
    return name == other.name && phone == other.phone;
}

// 为了在容器中使用，所以要提供小于运算符
bool Contact::operator<(const Contact& other) const {
    if (name != other.name) {
        return name < other.name;
    }
    return phone < other.phone;
}