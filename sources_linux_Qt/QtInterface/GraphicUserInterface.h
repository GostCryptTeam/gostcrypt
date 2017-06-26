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
    GostCrypt::VolumeInfoList receiveGetAllVolumes();
    void init();
    void stop();
    ~GraphicUserInterface();

public slots:
    void receive(const QString&);
    void receiveMount(const QString&, const QString&);
    void receiveAutoMount(const QString& aPassword);
    void receiveDismount(const QString&);
    void receiveDismountAll();
    void receiveSudoPassword(const QString& aPwd);
    void receiveSudoEndPassword();
    void receiveCreateVolume(shared_ptr <GostCrypt::VolumeCreationOptions>);
    void receiveChangePassword(const QString &volumePath, const QString &oldPassword, const QString &newPassword, shared_ptr <GostCrypt::KeyfileList> oldKeyFiles, shared_ptr <GostCrypt::KeyfileList> newKeyFiles);

signals:
    void sendVolume(GostCrypt::MountOptions aOptions);
    void askSudoPassword();
    void sendVolumeInfos(string AuxMountPoint, wstring EncryptionAlgorithmName, string Path, uint64 Size);
    void confirmSudoPassword();
    void mountVolumePasswordIncorrect();

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
            if(mLoop.isRunning() == false) mLoop.exec();
            GostCrypt::StringConverter::ToSingle (mPassword.toStdWString(), passwordStr);
        }
        void sendPassword(const QString& aPwd) {
            mPassword = aPwd;
            mLoop.quit();
        }
        void quitExec() {
            mLoop.quit();
        }
    };
    AdminPasswordRequestHandler *mAdminPasswordRequestHandler;

};

#endif // GRAPHICUSERINTERFACE_H
