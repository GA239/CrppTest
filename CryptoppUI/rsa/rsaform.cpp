#include "rsaform.h"
#include "ui_rsaform.h"

#include <QVBoxLayout>
#include <qlabel.h>

#include <qfiledialog.h>
#include <qinputdialog.h>

#include <qmessagebox.h>
#include <qtextstream.h>
#include <sstream>
#include "rsadialog.h"

RsaForm::RsaForm(MainWindow *parent) :
  QWidget(parent->centralWidget()),
    ui(new Ui::RsaForm)
{
    ui->setupUi(this);
    this->parent = parent;
    //this->pubk = NULL;
    //this->prik = NULL;

    this->textedit = NULL;
    this->chbx = NULL;
    this->ActionButton = NULL;

    bool flag = false;
    int res = 0;
    while(!flag)
    {
        RsaDialog* form = new RsaDialog(this);
        if (form->exec() != QDialog::Accepted)
        {
            res = form->getResult();
            if(res == 5)
            {
                if(!this->keyGeneration())
                {
                    QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("Ключ не создан"));
                }
            }
            else
            {
                flag = true;
                if(res == 3)
                    this->defaultUI(true,res);
                else if (res == 4)
                    this->defaultUI(true,res);
                else
                    this->defaultUI(false,res);
            }
        }
        else
            QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("Не работает"));
        delete form;
    }
}

RsaForm::~RsaForm()
{
    if(this->textedit != NULL)
        delete this->textedit;
    if(this->chbx != NULL)
        delete this->chbx;
    if(this->ActionButton != NULL)
        delete this->ActionButton;
    delete ui;
}

void RsaForm::openPublicKeySlot()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Открыть файл"),
                                QDir::homePath(), "(*.pk);;All files (*.*)");
    if(!this->createPublicKey(fileName))
    {
        if(this->pubk.Validate(CryptoPP::NullRNG(),0))
        {
            this->chbx->setCheckable(true);
            this->chbx->setChecked(true);
            this->ActionButton->setVisible(true);
            QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("ключ прочитан"));
            return;
        }
    }
    this->chbx->setCheckable(true);
    this->chbx->setChecked(false);
    this->chbx->setCheckable(false);
    this->ActionButton->setVisible(false);
    QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("ошибка ключа"));
    return;
}

void RsaForm::encrypte()
{


    std::string message;
    CryptoPP::Integer m, c;

    //message = "hello my name is andrew. Itis very secret message thanksssssstds";
    message = this->textedit->toPlainText().toUtf8().constData();
    if(message.empty())
    {
        QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("Отсутствует текст"));
        return;
    }
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

void RsaForm::decrypte()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Открыть файл"),
                                QDir::currentPath(),
                                "(*.result);;All files (*.*)");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
             return;

    QString qstr;
    QTextStream in(&file);
    in >> qstr;
    file.close();

    CryptoPP::Integer c = CryptoPP::Integer(qstr.toUtf8().constData());

    CryptoPP::AutoSeededRandomPool prng;
    // Decrypt
    CryptoPP::Integer r = this->prik.CalculateInverse(prng, c);
    //cout << "r: " << hex << r << endl;

    std::string recovered;

    size_t req = r.MinEncodedSize();
    recovered.resize(req);
    r.Encode((byte *)recovered.data(), recovered.size());
    QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("Текст Расшифрован"));
    this->textedit->setText(QString::fromStdString(recovered));
}

void RsaForm::openPrivateKeySlot()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Открыть файл"),
                                QDir::homePath(), "(*.prk);;All files (*.*)");
    if(!this->createPrivateKey(fileName))
    {
        if(this->prik.Validate(CryptoPP::NullRNG(),0))
        {
            this->chbx->setCheckable(true);
            this->chbx->setChecked(true);
            this->ActionButton->setVisible(true);
            QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("ключ прочитан"));
            return;
        }
    }
    this->chbx->setCheckable(true);
    this->chbx->setChecked(false);
    this->chbx->setCheckable(false);
    this->ActionButton->setVisible(false);
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

bool RsaForm::keyGeneration()
{
    ///////////////////////////////////////
    // Pseudo Random Number Generator
    CryptoPP::AutoSeededRandomPool rng;
    ///////////////////////////////////////
    bool ok;
    int i = QInputDialog::getInt(this, QString::fromUtf8("Введите длину ключа"),
                             QString::fromUtf8("длина:"), 1024, 0, 10000, 1024, &ok);
    if(!ok)
        return false;

    if(i > 11000)
    {
        QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("довольно длинный ключик получается..Давайте поменьше"));
        return false;
    }
    // Generate Parameters
    CryptoPP::InvertibleRSAFunction params;
    params.GenerateRandomWithKeySize(rng, i);


    /*
    ///////////////////////////////////////
    // Generated Parameters
    const CryptoPP::Integer& n = params.GetModulus();
    const CryptoPP::Integer& p = params.GetPrime1();
    const CryptoPP::Integer& q = params.GetPrime2();
    const CryptoPP::Integer& d = params.GetPrivateExponent();
    const CryptoPP::Integer& e = params.GetPublicExponent();

    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::RSA::PrivateKey privKey;
    privKey.GenerateRandomWithKeySize(sr, 2039);
    CryptoPP::RSA::PublicKey pubKey(privKey);
    */

    CryptoPP::RSA::PrivateKey privKey(params);
    CryptoPP::RSA::PublicKey pubKey(params);
    if(!privKey.Validate(rng,0))
    {
        QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("ошибка создания секретного ключа"));
        return false;
    }
    if(!pubKey.Validate(rng,0))
    {
        QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("ошибка создания публичного ключа"));
        return false;
    }
    std::stringstream ss1;
    ss1 <<  pubKey.GetModulus();
    std::string s = ss1.str();
    QString nstr = QString::fromStdString(s);
    std::stringstream ss2;
    ss2 << pubKey.GetPublicExponent();
    s = ss2.str();
    QString estr = QString::fromStdString(s);
    std::stringstream s3;
    s3 << privKey.GetPrivateExponent();
    s = s3.str();
    QString dstr = QString::fromStdString(s);
    std::stringstream s4;
    s4 << privKey.GetPrime1();
    s = s4.str();
    QString pstr = QString::fromStdString(s);
    std::stringstream s5;
    s5 << privKey.GetPrime2();
    s = s5.str();
    QString qstr = QString::fromStdString(s);

    QString dir = QFileDialog::getExistingDirectory(this,
                               QString::fromUtf8("Открыть папку"),
                               QDir::currentPath(),
                               QFileDialog::ShowDirsOnly
                               | QFileDialog::DontResolveSymlinks);

    QString text = QInputDialog::getText(this,
                                 QString::fromUtf8("Ведите имя файла ключа"),
                                 QString::fromUtf8("Введите значение имени файла ключа:"),
                                 QLineEdit::Normal,
                                 QDir::home().dirName(), &ok);

    if (ok && !text.isEmpty())
    {
        QString name = dir + "/" + text;
        QString pkname = name + ".pk";
        QSettings settpk(pkname, QSettings::IniFormat);
        settpk.setValue("public/n", nstr);
        settpk.setValue("public/e", estr);
        QString prkname = name + ".prk";
        QSettings settpr(prkname, QSettings::IniFormat);
        settpr.setValue("public/n", nstr);
        settpr.setValue("public/e", estr);
        settpr.setValue("private/d", dstr);
        settpr.setValue("private/p", pstr);
        settpr.setValue("private/q", qstr);
        QMessageBox::information(this,QString::fromUtf8("Сообщение"),("Ключи созданы  " + name));
        return true;
    }
    return false;
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

void RsaForm::defaultUI(bool succes, int mode)
{
    QVBoxLayout *leftLayout = new QVBoxLayout;
    QVBoxLayout *rightLayout = new QVBoxLayout;
    QLabel *mainlabel = new QLabel(this);
    leftLayout->addWidget(mainlabel);
    QPushButton *returnButton = new QPushButton(this);
    returnButton->setText(QString::fromUtf8("Вернуться"));

    if(succes)
    {
        mainlabel->setText(QString::fromUtf8("Текст"));

        this->chbx = new QCheckBox(this);
        this->chbx->setText(QString::fromUtf8("ключ прочитан"));
        this->chbx->setCheckable(false);

        QPushButton *getKeyButton = new QPushButton(this);
        getKeyButton->setText(QString::fromUtf8("Открыть"));

        this->ActionButton = new QPushButton(this);
        this->ActionButton->setVisible(false);
        this->textedit = new QTextEdit(this);
        QLabel *label = new QLabel(this);
        if(mode == 3)
        {
            label->setText(QString::fromUtf8("Открыть публичный ключ"));
            this->ActionButton->setText(QString::fromUtf8("Зашифровать"));
            connect(getKeyButton, SIGNAL(clicked()), this, SLOT(openPublicKeySlot()));
            connect(this->ActionButton, SIGNAL(clicked()), this, SLOT(encrypte()));
        }
        if(mode == 4)
        {
            this->textedit->setReadOnly(true);
            label->setText(QString::fromUtf8("Открыть секретный ключ"));
            this->ActionButton->setText(QString::fromUtf8("Расшифровать"));
            connect(getKeyButton, SIGNAL(clicked()), this, SLOT(openPrivateKeySlot()));
            connect(this->ActionButton, SIGNAL(clicked()), this, SLOT(decrypte()));
        }
        leftLayout->addWidget(this->textedit);
        rightLayout->addWidget(label);
        rightLayout->addWidget(getKeyButton);
        rightLayout->addWidget(this->chbx);
        rightLayout->addWidget(this->ActionButton);
        rightLayout->addStretch(1);
        connect(returnButton, SIGNAL(clicked()), this->parent, SLOT(moveToRsa()));
    }
    else
    {
        mainlabel->setText(QString::fromUtf8("Не выбран режим"));
        connect(returnButton, SIGNAL(clicked()), this->parent, SLOT(moveToMain()));
    }

    rightLayout->addWidget(returnButton);
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    return;
}
