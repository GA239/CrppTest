#include "mainform.h"
#include "ui_mainform.h"

#include <QPushButton>
#include <QVBoxLayout>

MainForm::MainForm(QMainWindow *parent) :
    QWidget(parent->centralWidget()),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);
    QVBoxLayout *verLayout = new QVBoxLayout;
    this->setLayout(verLayout);

    QPushButton *moveRsaButon = new QPushButton(this);
    moveRsaButon->setText("rsa");
    this->layout()->addWidget(moveRsaButon);

    connect(moveRsaButon, SIGNAL(clicked()), parent, SLOT(moveToRsa()));
}

MainForm::~MainForm()
{
    delete ui;
}
