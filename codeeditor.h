#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class LineNumberArea;

/**
 * @brief 从 QPlainTextEdit 派生的编辑器，支持左侧行号显示
 * 参考 Qt 官方 Code Editor 示例实现
 */
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  lineNumberAreaWidth();
    void setLineNumberVisible(bool visible);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *m_lineNumberArea;
};

/**
 * @brief 行号绘制区域
 */
class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *editor);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditor *m_codeEditor;
};

#endif // CODEEDITOR_H
