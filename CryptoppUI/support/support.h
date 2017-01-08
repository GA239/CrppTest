#ifndef SUPPORT_H
#define SUPPORT_H

#include <QSettings>

#define CRYPTOPP_DEFAULT_NO_DLL
#include <cryptopp_builded/cryptopp/dll.h>
#ifdef CRYPTOPP_WIN32_AVAILABLE
#include <windows.h>
#endif

#include <sstream>


typedef enum
{ MAIN_FORM,
  RSA_FORM
} Forms;

class Support
{
public:
    Support();
    ~Support();

    CryptoPP::Integer qstrToInt(QString str);
    QString intToQStr(CryptoPP::Integer dec);
    QString showMetaInfo(CryptoPP::RSA::PublicKey pk);
    QString showMetaInfo(CryptoPP::RSA::PrivateKey prk);
    void message(QString str);

private:

};

#endif // SUPPORT_H
