#ifndef VOLUMEHOSTTYPE_H
#define VOLUMEHOSTTYPE_H

namespace GostCrypt {
namespace Volume {
	struct VolumeHostType
	{
		enum Enum
		{
			Unknown,
			File,
			Device
		};
	};
}
}

#endif // VOLUMEHOSTTYPE_H
