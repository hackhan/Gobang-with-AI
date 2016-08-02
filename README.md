Gobang_with_AI
=====
带有简易 AI 的五子棋游戏

界面采用开源图形库 ege 编写 
HomePage: https://github.com/misakamm/xege

AI 基于极大极小搜索和估值函数

### 编译说明：
        1. 开发环境 MinGW32(g++ version 4.9.2)
        2. 从 ege 官网获取并安装图形库：http://xege.org/
        3. 键入命令 make -f Makefile.release 即可完成编译

### 编译选项说明：
默认的 Makefile 是 Debug 版本，关键参数：
        -g
        -O0        // 禁用优化
        -DDEBUG    // 定义 DEBUG 宏，方便输出调试信息
Makefile.release 是 Release 版本，关键参数：
        -O2        // 采用二级优化
        -mwindows  // 去掉控制台窗口
均采用 gnu++11 标准

### 规则简介：
        1. 采用 15 × 15 的棋盘
        2. 采用指定开局规则，由黑方从 26 种开局中任选一种
        3. 白方有“三手可交换”权
        4. 黑方行驶五手 N 打规则
        5. 黑方有三三禁手、四四禁手、长连禁手

### 目前存在的问题：
        1. 极大极小搜索只能深入一层
        2. α-β 剪枝没有实现
        3. 无法判断对手五手 N 打形状是否对称
