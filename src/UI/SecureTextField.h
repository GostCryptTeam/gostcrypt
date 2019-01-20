#ifndef SECURETEXTFIELD_H
#define SECURETEXTFIELD_H
#include <QApplication>
#include <QSharedPointer>

/**
 * @brief Class that stores the password entered
 * on the interface in the memory of the C++ thread
 * to protect it
 */
class SecureTextField : public QObject
{
    Q_OBJECT
 public:
    /**
     * @brief Class constructor
     *
     * @param parent Parent object
     */
    explicit SecureTextField(QObject* parent = 0) { (void)parent; }

 private:
    QSharedPointer<QByteArray> m_password; /**< password stored in C++ memory */
};

#endif // SECURETEXTFIELD_H
