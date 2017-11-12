#include "UserSettings.h"
#include <QDebug>
#include <QThread>
#define NB_PATH 5

UserSettings::UserSettings() : mSettings(ORGANISATION, APPLICATION)
{
    qRegisterMetaTypeStreamOperators<QList<QString>> ("QList<QString>");
    qRegisterMetaTypeStreamOperators<QList<volumeInfo>> ("QList<volumeInfo>");

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
        mSettings.setValue("Perf-Use", 0);
        mSettings.setValue("Perf-NumberOfCore", QThread::idealThreadCount());
        mSettings.setValue("Favorite-Volumes", QVariant::fromValue(QList<volumeInfo>()));
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
    QList<volumeInfo> favoritePath = mSettings.value("Favorite-Volumes").value<QList<volumeInfo>>();
    for(auto i : favoritePath)
        if(i.sPath == aPath) return true;
    return false;
}

bool UserSettings::setFavoriteVolumeSetting(QVariantMap options)
{
    QList<volumeInfo> favoritePath = mSettings.value("Favorite-Volumes").value<QList<volumeInfo>>();
    for(volumeInfo i : favoritePath)
        if(i.sPath == GI_KEY_MAP(options, "sPath").toString())
        {
            volumeInfo tmp;
            setVolumeInfos(tmp,
                     GI_KEY_MAP(options, "sName").toString(),
                     GI_KEY_MAP(options, "sPath").toString(),
                     GI_KEY_MAP(options, "sReadOnly").toBool(),
                     GI_KEY_MAP(options, "sRemovableMedium").toBool(),
                     GI_KEY_MAP(options, "sUponLogon").toBool(),
                     GI_KEY_MAP(options, "sMountWhenDeviceConnected").toBool(),
                     GI_KEY_MAP(options, "sDoNotMountVolumeOnMountAllFavorite").toBool());
            favoritePath.removeOne(i);
            favoritePath.append(tmp);
            mSettings.setValue("Favorite-Volumes", QVariant::fromValue(favoritePath));
            mSettings.sync();
            return true;
        }
    //Volume doesn't exists, should never be called
    return false;
}

QVariantMap UserSettings::getFavoriteVolumeSetting(const QString &aPath)
{
    QList<volumeInfo> favoritePath = mSettings.value("Favorite-Volumes").value<QList<volumeInfo>>();
    for(auto i : favoritePath) {
        if(i.sPath == aPath) {
            QVariantMap map;
            map.insert("sPath", i.sPath);
            map.insert("sName", i.sName);
            map.insert("sUponLogon", i.sUponLogon);
            map.insert("sReadOnly", i.sReadOnly);
            map.insert("sRemovableMedium", i.sRemovableMedium);
            map.insert("sMountWhenDeviceConnected", i.sMountWhenDeviceConnected);
            map.insert("sOpenExplorerWhenMounted", i.sOpenExplorerWhenMounted);
            map.insert("sDoNotMountVolumeOnMountAllFavorite", i.sDoNotMountVolumeOnMountAllFavorite);
            return map;
        }
    }
    return QVariantMap();
}

qint32 UserSettings::getNumberOfCore() const
{
    return QThread::idealThreadCount();
}

void UserSettings::setVolumeInfos(volumeInfo& v, QString aName, QString aPath, bool aReadOnly, bool aRemovableMedium, bool aUponLogon, bool aMountWhenDeviceConnected, bool aOpenExplorerWhenMounted, bool aDoNotMountVolumeOnMountAllFavorite)
{
    v.sName = aName;
    v.sPath = aPath;
    v.sReadOnly = aReadOnly;
    v.sRemovableMedium = aRemovableMedium;
    v.sUponLogon = aUponLogon;
    v.sMountWhenDeviceConnected = aMountWhenDeviceConnected;
    v.sOpenExplorerWhenMounted = aOpenExplorerWhenMounted;
    v.sDoNotMountVolumeOnMountAllFavorite = aDoNotMountVolumeOnMountAllFavorite;
}

QVariantList UserSettings::getFavoritesVolumes() const
{
    QList<volumeInfo> favoritePath = mSettings.value("Favorite-Volumes").value<QList<volumeInfo>>();
    QVariantList qVariant;
    for(auto i : favoritePath)
        qVariant << i.sPath << i.sName;
    return qVariant;
}

void UserSettings::setFavoritesVolumes(QString aPath)
{
    QList<volumeInfo> favoritePath = mSettings.value("Favorite-Volumes").value<QList<volumeInfo>>();
    for(auto i : favoritePath)
        if(i.sPath == aPath)
        {
            favoritePath.removeOne(i);
            mSettings.setValue("Favorite-Volumes", QVariant::fromValue(favoritePath));
            mSettings.sync();
#ifdef QT_DEBUG
            qDebug() << "[DEBUG] : This volume is already in favorites. Removing it... (" << aPath << ")";
#endif
            return;
        }
    volumeInfo v = volumeInfo();
    v.sPath = aPath;
    qDebug() << v.sUponLogon;
    favoritePath.append(v);
    mSettings.setValue("Favorite-Volumes", QVariant::fromValue(favoritePath));
    mSettings.sync();
#ifdef QT_DEBUG
            qDebug() << "[DEBUG] : Adding a new volume to the favorites. (" << aPath << ")";
#endif
}
