#ifndef RSAFORM_H
#define RSAFORM_H

#include <QWidget>
#include "mainwindow.h"

#include <qtextedit.h>
#include <qcheckbox.h>
#include <QPushButton>


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
    void decrypte();


private:
    bool createPublicKey(QString pkey);
    bool keyGeneration();
    bool createPrivateKey(QString pkey);
    void defaultUI(bool succes, int mode);

private:
    Ui::RsaForm *ui;
    CryptoPP::RSA::PublicKey pubk;
    CryptoPP::RSA::PrivateKey prik;

    QTextEdit *textedit;
    QCheckBox* chbx;
    QPushButton *ActionButton;
    MainWindow* parent;
};

#endif // RSAFORM_H
