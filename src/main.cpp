#include "httplib.h"
#include <iostream>
#include <string>
#include <fstream>
#include <regex>

// 辅助函数：去掉字符串前后的空格
std::string trim(const std::string& s) {
    size_t first = s.find_first_not_of(" \r\n\t");
    if (first == std::string::npos) return "";
    size_t last = s.find_last_not_of(" \r\n\t");
    return s.substr(first, (last - first + 1));
}

std::string handleUser(std::string targetId, std::string targetPwd = "", int mode = 0) {
    std::ifstream inFile("users.txt");
    if (!inFile) return "";
    std::string line;
    targetId = trim(targetId);
    targetPwd = trim(targetPwd);

    while (std::getline(inFile, line)) {
        // 兼容性查重：只要包含这个学号且它是独立的列
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
    auto cors = [](const auto&, auto& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
    };
    svr.Options(R"(.*)", cors);

    svr.Post("/register", [&](const httplib::Request& req, httplib::Response& res) {
        cors(req, res);
        // 分别搜索，全部用 m[1] 保证准确
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

    std::cout << ">>> Server Solidified! Port 8080" << std::endl;
    svr.listen("0.0.0.0", 8080);
    return 0;
}