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
        return parent::qwhat() + message + QStringLiteral("\n"); \
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



namespace GostCrypt
{

/**
 * @brief Initialize the GostCryptException class and subclasses to make them serializable across processes
 *
 */
void initGostCryptException();

/**
 * @brief Parent class of all GostCrypt Exceptions
 *
 */
class GostCryptException : public QException
{
 public:
    /**
     * @brief Default constructor used when deserializing
     *
     */
    GostCryptException() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    */
    GostCryptException(QString fonction, QString filename, quint32 line) : fonction(fonction),
        filename(QFileInfo(filename).fileName()), line(line), requestId(0) {}
    /**
     * @brief Constructor used to copy an exception and add the ID of the request being processed when the exception was thrown
     *
     * @param copy GostCrypt Exception to copy
     * @param requestId ID of the request being processed when the exception was thrown
     */
    GostCryptException(const GostCryptException& copy, quint32 requestId) : GostCryptException(copy) { this->requestId = requestId; }
    /**
     * @brief Return the line number where the exception was thrown
     *
     * @return quint32 line number where the exception was thrown
     */
    quint32 getLine() const {return line; }
    /**
     * @brief Return the name of the file where the exception is thrown
     *
     * @return QString Name of the file where the exception is thrown
     */
    QString getFilename() const {return filename; }
    /**
     * @brief Return the name of the function in which the exception was thrown
     *
     * @return QString Name of the function in which the exception was thrown
     */
    QString getFonction() const {return fonction; }
    /**
     * @brief Return the ID of the request being processed when the exception was thrown
     *
     * @return quint32 ID of the request being processed when the exception was thrown
     */
    quint32 getRequestId() const {return requestId; }

    /**
     * @brief Return a pointer to a deep copy of this exception
     *
     * @return Pointer to a deep copy of this exception
     */
    virtual GostCryptException* clone() const { return new GostCryptException(*this); }
    /**
     * @brief Return a pointer to a deep copy of this exception with a different requestId
     *
     * @param requestId New request ID for the exception copy
     * @return Pointer to a deep copy of this exception with a different requestId
     */
    virtual GostCryptException* clone(quint32 requestId) const { return new GostCryptException(*this, requestId); }

    /**
     * @brief Return the description of the exception as a const char pointer
     *
     * @return Description of the exception as a const char pointer
     */
    const char* what() const throw ()
    {
        return qwhat().toLocal8Bit().data();
    }
    /**
     * @brief Return the description of the exception as QString
     *
     * @return Description of the exception as QString
     */
    virtual QString qwhat() const
    {
        return "\nException:\t" + getName() + "\nFonction:\t" + fonction + "\nFile position:\t" +
               getPosition() + "\nMessage:\t";
    }
    /**
     * @brief fReturn the name of an exception. The name of the exception begin with the names of the parent exceptions separated by a slash
     *
     * @return Name of the exception
     */
    virtual QString getName() const
    {
        return "";
    }
    /**
     * @brief Return the message corresponding to this exception
     *
     * @return Message corresponding to this exception
     */
    virtual QString getMessage() const
    {
        return QString();
    }
    /**
     * @brief Return a QVariant storing this exception. Used to serialize exceptions across processes
     *
     * @return QVariant storing this exception
     */
    virtual QVariant toQVariant() const
    {
        return QVariant::fromValue(*this);
    }
    /**
     * @brief Throw this exception
     *
     */
    virtual void raise() const
    {
        throw* this;
    }
    /**
     * @brief Return to the message that should be displayed when explaining the exception depending on whether or not GostCrypt has been compiled in debug or release mode
     *
     * @return QString return the exception name
     */
    QString displayedMessage() const
    {
#ifdef QT_DEBUG
        return qwhat();
#else
        return getMessage();
#endif
    }

 protected:
    /**
     * @brief Return the position at which the exception was thrown as a QString with the format [file_name]:[line_number]
     *
     * @return Position at which the exception was thrown
     */
    QString getPosition() const
    {
        return filename + ":" + QString::number(line);
    }
    QString fonction; /**< Name of the function in which the exception is thrown */
    QString filename; /**< Name of the file where the exception is thrown */
    quint32 line; /**< Line where the exception is thrown */
    quint32 requestId; /**< ID of the request being processed when the exception was thrown. Is set  to 0 if the request ID is unknown or not relevant. */
    DEC_SERIALIZABLE(GostCryptException);
};

#define SystemExceptionException() SystemException(__PRETTY_FUNCTION__, __FILE__, __LINE__);
/**
 * @brief Parent of the exceptions thrown after a system error, usually a failed system call
 *
 */
class SystemException : public GostCryptException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    *
    */
    SystemException() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    */
    SystemException(QString fonction, QString filename, quint32 line) : GostCryptException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(SystemException, GostCryptException, "")
    DEC_SERIALIZABLE(SystemException);
};

#define FailedOpenFileException(file) GostCrypt::FailedOpenFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
/**
 * @brief Exception thrown when the opening of a file failed
 *
 */
class FailedOpenFile : public SystemException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    *
    */
    FailedOpenFile() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    * @param file Path of the file GostCrypt tried to open
    */
    FailedOpenFile(QString fonction, QString filename, quint32 line,
                   QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
    DEF_EXCEPTION_WHAT(FailedOpenFile, SystemException,
                       "Unable to open file \"" + file.absoluteFilePath() + "\".\n")
 protected:
    QFileInfo file; /**< Path of the file GostCrypt tried to open */
    DEC_SERIALIZABLE(FailedOpenFile);
};

#define FailedReadFileException(file) GostCrypt::FailedReadFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
/**
 * @brief Exception thrown when reading to a file failed
 *
 */
class FailedReadFile : public SystemException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    *
    */
    FailedReadFile() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    * @param file Path of the file GostCrypt tried to read
    */
    FailedReadFile(QString fonction, QString filename, quint32 line,
                   QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
    DEF_EXCEPTION_WHAT(FailedReadFile, SystemException,
                       "Unable to read file \"" + file.absoluteFilePath() + "\".\n")
 protected:
    QFileInfo file; /**< Path of the file GostCrypt tried to write on */
    DEC_SERIALIZABLE(FailedReadFile);
};

#define FailedWriteFileException(file) GostCrypt::FailedWriteFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
/**
 * @brief Exception thrown when writing to a file failed
 *
 */
class FailedWriteFile : public SystemException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    *
    */
    FailedWriteFile() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    * @param file Path of the file GostCrypt tried to write on
    */
    FailedWriteFile(QString fonction, QString filename, quint32 line,
                    QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
    DEF_EXCEPTION_WHAT(FailedWriteFile, SystemException,
                       "Unable to write on file \"" + file.absoluteFilePath() + "\".\n")
 protected:
    QFileInfo file; /**< Path of the file GostCrypt tried to write on */
    DEC_SERIALIZABLE(FailedWriteFile);
};

#define FailedFlushFileException(file) GostCrypt::FailedFlushFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
/**
 * @brief Exception thrown when flushing a file failed
 *
 */
class FailedFlushFile : public SystemException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    *
    */
    FailedFlushFile() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    * @param file Path of the file GostCrypt tried to flush
    */
    FailedFlushFile(QString fonction, QString filename, quint32 line,
                    QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
    DEF_EXCEPTION_WHAT(FailedFlushFile, SystemException,
                       "Unable to flush data on file \"" + file.absoluteFilePath() + "\".\n")
 protected:
    QFileInfo file; /**< Path of the file GostCrypt tried to flush */
    DEC_SERIALIZABLE(FailedFlushFile);
};

#define FailedLseekFileException(file) GostCrypt::FailedLseekFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
/**
 * @brief Exception thrown when lseeking in a file failed
 *
 */
class FailedLseekFile : public SystemException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    *
    */
    FailedLseekFile() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    * @param file Path of the file GostCrypt tried to lseek in
    */
    FailedLseekFile(QString fonction, QString filename, quint32 line,
                    QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
    DEF_EXCEPTION_WHAT(FailedLseekFile, SystemException,
                       "Unable to lseek file \"" + file.absoluteFilePath() + "\".\n")
 protected:
    QFileInfo file; /**< Path of the file GostCrypt tried to lseek in */
    DEC_SERIALIZABLE(FailedLseekFile);
};

#define FailedStatFileException(file) GostCrypt::FailedStatFile(__PRETTY_FUNCTION__, __FILE__, __LINE__, file);
/**
 * @brief Exception thrown when trying to get file information (stat system call)
 *
 */
class FailedStatFile : public SystemException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    *
    */
    FailedStatFile() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    * @param file Path of the file GostCrypt tried to get information about
    */
    FailedStatFile(QString fonction, QString filename, quint32 line,
                   QFileInfo file) : SystemException(fonction, filename, line), file(file) {}
    DEF_EXCEPTION_WHAT(FailedStatFile, SystemException,
                       "Unable to lseek file \"" + file.absoluteFilePath() + "\".\n")
 protected:
    QFileInfo file; /**< Path of the file GostCrypt tried to get information about */
    DEC_SERIALIZABLE(FailedStatFile);
};

#define FailedCreateDirectoryException(dir) GostCrypt::FailedCreateDirectory(__PRETTY_FUNCTION__, __FILE__, __LINE__, dir);
/**
 * @brief Exception thrown when the directory creation fails
 *
 */
class FailedCreateDirectory : public SystemException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    *
    */
    FailedCreateDirectory() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    * @param dir Path of the directory that failed to be created
    */
    FailedCreateDirectory(QString fonction, QString filename, quint32 line,
                          QFileInfo dir) : SystemException(fonction, filename, line), dir(dir) {}
    DEF_EXCEPTION_WHAT(FailedCreateDirectory, SystemException,
                       "Fail to create directory \"" + dir.absoluteFilePath() + "\".\n")
 protected:
    QFileInfo dir; /**< Path of the directory that failed to be created */
    DEC_SERIALIZABLE(FailedCreateDirectory);
};

#define FailedMemoryAllocationException() GostCrypt::FailedMemoryAllocation(__PRETTY_FUNCTION__, __FILE__, __LINE__);
/**
 * @brief Exception thrown when a memory allocation failed
 *
 */
class FailedMemoryAllocation : public SystemException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    *
    */
    FailedMemoryAllocation() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    */
    FailedMemoryAllocation(QString fonction, QString filename, quint32 line) : SystemException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(FailedMemoryAllocation, SystemException, "Fail to allocate memory\n")

    DEC_SERIALIZABLE(FailedMemoryAllocation);
};

#define UnknowExceptionException() GostCrypt::UnknowException(__PRETTY_FUNCTION__, __FILE__, __LINE__)
/**
 * @brief Exception thrown when an unknown exception has been thrown
 *
 */
class UnknowException : public GostCryptException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    *
    */
    UnknowException() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    */
    UnknowException(QString fonction, QString filename, quint32 line) : GostCryptException(fonction,
                filename, line) {}
    DEF_EXCEPTION_WHAT(UnknowException, GostCryptException, "Unknow exception")
    DEC_SERIALIZABLE(UnknowException);
};

#define ExternalExceptionException(ex) GostCrypt::ExternalException(__PRETTY_FUNCTION__, __FILE__, __LINE__, ex)
/**
 * @brief Exception thrown when a non-GostCrypt exception has been thrown. This allow for the information about this original exception to be transmitted across processes
 *
 */
class ExternalException : public GostCryptException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    *
    */
    ExternalException() {}
    /**
    * @brief Constructor used when throwing the exception
    *
    * @param fonction Name of the function in which the exception is thrown
    * @param filename Name of the file where the exception is thrown
    * @param line Line where the exception is thrown
    * @param ex Original exception
    */
    ExternalException(QString fonction, QString filename, quint32 line,
                      std::exception ex) : GostCryptException(fonction, filename, line)
    {
        info = QString(ex.what());
    }
    DEF_EXCEPTION_WHAT(ExternalException, GostCryptException, "External exception: " + info)
 protected:
    QString info; /**< Description of the original exception */

    DEC_SERIALIZABLE(ExternalException);
};

#define InvalidParameterException(param, comment) GostCrypt::InvalidParameter(__PRETTY_FUNCTION__, __FILE__, __LINE__, param, comment);
/**
 * @brief Exception thrown when a given parameter is invalid and prevent the action from beeing executed
 */
class InvalidParameter : public GostCryptException
{
 public:
    /**
    * @brief Default constructor used when deserializing
    */
    InvalidParameter() {}
    /**
     * @brief Constructor used when throwing the exception
     *
     * @param fonction Name of the function where the exception is thrown
     * @param filename Name of the file where the exception is thrown
     * @param line Line where the exception is thrown
     * @param param Name of the incorrect parameter
     * @param comment Comment giving more information about why the parameter is incorrect
     */
    InvalidParameter(QString fonction, QString filename, quint32 line, QString param,
                     QString comment) : GostCryptException(fonction, filename, line), param(param), comment(comment) {}
    DEF_EXCEPTION_WHAT(InvalidParameter, GostCryptException,
                       "The parameter " + param + " is invalid.\n")
 protected:
    QString param; /**< Name of the incorrect parameter */
    QString comment; /**< Comment giving more information about why the parameter is incorrect */
    DEC_SERIALIZABLE(InvalidParameter);
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
SERIALIZABLE(GostCrypt::UnknowException)
SERIALIZABLE(GostCrypt::ExternalException)
SERIALIZABLE(GostCrypt::InvalidParameter)



#endif // GOSTCRYPTEXCEPTION_H
