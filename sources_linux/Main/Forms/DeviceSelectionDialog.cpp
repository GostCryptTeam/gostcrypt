/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#include "Main/GraphicUserInterface.h"
#include "Main/Resources.h"
#include "DeviceSelectionDialog.h"

namespace GostCrypt
{
	DeviceSelectionDialog::DeviceSelectionDialog (wxWindow* parent)
		: DeviceSelectionDialogBase (parent)
	{
		wxBusyCursor busy;

		list <int> colPermilles;

		DeviceListCtrl->InsertColumn (ColumnDevice, LangString["DEVICE"], wxLIST_FORMAT_LEFT, 1);
		colPermilles.push_back (447);
		
		DeviceListCtrl->InsertColumn (ColumnSize, LangString["SIZE"], wxLIST_FORMAT_RIGHT, 1);
		colPermilles.push_back (153);

		DeviceListCtrl->InsertColumn (ColumnMountPoint, LangString["MOUNT_POINT"], wxLIST_FORMAT_LEFT, 1);
		colPermilles.push_back (396);
 
		wxImageList *imageList = new wxImageList (16, 12, true);
		imageList->Add (Resources::GetDriveIconBitmap(), Resources::GetDriveIconMaskBitmap());
		DeviceListCtrl->AssignImageList (imageList, wxIMAGE_LIST_SMALL);

		DeviceList = Core->GetHostDevices();

		foreach_ref (HostDevice &device, DeviceList)
		{
			if (device.Size == 0)
				continue;

			vector <wstring> fields (DeviceListCtrl->GetColumnCount());

			if (DeviceListCtrl->GetItemCount() > 0)
				Gui->AppendToListCtrl (DeviceListCtrl, fields);

			fields[ColumnDevice] = wstring (device.Path) + L":";
			fields[ColumnMountPoint] = device.MountPoint;

			fields[ColumnSize] = Gui->SizeToString (device.Size);
			Gui->AppendToListCtrl (DeviceListCtrl, fields, 0, &device); 

			foreach_ref (HostDevice &partition, device.Partitions)
			{
				fields[ColumnDevice] = wstring (partition.Path);

				fields[ColumnMountPoint] = partition.MountPoint;

				fields[ColumnSize] = Gui->SizeToString (partition.Size);
				Gui->AppendToListCtrl (DeviceListCtrl, fields, -1, &partition);
			}
		}

		Gui->SetListCtrlWidth (DeviceListCtrl, 73);
		Gui->SetListCtrlHeight (DeviceListCtrl, 16);
		Gui->SetListCtrlColumnWidths (DeviceListCtrl, colPermilles);

		Fit();
		Layout();
		Center();

		StdButtonsOK->Disable();
		StdButtonsOK->SetDefault();
	}
	
	void DeviceSelectionDialog::OnListItemActivated (wxListEvent& event)
	{
		if (StdButtonsOK->IsEnabled())
			EndModal (wxID_OK);
	}

	void DeviceSelectionDialog::OnListItemDeselected (wxListEvent& event)
	{
		if (DeviceListCtrl->GetSelectedItemCount() == 0)
			StdButtonsOK->Disable();
	}

	void DeviceSelectionDialog::OnListItemSelected (wxListEvent& event)
	{
		HostDevice *device = (HostDevice *) (event.GetItem().GetData());
		if (device)
		{
			SelectedDevice = *device;
			StdButtonsOK->Enable();
		}
		else
			StdButtonsOK->Disable();
	}
}
