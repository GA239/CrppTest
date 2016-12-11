#ifndef RSADIALOG_H
#define RSADIALOG_H

#include <QDialog>

namespace Ui {
class RsaDialog;
}

class RsaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RsaDialog(QWidget *parent = 0);
    ~RsaDialog();
    int getResult();

public slots:
    encr();
    decr();
    kg();
    bk();

private:
    Ui::RsaDialog *ui;
    int result;
};

#endif // RSADIALOG_H
