#include <iostream>
#include <string>

int main() {
    std::string taskName;
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "Time Manager: System Ready!" << std::endl;
    std::cout << "Please enter a new task name: ";
    
    // 获取用户输入的任务名
    std::getline(std::cin, taskName);
    
    std::cout << "SUCCESS: Task [" << taskName << "] has been added." << std::endl;
    std::cout << "-----------------------------------" << std::endl;
    
    return 0;
}