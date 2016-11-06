#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mainform.h"
#include "rsa/rsaform.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->current_form = NULL;
    moveToForm(MAIN_FORM);
}

MainWindow::~MainWindow()
{
    delete this->current_form;
    delete ui;
}

///public slots:
void MainWindow::moveToRsa()
{
    this->moveToForm(RSA_FORM);
    return;
}

void MainWindow::moveToMain()
{
    this->moveToForm(MAIN_FORM);
    return;
}


///private:
void MainWindow::moveToForm(Forms f)
{
    if(this->current_form != NULL)
    {
        delete this->current_form;
        this->current_form = NULL;
    }
    switch ( f )
    {
         case MAIN_FORM:
            this->current_form = new MainForm(this);
            break;
         case RSA_FORM:
            this->current_form = new RsaForm(this);
            break;
         default:
            this->current_form = new MainForm(this);
    }
    this->setCentralWidget(this->current_form);
    this->current_form->show();
    return;
}
