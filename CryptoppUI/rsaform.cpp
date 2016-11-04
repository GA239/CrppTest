#include "rsaform.h"
#include "ui_rsaform.h"

RsaForm::RsaForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RsaForm)
{
    ui->setupUi(this);
}

RsaForm::~RsaForm()
{
    delete ui;
}
