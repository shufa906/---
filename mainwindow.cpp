#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "codeeditor.h"
#include "aboutdialog.h"
#include "finddialog.h"
#include "replacedialog.h"

#include <QMenuBar>
#include <QStyle>
#include <QLabel>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QTextStream>
#include <QFile>
#include <QFontDialog>
#include <QColorDialog>
#include <QKeySequence>
#include <QTimer>
#include <QTextCursor>
#include <QTextDocument>
#include <QCloseEvent>
#include <QActionGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_findDialog(nullptr)
    , m_replaceDialog(nullptr)
    , m_isModified(false)
{
    ui->setupUi(this);

    // 中心编辑器：使用派生类 CodeEditor（支持行号）
    m_editor = new CodeEditor(this);
    setCentralWidget(m_editor);

    // 状态栏标签
    m_lengthLabel  = new QLabel("length: 0", this);
    m_linesLabel   = new QLabel("lines: 1", this);
    m_cursorLabel  = new QLabel("Ln: 1  Col: 1", this);
    statusBar()->addPermanentWidget(m_lengthLabel);
    statusBar()->addPermanentWidget(m_linesLabel);
    statusBar()->addPermanentWidget(m_cursorLabel);

    setupActions();
    setupMenusAndToolBar();

    // 信号连接
    connect(m_editor, &CodeEditor::modificationChanged,
            this, [this](bool changed) {
        m_isModified = changed;
        updateWindowTitle();
        updateActionsState();
    });
    connect(m_editor, &CodeEditor::cursorPositionChanged,
            this, &MainWindow::updateCursorStatus);
    connect(m_editor, &CodeEditor::textChanged,
            this, &MainWindow::updateCursorStatus);
    connect(m_editor, &CodeEditor::copyAvailable,
            this, &MainWindow::updateActionsState);
    connect(m_editor, &CodeEditor::undoAvailable,
            this, &MainWindow::updateActionsState);
    connect(m_editor, &CodeEditor::redoAvailable,
            this, &MainWindow::updateActionsState);

    // 选做：定时自动保存（30 秒），仅对已打开过的文件生效
    m_autoSaveTimer = new QTimer(this);
    connect(m_autoSaveTimer, &QTimer::timeout, this, &MainWindow::onAutoSave);
    m_autoSaveTimer->start(30000);

    resize(900, 640);
    updateWindowTitle();
    updateActionsState();
    updateCursorStatus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ---------------------------------------------------------------------------
// Action 初始化
// ---------------------------------------------------------------------------

void MainWindow::setupActions()
{
    const QStyle *style = this->style();

    // 文件
    m_actionNew = new QAction(style->standardIcon(QStyle::SP_FileIcon), "新建(&N)", this);
    m_actionNew->setShortcut(QKeySequence::New);
    m_actionNew->setStatusTip("新建文本文件");
    connect(m_actionNew, &QAction::triggered, this, &MainWindow::onNewFile);

    m_actionOpen = new QAction(style->standardIcon(QStyle::SP_DirOpenIcon), "打开(&O)", this);
    m_actionOpen->setShortcut(QKeySequence::Open);
    m_actionOpen->setStatusTip("打开已有文本文件");
    connect(m_actionOpen, &QAction::triggered, this, &MainWindow::onOpenFile);

    m_actionSave = new QAction(style->standardIcon(QStyle::SP_DialogSaveButton), "保存(&S)", this);
    m_actionSave->setShortcut(QKeySequence::Save);
    m_actionSave->setStatusTip("保存当前文件");
    connect(m_actionSave, &QAction::triggered, this, &MainWindow::onSaveFile);

    m_actionSaveAs = new QAction(style->standardIcon(QStyle::SP_DialogSaveButton), "另存为(&A)", this);
    m_actionSaveAs->setStatusTip("将文件另存为");
    connect(m_actionSaveAs, &QAction::triggered, this, &MainWindow::onSaveAsFile);

    m_actionExit = new QAction("退出(&X)", this);
    connect(m_actionExit, &QAction::triggered, this, &QWidget::close);

    // 编辑
    m_actionUndo = new QAction(style->standardIcon(QStyle::SP_ArrowBack), "撤销(&U)", this);
    m_actionUndo->setShortcut(QKeySequence::Undo);
    connect(m_actionUndo, &QAction::triggered, this, &MainWindow::onUndo);

    m_actionRedo = new QAction(style->standardIcon(QStyle::SP_ArrowForward), "恢复(&R)", this);
    m_actionRedo->setShortcut(QKeySequence::Redo);
    connect(m_actionRedo, &QAction::triggered, this, &MainWindow::onRedo);

    m_actionCut = new QAction(style->standardIcon(QStyle::SP_TrashIcon), "剪切(&T)", this);
    m_actionCut->setShortcut(QKeySequence::Cut);
    connect(m_actionCut, &QAction::triggered, this, &MainWindow::onCut);

    m_actionCopy = new QAction(style->standardIcon(QStyle::SP_DialogResetButton), "复制(&C)", this);
    m_actionCopy->setShortcut(QKeySequence::Copy);
    connect(m_actionCopy, &QAction::triggered, this, &MainWindow::onCopy);

    m_actionPaste = new QAction(style->standardIcon(QStyle::SP_DialogSaveButton), "粘贴(&P)", this);
    m_actionPaste->setShortcut(QKeySequence::Paste);
    connect(m_actionPaste, &QAction::triggered, this, &MainWindow::onPaste);

    m_actionSelectAll = new QAction("全选(&A)", this);
    connect(m_actionSelectAll, &QAction::triggered, this, &MainWindow::onSelectAll);

    m_actionFind = new QAction(style->standardIcon(QStyle::SP_BrowserReload), "查找(&F)", this);
    m_actionFind->setShortcut(QKeySequence::Find);
    connect(m_actionFind, &QAction::triggered, this, &MainWindow::onShowFindDialog);

    m_actionReplace = new QAction(style->standardIcon(QStyle::SP_BrowserReload), "替换(&R)", this);
    m_actionReplace->setShortcut(QKeySequence::Replace);
    connect(m_actionReplace, &QAction::triggered, this, &MainWindow::onShowReplaceDialog);

    // 格式
    m_actionWrap = new QAction("自动换行(&W)", this);
    m_actionWrap->setCheckable(true);
    m_actionWrap->setChecked(true);
    connect(m_actionWrap, &QAction::toggled, this, &MainWindow::onToggleWrap);

    m_actionLineNumber = new QAction("显示行号(&L)", this);
    m_actionLineNumber->setShortcut(QKeySequence("Ctrl+L"));
    m_actionLineNumber->setCheckable(true);
    m_actionLineNumber->setChecked(true);
    connect(m_actionLineNumber, &QAction::toggled, this, &MainWindow::onToggleLineNumber);

    m_actionFont = new QAction("字体(&F)", this);
    connect(m_actionFont, &QAction::triggered, this, &MainWindow::onChooseFont);

    m_actionTextColor = new QAction(style->standardIcon(QStyle::SP_CommandLink), "字体颜色", this);
    connect(m_actionTextColor, &QAction::triggered, this, &MainWindow::onChooseTextColor);

    m_actionBgColor = new QAction("编辑器背景色", this);
    connect(m_actionBgColor, &QAction::triggered, this, &MainWindow::onChooseBackgroundColor);

    // 查看
    m_actionToggleToolBar = new QAction("工具栏(&T)", this);
    m_actionToggleToolBar->setCheckable(true);
    m_actionToggleToolBar->setChecked(true);
    connect(m_actionToggleToolBar, &QAction::toggled, this, &MainWindow::onToggleToolBar);

    m_actionToggleStatusBar = new QAction("状态栏(&S)", this);
    m_actionToggleStatusBar->setCheckable(true);
    m_actionToggleStatusBar->setChecked(true);
    connect(m_actionToggleStatusBar, &QAction::toggled, this, &MainWindow::onToggleStatusBar);

    // 帮助
    m_actionAbout = new QAction("关于(&A)", this);
    connect(m_actionAbout, &QAction::triggered, this, &MainWindow::onShowAboutDialog);
}

void MainWindow::setupMenusAndToolBar()
{
    // 菜单：文件
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    fileMenu->addAction(m_actionNew);
    fileMenu->addAction(m_actionOpen);
    fileMenu->addAction(m_actionSave);
    fileMenu->addAction(m_actionSaveAs);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actionExit);

    // 菜单：编辑
    QMenu *editMenu = menuBar()->addMenu("编辑(&E)");
    editMenu->addAction(m_actionUndo);
    editMenu->addAction(m_actionRedo);
    editMenu->addSeparator();
    editMenu->addAction(m_actionCut);
    editMenu->addAction(m_actionCopy);
    editMenu->addAction(m_actionPaste);
    editMenu->addSeparator();
    editMenu->addAction(m_actionFind);
    editMenu->addAction(m_actionReplace);
    editMenu->addSeparator();
    editMenu->addAction(m_actionSelectAll);

    // 菜单：格式
    QMenu *formatMenu = menuBar()->addMenu("格式(&O)");
    formatMenu->addAction(m_actionWrap);
    formatMenu->addAction(m_actionLineNumber);
    formatMenu->addSeparator();
    formatMenu->addAction(m_actionFont);
    formatMenu->addAction(m_actionTextColor);
    formatMenu->addAction(m_actionBgColor);

    // 菜单：查看
    QMenu *viewMenu = menuBar()->addMenu("查看(&V)");
    viewMenu->addAction(m_actionToggleToolBar);
    viewMenu->addAction(m_actionToggleStatusBar);

    // 菜单：帮助
    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");
    helpMenu->addAction(m_actionAbout);

    // 工具栏（按功能分组）
    m_toolBar = addToolBar("主工具栏");
    m_toolBar->setObjectName("mainToolBar");
    m_toolBar->setMovable(true);

    m_toolBar->addAction(m_actionNew);
    m_toolBar->addAction(m_actionOpen);
    m_toolBar->addAction(m_actionSave);
    m_toolBar->addAction(m_actionSaveAs);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionUndo);
    m_toolBar->addAction(m_actionRedo);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionCut);
    m_toolBar->addAction(m_actionCopy);
    m_toolBar->addAction(m_actionPaste);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionFind);
    m_toolBar->addAction(m_actionReplace);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_actionLineNumber);
    m_toolBar->addAction(m_actionTextColor);
}

// ---------------------------------------------------------------------------
// 文件操作
// ---------------------------------------------------------------------------

void MainWindow::onNewFile()
{
    if (!maybeSave())
        return;

    m_editor->clear();
    m_currentFile.clear();
    m_isModified = false;
    setCurrentFile(QString());
    updateWindowTitle();
}

void MainWindow::onOpenFile()
{
    if (!maybeSave())
        return;

    const QString fileName = QFileDialog::getOpenFileName(
        this, "打开文件", QString(),
        "文本文件 (*.txt *.md *.cpp *.h *.pro *.ui);;所有文件 (*.*)");

    if (fileName.isEmpty())
        return;

    loadFile(fileName);
}

bool MainWindow::onSaveFile()
{
    if (m_currentFile.isEmpty())
        return onSaveAsFile();
    return setCurrentFile(m_currentFile) ? true : onSaveAsFile();
}

bool MainWindow::onSaveAsFile()
{
    const QString fileName = QFileDialog::getSaveFileName(
        this, "另存为", QString(),
        "文本文件 (*.txt);;所有文件 (*.*)");

    if (fileName.isEmpty())
        return false;

    return setCurrentFile(fileName);
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "打开失败",
                             QString("无法打开文件：%1\n错误原因：%2")
                                 .arg(fileName, file.errorString()));
        return;
    }

    // 异常/不存在文件已在上面拦截；读取内容
    const QByteArray data = file.readAll();
    file.close();

    QTextStream in(data);
    in.setCodec("UTF-8");
    const QString text = in.readAll();

    m_editor->setPlainText(text);
    m_currentFile = fileName;
    m_isModified = false;
    m_editor->document()->setModified(false);

    setCurrentFile(fileName);
    updateWindowTitle();
    statusBar()->showMessage("文件已打开：" + fileName, 3000);
}

bool MainWindow::setCurrentFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "保存失败",
                             QString("无法写入文件：%1\n错误原因：%2")
                                 .arg(fileName, file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << m_editor->toPlainText();
    file.close();

    m_currentFile = fileName;
    m_isModified = false;
    m_editor->document()->setModified(false);

    updateWindowTitle();
    statusBar()->showMessage("文件已保存：" + fileName, 3000);
    return true;
}

bool MainWindow::maybeSave()
{
    if (!m_editor->document()->isModified())
        return true;

    const QMessageBox::StandardButton ret = QMessageBox::warning(
        this, "未保存提示",
        QString("文档“%1”已被修改，是否保存更改？")
            .arg(strippedName(m_currentFile)),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
        return onSaveFile();
    if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

// ---------------------------------------------------------------------------
// 编辑操作
// ---------------------------------------------------------------------------

void MainWindow::onUndo()      { m_editor->undo(); }
void MainWindow::onRedo()      { m_editor->redo(); }
void MainWindow::onCut()      { m_editor->cut(); }
void MainWindow::onCopy()     { m_editor->copy(); }
void MainWindow::onPaste()     { m_editor->paste(); }
void MainWindow::onSelectAll() { m_editor->selectAll(); }

// ---------------------------------------------------------------------------
// 格式操作
// ---------------------------------------------------------------------------

void MainWindow::onChooseFont()
{
    bool ok = false;
    const QFont font = QFontDialog::getFont(&ok, m_editor->font(), this);
    if (ok)
        m_editor->setFont(font);
}

void MainWindow::onChooseTextColor()
{
    const QColor color = QColorDialog::getColor(m_editor->palette().color(QPalette::Text),
                                                 this, "选择字体颜色");
    if (color.isValid()) {
        QTextCharFormat fmt;
        fmt.setForeground(color);
        m_editor->mergeCurrentCharFormat(fmt);
    }
}

void MainWindow::onChooseBackgroundColor()
{
    const QColor color = QColorDialog::getColor(m_editor->palette().color(QPalette::Base),
                                                this, "选择编辑器背景色");
    if (color.isValid()) {
        QPalette pal = m_editor->palette();
        pal.setColor(QPalette::Base, color);
        m_editor->setPalette(pal);
    }
}

void MainWindow::onToggleWrap(bool checked)
{
    m_editor->setLineWrapMode(checked ? QPlainTextEdit::WidgetWidth
                                      : QPlainTextEdit::NoWrap);
}

void MainWindow::onToggleLineNumber(bool checked)
{
    m_editor->setLineNumberVisible(checked);
}

// ---------------------------------------------------------------------------
// 查看
// ---------------------------------------------------------------------------

void MainWindow::onToggleToolBar(bool checked)
{
    m_toolBar->setVisible(checked);
}

void MainWindow::onToggleStatusBar(bool checked)
{
    statusBar()->setVisible(checked);
}

// ---------------------------------------------------------------------------
// 对话框
// ---------------------------------------------------------------------------

void MainWindow::onShowFindDialog()
{
    if (!m_findDialog) {
        m_findDialog = new FindDialog(this);
        connect(m_findDialog, &FindDialog::findNext,
                this, &MainWindow::doFindNext);
    }
    m_findDialog->show();
    m_findDialog->raise();
    m_findDialog->activateWindow();
}

void MainWindow::onShowReplaceDialog()
{
    if (!m_replaceDialog) {
        m_replaceDialog = new ReplaceDialog(this);
        connect(m_replaceDialog, &ReplaceDialog::findNext,
                this, &MainWindow::doFindNext);
        connect(m_replaceDialog, &ReplaceDialog::replaceOne,
                this, &MainWindow::doReplaceOne);
        connect(m_replaceDialog, &ReplaceDialog::replaceAll,
                this, &MainWindow::doReplaceAll);
    }
    m_replaceDialog->show();
    m_replaceDialog->raise();
    m_replaceDialog->activateWindow();
}

void MainWindow::onShowAboutDialog()
{
    AboutDialog *dialog = new AboutDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}

// ---------------------------------------------------------------------------
// 查找 / 替换实现
// ---------------------------------------------------------------------------

void MainWindow::doFindNext(const QString &text, bool up, bool caseSensitive)
{
    if (text.isEmpty())
        return;

    QTextCursor cursor = m_editor->textCursor();
    QTextDocument::FindFlags flags;
    if (caseSensitive)
        flags |= QTextDocument::FindCaseSensitively;
    if (up)
        flags |= QTextDocument::FindBackward;

    QTextCursor found = up ? m_editor->document()->find(text, cursor, flags)
                           : m_editor->document()->find(text, cursor, flags);

    // 连续查找：找不到时从另一端继续
    if (found.isNull()) {
        QTextCursor start = m_editor->textCursor();
        start.movePosition(up ? QTextCursor::End : QTextCursor::Start);
        found = m_editor->document()->find(text, start, flags);
    }

    if (found.isNull()) {
        QMessageBox::information(this, "查找", QString("未找到“%1”。").arg(text));
        return;
    }

    m_editor->setTextCursor(found);
    m_editor->setFocus();
}

void MainWindow::doReplaceOne(const QString &search, const QString &replace,
                              bool up, bool caseSensitive)
{
    if (search.isEmpty())
        return;

    QTextCursor cursor = m_editor->textCursor();
    if (cursor.selectedText() == search) {
        cursor.insertText(replace);
    }

    doFindNext(search, up, caseSensitive);
}

void MainWindow::doReplaceAll(const QString &search, const QString &replace,
                              bool caseSensitive)
{
    if (search.isEmpty())
        return;

    QTextDocument::FindFlags flags;
    if (caseSensitive)
        flags |= QTextDocument::FindCaseSensitively;

    int count = 0;
    QTextCursor cursor(m_editor->document());
    cursor.beginEditBlock();

    while (!cursor.isNull()) {
        cursor = m_editor->document()->find(search, cursor, flags);
        if (cursor.isNull())
            break;
        cursor.insertText(replace);
        ++count;
    }

    cursor.endEditBlock();

    QMessageBox::information(this, "全部替换",
                             QString("共替换 %1 处。").arg(count));
}

// ---------------------------------------------------------------------------
// 状态刷新
// ---------------------------------------------------------------------------

void MainWindow::updateActionsState()
{
    const bool hasSelection = m_editor->textCursor().hasSelection();

    m_actionCut->setEnabled(hasSelection);
    m_actionCopy->setEnabled(hasSelection);
    m_actionPaste->setEnabled(m_editor->canPaste());
    m_actionUndo->setEnabled(m_editor->document()->isUndoAvailable());
    m_actionRedo->setEnabled(m_editor->document()->isRedoAvailable());
}

void MainWindow::updateCursorStatus()
{
    const QTextCursor cursor = m_editor->textCursor();
    const int line = cursor.blockNumber() + 1;
    const int col  = cursor.columnNumber() + 1;

    const int length = m_editor->toPlainText().length();
    const int lines  = m_editor->blockCount();

    m_lengthLabel->setText(QString("length: %1").arg(length));
    m_linesLabel->setText(QString("lines: %1").arg(lines));
    m_cursorLabel->setText(QString("Ln: %1  Col: %2").arg(line).arg(col));
}

void MainWindow::updateWindowTitle()
{
    const QString name = strippedName(m_currentFile);
    const QString title = (m_isModified ? "*" : "")
                          + (name.isEmpty() ? "Untitled" : name)
                          + " - Editor";
    setWindowTitle(title);
}

void MainWindow::onAutoSave()
{
    // 选做：仅对已保存过、且有修改的文件自动保存
    if (!m_currentFile.isEmpty() && m_editor->document()->isModified()) {
        setCurrentFile(m_currentFile);
        statusBar()->showMessage("已定时自动保存", 3000);
    }
}

// ---------------------------------------------------------------------------
// 事件与工具
// ---------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
}

QString MainWindow::strippedName(const QString &fullName)
{
    return QFileInfo(fullName).fileName();
}

void MainWindow::openAboutDialog()
{
    onShowAboutDialog();
}

void MainWindow::openFindDialog()
{
    onShowFindDialog();
}

void MainWindow::openReplaceDialog()
{
    onShowReplaceDialog();
}

void MainWindow::syncDialogGeometry()
{
    QApplication::processEvents();
    const QWidgetList all = findChildren<QWidget *>();
    for (QWidget *w : all) {
        if (w->isWindow() && w != this) {
            w->adjustSize();
            w->raise();
            w->activateWindow();
        }
    }
    QApplication::processEvents();
}

void MainWindow::selectFirstLine()
{
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::Start);
    cursor.select(QTextCursor::LineUnderCursor);
    m_editor->setTextCursor(cursor);
}

void MainWindow::markModified()
{
    m_editor->document()->setModified(true);
    m_isModified = true;
    updateWindowTitle();
}

void MainWindow::appendCharForDemo()
{
    QTextCursor cursor = m_editor->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(" ");
}

void MainWindow::fillDialogKeyword(const QString &keyword)
{
    if (m_findDialog)
        m_findDialog->setSearchText(keyword);
    if (m_replaceDialog)
        m_replaceDialog->setSearchText(keyword);
}

void MainWindow::captureWindow(const QString &outPath)
{
    // 保证布局完成后再抓取
    repaint();
    QPixmap shot(size());
    shot.fill(Qt::white);
    render(&shot);
    shot.save(outPath);
}
