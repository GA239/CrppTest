#include "support.h"
#include <qmessagebox.h>

Support::Support()
{}

Support::~Support()
{}

CryptoPP::Integer Support::qstrToInt(QString str)
{
    //qDebug() << str;

    QString cstr = str.toLatin1();
    std::reverse(cstr.begin(), cstr.end());
    str = cstr;

    QString hexstr = str.toLocal8Bit().toHex();
    hexstr = "0x" + hexstr;
    //cout << " hexstr: " << hexstr.toUtf8().constData()<< endl;

    CryptoPP::Integer dec(hexstr.toUtf8().constData());
    //cout << "Integer dec: " << dec << endl;
    return dec;
}

QString Support::intToQStr(CryptoPP::Integer dec)
{
    //cout << "Integer dec: " << dec << endl;
    std::stringstream ss;
    ss << std::hex <<  dec;

    std::string s = ss.str();
    QString hexqstr = QString::fromStdString(s);
    //cout << " hexqstr: " << hexqstr.toUtf8().constData()<< endl;

    QString qstr(QByteArray::fromHex(hexqstr.toLatin1()));

    QString cstr = qstr.toLatin1();
    std::reverse(cstr.begin(), cstr.end());
    qstr = cstr;
    //qDebug() << qstr;

    return qstr;
}

QString Support::showMetaInfo(CryptoPP::RSA::PublicKey pk)
{
    return this->intToQStr(pk.GetPublicExponent());
}
QString Support::showMetaInfo(CryptoPP::RSA::PrivateKey prk)
{
    return this->intToQStr(prk.GetPublicExponent());
}

void Support::message(QString str)
{
    QMessageBox::information(0,QString::fromUtf8("Сообщение"),str);
}



