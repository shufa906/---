#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

class QLineEdit;
class QCheckBox;
class QRadioButton;
class QPushButton;

/**
 * @brief 查找对话框：支持向上/向下、区分大小写、连续查找
 */
class FindDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindDialog(QWidget *parent = nullptr);

    QString searchText() const;
    void setSearchText(const QString &text);
    bool    caseSensitive() const;
    bool    searchUp() const;

signals:
    void findNext(const QString &text, bool up, bool caseSensitive);

private slots:
    void onFindClicked();

private:
    QLineEdit   *m_searchEdit;
    QCheckBox   *m_caseCheck;
    QRadioButton *m_upRadio;
    QRadioButton *m_downRadio;
    QPushButton *m_findButton;
};

#endif // FINDDIALOG_H
