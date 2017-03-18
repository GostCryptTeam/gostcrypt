
#include "VolumeAlert.h"

namespace GostCrypt
{
	VolumeAlert::VolumeAlert()
	{
	}

	void VolumeAlert::SetAlert(int message, wstring subject)
	{
		wxString foo;
		foo.Printf(wxT("%d\n"), message);
		wxMessageBox(foo);

		return;
	}
}