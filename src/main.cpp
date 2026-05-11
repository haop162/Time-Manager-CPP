#include "httplib.h"
#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <vector>

// 辅助函数：去掉字符串前后的空格
std::string trim(const std::string& s) {
    size_t first = s.find_first_not_of(" \r\n\t");
    if (first == std::string::npos) return "";
    size_t last = s.find_last_not_of(" \r\n\t");
    return s.substr(first, (last - first + 1));
}

// 用户处理辅助函数：mode 0 为查重，mode 1 为登录验证
std::string handleUser(std::string targetId, std::string targetPwd = "", int mode = 0) {
    std::ifstream inFile("users.txt");
    if (!inFile) return "";
    std::string line;
    targetId = trim(targetId);
    targetPwd = trim(targetPwd);

    while (std::getline(inFile, line)) {
        if (trim(line).empty()) continue;
        size_t pos1 = line.find(" | ");
        size_t pos2 = line.rfind(" | ");
        if (pos1 != std::string::npos && pos2 != std::string::npos && pos1 != pos2) {
            std::string u = trim(line.substr(0, pos1));
            std::string p = trim(line.substr(pos1 + 3, pos2 - (pos1 + 3)));
            std::string name = trim(line.substr(pos2 + 3));

            if (mode == 0 && u == targetId) return "exists";
            if (mode == 1 && u == targetId && p == targetPwd) return name;
        }
    }
    return "";
}

int main() {
    httplib::Server svr;

    // 通用跨域处理
    auto cors = [](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    };

    // 监听 OPTIONS 请求以处理浏览器预检
    svr.Options(R"(.*)", cors);

    // ==========================================
    // 1. 用户模块 (注册与登录)
    // ==========================================

    svr.Post("/register", [&](const httplib::Request& req, httplib::Response& res) {
        cors(req, res);
        std::regex uReg("\"username\":\"(.*?)\""), pReg("\"password\":\"(.*?)\""), nReg("\"realname\":\"(.*?)\"");
        std::smatch m;
        std::string u, p, n;
        if (std::regex_search(req.body, m, uReg)) u = m[1];
        if (std::regex_search(req.body, m, pReg)) p = m[1];
        if (std::regex_search(req.body, m, nReg)) n = m[1];

        if (handleUser(u) == "exists") {
            std::cout << ">>> 拦截到重复学号: " << u << std::endl;
            res.set_content("{\"status\":\"fail\",\"msg\":\"学号已存在\"}", "application/json");
        } else {
            std::ofstream out("users.txt", std::ios::app);
            out << u << " | " << p << " | " << n << std::endl;
            out.close();
            res.set_content("{\"status\":\"success\"}", "application/json");
        }
    });

    svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
        cors(req, res);
        std::regex uReg("\"username\":\"(.*?)\""), pReg("\"password\":\"(.*?)\"");
        std::smatch m;
        std::string u, p;
        if (std::regex_search(req.body, m, uReg)) u = m[1];
        if (std::regex_search(req.body, m, pReg)) p = m[1];

        std::string realName = handleUser(u, p, 1);
        if (!realName.empty()) {
            res.set_content("{\"status\":\"success\",\"name\":\"" + realName + "\"}", "application/json");
        } else {
            res.set_content("{\"status\":\"fail\"}", "application/json");
        }
    });

    // ==========================================
    // 2. 事务日程管理 (任务存储)
    // ==========================================

    svr.Post("/add_task", [&](const httplib::Request& req, httplib::Response& res) {
        cors(req, res);
        std::cout << ">>> 收到任务添加请求: " << req.body << std::endl;

        std::regex uReg("\"username\":\"(.*?)\""), tReg("\"task_name\":\"(.*?)\""), 
                   dReg("\"task_time\":\"(.*?)\""), pReg("\"priority\":\"(.*?)\"");
        std::smatch m;
        std::string u, task, time, prio;
        
        if (std::regex_search(req.body, m, uReg)) u = m[1];
        if (std::regex_search(req.body, m, tReg)) task = m[1];
        if (std::regex_search(req.body, m, dReg)) time = m[1];
        if (std::regex_search(req.body, m, pReg)) prio = m[1];

        if (u.empty() || task.empty()) {
            res.set_content("{\"status\":\"fail\"}", "application/json");
            return;
        }

        std::ofstream out("tasks.txt", std::ios::app);
        if (out.is_open()) {
            out << trim(u) << " | " << trim(task) << " | " << trim(time) << " | " << trim(prio) << std::endl;
            out.close();
            std::cout << ">>> 成功写入任务至 tasks.txt" << std::endl;
            res.set_content("{\"status\":\"success\"}", "application/json");
        } else {
            res.set_content("{\"status\":\"fail\",\"msg\":\"无法打开文件\"}", "application/json");
        }
    });

    svr.Get("/get_tasks", [&](const httplib::Request& req, httplib::Response& res) {
        cors(req, res);
        std::string targetUser = trim(req.get_param_value("username"));
        std::string jsonResult = "[";
        
        std::ifstream inFile("tasks.txt");
        std::string line;
        bool first = true;
        while (std::getline(inFile, line)) {
            if (trim(line).empty()) continue;
            
            size_t p1 = line.find(" | ");
            size_t p2 = line.find(" | ", p1 + 3);
            size_t p3 = line.find(" | ", p2 + 3);

            if (p1 != std::string::npos && p2 != std::string::npos && p3 != std::string::npos) {
                std::string u = trim(line.substr(0, p1));
                if (u == targetUser) {
                    if (!first) jsonResult += ",";
                    jsonResult += "{\"task_name\":\"" + trim(line.substr(p1 + 3, p2 - p1 - 3)) + 
                                  "\",\"task_time\":\"" + trim(line.substr(p2 + 3, p3 - p2 - 3)) + 
                                  "\",\"priority\":\"" + trim(line.substr(p3 + 3)) + "\"}";
                    first = false;
                }
            }
        }
        jsonResult += "]";
        res.set_content(jsonResult, "application/json");
    });

    // ==========================================
    // 3. 学习规划管理 (JSON 全量同步)
    // ==========================================

    svr.Post("/save_plans", [&](const httplib::Request& req, httplib::Response& res) {
        cors(req, res);
        
        std::regex uReg("\"username\":\"(.*?)\"");
        std::smatch m;
        std::string u;
        if (std::regex_search(req.body, m, uReg)) u = m[1];

        // 提取 JSON 数组逻辑：寻找第一个 [ 和最后一个 ] 之间的内容
        size_t first = req.body.find('[');
        size_t last = req.body.rfind(']');
        
        if (!u.empty() && first != std::string::npos && last != std::string::npos) {
            std::string plansData = req.body.substr(first, last - first + 1);
            
            // 为每个用户创建独立存储
            std::ofstream out("study_plans_" + u + ".txt");
            if (out.is_open()) {
                out << plansData;
                out.close();
                std::cout << ">>> 用户 " << u << " 的学习计划已同步" << std::endl;
                res.set_content("{\"status\":\"success\"}", "application/json");
            } else {
                res.set_content("{\"status\":\"fail\"}", "application/json");
            }
        } else {
            res.set_content("{\"status\":\"fail\",\"msg\":\"解析异常\"}", "application/json");
        }
    });

    svr.Get("/get_plans", [&](const httplib::Request& req, httplib::Response& res) {
        cors(req, res);
        std::string u = trim(req.get_param_value("username"));
        std::ifstream inFile("study_plans_" + u + ".txt");
        std::string content;
        if (inFile) {
            std::string line;
            while (std::getline(inFile, line)) content += line;
        } else {
            content = "[]"; 
        }
        res.set_content(content, "application/json");
    });

    // ---------------------------------------------------------
// 5. 删除任务接口 (添加在 get_tasks 之后)
// ---------------------------------------------------------
svr.Post("/delete_task", [&](const httplib::Request& req, httplib::Response& res) {
    cors(req, res);
    
    // 解析要删除的任务信息
    std::regex uReg("\"username\":\"(.*?)\""), nReg("\"task_name\":\"(.*?)\""), tReg("\"task_time\":\"(.*?)\"");
    std::smatch m;
    std::string u, taskName, taskTime;
    
    if (std::regex_search(req.body, m, uReg)) u = m[1];
    if (std::regex_search(req.body, m, nReg)) taskName = m[1];
    if (std::regex_search(req.body, m, tReg)) taskTime = m[1];

    if (u.empty() || taskName.empty()) {
        res.set_content("{\"status\":\"fail\"}", "application/json");
        return;
    }

    // 读取文件并过滤掉要删除的那一行
    std::ifstream inFile("tasks.txt");
    std::string line, newContent = "";
    bool found = false;

    while (std::getline(inFile, line)) {
        if (trim(line).empty()) continue;
        // 只有不匹配当前删除条件的行才保留
        // 匹配规则：学号 | 任务名 | 时间
        if (line.find(u) != std::string::npos && 
            line.find(taskName) != std::string::npos && 
            line.find(taskTime) != std::string::npos) {
            found = true;
            continue; // 跳过这一行，实现删除
        }
        newContent += line + "\n";
    }
    inFile.close();

    // 将新内容写回文件
    std::ofstream out("tasks.txt");
    out << newContent;
    out.close();

    if (found) {
        res.set_content("{\"status\":\"success\"}", "application/json");
    } else {
        res.set_content("{\"status\":\"fail\",\"msg\":\"未找到该任务\"}", "application/json");
    }
});


    svr.listen("0.0.0.0", 8080);
    return 0;
}