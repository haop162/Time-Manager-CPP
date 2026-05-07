#include <iostream>
#include <string>
#include <fstream>
#include <ctime>

// --- 核心工具：检查账号密码是否匹配，或者账号是否存在 ---
bool checkUser(std::string username, std::string password = "", int mode = 0) {
    std::ifstream inFile("users.txt");
    if (!inFile) return false; 

    std::string line;
    while (std::getline(inFile, line)) {
        size_t pos = line.find(" | ");
        if (pos != std::string::npos) {
            std::string u = line.substr(0, pos);
            std::string p = line.substr(pos + 3);
            if (mode == 0 && u == username) return true; // 账号已存在
            if (mode == 1 && u == username && p == password) return true; // 登录成功
        }
    }
    return false;
}

int main() {

    std::string user, pwd;
    bool loggedIn = false;

    std::cout << "--- Welcome to Student Time Manager ---" << std::endl;
    
    while (!loggedIn) {
        std::cout << "\n1. Login  2. Register  3. Exit\nChoose: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore(); // 清除回车

        if (choice == 3) return 0;

        std::cout << "Username: "; std::getline(std::cin, user);
        std::cout << "Password: "; std::getline(std::cin, pwd);

        if (choice == 1) { // 登录
            if (checkUser(user, pwd, 1)) {
                std::cout << "SUCCESS: Welcome, " << user << "!" << std::endl;
                loggedIn = true;
            } else {
                std::cout << "ERROR: Invalid username or password!" << std::endl;
            }
        } else if (choice == 2) { // 注册
            if (checkUser(user)) {
                std::cout << "ERROR: Username already exists!" << std::endl;
            } else {
                std::ofstream out("users.txt", std::ios::app);
                out << user << " | " << pwd << std::endl;
                out.close();
                std::cout << "SUCCESS: Registered! Please login." << std::endl;
            }
        }
    }

    // --- 登录成功后，才进入你原来的任务管理循环 ---


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