#ifndef GRAPHICUSERINTERFACE_H
#define GRAPHICUSERINTERFACE_H

#include <QObject>
#include "Core/Core.h"
#include "Core/Unix/CoreService.h"
#include "Volume/Volume.h"
#include "Platform/Platform.h"
#include "Volume/EncryptionThreadPool.h"
#include "Volume/VolumeInfo.h"

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

signals:
    void sendVolume(GostCrypt::MountOptions aOptions);
};

#endif // GRAPHICUSERINTERFACE_H
