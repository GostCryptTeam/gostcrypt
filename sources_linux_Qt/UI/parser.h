#ifndef PARSER_H
#define PARSER_H

#include "Core/Core.h"
#include "Core/Unix/CoreService.h"
#include "Platform/SystemLog.h"
#include "volumecreation.h"

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

uint64 parseSize(QString s, bool *ok);

void parseMount(QCoreApplication &app, QCommandLineParser &parser, GostCrypt::MountOptions *options);
void parseCreate(QCoreApplication &app, QCommandLineParser &parser, VolumeCreation *options);
void parseDismount(QCoreApplication &app, QCommandLineParser &parser, QString *volume);
void parseList(QCoreApplication &app, QCommandLineParser &parser, WhatToList *item);

}

#endif // PARSER_H
