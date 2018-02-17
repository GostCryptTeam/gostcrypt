#include "UserInterface.h"

/*! converts byte to MB, GB, KB */
QString UserInterface::formatSize(quint64 sizeInByte, bool withFontColor) {
    if (sizeInByte < 1024) return ((withFontColor) ? QString("<font color=#6e9f45>") : QString(""))
        + QString::number(sizeInByte)
        + ((withFontColor) ? QString("</font>") : QString(""))
        + QString(" B");

    else if (sizeInByte < 1048576) return ((withFontColor) ? QString("<font color=#6e9f45>") : QString(""))
        + QString::number((float)sizeInByte / (float)1024, 'f', 1)
        + ((withFontColor) ? QString("</font>") : QString(""))
        + QString(" KB");

    else if (sizeInByte < 1073741824) return ((withFontColor) ? QString("<font color=#6e9f45>") : QString(""))
        + QString::number((float)sizeInByte / (float)1048576, 'f', 1)
        + ((withFontColor) ? QString("</font>") : QString(""))
        + QString(" MB");

    else return ((withFontColor) ? QString("<font color=#6e9f45>") : QString(""))
        + QString::number((float)sizeInByte / (float)1073741824, 'f', 1)
        + ((withFontColor) ? QString("</font>") : QString(""))
        + QString(" GB");
}
