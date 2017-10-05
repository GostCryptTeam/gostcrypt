#ifndef SECURETEXTFIELD_H
#define SECURETEXTFIELD_H
#include <QQuickItem>

class SecureTextField : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
public:
    SecureTextField() {

    }
    ~SecureTextField() {

    }

    void setPassword(const QString aPwd) {
        mPwd = QByteArray::fromStdString(aPwd.toStdString());
    }

    QString password()
    {
        return QString(mPwd).fill(QChar('*'));
    }

signals:
    void passwordChanged();

private:
    QByteArray mPwd;
};

#endif // SECURETEXTFIELD_H
