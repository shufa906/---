#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class CodeEditor;
class FindDialog;
class ReplaceDialog;
class QPlainTextEdit;
class QLabel;
class QAction;
class QToolBar;
class QTimer;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadFile(const QString &fileName);
    void captureWindow(const QString &outPath);
    void openAboutDialog();
    void openFindDialog();
    void openReplaceDialog();
    void selectFirstLine();
    void markModified();
    void appendCharForDemo();
    void syncDialogGeometry();
    void fillDialogKeyword(const QString &keyword);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // 文件操作
    void onNewFile();
    void onOpenFile();
    bool onSaveFile();
    bool onSaveAsFile();

    // 编辑操作
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onSelectAll();

    // 格式
    void onChooseFont();
    void onChooseTextColor();
    void onChooseBackgroundColor();
    void onToggleWrap(bool checked);
    void onToggleLineNumber(bool checked);

    // 查看
    void onToggleToolBar(bool checked);
    void onToggleStatusBar(bool checked);

    // 对话框
    void onShowFindDialog();
    void onShowReplaceDialog();
    void onShowAboutDialog();

    // 查找/替换执行
    void doFindNext(const QString &text, bool up, bool caseSensitive);
    void doReplaceOne(const QString &search, const QString &replace,
                      bool up, bool caseSensitive);
    void doReplaceAll(const QString &search, const QString &replace,
                      bool caseSensitive);

    // 状态刷新
    void updateActionsState();
    void updateCursorStatus();
    void updateWindowTitle();
    void onAutoSave();

private:
    void setupActions();
    void setupMenusAndToolBar();
    bool maybeSave();
    bool setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullName);

    Ui::MainWindow *ui;
    CodeEditor     *m_editor;

    FindDialog     *m_findDialog;
    ReplaceDialog  *m_replaceDialog;

    QString         m_currentFile;
    bool            m_isModified;

    QLabel         *m_lengthLabel;
    QLabel         *m_linesLabel;
    QLabel         *m_cursorLabel;

    QAction *m_actionNew;
    QAction *m_actionOpen;
    QAction *m_actionSave;
    QAction *m_actionSaveAs;
    QAction *m_actionExit;

    QAction *m_actionUndo;
    QAction *m_actionRedo;
    QAction *m_actionCut;
    QAction *m_actionCopy;
    QAction *m_actionPaste;
    QAction *m_actionSelectAll;
    QAction *m_actionFind;
    QAction *m_actionReplace;

    QAction *m_actionWrap;
    QAction *m_actionLineNumber;
    QAction *m_actionFont;
    QAction *m_actionTextColor;
    QAction *m_actionBgColor;

    QAction *m_actionToggleToolBar;
    QAction *m_actionToggleStatusBar;
    QAction *m_actionAbout;

    QToolBar *m_toolBar;
    QTimer   *m_autoSaveTimer;
};

#endif // MAINWINDOW_H
