#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QObject>
#include <Core/CoreBase.h>
#define DEC_PRINT_SLOT(requestName) virtual void print ## requestName (QSharedPointer<GostCrypt::Core::requestName ## Response> r)

/**
 * @brief Abstract class inherited by CmdLineInterface and GraphicInterface
 * Centralizes common functionality
 */
class UserInterface : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Class constructor
     *
     * @param parent Parent object
     */
    explicit UserInterface(QObject *parent = nullptr) : QObject(parent) {}
    /**
     * @brief Creates the graphical interface or commandline interface and sends all the necessary objects to it in context
     *
     * @param argc The number of arguments in the main program
     * @param argv Arguments of the main program
     * @return int Returns what the "exec" method returns at the end of the interface execution
     */
    virtual int start(int argc, char **argv) = 0; //TODO : never used
    /**
     * @brief Converts A size in bytes into a formatted character string (or not formatted)
     *
     * @param sizeInByte Size in bytes to convert
     * @param withFontColor This boolean allows you to specify whether you want an output formatted in HTML (true) or not (false)
     * @return QString Character string of the size converted to the desired format
     */
    static QString formatSize(quint64 sizeInByte, bool withFontColor);
    static quint64 parseSize(QString s, bool *ok);
signals:
    /**
     * @brief A signal that is sent by GraphicInterface or CLI to the core.
     *
     * @param request Serializable arguments to send with the request
     */
    void request(QVariant request);
    /**
     * @brief
     * End of program request
     */
    void exit();
    /**
     * @brief Sends sudo password to core
     *
     * @param password The password entered by the user
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
     * slot called when the core requests
     * the sudo password from the user
     */
    virtual void askSudoPassword() = 0;

protected:
    QSharedPointer<GostCrypt::Core::CoreBase> core; /**< Pointer to the core class */
};

#endif // USERINTERFACE_H
