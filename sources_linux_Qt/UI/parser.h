#ifndef PARSER_H
#define PARSER_H

#include "NewCore/CoreParams.h"

#include <QCommandLineParser>
#include <iostream>
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

    quint64 parseSize(QString s, bool *ok);

    void parseMount(QCoreApplication &app, QCommandLineParser &parser, GostCrypt::NewCore::MountVolumeParams *options);
    void parseCreate(QCoreApplication &app, QCommandLineParser &parser, GostCrypt::NewCore::CreateVolumeParams *options);
    void parseDismount(QCoreApplication &app, QCommandLineParser &parser, GostCrypt::NewCore::DismountVolumeParams *volume);
    void parseList(QCoreApplication &app, QCommandLineParser &parser, Parser::WhatToList *item);

}

#endif // PARSER_H
