/* This file was written for the GostCrypt project
under the GPLV3 license.

*/


#ifndef GST_HEADER_Volume_VolumeAlert
#define GST_HEADER_Volume_VolumeAlert

#include "Platform/Platform.h"
//#include <wx/wx.h>
#include <wx/msgdlg.h>

namespace GostCrypt
{
	struct VolumeAlert : public Exception
	{
	public:
		VolumeAlert ();
		
		void SetAlert (const int message, const wstring subject);
	};

}

#endif // GST_HEADER_Volume_VolumeAlert