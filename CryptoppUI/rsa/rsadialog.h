#ifndef RSADIALOG_H
#define RSADIALOG_H

#include <QDialog>

typedef enum
{
    ENCRYPTE,
    DECRYPTE,
    KEYGEN,
    RETURN

}RSAMOD;

namespace Ui {
class RsaDialog;
}

class RsaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RsaDialog(QWidget *parent = 0);
    ~RsaDialog();
    RSAMOD getResult();

public slots:
    encr();
    decr();
    kg();
    bk();

private:
    Ui::RsaDialog *ui;
    RSAMOD result;
};

#endif // RSADIALOG_H
