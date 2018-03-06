#ifndef TRANSLATIONAPP_H
#define TRANSLATIONAPP_H

#include <QtGui>
#include <QApplication>
#include <QTranslator>
#include <QLocale>

/**
 * @brief The translationApp class
 * This class allow the user to dynamically change the application
 * language by choosing from the list
 */
class TranslationApp : public QObject
{
    Q_OBJECT
    /**
     * @brief Notifies QML of language change
     */
    Q_PROPERTY(QString tr READ tr NOTIFY languageChanged)

 public:
    /**
     * @brief TranslationApp's constructor
     * The constructor retrieves the system language as well
     * as the list of available translations for GostCrypt.
     * It then fills in the useful class attributes and translates
     * if possible gostcrypt into the language of the computer from the start.
     */
    TranslationApp();
    /**
    * @brief TranslationApp destructor
    * Freeing memory
    */
    ~TranslationApp();
    /**
     * @brief tr
     * This function is used to dynamically notify qml
     * of language change
     * @return QString an empty text
     */
    QString tr();
    /**
    * @brief setLanguage
    * @param Language The language suffix code
    * associated with the language to be used
    */
    Q_INVOKABLE void setLanguage(const QString& language);
    /*!
    * @brief getAvaibleLanguages
    * @return QVariantList A list of all avaible volumes
    */
    Q_INVOKABLE QVariantList getAvaibleLanguages() const;
 signals:
    /**
     * @brief languageChanged
     * A signal which notify qml fromthe language change
     */
    void languageChanged();

 private:
    QString mTranslationpath; /**< Contains the absolute path of the translation repertory */
    QList <QPair<QTranslator*, QString>>
                                      mListOfTranslationsAvaible; /**< Contains a list of all the translation objects associated with their language code */
    QTranslator* currentTranslator;   /**< A pointer to the current translator */
};

#endif // TRANSLATIONAPP_H
