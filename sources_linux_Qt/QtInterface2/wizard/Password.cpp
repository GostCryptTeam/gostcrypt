#include "Password.h"


void Ui_Creation::Volume_Password()
{
    /*!< QGroupbox for Volume Password*/
    VolumePassword->setGeometry(QRect(400, 0, 400, 500));
    VolumePassword->setTitle(QString::fromUtf8(GetValueFromKey(xml,std::string("PASSWORD_TITLE")).c_str()));

    /*!< Pawword description*/
    PasswordDescription->setGeometry(QRect(10, 135, 380, 280));
    PasswordDescription->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("PASSWORD_HELP")).c_str()));
    PasswordDescription->setWordWrap(true);

    /*!< Text "Pawword" and "Confirm"*/
    Pass->setGeometry(QRect(10, 55, 80, 20));
    Pass->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDT_NEW_PASSWORD")).c_str()));

    ConfirmText->setGeometry(QRect(10, 100, 120, 20));
    ConfirmText->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDT_CONFIRM_PASSWORD")).c_str()));

    Password->setGeometry(QRect(140, 52, 240, 30));
    Password->setEchoMode(QLineEdit::Password);
    Password->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);
    Confirm->setGeometry(QRect(140, 94, 240, 30));
    Confirm->setEchoMode(QLineEdit::Password);
    Confirm->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);

}
