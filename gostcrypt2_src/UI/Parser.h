#ifndef PARSER_H
#define PARSER_H

#include "Core/CoreRequest.h"
#include "Volume/VolumePassword.h"
#include <iostream>
#include <QCommandLineParser>
#include <QException>
#include <QTranslator>

namespace Parser {

    // default values for creating a volume
    #define DEFAULT_ALGORITHM "Gost Grasshopper"
    #define DEFAULT_KDF "HMAC-Whirlpool"
    #define DEFAULT_SIZE 10485760 // 10Mio
    #define DEFAULT_OUTER_SIZE 1.0
    #define DEFAULT_INNER_SIZE 0.7

    /**
     * @brief
     *
     */
    class ParseException : QException {
    public:
        /**
         * @brief
         *
         */
        ParseException() : message("") { }
        /**
         * @brief
         *
         * @param p
         */
        ParseException(const ParseException &p) : message(p.message) {}
        /**
         * @brief
         *
         * @param message
         */
        explicit ParseException(const QString &message) { this->message = message; }
        /**
         * @brief
         *
         */
        void raise() const { throw *this; }
        /**
         * @brief
         *
         * @return ParseException
         */
        ParseException *clone() const { return new ParseException(*this); }
        /**
         * @brief
         *
         * @return QString
         */
        QString getMessage() { return this->message; }
    private:
        QString message; /**< TODO: describe */
    };

    /**
     * @brief
     *
     */
    typedef enum _WhatToList
    {
        Volumes,
        Algorithms,
        Hashs,
        Devices
    /**
     * @brief
     *
     */
    } WhatToList;

    /**
     * @brief
     *
     * @param parser
     * @param options
     */
    void parseMount(QCommandLineParser &parser, QSharedPointer <GostCrypt::Core::MountVolumeRequest> options);
    /**
     * @brief
     *
     * @param parser
     * @param options
     */
    void parseCreate(QCommandLineParser &parser, QSharedPointer <GostCrypt::Core::CreateVolumeRequest> options);
    /**
     * @brief
     *
     * @param parser
     * @param volume
     */
    void parseDismount(QCommandLineParser &parser, QSharedPointer <GostCrypt::Core::DismountVolumeRequest> volume);
    /**
     * @brief
     *
     * @param parser
     * @param item
     */
    void parseList(QCommandLineParser &parser, Parser::WhatToList *item);
    /**
     * @brief
     *
     * @param parser
     * @param files
     */
    void parseCreateKeyFiles(QCommandLineParser &parser, QStringList &files);
    /**
     * @brief
     *
     * @param parser
     * @param options
     */
    void parseBenchmark(QCommandLineParser &parser, QSharedPointer<GostCrypt::Core::BenchmarkAlgorithmsRequest> options);
    /**
     * @brief
     *
     * @param s
     * @param ok
     * @return quint64
     */
    quint64 parseSize(QString s, bool *ok);
    /**
     * @brief
     *
     * @param volume
     * @param p
     * @return bool
     */
    bool askPassword(std::string volume, QString &p);

}



#endif // PARSER_H
