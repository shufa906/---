#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;
class QRadioButton;
class QPushButton;

/**
 * @brief 替换对话框：查找下一个、替换、全部替换
 */
class ReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReplaceDialog(QWidget *parent = nullptr);

    QString searchText() const;
    void setSearchText(const QString &text);
    QString replaceText() const;
    bool    caseSensitive() const;
    bool    searchUp() const;

signals:
    void findNext(const QString &text, bool up, bool caseSensitive);
    void replaceOne(const QString &search, const QString &replace,
                    bool up, bool caseSensitive);
    void replaceAll(const QString &search, const QString &replace,
                    bool caseSensitive);

private slots:
    void onFindClicked();
    void onReplaceClicked();
    void onReplaceAllClicked();

private:
    QLineEdit   *m_searchEdit;
    QLineEdit   *m_replaceEdit;
    QCheckBox   *m_caseCheck;
    QRadioButton *m_upRadio;
    QRadioButton *m_downRadio;
    QPushButton *m_findButton;
    QPushButton *m_replaceButton;
    QPushButton *m_replaceAllButton;
};

#endif // REPLACEDIALOG_H
