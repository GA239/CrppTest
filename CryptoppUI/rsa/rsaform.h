#ifndef RSAFORM_H
#define RSAFORM_H

#include <QWidget>
#include "mainwindow.h"

#include <qtextedit.h>
#include <qcheckbox.h>


#define CRYPTOPP_DEFAULT_NO_DLL
#include <cryptopp_builded/cryptopp/dll.h>
#ifdef CRYPTOPP_WIN32_AVAILABLE
#include <windows.h>
#endif

#include <cryptopp_builded/cryptopp/rsa.h>
//USING_NAMESPACE(CryptoPP)


namespace Ui {
class RsaForm;
}

class RsaForm : public QWidget
{
    Q_OBJECT

public:
    explicit RsaForm(MainWindow *parent = 0);
    ~RsaForm();

public slots:
    void openPrivateKeySlot();
    void openPublicKeySlot();
    void encrypte();


private:
    bool createPublicKey(QString pkey);
    bool createPrivateKey(QString pkey);

private:
    Ui::RsaForm *ui;
    CryptoPP::RSA::PublicKey pubk;
    CryptoPP::RSA::PrivateKey prik;

    QTextEdit *textedit;
};

#endif // RSAFORM_H
