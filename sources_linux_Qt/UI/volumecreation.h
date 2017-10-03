//#ifndef VOLUMECREATION_H
//#define VOLUMECREATION_H

//#include <QDebug>
//#include <QObject>
//#include <vector>

//#define setterInfo(INFO, TYPE) Q_INVOKABLE void set##INFO (const TYPE &value) { setInfo<TYPE>(#INFO, value); }

//class VolumeCreation : public QObject {
//    Q_OBJECT
//public:
//    VolumeCreation();
//    ~VolumeCreation();

//    Q_INVOKABLE void createVolume();
//    Q_INVOKABLE void abort();
//    Q_INVOKABLE QStringList getAlgos() const;
//    Q_INVOKABLE QStringList getHashs() const;
//    Q_INVOKABLE QString getAlgosDescription(const unsigned int& aIndex);
//    Q_INVOKABLE QString getfreeSpace();

//    Q_INVOKABLE void setPath(const QString& value);
//    Q_INVOKABLE void setType(const int& value);
//    Q_INVOKABLE void setSize(const uint64& value);
//    Q_INVOKABLE void setPassword(const QString& value);
//    Q_INVOKABLE void setKeyFiles(const QString& value);
//    Q_INVOKABLE void setVolumeHeaderKdf(const QString& value);
//    Q_INVOKABLE void setEA(const QString& value);
//    Q_INVOKABLE void setQuick(const bool value);
//    Q_INVOKABLE void setFileSystem(const int& value);
//    Q_INVOKABLE void setFileSystemClusterSize(const uint32& value);
//    Q_INVOKABLE void setSectorSize(const uint32& value);
//    Q_INVOKABLE void sendRandomPosition(const int& x, const int& y);

//    Q_INVOKABLE float getPercent();
//    Q_INVOKABLE int getTimeRemaining();
//    Q_INVOKABLE int getSpeed();
//private:
//    shared_ptr<GostCrypt::EncryptionAlgorithm> getAlgo(QString name);
//    shared_ptr<GostCrypt::Hash> getHash(QString name);

//    GostCrypt::SharedPtr<GostCrypt::VolumeCreationOptions> infos;
//    GostCrypt::VolumeCreator *creator;
//    GostCrypt::VolumeCreator::ProgressInfo pi;
//    std::vector<GostCrypt::SharedPtr<GostCrypt::EncryptionAlgorithm>> algorithms;
//};

//#endif // VOLUMECREATION_H
