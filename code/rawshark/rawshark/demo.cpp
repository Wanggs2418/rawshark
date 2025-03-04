#include <iostream>
#include <windows.h>

int main() {
    // 设置控制台输入和输出编码为UTF-8
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    std::cout << "Json如下：\t" << std::endl;
    return 0;
}