#include "replacedialog.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>

ReplaceDialog::ReplaceDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Replace");
    setFixedSize(400, 210);

    m_searchEdit  = new QLineEdit(this);
    m_replaceEdit = new QLineEdit(this);
    m_caseCheck   = new QCheckBox("区分大小写", this);
    m_upRadio     = new QRadioButton("向上(U)", this);
    m_downRadio   = new QRadioButton("向下(D)", this);
    m_downRadio->setChecked(true);

    m_findButton        = new QPushButton("查找下一个(F)", this);
    m_replaceButton     = new QPushButton("替换(R)", this);
    m_replaceAllButton  = new QPushButton("全部替换(A)", this);
    QPushButton *cancelButton = new QPushButton("取消", this);

    connect(m_findButton, &QPushButton::clicked, this, &ReplaceDialog::onFindClicked);
    connect(m_replaceButton, &QPushButton::clicked, this, &ReplaceDialog::onReplaceClicked);
    connect(m_replaceAllButton, &QPushButton::clicked, this, &ReplaceDialog::onReplaceAllClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    QGroupBox *dirGroup = new QGroupBox("方向", this);
    QHBoxLayout *dirLayout = new QHBoxLayout(dirGroup);
    dirLayout->addWidget(m_upRadio);
    dirLayout->addWidget(m_downRadio);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("查找目标：", m_searchEdit);
    formLayout->addRow("替换为：", m_replaceEdit);
    formLayout->addRow(dirGroup);
    formLayout->addRow(m_caseCheck);

    QVBoxLayout *rightButtons = new QVBoxLayout;
    rightButtons->addWidget(m_findButton);
    rightButtons->addWidget(m_replaceButton);
    rightButtons->addWidget(m_replaceAllButton);
    rightButtons->addWidget(cancelButton);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(formLayout, 1);
    mainLayout->addLayout(rightButtons);
}

QString ReplaceDialog::searchText() const
{
    return m_searchEdit->text();
}

void ReplaceDialog::setSearchText(const QString &text)
{
    m_searchEdit->setText(text);
}

QString ReplaceDialog::replaceText() const
{
    return m_replaceEdit->text();
}

bool ReplaceDialog::caseSensitive() const
{
    return m_caseCheck->isChecked();
}

bool ReplaceDialog::searchUp() const
{
    return m_upRadio->isChecked();
}

void ReplaceDialog::onFindClicked()
{
    if (m_searchEdit->text().isEmpty()) {
        QMessageBox::information(this, "提示", "请输入查找目标。");
        return;
    }
    emit findNext(m_searchEdit->text(), m_upRadio->isChecked(),
                  m_caseCheck->isChecked());
}

void ReplaceDialog::onReplaceClicked()
{
    if (m_searchEdit->text().isEmpty()) {
        QMessageBox::information(this, "提示", "请输入查找目标。");
        return;
    }
    emit replaceOne(m_searchEdit->text(), m_replaceEdit->text(),
                    m_upRadio->isChecked(), m_caseCheck->isChecked());
}

void ReplaceDialog::onReplaceAllClicked()
{
    if (m_searchEdit->text().isEmpty()) {
        QMessageBox::information(this, "提示", "请输入查找目标。");
        return;
    }
    emit replaceAll(m_searchEdit->text(), m_replaceEdit->text(),
                     m_caseCheck->isChecked());
}
