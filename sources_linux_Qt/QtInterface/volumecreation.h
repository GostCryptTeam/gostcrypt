#ifndef VOLUMECREATION_H
#define VOLUMECREATION_H

#include <QObject>
#include "../Core/VolumeCreator.h"

class VolumeCreation : public QObject {
    Q_OBJECT
public:
    VolumeCreation();
    ~VolumeCreation() {}

    template<typename T>
        void setInfo(const QString& aKey, T aValue);
    void createVolume();
    void abort();

private:
    GostCrypt::VolumeCreationOptions mInfos;
    GostCrypt::VolumeCreator mVCreator;
};

#endif // VOLUMECREATION_H
