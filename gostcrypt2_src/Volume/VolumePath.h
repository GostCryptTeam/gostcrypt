#ifndef VOLUMEPATH_H
#define VOLUMEPATH_H


#include <QString>

namespace GostCrypt {
namespace Volume {

class VolumePath //TODO Should derive from FilesystemPath
{
public:
	VolumePath () { }
    VolumePath (const std::wstring &path):Data(path) {  }
    VolumePath (const FilePath &path):Data(path) {  }

	bool operator== (const VolumePath &other) const { return Data == other.Data; }
	bool operator!= (const VolumePath &other) const { return Data != other.Data; }
    operator FilePath () const { return FilePath (QString::fromStdWString(Data)); }
    operator std::wstring () const { return Data; }
    QString toQString() const { return QString::fromStdWString(Data); }

    bool IsDevice () const { return FilePath (QString::fromStdWString(Data)).IsBlockDevice() || FilePath (QString::fromStdWString(Data)).IsCharacterDevice(); }
	bool IsEmpty () const { return Data.empty(); }

protected:
    std::wstring Data;
};

typedef std::list <VolumePath> VolumePathList;

}
}

#endif // VOLUMEPATH_H
