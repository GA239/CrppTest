#include "rsadialog.h"
#include "ui_rsadialog.h"

#include <QPushButton>
#include <QVBoxLayout>

RsaDialog::RsaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RsaDialog)
{
    ui->setupUi(this);

    this->resize(parent->width()/2,parent->height()/2);
    QVBoxLayout *verLayout = new QVBoxLayout;
    this->setLayout(verLayout);

    QPushButton *Keygen = new QPushButton(this);
    Keygen->setText("Keygen");
    this->layout()->addWidget(Keygen);
    connect(Keygen, SIGNAL(clicked()), this, SLOT(kg()));

    QPushButton *EncrButton = new QPushButton(this);
    EncrButton->setText("encrypt");
    this->layout()->addWidget(EncrButton);
    connect(EncrButton, SIGNAL(clicked()), this, SLOT(encr()));

    QPushButton *DecrButton = new QPushButton(this);
    DecrButton->setText("decrypt");
    this->layout()->addWidget(DecrButton);
    connect(DecrButton, SIGNAL(clicked()), this, SLOT(decr()));

    QPushButton *returnButton = new QPushButton(this);
    returnButton->setText("cancel");
    this->layout()->addWidget(returnButton);
    connect(returnButton, SIGNAL(clicked()), this, SLOT(bk()));
}

RsaDialog::~RsaDialog()
{
    delete ui;
}

int RsaDialog::getResult()
{
    return this->result;
}

RsaDialog::encr()
{
    this->result = 3;
    reject();
}

RsaDialog::decr()
{
    this->result = 4;
    reject();
}

RsaDialog::kg()
{
    this->result = 5;
    reject();
}

RsaDialog::bk()
{
    this->result = 6;
    reject();
}
