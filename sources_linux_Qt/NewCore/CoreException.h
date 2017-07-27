#ifndef COREEXCEPTION_H
#define COREEXCEPTION_H
#include <QFileInfo>
#include <QException>
#include <QString>
#include <QVariant>
#include "SerializationUtil.h"

#define DEF_EXCEPTION_WHAT(exceptionName, parent, message) \
	virtual QString getName() const { \
		return parent::getName() + "/"#exceptionName; \
	} \
	virtual QString qwhat() const { \
		return parent::qwhat() + message; \
	}

namespace GostCrypt {
	namespace NewCore {
        bool initCoreException();

		class CoreException : public QException {
			public:
                CoreException() {}
                CoreException(QString fonction, QString filename, quint32 line) : fonction(fonction), filename(QFileInfo(filename).fileName()), line(line) {}
				quint32 getLine() const {return line; }
				QString getFilename() const {return filename; }
				QString getFonction() const {return fonction; }
				void raise() const { throw *this; }
				CoreException *clone() const { return new CoreException(*this); }
				const char * what () const throw () {
					return qwhat().toLocal8Bit().data();
				}
			protected:
				virtual QString qwhat() const {
					return "\n\nException:\t" + getName() + "\nFonction:\t" + fonction + "\nFile position:\t" + getPosition() + "\nMessage:\t";
				}
				QString getPosition() const {
					return filename+":"+QString::number(line);
				}
				virtual QString getName() const {
					return "/CoreException";
				}

				QString filename;
				quint32 line;
				QString fonction;
			DEC_SERIALIZABLE(CoreException);
		};

		#define SystemExceptionException() SystemException(__PRETTY_FUNCTION__, __FILE__, __LINE__);
		class SystemException : public CoreException {
			public:
				SystemException() {}
				SystemException(QString fonction, QString filename, quint32 line) : CoreException(fonction, filename, line) {}
			protected:
				DEF_EXCEPTION_WHAT(SystemException, CoreException, "")

			DEC_SERIALIZABLE(SystemException);
		};



		#define FailedOpenFileException(file) FailedOpenFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
		class FailedOpenFile : public SystemException {
			public:
                FailedOpenFile() {}
                FailedOpenFile(QString fonction, QString filename, quint32 line, QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
			protected:
				QFileInfo file;
				DEF_EXCEPTION_WHAT(FailedOpenFile, SystemException, " Unable to open file \""+file.canonicalFilePath() + "\".")


			DEC_SERIALIZABLE(FailedOpenFile);
		};

		#define DeviceNotMountedException(devicePath) DeviceNotMounted(__PRETTY_FUNCTION__, __FILE__, __LINE__, devicePath);
		class DeviceNotMounted : public SystemException {
			public:
                DeviceNotMounted() {}
                DeviceNotMounted(QString fonction, QString filename, quint32 line, QFileInfo device) : SystemException(fonction, filename, line), device(device) {}
			protected:
				QFileInfo device;

				DEF_EXCEPTION_WHAT(DeviceNotMounted, SystemException, " The device "+device.canonicalFilePath() + " is not mounted.")
			DEC_SERIALIZABLE(DeviceNotMounted);
		};
	}
}

SERIALIZABLE(GostCrypt::NewCore::CoreException)
SERIALIZABLE(GostCrypt::NewCore::SystemException)
SERIALIZABLE(GostCrypt::NewCore::FailedOpenFile)
SERIALIZABLE(GostCrypt::NewCore::DeviceNotMounted)
#endif // COREEXCEPTION_H
