#ifndef SECURETEXTFIELD_H
#define SECURETEXTFIELD_H
#include <QApplication>
#include <QSharedPointer>

class SecureTextField : public QObject
{
    Q_OBJECT
public:
    explicit SecureTextField(QObject *parent = 0) { (void)parent; }

    //Q_INVOKABLE void addCharacter(const QString* aCharacter);

private:
    QSharedPointer<QByteArray> m_password;
};

#endif // SECURETEXTFIELD_H
