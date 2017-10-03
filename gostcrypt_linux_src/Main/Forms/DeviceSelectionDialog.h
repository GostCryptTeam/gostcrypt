/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Main_Forms_DeviceSelectionDialog
#define GST_HEADER_Main_Forms_DeviceSelectionDialog

#include "Forms.h"
#include "Main/Main.h"

namespace GostCrypt
{
	class DeviceSelectionDialog : public DeviceSelectionDialogBase
	{
	public:
		DeviceSelectionDialog (wxWindow* parent);

		HostDeviceList DeviceList;
		HostDevice SelectedDevice;

	protected:
		enum
		{
			ColumnDevice = 0,
			ColumnSize,
			ColumnMountPoint
		};

		void OnListItemActivated (wxListEvent& event);
		void OnListItemDeselected (wxListEvent& event);
		void OnListItemSelected (wxListEvent& event);
	};
}

#endif // GST_HEADER_Main_Forms_DeviceSelectionDialog
