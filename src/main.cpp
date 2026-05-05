#include <iostream>
#include <string>

int main() {
    std::string taskName;
    std::cout << "--- Time Manager v1.1 ---" << std::endl;
    std::cout << "(Type 'exit' to quit the program)" << std::endl;

    // 功能一：使用 while 循环让程序持续运行
    while (true) {
        std::cout << "\nPlease enter a new task name: ";
        std::getline(std::cin, taskName);

        // 检查用户是否想退出
        if (taskName == "exit") {
            std::cout << "Goodbye! Program closing..." << std::endl;
            break; // 跳出循环，结束程序
        }

        if (taskName.empty()) {
            std::cout << "Warning: Task name cannot be empty!" << std::endl;
            continue;
        }

        std::cout << "SUCCESS: Task [" << taskName << "] has been added." << std::endl;
    }

    return 0;
}