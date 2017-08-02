#include "MountFilesystemManager.h"

namespace GostCrypt {
namespace NewCore {


        void MountFilesystemManager::MountFilesystem(const QSharedPointer<QFileInfo> devicePath, const QSharedPointer<QFileInfo> mountPoint, const QString &filesystemType, bool readOnly, const QString &systemMountOptions)
        {

        }
        /*
        {
            bool fsMounted = false;

            try
            {
                if (!FilesystemSupportsUnixPermissions (devicePath))
                {
                    stringstream userMountOptions;
                    userMountOptions << "uid=" << GetRealUserId() << ",gid=" << GetRealGroupId() << ",umask=077" << (!systemMountOptions.empty() ? "," : "");

                    CoreUnix::MountFilesystem (devicePath, mountPoint, filesystemType, readOnly, userMountOptions.str() + systemMountOptions);
                    fsMounted = true;
                }
            }
            catch (...) { }

            if (!fsMounted)
                CoreUnix::MountFilesystem (devicePath, mountPoint, filesystemType, readOnly, systemMountOptions);
        }
        {
            if (GetMountedFilesystems (DevicePath(), mountPoint).size() > 0)
                throw MountPointUnavailable (SRC_POS);

            list <string> args;
            string options;

            if (!filesystemType.empty())
            {
    #ifdef GST_SOLARIS
                args.push_back ("-F");
    #else
                args.push_back ("-t");
    #endif
                args.push_back (filesystemType);
            }

            if (readOnly)
                options = "-oro";

            if (!systemMountOptions.empty())
            {
                if (options.empty())
                    options = "-o";
                else
                    options += ",";

                options += systemMountOptions;
            }

            if (!options.empty())
                args.push_back (options);

            args.push_back ("--");
            args.push_back (devicePath);
            args.push_back (mountPoint);

            Process::Execute ("mount", args);
        }
         //*/

        void MountFilesystemManager::DismountFilesystem(const QSharedPointer<QFileInfo> mountPoint, bool force)
        {

        }
        /*
            {
                list <string> args;

        #ifdef GST_MACOSX
                if (force)
                    args.push_back ("-f");
        #else
                (void)force;
        #endif
                args.push_back ("--");
                args.push_back (mountPoint);

                Process::Execute ("umount", args);
            }
        //*/

    }
}
