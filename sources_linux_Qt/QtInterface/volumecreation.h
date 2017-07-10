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
    Q_INVOKABLE QStringList getAlgos() const;
    Q_INVOKABLE QStringList getHashs() const;
    Q_INVOKABLE QString getAlgosDescription(const int& aIndex);
    Q_INVOKABLE QString getfreeSpace();

private:
    GostCrypt::VolumeCreationOptions mInfos;
    GostCrypt::VolumeCreator mVCreator;
};

#endif // VOLUMECREATION_H
