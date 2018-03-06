#ifndef CMDLINEINTERFACE_H
#define CMDLINEINTERFACE_H

#include <QObject>
#include <QVariant>
#include <QCoreApplication>
#include <QCommandLineParser>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include "Parser.h"
#include "UserInterface.h"
#include "Core/CoreRequest.h"
#include "Core/CoreResponse.h"
#include "Core/CoreBase.h"

#define MK_ENUM(name) name // TODO move to external file ?
#define MK_STRTAB(name) #name
#define MK_ALL_COMMANDS(func) { \
    func(mount), \
    func(createvolume), \
    func(create), \
    func(umount), \
    func(unmount), \
    func(dismount), \
    func(dismountall), \
    func(createkeyfiles), \
    func(list), \
    func(benchmark), \
}

#define CONNECT_SIGNAL(requestName) app.connect(core.data(), SIGNAL(send ## requestName (QSharedPointer<GostCrypt::Core::requestName ## Response>)), this, SLOT(print ## requestName (QSharedPointer<GostCrypt::Core::requestName ## Response>)))

// redefines the notify function of QCoreApplication to catch all exceptions at once
/**
 * @brief
 *
 */
class MyApplication : public QCoreApplication
{
    Q_OBJECT
 public:
    /**
     * @brief
     *
     * @param argc
     * @param argv
     */
    MyApplication(int& argc, char** argv) : QCoreApplication(argc, argv) {}
    /**
     * @brief
     *
     * @param receiver
     * @param event
     * @return bool
     */
    bool notify(QObject* receiver, QEvent* event);
 signals:
    /**
     * @brief
     *
     */
    void askExit();
};

/**
 * @brief
 *
 */
class CmdLineInterface : public UserInterface
{
    Q_OBJECT
 public:
    /**
     * @brief
     *
     * @param parent
     */
    explicit CmdLineInterface(QObject* parent = nullptr);
    /**
     * @brief
     *
     * @return QTextStream
     */
    static QTextStream& qStdOut();
    /**
     * @brief
     *
     * @param argc
     * @param argv
     * @return int
     */
    int start(int argc, char** argv);
    /**
     * @brief
     *
     * @param sizeInByte
     * @return QString
     */
    static QString formatSize(quint64 sizeInByte);
 private slots:  // NOLINT
    DEC_PRINT_SLOT(CreateVolume);
    DEC_PRINT_SLOT(MountVolume);
    DEC_PRINT_SLOT(DismountVolume);
    DEC_PRINT_SLOT(GetMountedVolumes);
    DEC_PRINT_SLOT(GetEncryptionAlgorithms);
    DEC_PRINT_SLOT(GetDerivationFunctions);
    DEC_PRINT_SLOT(GetHostDevices);
    DEC_PRINT_SLOT(CreateKeyFile);
    DEC_PRINT_SLOT(ChangeVolumePassword);
    DEC_PRINT_SLOT(ProgressUpdate);
    DEC_PRINT_SLOT(BenchmarkAlgorithms);
    /**
     * @brief
     *
     */
    virtual void askSudoPassword();
 private:
    /**
     * @brief
     *
     */
    void processRequest();

    QCommandLineParser parser; /**< TODO: describe */

    /**
     * @brief
     *
     */
    struct FirstCMD
    {
        /**
         * @brief
         *
         */
        enum Enum MK_ALL_COMMANDS(MK_ENUM);
        static const QStringList Str; /**< TODO: describe */
    };
};

#endif // CMDLINEINTERFACE_H
