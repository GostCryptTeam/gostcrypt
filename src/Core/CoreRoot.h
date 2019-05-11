#ifndef COREROOT_H
#define COREROOT_H

#include <QSharedPointer>
#include <fstream>
#include <QProcess>
#include <QByteArray>
#include "CoreBase.h"
#include "CoreRequest.h"
#include "CoreResponse.h"
#include "RandomGenerator.h"

namespace GostCrypt
{
namespace Core
{
/**
 * @brief Class defining the core object containing methods for each action handled by GostCrypt (mount, dismount, create a volume, etc). This class is used by the process if it has root privileges. Therefore this class directly implement GostCrypt action. GostCrypt actions that do not require root privileges are inherited from the parent class CoreBase and the GostCrypt actions that require root privileges are implemented in this class.
 *
 */
class CoreRoot : public CoreBase
{
    Q_OBJECT
 public:
    /**
     * @brief Default Constructor
     *
     * @param parent Parent object (See Qt documentation)
     */
    explicit CoreRoot(QObject* parent = nullptr);
 public slots:  // NOLINT
    /**
     * @brief Terminate all processes launched by this object (except background processes) and emit the signal exited when it is done.
     *
     */
    virtual void exit();
    /**
     * @brief Execute the GostCrypt action corresponding to the type of the CoreRequest sub-class given in r
     *
     * @param r QVariant containing the request object which is a child of CoreRequest. Its type indicate the action type to execute and its content provide the parameters for this action
     */
    virtual void request(QVariant r);
    /**
     * @brief Since object of this class can be instanciated only by processes launched with the root user, this slot is useless in CoreRoot. This implementation is only required because this slot is defined in the parent class CoreBase.
     *
     * @param password QString containning the sudoer's password
     */
    virtual void receiveSudoPassword(QString password) {Q_UNUSED(password);}
 private slots:  // NOLINT
    /**
     * @brief Continue to mount the volume after the Fuse process has been successfully launched
     *
     * @param params Parameters of the mount volume action
     * @param response Response of the mount volume action which may already contain some results
     */
    virtual void continueMountVolume(QSharedPointer<Core::MountVolumeRequest> params,
                             QSharedPointer<Core::MountVolumeResponse> response);
    /**
     * @brief Mount the inner hidden volume instance so that it can be formatted with a file system. This function is part of the volume creation action.
     *
     * @param params Parameters of the volume creation action
     * @param response Response of the volume creation action which may already contain some results
     */
    void continueFormatHidden(QSharedPointer<CreateVolumeRequest> params,
                              QSharedPointer<CreateVolumeResponse> response);
    /**
     * @brief Slots called after the volume creation is finished in order to emit the CreateVolumeResponse if asked. This function is part of the volume creation action.
     *
     * @param params Parameters of the volume creation action
     * @param response Response of the volume creation action which may contain some results
     */
    void finishCreateVolume(QSharedPointer<CreateVolumeRequest> params,
                            QSharedPointer<CreateVolumeResponse> response);
    /**
     * @brief Format the mounted volume instance with the file system and dismount it. This function is part of the volume creation action.
     *
     * @param mountResponse Response of the mount volume action executed just before
     */
    void continueMountFormat(QSharedPointer<MountVolumeResponse> mountResponse);
    /**
     * @brief Slot called after the volume instance is dismounted in order to continue the volume creation action.
     *
     * @param dismountResponse Response of the dismount volume action executed just before
     */
    void continueDismountFormat(QSharedPointer<DismountVolumeResponse> dismountResponse);
 private:
    /**
     * @brief Function in charge of the mount volume action
     *
     * @param params Parameters of the function
     */
    virtual void mountVolume(QSharedPointer<MountVolumeRequest> params);
    /**
     * @brief Function in charge of the dismount volume action
     *
     * @param params Parameters of the function
     * @return Smart pointer to the response object containing the results of the action
     */
    QSharedPointer<DismountVolumeResponse> dismountVolume(QSharedPointer<DismountVolumeRequest> params);
    /**
     * @brief Function in charge of the volume creation action
     *
     * @param params Parameters of the function
     */
    void createVolume(QSharedPointer<CreateVolumeRequest> params);

    /**
     * @brief Write the ciphered volume header on the volume container (file or device)
     *
     * @param file STL object corresponding to the volume container
     * @param params Parameters for the creation of the volume instance header
     * @param layout Layout of the volume instance to use
     * @param containerSize Size of the volume container in bytes
     */
    void writeHeaderToFile(std::fstream& file, QSharedPointer<CreateVolumeRequest::VolumeParams> params,
                           QSharedPointer<Volume::VolumeLayout> layout, quint64 containerSize);
    /**
     * @brief Mount the volume instance in order to format it with the file system. This function is part of the volume creation action.
     *
     * @param volume Path of the volume container to mount
     * @param password Password of the volume instance to mount
     * @param keyfiles Keyfiles of the volume instance to mount
     * @param filesystem File system type to built in the volume instance
     * @param parentParams Parameters of the volume creation action. Used to continue the volume creation after
     * @param parentResponse Response of the volume creation action which may contain some results
     * @param id Information used to report the action progress duing the volume instance(s) file system building
     */
    void mountFormatVolume(QFileInfo volume, QSharedPointer<QByteArray> password,
                           QList<QFileInfo> keyfiles, QString filesystem, QSharedPointer<CreateVolumeRequest> parentParams,
                           QSharedPointer<CreateVolumeResponse> parentResponse, ProgressTrackingParameters id);
    uid_t realUserId; /**< ID of the user who launched GostCrypt */
    gid_t realGroupId; /**< ID of the group who launched GostCrypt */
     signals:
    /**
     * @brief Signal emitted after the current volume instance has been mounted, formatted with the file system and dismounted in order to continue the volume creation action
     *
     * @param params Parameters of the volume creation action
     * @param response Response of the volume creation action which may contain some results
     */
    void formatVolumeDone(QSharedPointer<CreateVolumeRequest> params,
                          QSharedPointer<CreateVolumeResponse> response);

};
}
}

#endif // COREROOT_H
