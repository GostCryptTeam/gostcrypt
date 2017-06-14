#include "GraphicUserInterface.h"

#include <iostream>
#include <QDebug>
#include <QCoreApplication>

GraphicUserInterface::GraphicUserInterface(QObject * parent)
    :   QObject(parent),
        mAdminPasswordRequestHandler(this)
{
    this->init();
}

void GraphicUserInterface::init() {
    // Start core service
    GostCrypt::CoreService::Start();
    // Start encryption thread pool
    GostCrypt::EncryptionThreadPool::Start();

    GostCrypt::Core->Init();
    GostCrypt::Core->SetApplicationExecutablePath (QCoreApplication::applicationFilePath().toStdWString());

    GostCrypt::Core->SetAdminPasswordCallback (shared_ptr <GostCrypt::GetStringFunctor> (&mAdminPasswordRequestHandler));
}

void GraphicUserInterface::receive(const QString& str)
{
#ifdef QT_DEBUG
    qDebug() << str;
#endif
}


void GraphicUserInterface::receiveMount(const QString& aPath, const QString& aPassword)
{
#ifdef QT_DEBUG
    qDebug() << "Monter : " << aPath << " " << "********";
#endif
    GostCrypt::VolumePath *volumePath = new GostCrypt::VolumePath(aPath.toStdString());
    GostCrypt::VolumePassword *volumePassword = new GostCrypt::VolumePassword(aPassword.toStdWString());
    GostCrypt::MountOptions options;
    try {
        if(GostCrypt::Core->IsVolumeMounted (*volumePath)) {
            qDebug() << "Volume already mounted";
            return;
        }
        options.Password.reset(volumePassword);
        options.Path.reset(volumePath);
        shared_ptr <GostCrypt::VolumeInfo> volumeData = GostCrypt::Core->MountVolume (options);
        emit sendVolumeInfos((string)volumeData.get()->AuxMountPoint, volumeData.get()->EncryptionAlgorithmName, (string)volumeData.get()->Path, volumeData.get()->Size);
        //TEST : //emit sendVolumeInfos(string("1"), wstring(L"2"), string("3"), uint64(555));
    } catch (GostCrypt::SystemException e) {
        qDebug() << "Exception catch";
    }
}

void GraphicUserInterface::receiveAutoMount()
{
#ifdef QT_DEBUG
    qDebug() << "Monter auto";
#endif
    // Voir Main/Forms/MainFrame.cpp:530
}

void GraphicUserInterface::receiveDismountAll()
{
#ifdef QT_DEBUG
    qDebug() << "Tout démonter";
#endif
    GostCrypt::VolumeInfoList volumes = GostCrypt::Core->GetMountedVolumes();
    for(GostCrypt::SharedPtr<GostCrypt::VolumeInfo> volume : volumes){
        GostCrypt::Core->DismountVolume(volume);
    }
}

void GraphicUserInterface::receiveSudoPassword(const QString &aPwd)
{
    mAdminPasswordRequestHandler.sendPassword(aPwd);
}

void GraphicUserInterface::receiveDismount(const QString& aStr)
{
#ifdef QT_DEBUG
    qDebug() << "On démonte " << aStr;
#endif
    GostCrypt::VolumePath path = GostCrypt::VolumePath(aStr.toStdString());
    GostCrypt::SharedPtr<GostCrypt::VolumeInfo> volume = GostCrypt::Core->GetMountedVolume(path);
    if(volume) GostCrypt::Core->DismountVolume(volume);
}

void GraphicUserInterface::receiveCreateVolume(shared_ptr <GostCrypt::VolumeCreationOptions> aCreate){
#ifdef QT_DEBUG
    qDebug() << "Création de volume";
#endif
    GostCrypt::VolumeCreator *creator = new GostCrypt::VolumeCreator();
    creator->CreateVolume(aCreate);
    try{
        creator->CheckResult();
    }catch(...) { qDebug() << "Erreur lors de la création"; return; }
}

void GraphicUserInterface::stop() {
    GostCrypt::CoreService::Stop();
    GostCrypt::EncryptionThreadPool::Stop();
}

GraphicUserInterface::~GraphicUserInterface()
{
    this->stop();
}
