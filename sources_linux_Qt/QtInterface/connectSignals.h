#ifndef MODELCLASS_H
#define MODELCLASS_H

#include <QQuickItem>
#include <QString>
#include <QStringList>
#include "GraphicUserInterface.h"

/*! \class connectSignals
  * \brief connects the QML interface with the GraphicUserInterface methods
  */
class ConnectSignals : public QObject
{
    Q_OBJECT

public:
    ConnectSignals(GraphicUserInterface* aGUI = nullptr);
    ~ConnectSignals(){}

    Q_INVOKABLE void connectReceiveMount(const QString& aPath, const QString& aPwd);
    Q_INVOKABLE void connectReceiveAutoMount();
    Q_INVOKABLE void connectReceiveDismount(const QString&);
    Q_INVOKABLE void connectReceiveDismountAll();
    Q_INVOKABLE void connectSudo(const QString& aPwd);

signals:
    void sendReceiveMount(const QString& aPath);
    void sendSubWindowAskSudoPassword();

private:
    GraphicUserInterface* mGUI;

public slots:
    void subWindowAskSudoPassword();

};


#endif
