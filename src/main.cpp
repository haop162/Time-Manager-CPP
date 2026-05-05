#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

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
            break;
        }

        // 清空功能
        if (taskName == "clear") {
            std::ofstream ofs("tasks.txt", std::ios::trunc);
            ofs.close();
            std::cout << "SUCCESS: All tasks cleared!" << std::endl;
            continue;
        }

        if (taskName.empty()) {
            std::cout << "Warning: Task name cannot be empty!" << std::endl;
            continue;
        }

    // --- 功能二：写入文件 ---
        std::ofstream outFile("tasks.txt", std::ios::app); 
        
        if (outFile.is_open()) {
            // 获取当前时间并处理换行符
            time_t now = time(0);
            char* dt = ctime(&now);
            std::string timeStr(dt);
            if (!timeStr.empty()) timeStr.pop_back(); 

            // 写入带时间戳的任务
            outFile << "[" << timeStr << "] " << taskName << std::endl; // 把任务写入文件
            outFile.close(); // 写完记得关门
            std::cout << "SUCCESS: Task saved to disk!" << std::endl;
        } else {
            std::cout << "Error: Could not open file for writing!" << std::endl;
        }

        std::cout << "SUCCESS: Task [" << taskName << "] has been added." << std::endl;
    }

    return 0;
}