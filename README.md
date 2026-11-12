# 实验2 简单文本编辑器（Qt 5.12.11）

基于 Qt Widgets 实现的多窗口文本编辑器，对应课程《Qt 应用程序开发》实验二。

## 功能清单

- 文件操作：新建、打开、保存、另存为；未保存关闭时弹出提示；异常文件给出错误提示
- 编辑操作：撤销、重做、剪切、复制、粘贴、全选；按钮使能状态随选中文本/撤销栈自动更新
- 查找 / 替换：向上 / 向下、区分大小写、查找下一个、替换一处、全部替换；未找到给出提示
- 格式设置：字体、字体颜色、编辑器背景色、自动换行
- 行号显示：从 `QPlainTextEdit` 派生 `CodeEditor`，左侧绘制行号，可开关
- 状态栏：实时显示字符数、行数、光标行列位置
- 标题同步：根据文件修改状态在标题前加 `*`
- 选做：30 秒定时自动保存（仅对已保存过的文件生效）
- 对话框：关于、查找、替换

## 目录结构

```
Experiment2-TextEditor/
├── TextEditor.pro          # qmake 工程文件
├── main.cpp                # 程序入口
├── mainwindow.h/.cpp/.ui   # 主窗口、菜单、工具栏、状态栏
├── codeeditor.h/.cpp       # 带行号的编辑器控件
├── aboutdialog.h/.cpp      # 关于对话框
├── finddialog.h/.cpp        # 查找对话框
├── replacedialog.h/.cpp    # 替换对话框
├── docs/
│   ├── sample.txt          # 演示用示例文本
│   ├── capture_window.py   # 窗口截图辅助脚本（PrintWindow）
│   └── make_screenshots.ps1# 批量生成截图脚本
└── screenshots/             # 运行结果截图
```

## 编译运行

环境：Windows + Qt 5.12.11（MinGW 7.3.0 64-bit）。

### Qt Creator
1. 用 Qt Creator 打开 `TextEditor.pro`
2. 选择 Desktop Qt 5.12.11 MinGW 64-bit 套件
3. 点击“构建”，再点击“运行”

### 命令行（MinGW）

```bat
qmake TextEditor.pro
mingw32-make
debug\TextEditor.exe
```

## 命令行截图参数（调试用，不影响正常运行）

```bat
TextEditor.exe --load docs\sample.txt --shot screenshots\01.png --delay 600
TextEditor.exe --dialog about
TextEditor.exe --dialog find
TextEditor.exe --dialog replace
TextEditor.exe --select 1
TextEditor.exe --modified 1
```

## 测试边界场景

1. 新建文件后直接关闭：弹出“是否保存更改”提示
2. 打开不存在或无读取权限的文件：状态栏 / 弹窗提示无法打开
3. 查找一个不存在的关键词：提示“未找到”
4. 未选中文字时剪切 / 复制按钮置灰；撤销 / 重做按钮随历史栈置灰
5. 全部替换后弹窗提示替换数量
