#ifndef RSAFORM_H
#define RSAFORM_H

#include <QWidget>
#include "mainwindow.h"
#include "rsadialog.h"

#include <qlineedit.h>
#include <qlabel.h>
#include <QPushButton>

#include <cryptopp_builded/cryptopp/rsa.h>


#define RSAMASK "1RSA_"

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
    QString keyGeneration();
    bool createPrivateKey(QString pkey);
    void defaultUI(bool succes, RSAMOD mode);

private:
    Ui::RsaForm *ui;
    CryptoPP::RSA::PublicKey pubk;
    CryptoPP::RSA::PrivateKey prik;

    QLabel *metainfo;
    QLineEdit *textedit;
    QPushButton *ActionButton;
    MainWindow* parent;
};

#endif // RSAFORM_H
