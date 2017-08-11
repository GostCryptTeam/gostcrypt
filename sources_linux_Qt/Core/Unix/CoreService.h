/*
 Copyright (c) 2008-2010 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Core_Unix_CoreService
#define GST_HEADER_Core_Unix_CoreService

#include "CoreServiceRequest.h"
#include "Platform/Stream.h"
#include "Platform/Unix/Pipe.h"
#include "Core/Core.h"

/**
 * @brief Class used to manage communication between UI thread and worker thread who will process requests
 */

namespace GostCrypt
{
	// This service facilitates process forking and elevation of user privileges
	class CoreService
	{
	public:
                 /**
                 * @brief ProcessElevatedRequests is called by elevated thread to handle request which need elevated privilege
                 * It will wait for sync byte and call Process Requests method to process these requests as usual
                 */
                static void ProcessElevatedRequests ();
                /**
                 * @brief ProcessRequests Read serialized Request object on inputFD, process them and send the serialized Response object to outputFD. Run permanently until ExitRequest
                 * @param inputFD File descriptor on which the method read requests
                 * @param outputFD File descriptor on which the method write reponses
                 */
		static void ProcessRequests (int inputFD = -1, int outputFD = -1);

                /*
                 * Folowwing methods will send corresponding requests to the worker thread using the ServiceInputStream attribute
                 */
                static void RequestCheckFilesystem (shared_ptr <VolumeInfo> mountedVolume, bool repair);
		static void RequestDismountFilesystem (const DirectoryPath &mountPoint, bool force);
                static shared_ptr <VolumeInfo> RequestDismountVolume (shared_ptr <VolumeInfo> mountedVolume, bool ignoreOpenFiles = false, bool syncVolumeInfo = false);
		static uint32 RequestGetDeviceSectorSize (const DevicePath &devicePath);
		static uint64 RequestGetDeviceSize (const DevicePath &devicePath);
		static HostDeviceList RequestGetHostDevices (bool pathListOnly);
		static shared_ptr <VolumeInfo> RequestMountVolume (MountOptions &options);
		static void RequestSetFileOwner (const FilesystemPath &path, const UserId &owner);

                /**
                 * @brief SetAdminPasswordCallback change the function called when CoreService need the sudo password for privilege elevation
                 * @param functor Functor which will be called
                 */
                static void SetAdminPasswordCallback (shared_ptr <GetStringFunctor> functor) { AdminPasswordCallback = functor; }

                /**
                 * @brief Start the worker thread and initialize ServiceInputStream and ServiceOutputStream to communicate with it
                 */
                static void Start ();

                /**
                 * @brief Stop the worker threads (userland one and elevated one if launched)
                 */
		static void Stop ();

	protected:

                /**
                 * @brief Read reponse from the worker thread on ServiceOutputStream
                 */
                template <class T> static unique_ptr <T> GetResponse ();

                /**
                 * @brief Write the given request on ServiceInputStream to send it to the worker thread
                 */
		template <class T> static unique_ptr <T> SendRequest (CoreServiceRequest &request);

                /**
                 * @brief StartElevated start the eletaved worker process. It is called by ProcessRequests when a request need elevationPriviledges and the elevated worker is not started yet
                 * @param request the request who need elevated priviledge
                 */
                static void StartElevated (const CoreServiceRequest &request);


                /**
                 * @brief AdminPasswordCallback function called when CoreService need the sudo password for privilege elevation
                 */
		static shared_ptr <GetStringFunctor> AdminPasswordCallback;

                /**
                 * @brief AdminInputPipe Pipe connecting the worker thread to the elevated worker thread used to forward requests
                 */
		static shared_ptr <Pipe> AdminInputPipe;
                /**
                 * @brief AdminOutputPipe Pipe connection the elevated worker thread to the worker thread used to retrieve Response
                 */
		static shared_ptr <Pipe> AdminOutputPipe;

                /**
                 * @brief InputPipe Pipe connecting the UI thread to the worker thread used to send requests
                 */
                static shared_ptr <Pipe> InputPipe;

                /**
                 * @brief OutputPipe Pipe connection the worker thread to the UI thread used to retrieve Response
                 */
		static shared_ptr <Pipe> OutputPipe;

                /**
                 * @brief ServiceInputStream Stream used to send Request to next worker thread (worker threead if we are in the UI thread and elevated worker thread if we are already in the worker thread)
                 */
		static shared_ptr <Stream> ServiceInputStream;

                /**
                 * @brief ServiceOutputStream Stream used to retrieve Response from next worker thread (worker threead if we are in the UI thread and elevated worker thread if we are already in the worker thread)
                 */
		static shared_ptr <Stream> ServiceOutputStream;

                /**
                 * @brief ElevatedPrivileges is true if we are in the elevated worker thread, false otherwise
                 */
                static bool ElevatedPrivileges;

                /**
                 * @brief ElevatedServiceAvailable is True when the elevated worker thread has been launched
                 */
		static bool ElevatedServiceAvailable;

                /**
                 * @brief Running is True while the worker Thread should keep running. This boolean is used to stop the infiite working loop
                 */
		static bool Running;

	private:
		CoreService ();
	};

#define GST_CORE_SERVICE_CMDLINE_OPTION "coreservice"
}

#endif // GST_HEADER_Core_Unix_CoreService
