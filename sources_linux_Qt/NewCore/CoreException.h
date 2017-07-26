#ifndef COREEXCEPTION_H
#define COREEXCEPTION_H
#include <QFileInfo>
#include <QException>
#include "SerializationUtil.h"

namespace GostCrypt {
	namespace NewCore {
        bool initCoreException();

		class CoreException : public QException {
			public:
                CoreException() {}
				void raise() const { throw *this; }
				CoreException *clone() const { return new CoreException(*this); }
			DEC_SERIALIZABLE(CoreException);
		};

		class SystemException : public CoreException {
			DEC_SERIALIZABLE(SystemException);
		};
		class FailedOpenFile : public SystemException {
			public:
                FailedOpenFile() {}
                FailedOpenFile(QFileInfo file) : file(file) {}
                QFileInfo getFile() const { return file; }
			protected:
				QFileInfo file;
			DEC_SERIALIZABLE(FailedOpenFile);
		};
	}
}

SERIALIZABLE(GostCrypt::NewCore::CoreException)
SERIALIZABLE(GostCrypt::NewCore::SystemException)
SERIALIZABLE(GostCrypt::NewCore::FailedOpenFile)
#endif // COREEXCEPTION_H
