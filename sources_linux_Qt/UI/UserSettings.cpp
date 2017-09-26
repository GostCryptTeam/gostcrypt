#include "UserSettings.h"
#include <QDebug>
#define NB_PATH 5

UserSettings::UserSettings() : mSettings(ORGANISATION, APPLICATION)
{
    qRegisterMetaTypeStreamOperators<QList<QString>> ("QList<QString>");
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
        mSettings.setValue("Favorite-Volumes", QVariant::fromValue(QList<QString>()));
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

void UserSettings::addVolumePath(const QUrl &path)
{
    QString canonicalPath = path.path();
    //Checks if the path is already in the list
    for(int i=0; i<NB_PATH; i++)
        if( mSettings.value("MountV-path"+ QString::number(i)).toString() == canonicalPath/* && i!=0*/)
        {
            //permutate the path with the first element
            mSettings.setValue("MountV-path"+ QString::number(i), mSettings.value("MountV-path0"));
            mSettings.setValue("MountV-path0", canonicalPath);
            return;
        }
    //Append the new path
    for(int i=NB_PATH-1; i>0; i--)
        mSettings.setValue("MountV-path"+ QString::number(i), mSettings.value("MountV-path"+ QString::number(i-1)));
    mSettings.setValue("MountV-path0", canonicalPath);
}

void UserSettings::erasePaths()
{
    for(int i=0; i<NB_PATH; i++)
        mSettings.setValue("MountV-path"+ QString::number(i), "");
}

bool UserSettings::isFavorite(const QString& aPath) const
{
    QList<QString> favoritePath = mSettings.value("Favorite-Volumes").value<QList<QString>>();
    for(auto i : favoritePath)
        if(i == aPath) return true;
    return false;
}

QVariantList UserSettings::getFavoritesVolumes() const
{
    QList<QString> favoritePath = mSettings.value("Favorite-Volumes").value<QList<QString>>();
    QVariantList qVariant;
    for(auto i : favoritePath)
        qVariant << i;
    return qVariant;
}

void UserSettings::setFavoritesVolumes(QString aPath)
{
    QList<QString> favoritePath = mSettings.value("Favorite-Volumes").value<QList<QString>>();
    for(auto i : favoritePath)
        if(i == aPath)
        {
            favoritePath.removeOne(i);
            mSettings.setValue("Favorite-Volumes", QVariant::fromValue(favoritePath));
            mSettings.sync();
#ifdef QT_DEBUG
            qDebug() << "[DEBUG] : This volume is already in favorites. Removing it... (" << aPath << ")";
#endif
            return;
        }
    favoritePath.append(aPath);
    mSettings.setValue("Favorite-Volumes", QVariant::fromValue(favoritePath));
    mSettings.sync();
#ifdef QT_DEBUG
            qDebug() << "[DEBUG] : Adding a new volume to the favorites. (" << aPath << ")";
#endif
}
