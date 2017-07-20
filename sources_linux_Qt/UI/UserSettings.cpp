#include "UserSettings.h"
#include <QDebug>
#define NB_PATH 5

UserSettings::UserSettings() : mSettings(ORGANISATION, APPLICATION)
{
    //Checking if the setting file is empty
    if (mSettings.contains("default")) {
        qDebug() << "[Debug] : QSettings file exists.";
    }else{
        qDebug() << "[Debug] : Creating a default QSettings file.";
        mSettings.setValue("default", 0);
        mSettings.setValue("MountV-ShowPassword", 0);
        mSettings.setValue("MountV-SaveHistory", 0);
        mSettings.setValue("MountV-CachePwd", 0);
        mSettings.setValue("MountV-UseKeyFiles", 0);

        for(int i=0; i<NB_PATH; i++)
             mSettings.setValue("MountV-path"+QString::number(i), "");
    }
}

UserSettings::~UserSettings()
{
#ifdef QT_DEBUG
    //mSettings.clear();
#endif
}

void UserSettings::setSetting(const QString &aKey, const QString &aValue)
{
    mSettings.setValue(aKey, aValue);
}


QVariant UserSettings::getSetting(const QString &aKey) const
{
    return mSettings.value(aKey).toString();
}

QStringList UserSettings::getVolumePaths(const bool aEmptyStart)
{
    QStringList table;
    QString text;
    if(aEmptyStart)
        table.append("");
    for(int i=0; i<NB_PATH; i++)
        if((text = mSettings.value("MountV-path"+ QString::number(i)).toString()) != "")
            table.append(text);

    return table;
}

void UserSettings::addVolumePath(const QString &path)
{
    //Checks if the path is already in the list
    for(int i=0; i<NB_PATH; i++)
        if( mSettings.value("MountV-path"+ QString::number(i)).toString() == path/* && i!=0*/)
        {
            //permutate the path with the first element
            mSettings.setValue("MountV-path"+ QString::number(i), mSettings.value("MountV-path0"));
            mSettings.setValue("MountV-path0", path);
            return;
        }
    //Append the new path
    for(int i=NB_PATH-1; i>0; i--)
        mSettings.setValue("MountV-path"+ QString::number(i), mSettings.value("MountV-path"+ QString::number(i-1)));
    mSettings.setValue("MountV-path0", path);
}

void UserSettings::erasePaths()
{
    for(int i=0; i<NB_PATH; i++)
        mSettings.setValue("MountV-path"+ QString::number(i), "");
}