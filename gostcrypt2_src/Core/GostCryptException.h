#ifndef GOSTCRYPTEXCEPTION_H
#define GOSTCRYPTEXCEPTION_H

#include <QException>
#include <QVariant>
#include "SerializationUtil.h"

#define DEF_EXCEPTION_WHAT(exceptionName, parent, message) \
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
    }

namespace GostCrypt {

void initGostCryptException();

class GostCryptException : public QException {
    public:
        GostCryptException() {}
        GostCryptException(QString fonction, QString filename, quint32 line) : fonction(fonction), filename(QFileInfo(filename).fileName()), line(line) {}
        quint32 getLine() const {return line; }
        QString getFilename() const {return filename; }
        QString getFonction() const {return fonction; }
        quint32 getRequestId() const {return *requestId; }
        void setRequestId(quint32 requestId) const {*this->requestId = requestId; }

        GostCryptException *clone() const { return new GostCryptException(*this); }
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
        QSharedPointer<quint32> requestId;
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
}

SERIALIZABLE(GostCrypt::SystemException)
SERIALIZABLE(GostCrypt::GostCryptException)
SERIALIZABLE(GostCrypt::FailedOpenFile)
SERIALIZABLE(GostCrypt::FailedCreateDirectory)

#endif // GOSTCRYPTEXCEPTION_H
