#include "rsaform.h"
#include "ui_rsaform.h"

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
    this->parent = parent;
    RsaDialog* form = new RsaDialog(this);
    if (form->exec() != QDialog::Accepted)
    {
        int res = form->getResult();
        if(res == 3)
            this->defaultUI(true,res);
        else if (res == 4)
            this->defaultUI(true,res);
        else
            this->defaultUI(false,res);
    }
    else
        QMessageBox::information(this,QString::fromUtf8("Сообщение"),QString::fromUtf8("Не работает"));
    delete form;
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
    {
        if(this->pubk.Validate(CryptoPP::NullRNG(),0))
        {
            this->chbx->setCheckable(true);
            this->chbx->setChecked(true);
            this->chbx->setCheckable(false);
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
                                QDir::homePath(), "(*.key);;All files (*.*)");
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
    }
    else
    {
        mainlabel->setText(QString::fromUtf8("Не выбран режим"));
    }

    QPushButton *returnButton = new QPushButton(this);
    returnButton->setText(QString::fromUtf8("Вернуться"));
    connect(returnButton, SIGNAL(clicked()), this->parent, SLOT(moveToMain()));

    rightLayout->addWidget(returnButton);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(11);
    mainLayout->setSpacing(6);
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    return;
}
