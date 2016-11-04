#include "rsaform.h"
#include "ui_rsaform.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <qlabel.h>

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <sstream>
#include "rsadialog.h"

RsaForm::RsaForm(MainWindow *parent) :
  QWidget(parent->centralWidget()),
    ui(new Ui::RsaForm)
{
    ui->setupUi(this);

    QLabel *mainlabel = new QLabel(this);
    QLabel *label = new QLabel(this);
    QCheckBox* chbx = new QCheckBox(this);
    chbx->setText(QString::fromUtf8("ключ прочитан"));

    QPushButton *getKeyButton = new QPushButton(this);
    getKeyButton->setText(QString::fromUtf8("Открыть"));

    QPushButton *returnButton = new QPushButton(this);
    returnButton->setText(QString::fromUtf8("Вернуться"));
    connect(returnButton, SIGNAL(clicked()), parent, SLOT(moveToMain()));

    QPushButton *EncryptButton = new QPushButton(this);
    EncryptButton->setText(QString::fromUtf8("Зашифровать"));
    connect(EncryptButton, SIGNAL(clicked()), this, SLOT(encrypte()));

    mainlabel->setText(QString::fromUtf8("Главный текст"));
    this->textedit = new QTextEdit(this);


    RsaDialog* form = new RsaDialog(this);
    if (form->exec() != QDialog::Accepted)
    {
        int res = form->getResult();
        if(res == 3)
        {
            label->setText(QString::fromUtf8("Открыть публичный ключ"));
            connect(getKeyButton, SIGNAL(clicked()), this, SLOT(openPublicKeySlot()));

        }
        else if (res == 4)
        {
            label->setText(QString::fromUtf8("Открыть секретнвй ключ"));
            connect(getKeyButton, SIGNAL(clicked()), this, SLOT(openPrivateKeySlot()));
        }
        else
        {
            label->setText(QString::fromUtf8("Ничего не выбрано ... "));
        }
    }
    else
    {
        label->setText(QString::fromUtf8("Не работает"));
    }
    delete form;

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(mainlabel);
    leftLayout->addWidget(textedit);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(label);
    rightLayout->addWidget(getKeyButton);
    rightLayout->addWidget(chbx);
    rightLayout->addWidget(EncryptButton);
    rightLayout->addStretch(1);
    rightLayout->addWidget(returnButton);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
}

RsaForm::~RsaForm()
{
    delete this->textedit;
    delete ui;
}

void RsaForm::openPublicKeySlot()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Открыть файл"),
                                QDir::homePath(), "(*.key);;All files (*.*)");
    if(!this->createPublicKey(fileName))
        QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("ключ прочитан"));
    else
        QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("ошибка ключа"));
    return;
}

void RsaForm::encrypte()
{
    CryptoPP::AutoSeededRandomPool prng;

    /////////////////////////////////////////////////////////
    //CryptoPP::RSA::PublicKey pubKey = createPublicKey("sd");
    ///pubKey.Initialize(n, e);

    //RSA::PrivateKey privKey = createPrivateKey("sd");
    ///privKey.Initialize(n, e, d);
    /////////////////////////////////////////////////////////

    std::string message;
    CryptoPP::Integer m, c;

    //message = "hello my name is andrew. Itis very secret message thanksssssstds";
    message = this->textedit->toPlainText().toUtf8().constData();

    // Treat the message as a big endian array
    m = CryptoPP::Integer((const byte *)message.data(), message.size());

    QString fileName = QFileDialog::getSaveFileName(this,
                                QString::fromUtf8("Сохранить файл"),
                                QDir::currentPath(),
                                "(*.result);;All files (*.*)");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
             return;

    // Encrypt
    c = this->pubk.ApplyFunction(m);

    std::stringstream ss;
    ss << std::hex /*if required*/ << c;
    std::string s = ss.str();
    QString qstr = QString::fromStdString(s);

    QTextStream out(&file);
    out << qstr ;
    file.close();
    QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("Текст Зашифрован"));

}

void RsaForm::openPrivateKeySlot()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Открыть файл"),
                                QDir::homePath(), "(*.key);;All files (*.*)");
    if(!this->createPrivateKey(fileName))
        QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("ключ прочитан"));
    else
        QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("ошибка ключа"));
    return;
}

bool RsaForm::createPublicKey(QString pkey)
{
    QSettings* settings = new QSettings(pkey,QSettings::IniFormat);
    if ( settings->contains("public/n") && settings->contains("public/e") && !settings->contains("private/d"))
    {
        CryptoPP::Integer n(settings->value("public/n",0).toString().toUtf8().constData());
        CryptoPP::Integer e(settings->value("public/e",0).toString().toUtf8().constData());
        this->pubk.Initialize(n, e);
        delete settings;
        return 0;
    }
    delete settings;
    return 1;
}

bool RsaForm::createPrivateKey(QString pkey)
{
    QSettings* settings = new QSettings(pkey,QSettings::IniFormat);
    if ( settings->contains("public/n") && settings->contains("public/e") && settings->contains("private/d"))
    {
        CryptoPP::Integer n(settings->value("public/n",0).toString().toUtf8().constData());
        CryptoPP::Integer e(settings->value("public/e",0).toString().toUtf8().constData());
        CryptoPP::Integer d(settings->value("private/d",0).toString().toUtf8().constData());
        this->prik.Initialize(n, e, d);
        delete settings;
        return 0;
    }
    delete settings;
    return 1;
}
