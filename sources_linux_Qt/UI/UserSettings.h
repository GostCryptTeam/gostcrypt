#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QQuickItem>
#include <QStringList>
#include <QSettings>
#define APPLICATION "Gostcrypt"
#define ORGANISATION "The GostCrypt Team"

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
    Q_INVOKABLE void addVolumePath(const QString& path);

    Q_INVOKABLE void erasePaths();

    Q_INVOKABLE QVariantList getFavoritesVolumes() const;
    Q_INVOKABLE void setFavoritesVolumes(QString);
    Q_INVOKABLE bool isFavorite(const QString&) const;

private:
    QSettings mSettings;

};

#endif // USERSETTINGS_H
