#ifndef GOSTCRYPTEXCEPTION_H
#define GOSTCRYPTEXCEPTION_H

#include <QException>
#include <QVariant>
#include "SerializationUtil.h"

#define DEF_EXCEPTION_WHAT(exceptionName, parent, message) \
    exceptionName (const exceptionName &copy, quint32 requestId) : exceptionName (copy) { this->requestId = requestId; } \
    virtual QString getName() const { \
        return parent::getName() + "/"#exceptionName; \
    } \
    virtual QString getMessage() const{ \
        return QString() + message; \
    } \
    virtual QString qwhat() const { \
        return parent::qwhat() + message; \
    } \
    virtual QVariant toQVariant() const { \
        return QVariant::fromValue(*this); \
    } \
    virtual void raise() const { \
        throw *this; \
    } \
    virtual GostCryptException *clone() const { \
        return new exceptionName (*this); \
    } \
    virtual GostCryptException *clone(quint32 requestId) const { \
        return new exceptionName(*this, requestId); \
    }



namespace GostCrypt {

void initGostCryptException();

class GostCryptException : public QException {
    public:
        GostCryptException() {}
        GostCryptException(QString fonction, QString filename, quint32 line) : fonction(fonction), filename(QFileInfo(filename).fileName()), line(line) {}
        GostCryptException(const GostCryptException &copy, quint32 requestId) : GostCryptException(copy) { this->requestId = requestId; }
        quint32 getLine() const {return line; }
        QString getFilename() const {return filename; }
        QString getFonction() const {return fonction; }
        quint32 getRequestId() const {return requestId; }

        virtual GostCryptException *clone() const { return new GostCryptException(*this); }
        virtual GostCryptException *clone(quint32 requestId) const { return new GostCryptException(*this, requestId); }

        const char * what () const throw () {
            return qwhat().toLocal8Bit().data();
        }
        /**
         * @brief
         *
         * @return QString
         */
        virtual QString qwhat() const {
            return "\nException:\t" + getName() + "\nFonction:\t" + fonction + "\nFile position:\t" + getPosition() + "\nMessage:\t";
        }
        /**
         * @brief fonction used to get the name of an exception
         *
         * @return QString return the name of the esception
         */
        virtual QString getName() const {
            return "";
        }
        /**
         * @brief fonction used to get a message
         *
         * @return QString
         */
        virtual QString getMessage() const{
            return QString();
        }
        /**
         * @brief
         *
         * @return QVariant
         */
        virtual QVariant toQVariant() const {
            return QVariant::fromValue(*this);
        }
        /**
         * @brief
         *
         */
        virtual void raise() const {
            throw *this;
        }
        /**
         * @brief fonction to display the exception name as a message
         *
         * @return QString return the exception name
         */
        QString displayedMessage() const {
#ifdef QT_DEBUG
            return qwhat();
#else
            return getMessage();
#endif
        }

    protected:
        /**
         * @brief
         *
         * @return QString
         */
        QString getPosition() const {
            return filename+":"+QString::number(line);
        }
        QString fonction;
        QString filename;
        quint32 line;
        quint32 requestId;
    DEC_SERIALIZABLE(GostCryptException);
};

#define SystemExceptionException() SystemException(__PRETTY_FUNCTION__, __FILE__, __LINE__);
/**
 * @brief
 *
 */
class SystemException : public GostCryptException {
    public:
        SystemException() {}
        /**
         * @brief
         *
         * @param fonction
         * @param filename
         * @param line
         */
        SystemException(QString fonction, QString filename, quint32 line) : GostCryptException(fonction, filename, line) {}
        DEF_EXCEPTION_WHAT(SystemException, GostCryptException, "")
    DEC_SERIALIZABLE(SystemException);
};

#define FailedOpenFileException(file) GostCrypt::FailedOpenFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
/**
 * @brief
 *
 */
class FailedOpenFile : public SystemException {
    public:
        /**
         * @brief
         *
         */
        FailedOpenFile() {}
        /**
         * @brief
         *
         * @param fonction
         * @param filename
         * @param line
         * @param file
         */
        FailedOpenFile(QString fonction, QString filename, quint32 line, QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
        DEF_EXCEPTION_WHAT(FailedOpenFile, SystemException, "Unable to open file \""+file.absoluteFilePath() + "\".\n")
    protected:
        QFileInfo file; /**< TODO: describe */
    DEC_SERIALIZABLE(FailedOpenFile);
};

#define FailedReadFileException(file) GostCrypt::FailedReadFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
/**
 * @brief
 *
 */
class FailedReadFile : public SystemException {
    public:
        /**
         * @brief
         *
         */
        FailedReadFile() {}
        /**
         * @brief
         *
         * @param fonction
         * @param filename
         * @param line
         * @param file
         */
        FailedReadFile(QString fonction, QString filename, quint32 line, QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
        DEF_EXCEPTION_WHAT(FailedReadFile, SystemException, "Unable to read file \""+file.absoluteFilePath() + "\".\n")
    protected:
        QFileInfo file; /**< TODO: describe */
    DEC_SERIALIZABLE(FailedReadFile);
};

#define FailedWriteFileException(file) GostCrypt::FailedWriteFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
/**
 * @brief
 *
 */
class FailedWriteFile : public SystemException {
    public:
        /**
         * @brief
         *
         */
        FailedWriteFile() {}
        /**
         * @brief
         *
         * @param fonction
         * @param filename
         * @param line
         * @param file
         */
        FailedWriteFile(QString fonction, QString filename, quint32 line, QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
        DEF_EXCEPTION_WHAT(FailedWriteFile, SystemException, "Unable to write file \""+file.absoluteFilePath() + "\".\n")
    protected:
        QFileInfo file; /**< TODO: describe */
    DEC_SERIALIZABLE(FailedWriteFile);
};

#define FailedFlushFileException(file) GostCrypt::FailedFlushFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
/**
 * @brief
 *
 */
class FailedFlushFile : public SystemException {
    public:
        /**
         * @brief
         *
         */
        FailedFlushFile() {}
        /**
         * @brief
         *
         * @param fonction
         * @param filename
         * @param line
         * @param file
         */
        FailedFlushFile(QString fonction, QString filename, quint32 line, QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
        DEF_EXCEPTION_WHAT(FailedFlushFile, SystemException, "Unable to write file \""+file.absoluteFilePath() + "\".\n")
    protected:
        QFileInfo file; /**< TODO: describe */
    DEC_SERIALIZABLE(FailedFlushFile);
};

#define FailedLseekFileException(file) GostCrypt::FailedLseekFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
/**
 * @brief
 *
 */
class FailedLseekFile : public SystemException {
    public:
        /**
         * @brief
         *
         */
        FailedLseekFile() {}
        /**
         * @brief
         *
         * @param fonction
         * @param filename
         * @param line
         * @param file
         */
        FailedLseekFile(QString fonction, QString filename, quint32 line, QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
        DEF_EXCEPTION_WHAT(FailedLseekFile, SystemException, "Unable to lseek file \""+file.absoluteFilePath() + "\".\n")
    protected:
        QFileInfo file; /**< TODO: describe */
    DEC_SERIALIZABLE(FailedLseekFile);
};

#define FailedStatFileException(file) GostCrypt::FailedStatFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
/**
 * @brief
 *
 */
class FailedStatFile : public SystemException {
    public:
        /**
         * @brief
         *
         */
        FailedStatFile() {}
        /**
         * @brief
         *
         * @param fonction
         * @param filename
         * @param line
         * @param file
         */
        FailedStatFile(QString fonction, QString filename, quint32 line, QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
        DEF_EXCEPTION_WHAT(FailedStatFile, SystemException, "Unable to lseek file \""+file.absoluteFilePath() + "\".\n")
    protected:
        QFileInfo file; /**< TODO: describe */
    DEC_SERIALIZABLE(FailedStatFile);
};

#define FailedCreateDirectoryException(dir) GostCrypt::FailedCreateDirectory(__PRETTY_FUNCTION__, __FILE__, __LINE__, dir);
/**
 * @brief
 *
 */
class FailedCreateDirectory : public SystemException {
    public:
        /**
         * @brief
         *
         */
        FailedCreateDirectory() {}
        /**
         * @brief
         *
         * @param fonction
         * @param filename
         * @param line
         * @param dir
         */
        FailedCreateDirectory(QString fonction, QString filename, quint32 line, QString dir) : SystemException(fonction, filename, line), dir(dir) {}
        DEF_EXCEPTION_WHAT(FailedCreateDirectory, SystemException, "Fail to create directory \"" + dir + "\".\n")
    protected:
        QString dir; /**< TODO: describe */
    DEC_SERIALIZABLE(FailedCreateDirectory);
};

#define FailedMemoryAllocationException() GostCrypt::FailedMemoryAllocation(__PRETTY_FUNCTION__, __FILE__, __LINE__);
/**
 * @brief
 *
 */
class FailedMemoryAllocation : public SystemException {
    public:
        /**
         * @brief
         *
         */
        FailedMemoryAllocation() {}
        /**
         * @brief
         *
         * @param fonction
         * @param filename
         * @param line
         * @param dir
         */
        FailedMemoryAllocation(QString fonction, QString filename, quint32 line) : SystemException(fonction, filename, line) {}
        DEF_EXCEPTION_WHAT(FailedMemoryAllocation, SystemException, "Fail to allocate memory\n")

    DEC_SERIALIZABLE(FailedMemoryAllocation);
};

#define IncorrectParameterException(comment) GostCrypt::IncorrectParameter(__PRETTY_FUNCTION__, __FILE__, __LINE__, comment);
/**
 * @brief
 *
 */
class IncorrectParameter : public GostCryptException {
    public:
        /**
         * @brief
         *
         */
        IncorrectParameter() {}
        /**
         * @brief
         *
         * @param fonction
         * @param filename
         * @param line
         * @param parameterName
         */
        IncorrectParameter(QString fonction, QString filename, quint32 line, QString comment) : GostCryptException(fonction, filename, line), comment(comment) {}
        DEF_EXCEPTION_WHAT(IncorrectParameter, GostCryptException, "Parameter is incorrect ("+comment+")")
    protected:
        QString comment; /**< TODO: describe */

    DEC_SERIALIZABLE(IncorrectParameter);
};

#define UnknowExceptionException() GostCrypt::UnknowException(__PRETTY_FUNCTION__, __FILE__, __LINE__)
/**
 * @brief
 *
 */
class UnknowException : public GostCryptException {
    public:
        /**
         * @brief
         *
         */
        UnknowException() {}
        /**
         * @brief
         *
         * @param fonction
         * @param filename
         * @param line
         * @param parameterName
         */
        UnknowException(QString fonction, QString filename, quint32 line) : GostCryptException(fonction, filename, line) {}
        DEF_EXCEPTION_WHAT(UnknowException, GostCryptException, "Unknow exception")
    DEC_SERIALIZABLE(UnknowException);
};

#define ExternalExceptionException(ex) GostCrypt::ExternalException(__PRETTY_FUNCTION__, __FILE__, __LINE__, ex)
/**
 * @brief
 *
 */
class ExternalException : public GostCryptException {
    public:
        /**
         * @brief
         *
         */
        ExternalException() {}
        /**
         * @brief
         *
         * @param fonction
         * @param filename
         * @param line
         * @param parameterName
         */
        ExternalException(QString fonction, QString filename, quint32 line, std::exception ex) : GostCryptException(fonction, filename, line){
            info = QString(ex.what());
        }
        DEF_EXCEPTION_WHAT(ExternalException, GostCryptException, "External exception: "+info)
    protected:
        QString info; /**< TODO: describe */

    DEC_SERIALIZABLE(ExternalException);
};
}

SERIALIZABLE(GostCrypt::SystemException)
SERIALIZABLE(GostCrypt::GostCryptException)
SERIALIZABLE(GostCrypt::FailedOpenFile)
SERIALIZABLE(GostCrypt::FailedReadFile)
SERIALIZABLE(GostCrypt::FailedWriteFile)
SERIALIZABLE(GostCrypt::FailedFlushFile)
SERIALIZABLE(GostCrypt::FailedLseekFile)
SERIALIZABLE(GostCrypt::FailedStatFile)
SERIALIZABLE(GostCrypt::FailedCreateDirectory)
SERIALIZABLE(GostCrypt::FailedMemoryAllocation)
SERIALIZABLE(GostCrypt::IncorrectParameter)
SERIALIZABLE(GostCrypt::UnknowException)
SERIALIZABLE(GostCrypt::ExternalException)


#endif // GOSTCRYPTEXCEPTION_H
