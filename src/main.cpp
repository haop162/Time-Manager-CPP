#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

int main() {
    std::string taskName;
    std::string priority; // 用于给前端提供分类
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

        // --- 组长小优化：增加一个简单的优先级输入 ---
        std::cout << "Enter priority (Normal/Urgent): ";
        std::getline(std::cin, priority);
        if(priority.empty()) priority = "Normal";

    // --- 功能二：写入文件 ---
        std::ofstream outFile("tasks.txt", std::ios::app); 
        
        if (outFile.is_open()) {
            // 获取当前时间并处理换行符
            time_t now = time(0);
            
            // --- 核心改动：生成一个任务 ID 
            long long taskId = (long long)now; 

            char* dt = ctime(&now);
            std::string timeStr(dt);
            if (!timeStr.empty()) timeStr.erase(timeStr.length() - 1);

            // 写入带 ID 和时间戳的任务
            // 新格式：ID | 时间 | 优先级 | 内容
            outFile << taskId << " | " << timeStr << " | " << priority << " | " << taskName << std::endl; 
            
            outFile.close(); 
            std::cout << "SUCCESS: Task saved to disk!" << std::endl;
        } else {
            std::cout << "Error: Could not open file for writing!" << std::endl;
        }

        std::cout << "SUCCESS: Task [" << taskName << "] has been added." << std::endl;
    }

    return 0;
}