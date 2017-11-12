#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QQuickItem>
#include <QStringList>
#include <QSettings>
#include <QDataStream>
#define APPLICATION "Gostcrypt"
#define ORGANISATION "The GostCrypt Team"
#define GI_KEY_MAP(variant, key) variant.value(key)

struct volumeInfo {
    volumeInfo() :
        sName(""),
        sPath(""),
        sReadOnly(false),
        sRemovableMedium(false),
        sUponLogon(false),
        sMountWhenDeviceConnected(false),
        sOpenExplorerWhenMounted(false),
        sDoNotMountVolumeOnMountAllFavorite(false) {}
    QString sName;
    QString sPath;
    bool sReadOnly;
    bool sRemovableMedium;
    bool sUponLogon;
    bool sMountWhenDeviceConnected;
    bool sOpenExplorerWhenMounted;
    bool sDoNotMountVolumeOnMountAllFavorite;
    friend bool operator==(const volumeInfo& lhs, const volumeInfo& rhs) {
        if(lhs.sPath == rhs.sPath) return true;
        return false;
    }
    friend QDataStream &operator<<(QDataStream &out, const volumeInfo &v) {
        out << v.sPath << v.sName << v.sDoNotMountVolumeOnMountAllFavorite << v.sMountWhenDeviceConnected << v.sOpenExplorerWhenMounted << v.sReadOnly << v.sRemovableMedium << v.sUponLogon;
        return out;
    }
    friend QDataStream &operator>>(QDataStream &in, volumeInfo &v) {
        in >> v.sPath >> v.sName >> v.sDoNotMountVolumeOnMountAllFavorite >> v.sMountWhenDeviceConnected >> v.sOpenExplorerWhenMounted >> v.sReadOnly >> v.sRemovableMedium >> v.sUponLogon;
        return in;
    }
};


Q_DECLARE_METATYPE(volumeInfo)

/*!
 * \brief The UserSettings class
 * All user parameters are stored in
 * a file managed by the OS.
 */
class UserSettings : public QObject
{
    Q_OBJECT
public:
    UserSettings();
    ~UserSettings();

    /*!
     * \brief UserSettings::setSetting
     * \param key The index of the setting
     * \param value The value linked to the setting
     */
    Q_INVOKABLE void setSetting(const QString& aKey, const QString& aValue);

    /*!
     * \brief UserSettings::getSetting
     * \param key : The index of the setting to be returned
     * \return The value of the given key setting
     */
    Q_INVOKABLE QVariant getSetting(const QString& aKey) const;

    /*!
     * \brief getVolumePaths
     * \return All the lastest opened volumes
     */
    Q_INVOKABLE QStringList getVolumePaths(const bool aEmptyStart);

    /*!
     * \brief addVolumePaths
     * \param path : The path just opened
     */
    Q_INVOKABLE void addVolumePath(const QUrl& path);

    Q_INVOKABLE void erasePaths();

    Q_INVOKABLE QVariantList getFavoritesVolumes() const;
    Q_INVOKABLE void setFavoritesVolumes(QString);
    Q_INVOKABLE bool isFavorite(const QString&) const;
    Q_INVOKABLE bool setFavoriteVolumeSetting(QVariantMap options);
    Q_INVOKABLE QVariantMap getFavoriteVolumeSetting(const QString&);
    Q_INVOKABLE qint32 getNumberOfCore() const;

private:
    QSettings mSettings;
    void setVolumeInfos(volumeInfo& v,
                        QString aName = "",
                        QString aPath = "",
                        bool aReadOnly = false,
                        bool aRemovableMedium = false,
                        bool aUponLogon = false,
                        bool aMountWhenDeviceConnected = false,
                        bool aOpenExplorerWhenMounted = false,
                        bool aDoNotMountVolumeOnMountAllFavorite = false
            );


};

#endif // USERSETTINGS_H
