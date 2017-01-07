#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "support/support.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void moveToRsa();
    void moveToMain();

private:
    void moveToForm(Forms f = MAIN_FORM);

protected:

private:
    Ui::MainWindow *ui;
    QWidget *current_form;
};

#endif // MAINWINDOW_H
