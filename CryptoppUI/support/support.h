#ifndef SUPPORT_H
#define SUPPORT_H

#include <QSettings>
#include <QVariant>

typedef enum
{ MAIN_FORM,
  RSA_FORM
} Forms;

class Support
{
public:
    Support();
    ~Support();

    QSettings* settings;

private:

};

#endif // SUPPORT_H
