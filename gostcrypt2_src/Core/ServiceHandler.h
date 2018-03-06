#ifndef SERVICEHANDLER_H
#define SERVICEHANDLER_H

#include <QObject>
#include <QCoreApplication>
#include <QDataStream>
#include <QProcess>
#include <QVariant>
#include <QQueue>
#include <QFile>

//#define DEBUG_SERVICE_HANDLER

#define GENERATE_REQUESTS_DUMP

namespace GostCrypt
{
namespace Core
{
/**
 * @brief Class managing the service process from the parent process. It launches it, communicates and stops the Service process. This process is in charge of executing the requests sent to it through the standard input.
 *
 */
class ServiceHandler : public QObject
{
    Q_OBJECT
 public:
    ServiceHandler(QString programPath, QStringList args);
    /**
     * @brief Send the request contained in the given QVariant to the service process
     *
     * @param request QVariant containing the request for the service process
     */
    void sendToService(QVariant request);
    /**
     * @brief Return true if the service process is currently running
     *
     * @return Boolean true if the service process is currently running
     */
    bool isRunning();
    /**
     * @brief Ask the service process to exit
     *
     */
    void exit();
 protected:
    /**
     * @brief Return true if the service process is ready to receive requests.
     *
     * @return Boolean true if the service process is ready to receive requests.
     */
    bool isInitialized();
    /**
     * @brief Method called when the service process crash before we received the InitResponse
     *
     */
    virtual void processCrashedBeforeInitialization();
    /**
     * @brief Write the given data bytes to the service process standard input
     *
     * @param data Data bytes to write to the process standard input
     * @param newline If true a newline character will be added after the data bytes
     */
    void writeToStdin(QSharedPointer<QByteArray> data, bool newline = false);
 private:
    /**
     * @brief Send the request in the waitingRequests queue to the service process. Start the service process if it was not already started
     *
     */
    void sendRequests();
    /**
     * @brief Start the service process
     *
     */
    void startProcess();
    QString programPath; /**< Path of the service executable file */
    QStringList args; /**< Command line arguments to pass to the service process */
    QDataStream
    processStream; /**< Input/Output QDataStream used to serialize requests to the process standard input and serialize responses from the process standard output */
#ifdef GENERATE_REQUESTS_DUMP
    QDataStream
    requestsDumpStream; /**< Ouput QDataStream used to serialize the requests to a dump file for debug purposes */
    QFile requestDumpFile; /**< Dump file where to serialize all requests sent to the service process for debug purposes */
#endif
    QProcess process; /**< QProcess object corresponding to the service process */
    QQueue<QVariant>
    waitingRequests; /**< Queue of QVariants containing the requests waiting tobe sent to the service process */
    bool processInitialized; /**< Boolean true if the InitResponse has been received from the service process, that is to say that the service process is ready to process requests */
    bool askedToQuit; /**< Boolean true if the service process has been asked to exit. Used to differentiate service process crash from normal exit. */

 private slots:  // NOLINT
    /**
     * @brief Slot called when new data is available to read from the process. It tries to deserialize responses from the service process, and emit sendResponse when it succeed.
     *
     */
    void receive();
    /**
     * @brief Slots called when the process exit either because of a crash or because it was asked to terminate.
     *
     * @param exitCode Exit code returned by the service process when it terminated
     */
    void processExited(int exitCode);
    /**
     * @brief Slots called when new data as been written on the service process standard input. Used only in debug mode.
     *
     * @param bytes Number of bytes written on the service process standard input
     */
    void dbg_bytesWritten(qint64 bytes);
    /**
     * @brief Slots called when the service process started
     *
     */
    void processStarted();
 signals:
    /**
     * @brief Signal emitted when a reponse from the service process has been deserialized successfully and need to be forwarded to the UI
     *
     * @param response QVariant containing the response
     */
    void sendResponse(QVariant& response);
    /**
     * @brief Signal emitted when a reponse from the service process has been deserialized successfully. Used to try again to read data on the standard output in case several responses arrived at the same time
     *
     * @param response QVariant containing the response
     */
    void responseRead();
    /**
    * @brief Signal emitted when the service process is updating its progress in the processing of a request
    *
    * @param requestId Request Identifier given by the User Interface module
    * @param progress Progress of the request processing time between 0 and 1
    */
    void sendProgressUpdate(quint32 requestId, qreal progress);
    /**
     * @brief Signal emitted when the service process exited successfully (after beeing asked to)
     *
     */
    void exited();
    /**
     * @brief Signal emitted after the service process has started
     *
     */
    void started();
    /**
     * @brief Signal emitted after the InitResponse has been received from the service process, meaning that it is ready to process requests
     *
     */
    void initialized();
};
}
}

#endif // SERVICEHANDLER_H
