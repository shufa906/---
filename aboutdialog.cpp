#include "aboutdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("About");
    setFixedSize(360, 260);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("小牛文本编辑", this);
    QFont titleFont;
    titleFont.setPointSize(22);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *infoLabel = new QLabel(this);
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setText("开发者：XXX\n学号：XXXXXX\nemail：xxxxxx@qq.com");

    QLabel *versionLabel = new QLabel("V1.0 build 202611", this);
    versionLabel->setAlignment(Qt::AlignLeft);

    QPushButton *okButton = new QPushButton("OK", this);
    okButton->setMinimumWidth(90);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout *bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(versionLabel);
    bottomLayout->addStretch();
    bottomLayout->addWidget(okButton);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(infoLabel);
    mainLayout->addStretch();
    mainLayout->addLayout(bottomLayout);
}
