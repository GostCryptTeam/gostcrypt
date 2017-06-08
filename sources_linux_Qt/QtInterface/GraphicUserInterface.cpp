#include "GraphicUserInterface.h"
#include "Core/Core.h"
#include "Core/Unix/CoreService.h"
#include "Volume/Volume.h"
#include "Platform/Platform.h"
#include "Volume/EncryptionThreadPool.h"
#include <iostream>
#include <QDebug>
#include <QCoreApplication>

GraphicUserInterface::GraphicUserInterface(QObject * parent)
    : QObject(parent)
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
    // UserInterface.cpp:448
    // Très temoraire
    struct AdminPasswordRequestHandler : public GostCrypt::GetStringFunctor
            {
                virtual void operator() (string &passwordStr)
                {
                    wstring wPassword (L"a"); // Entrer le mot de passe sudo ici
                    GostCrypt::StringConverter::ToSingle (wPassword, passwordStr);
                }
            };

    GostCrypt::Core->SetAdminPasswordCallback (shared_ptr <GostCrypt::GetStringFunctor> (new AdminPasswordRequestHandler ()));
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
    try {
	    if(GostCrypt::Core->IsVolumeMounted (GostCrypt::VolumePath(aPath.toStdWString()))) {
    	    qDebug() << "Volume already mounted";
        	return;
	    }
	    GostCrypt::MountOptions options;
	   	GostCrypt::VolumePassword volumePassword = GostCrypt::VolumePassword(aPassword.toStdWString());
	    options.Password.reset(&volumePassword);
	    GostCrypt::VolumePath volumePath = GostCrypt::VolumePath(aPath.toStdString());
	    options.Path.reset(&volumePath);
	    GostCrypt::Core->MountVolume (options);
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
}

void GraphicUserInterface::receiveDismount(const QString& aStr)
{
#ifdef QT_DEBUG
    qDebug() << "On démonte " << aStr;
#endif
}

void GraphicUserInterface::stop() {
    GostCrypt::CoreService::Stop();
    GostCrypt::EncryptionThreadPool::Stop();
}

GraphicUserInterface::~GraphicUserInterface()
{
    this->stop();
}
