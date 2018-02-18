#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QObject>
#include <Core/CoreBase.h>
#define DEC_PRINT_SLOT(requestName) virtual void print ## requestName (QSharedPointer<GostCrypt::Core::requestName ## Response> r)

/**
 * @brief
 *
 */
class UserInterface : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param parent
     */
    explicit UserInterface(QObject *parent = nullptr) : QObject(parent) {}
    /**
     * @brief
     *
     * @param argc
     * @param argv
     * @return int
     */
    virtual int start(int argc, char **argv) = 0;
    /**
     * @brief
     *
     * @param sizeInByte
     * @param withFontColor
     * @return QString
     */
    static QString formatSize(quint64 sizeInByte, bool withFontColor);
    static quint64 parseSize(QString s, bool *ok);
signals:
    /**
     * @brief
     *
     * @param request
     */
    void request(QVariant request);
    /**
     * @brief
     *
     */
    void exit();
    /**
     * @brief
     *
     * @param password
     */
    void sendSudoPassword(QString password);
private slots:
    DEC_PRINT_SLOT(CreateVolume) = 0;
    DEC_PRINT_SLOT(MountVolume) = 0;
    DEC_PRINT_SLOT(DismountVolume) = 0;
    DEC_PRINT_SLOT(GetMountedVolumes) = 0;
    DEC_PRINT_SLOT(GetEncryptionAlgorithms) = 0;
    DEC_PRINT_SLOT(GetDerivationFunctions) = 0;
    DEC_PRINT_SLOT(GetHostDevices) = 0;
    DEC_PRINT_SLOT(CreateKeyFile) = 0;
    DEC_PRINT_SLOT(ChangeVolumePassword) = 0;
    DEC_PRINT_SLOT(ProgressUpdate) = 0;
    DEC_PRINT_SLOT(BenchmarkAlgorithms) = 0;
    /**
     * @brief
     *
     */
    virtual void askSudoPassword() = 0;

protected:
    QSharedPointer<GostCrypt::Core::CoreBase> core; /**< TODO: describe */
};

#endif // USERINTERFACE_H
