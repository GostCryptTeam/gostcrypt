#ifndef PARSER_H
#define PARSER_H

#include "NewCore/CoreParams.h"
#include <iostream>
#include <QCommandLineParser>
#include <QException>

namespace Parser {

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
        FileSystems
    } WhatToList;

    void parseMount(QCoreApplication &app, QCommandLineParser &parser, QSharedPointer <GostCrypt::NewCore::MountVolumeParams> options);
    void parseCreate(QCoreApplication &app, QCommandLineParser &parser, QSharedPointer <GostCrypt::NewCore::CreateVolumeParams> options);
    void parseDismount(QCoreApplication &app, QCommandLineParser &parser, QSharedPointer <GostCrypt::NewCore::DismountVolumeParams> options);
    void parseList(QCoreApplication &app, QCommandLineParser &parser, Parser::WhatToList *item);

    quint64 parseSize(QString s, bool *ok);
    bool askPassword(string volume, QString &p);
    GostCrypt::NewCore::FilesystemType::Enum parseFilesystem(QString fs);

}



#endif // PARSER_H
