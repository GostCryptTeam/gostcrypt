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
    Q_INVOKABLE void connectReceiveAutoMount(const QString& aPwd);
    Q_INVOKABLE void connectReceiveDismount(const QString&);
    Q_INVOKABLE void connectReceiveDismountAll();
    Q_INVOKABLE void debug_connectReceiveCreate(); // for test purposes only
    Q_INVOKABLE void connectSudo(const QString& aPwd);
    Q_INVOKABLE void openPath(const QString& aPath);

signals:
    void sendSubWindowAskSudoPassword();
    void sendSubWindowVolumeInfos(const QString& aMount, const QString& aAlgo, const QString& aPath, const QString& aSize);
    void sendSubWindowConfirmSudoPassword();

private:
    GraphicUserInterface* mGUI;

public slots:
    void subWindowAskSudoPassword();
    void subWindowSendVolumeInfos(string,wstring,string,uint64);
    void subWindowConfirmSudoPassword();
};


#endif
