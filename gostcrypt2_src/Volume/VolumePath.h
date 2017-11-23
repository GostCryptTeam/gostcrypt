#ifndef VOLUMEPATH_H
#define VOLUMEPATH_H

namespace GostCrypt {
namespace Volume {

class VolumePath
{
public:
	VolumePath () { }
	VolumePath (const wstring &path) { Data = path; }
	VolumePath (const FilesystemPath &path) { Data = path; }

	bool operator== (const VolumePath &other) const { return Data == other.Data; }
	bool operator!= (const VolumePath &other) const { return Data != other.Data; }
	operator FilesystemPath () const { return FilesystemPath (Data); }
	operator string () const { return StringConverter::ToSingle (Data); }
	operator wstring () const { return Data; }

	bool IsDevice () const { return FilesystemPath (Data).IsBlockDevice() || FilesystemPath (Data).IsCharacterDevice(); }
	bool IsEmpty () const { return Data.empty(); }

protected:
	wstring Data;
};

typedef list <VolumePath> VolumePathList;

}
}

#endif // VOLUMEPATH_H
