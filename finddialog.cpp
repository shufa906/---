#include "finddialog.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Find");
    setFixedSize(380, 180);

    m_searchEdit = new QLineEdit(this);
    m_caseCheck  = new QCheckBox("区分大小写", this);
    m_upRadio    = new QRadioButton("向上(U)", this);
    m_downRadio  = new QRadioButton("向下(D)", this);
    m_downRadio->setChecked(true);

    m_findButton = new QPushButton("查找下一个(F)", this);
    QPushButton *cancelButton = new QPushButton("取消", this);
    cancelButton->setDefault(true);

    connect(m_findButton, &QPushButton::clicked, this, &FindDialog::onFindClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    QGroupBox *dirGroup = new QGroupBox("方向", this);
    QHBoxLayout *dirLayout = new QHBoxLayout(dirGroup);
    dirLayout->addWidget(m_upRadio);
    dirLayout->addWidget(m_downRadio);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("查找目标：", m_searchEdit);
    formLayout->addRow(dirGroup);
    formLayout->addRow(m_caseCheck);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_findButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
}

QString FindDialog::searchText() const
{
    return m_searchEdit->text();
}

void FindDialog::setSearchText(const QString &text)
{
    m_searchEdit->setText(text);
}

bool FindDialog::caseSensitive() const
{
    return m_caseCheck->isChecked();
}

bool FindDialog::searchUp() const
{
    return m_upRadio->isChecked();
}

void FindDialog::onFindClicked()
{
    if (m_searchEdit->text().isEmpty()) {
        QMessageBox::information(this, "提示", "请输入查找目标。");
        return;
    }
    emit findNext(m_searchEdit->text(), m_upRadio->isChecked(),
                  m_caseCheck->isChecked());
}
