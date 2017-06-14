#ifndef GRAPHICUSERINTERFACE_H
#define GRAPHICUSERINTERFACE_H

#include <QDebug>
#include <QObject>
#include <QEventLoop>
#include "Core/Core.h"
#include "Core/Unix/CoreService.h"
#include "Volume/Volume.h"
#include "Platform/Platform.h"
#include "Volume/EncryptionThreadPool.h"
#include "Volume/VolumeInfo.h"
#include "Core/VolumeCreator.h"
#include "../Volume/VolumeInfo.h"

class GraphicUserInterface : public QObject
{
    Q_OBJECT
public:
    explicit GraphicUserInterface(QObject *parent = 0);
    void init();
    void stop();
    ~GraphicUserInterface();

public slots:
    void receive(const QString&);
    void receiveMount(const QString&, const QString&);
    void receiveAutoMount();
    void receiveDismount(const QString&);
    void receiveDismountAll();
    void receiveSudoPassword(const QString& aPwd);
    void receiveCreateVolume(shared_ptr <GostCrypt::VolumeCreationOptions>);

signals:
    void sendVolume(GostCrypt::MountOptions aOptions);
    void askSudoPassword();
    void sendVolumeInfos(string AuxMountPoint, wstring EncryptionAlgorithmName, string Path, uint64 Size);

private:
    class AdminPasswordRequestHandler : public GostCrypt::GetStringFunctor
    {
    private:
        QString mPassword;
        QEventLoop mLoop;
        GraphicUserInterface* parent;
    public:
        AdminPasswordRequestHandler(GraphicUserInterface* parent) : parent(parent) {}
        virtual void operator() (string &passwordStr)
        {
            emit parent->askSudoPassword();
            mLoop.exec();
            GostCrypt::StringConverter::ToSingle (mPassword.toStdWString(), passwordStr);
        }
        void sendPassword(const QString& aPwd) {
            mPassword = aPwd;
            mLoop.quit();
        }
    }mAdminPasswordRequestHandler;

};

#endif // GRAPHICUSERINTERFACE_H
