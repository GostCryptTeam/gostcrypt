#ifndef PARSER_H
#define PARSER_H

#include "NewCore/CoreRequest.h"
#include "Volume/VolumePassword.h"
#include <iostream>
#include <QCommandLineParser>
#include <QException>

namespace Parser {

    // default values for creating a volume
    #define DEFAULT_ALGORITHM "Gost Grasshopper"
    #define DEFAULT_KDF "HMAC-Whirlpool"
    #define DEFAULT_SIZE 10485760 // 10Mio
    #define DEFAULT_OUTER_SIZE 1.0
    #define DEFAULT_INNER_SIZE 0.7

    class ParseException : QException {
    public:
        ParseException() { this->message = ""; }
        ParseException(const QString &message) { this->message = message; }
        void raise() const { throw *this; }
        ParseException *clone() const { return new ParseException(*this); }
        QString getMessage() { return this->message; }
    private:
        QString message;
    };

    typedef enum _WhatToList
    {
        Volumes,
        Algorithms,
        Hashs,
        FileSystems,
        Devices
    } WhatToList;

    void parseMount(QCommandLineParser &parser, QSharedPointer <GostCrypt::NewCore::MountVolumeRequest> options);
    void parseCreate(QCommandLineParser &parser, QSharedPointer <GostCrypt::NewCore::CreateVolumeRequest> options);
    void parseDismount(QCommandLineParser &parser, QSharedPointer <GostCrypt::NewCore::DismountVolumeRequest> options);
    void parseList(QCommandLineParser &parser, Parser::WhatToList *item);
    void parseCreateKeyFiles(QCommandLineParser &parser, QStringList &files);

    quint64 parseSize(QString s, bool *ok);
    bool askPassword(string volume, QString &p);

}



#endif // PARSER_H
