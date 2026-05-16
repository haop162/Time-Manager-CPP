# Time-Manager-CPP 🚀

这是一个由 C++ 驱动的时间管理工具。

## 👥 团队分工
-成员	角色	学号	        GitHub
彭金豪	组长	202405567123	@haop162	              主要职责：核心调度算法、项目架构、仓库管理

李宇超	 开发	202405567121	@dakebubi915	
主要职责：数据持久化、控制台UI、前端交互

谢俊杰	 开发	202405567116	@wuye-source	
主要职责：单元测试、边界校验、文档撰写

## 🛠️ 技术架构

- **后端**: C++ + cpp-httplib
- **通信协议**: HTTP (RESTful API)
- **数据存储**: `users.txt`、`tasks.txt`、`study_plans_*.txt`
- **前端**: HTML5 / JavaScript / CSS3

## 📂 接口约定

 接口           方法       说明 
 `/register`    POST      用户注册 
 `/login`       POST      用户登录 
 `/add_task`    POST      添加任务 
 `/get_tasks`   GET       获取任务列表 
 `/delete_task` POST      删除任务 
 `/save_plans`  POST      保存学习规划 
 `/get_plans`   GET       获取学习规划 

## 🚀 如何运行
1. ## 🚀 如何运行

1. 编译后端
```bash
g++ src/main.cpp -o main -lws2_32
2. 运行后端 ./main
3. 在浏览器中访问 http://localhost:8080（或终端显示的端口号）使用系统