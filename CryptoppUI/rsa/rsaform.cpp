#include "rsaform.h"
#include "ui_rsaform.h"

#include <QVBoxLayout>

#include <qfiledialog.h>
#include <qinputdialog.h>

#include <qtextstream.h>
#include <sstream>

RsaForm::RsaForm(MainWindow *parent) :
  QWidget(parent->centralWidget()),
    ui(new Ui::RsaForm)
{
    ui->setupUi(this);
    this->parent = parent;

    this->textedit = NULL;
    this->ActionButton = NULL;
    this->metainfo = NULL;
    Support sup;

    bool flag = false;
    while(!flag)
    {
        RsaDialog* form = new RsaDialog(this);
        if (form->exec() != QDialog::Accepted)
        {
            if(form->getResult() == KEYGEN)
                sup.message(keyGeneration());
            else
            {
                flag = true;
                if(form->getResult() == ENCRYPTE)
                    this->defaultUI(true,form->getResult());
                else if (form->getResult() == DECRYPTE)
                    this->defaultUI(true,form->getResult());
                else
                    this->defaultUI(false,form->getResult());
            }
        }
        else
            sup.message(QString::fromUtf8("Не работает"));
        delete form;
    }
}

void RsaForm::defaultUI(bool succes, RSAMOD mode)
{
    QVBoxLayout *leftLayout = new QVBoxLayout;
    QVBoxLayout *rightLayout = new QVBoxLayout;
    QLabel *mainlabel = new QLabel(this);
    QPushButton *returnButton = new QPushButton(this);
    returnButton->setText(QString::fromUtf8("Вернуться"));

    if(succes)
    {
        mainlabel->setText(QString::fromUtf8("Текст"));
        QPushButton *getKeyButton = new QPushButton(this);
        getKeyButton->setText(QString::fromUtf8("Открыть"));

        this->ActionButton = new QPushButton(this);
        this->ActionButton->setVisible(false);
        this->textedit = new QLineEdit(this);
        this->metainfo = new QLabel(this);
        this->metainfo->setText(QString::fromUtf8("Метаинформация: "));

        QLabel *label = new QLabel(this);
        if(mode == ENCRYPTE)
        {
            this->textedit->setMaxLength(15);
            label->setText(QString::fromUtf8("Открыть публичный ключ"));
            this->ActionButton->setText(QString::fromUtf8("Зашифровать"));
            connect(getKeyButton, SIGNAL(clicked()), this, SLOT(openPublicKeySlot()));
            connect(this->ActionButton, SIGNAL(clicked()), this, SLOT(encrypte()));
        }
        if(mode == DECRYPTE)
        {
            this->textedit->setReadOnly(true);
            label->setText(QString::fromUtf8("Открыть секретный ключ"));
            this->ActionButton->setText(QString::fromUtf8("Расшифровать"));
            connect(getKeyButton, SIGNAL(clicked()), this, SLOT(openPrivateKeySlot()));
            connect(this->ActionButton, SIGNAL(clicked()), this, SLOT(decrypte()));
        }
        leftLayout->addWidget(mainlabel);
        leftLayout->addWidget(this->textedit);
        leftLayout->addWidget(this->metainfo);
        leftLayout->addStretch(1);
        rightLayout->addWidget(label);
        rightLayout->addWidget(getKeyButton);
        rightLayout->addWidget(this->ActionButton);
        rightLayout->addStretch(1);
        rightLayout->addWidget(returnButton);

        connect(returnButton, SIGNAL(clicked()), this->parent, SLOT(moveToRsa()));
    }
    else
    {
        mainlabel->setText(QString::fromUtf8("Режим не выбран!"));
        connect(returnButton, SIGNAL(clicked()), this->parent, SLOT(moveToMain()));

        leftLayout->addWidget(mainlabel);
        leftLayout->addWidget(returnButton);
    }

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    return;
}

RsaForm::~RsaForm()
{
    if(this->textedit != NULL)
        delete this->textedit;
    if(this->ActionButton != NULL)
        delete this->ActionButton;
    if(this->metainfo != NULL)
        delete this->metainfo;
    delete ui;
}

///** encrypte/decrypte **///

void RsaForm::encrypte()
{
    Support sup;
    std::string message;
    CryptoPP::Integer m, c;

    message = this->textedit->text().toUtf8().constData();
    if(message.empty())
    {
        sup.message(QString::fromUtf8("Отсутствует текст"));
        return;
    }
    // Treat the message as a big endian array
    m = CryptoPP::Integer((const byte *)message.data(), message.size());

    QString fileName = QFileDialog::getSaveFileName(this,
                                QString::fromUtf8("Сохранить зашифрованный файл"),
                                QDir::homePath(),
                                "(*.rsae);;All files (*.*)");
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
    sup.message(QString::fromUtf8("Текст Зашифрован"));
}

void RsaForm::decrypte()
{
    Support sup;
    QString fileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Открыть зашифрованный файл"),
                                QDir::homePath(),
                                "(*.rsae);;All files (*.*)");
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

    std::string recovered;

    size_t req = r.MinEncodedSize();
    recovered.resize(req);
    r.Encode((byte *)recovered.data(), recovered.size());
    sup.message(QString::fromUtf8("Текст Расшифрован"));
    this->textedit->setText(QString::fromStdString(recovered));
}

///** get keys from file **///

void RsaForm::openPrivateKeySlot()
{
    Support sup;
    QString fileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Открыть файл"),
                                QDir::homePath(), "(*.prk);;All files (*.*)");
    if(!this->createPrivateKey(fileName))
    {
        if(this->prik.Validate(CryptoPP::NullRNG(),0))
        {
            this->ActionButton->setVisible(true);
            this->metainfo->setText(this->metainfo->text() + sup.showMetaInfo(this->prik));
            sup.message(QString::fromUtf8("Открыт приватный ключ с метаинформацией: \n") + sup.showMetaInfo(this->prik));
            return;
        }
    }
    this->ActionButton->setVisible(false);
    sup.message(QString::fromUtf8("ошибка ключа"));
    return;
}

void RsaForm::openPublicKeySlot()
{
    Support sup;
    QString fileName = QFileDialog::getOpenFileName(this,
                                QString::fromUtf8("Открыть файл"),
                                QDir::homePath(), "(*.pk);;All files (*.*)");
    if(!this->createPublicKey(fileName))
    {
        if(this->pubk.Validate(CryptoPP::NullRNG(),0))
        {
            this->ActionButton->setVisible(true);
            this->metainfo->setText(this->metainfo->text() + sup.showMetaInfo(this->pubk));
            sup.message(QString::fromUtf8("Открыт публичный ключ с метаинформацией: \n") + sup.showMetaInfo(this->pubk));
            return;
        }
    }
    this->ActionButton->setVisible(false);
    sup.message(QString::fromUtf8("ошибка ключа"));
    return;
}

///** create instances of CryptoPP::RSA::PrivateKey / CryptoPP::RSA::PublicKey **///

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
        return false;
    }
    delete settings;
    return true;
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
        return false;
    }
    delete settings;
    return true;
}

///** generate RSA PrivateKey/PublicKey **///

QString RsaForm::keyGeneration()
{
    bool ok;
    Support sup;
    CryptoPP::AutoSeededRandomPool rng;

    CryptoPP::RSA::PrivateKey privKey;

    //CryptoPP::Integer e("22245245460426616102189742625441633226801");
    QString strd;

    while ( (strd.size() > 20) || (strd.size() == 0) )
    {
        strd = QInputDialog::getText(this,
                                     QString::fromUtf8("Введите метаинформацию"),
                                     QString::fromUtf8("Введите метаинформацию:"),
                                     QLineEdit::Normal,
                                     QDir::home().dirName(), &ok);

        if (!ok)
            return QString::fromUtf8("Ошибка ввода метаинформации. Ключ не создан");

        if(strd.size() > 20)
            sup.message(QString::fromUtf8("длина строки должна быть меньше 20 символов"));

        if(strd.isEmpty())
            sup.message(QString::fromUtf8("длина строки должна быть больше 0"));
    }
    strd = RSAMASK + strd;

    //QString strd = "1RSA_Gavrilov_A";
    CryptoPP::Integer e = sup.qstrToInt(strd);

    privKey.Initialize(rng,255,e);
    CryptoPP::RSA::PublicKey pubKey(privKey);

    if(!privKey.Validate(rng,0))
        return QString::fromUtf8("Ошибка создания секретного ключа. Ключ не создан");

    if(!pubKey.Validate(rng,0))
        return QString::fromUtf8("Ошибка создания публичного ключа. Ключ не создан");

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
                               QString::fromUtf8("Выбрать каталог для сохранения"),
                               QDir::homePath(),
                               QFileDialog::ShowDirsOnly);
    if(dir.isEmpty())
        return QString::fromUtf8("Каталог не выбран. Ключ не создан.");

    QString name = dir + "/" + strd;

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

    QString msg = QString::fromUtf8("Ключи с метaинформацией: \n") +
                  strd + "\n" +
                  QString::fromUtf8("созданы по адресу: \n") +
                  name;
    return msg;
}
