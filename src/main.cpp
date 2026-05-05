#include <iostream>
#include <string>
#include <fstream>

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

    // --- 功能二：写入文件 ---
        std::ofstream outFile("tasks.txt", std::ios::app); 
        
        if (outFile.is_open()) {
            outFile << taskName << std::endl; // 把任务写入文件
            outFile.close(); // 写完记得关门
            std::cout << "SUCCESS: Task saved to disk!" << std::endl;
        } else {
            std::cout << "Error: Could not open file for writing!" << std::endl;
        }

        std::cout << "SUCCESS: Task [" << taskName << "] has been added." << std::endl;
    }

    return 0;
}