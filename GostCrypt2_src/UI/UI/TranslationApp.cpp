#include "TranslationApp.h"

TranslationApp::TranslationApp() :
    currentTranslator(nullptr)
{
    //get the system language
    QString systemLang = QLocale::system().name();
    systemLang.truncate(systemLang.lastIndexOf('_'));
#ifdef QT_DEBUG
    qDebug() << "System default language is : " + systemLang;
#endif
    //Getting the folder translations's path
    mTranslationpath = QApplication::applicationDirPath();
    mTranslationpath.append("/translations/");
    QDir dir(mTranslationpath);
    //Getting all the translations at once
    QStringList listOfTranslations = dir.entryList(QStringList("gostcrypt_*.qm"));

    //For all translation file, loading the translation file
    for (int i = 0; i < listOfTranslations.size(); ++i) {
        //extracting all the prefixes
        QString langPrefix;
        langPrefix = listOfTranslations[i]; // "gostcrypt_*.qm"
        langPrefix.truncate(langPrefix.lastIndexOf('.')); // "gostcrypt_*"
        langPrefix.remove(0, langPrefix.indexOf('_') + 1); // "*"

        //saving and loading all theses files
        QTranslator * translator;
        translator = new QTranslator();
        if(!translator->load(dir.absolutePath()+"/"+listOfTranslations[i], "_"))
        {
#ifdef QT_DEBUG
            qDebug() << "Incorrect translation file (or incomplete) : " << dir.absolutePath()+"/"+listOfTranslations[i];
#endif
            delete translator;
        }else{
            //Saving all the languages currently avaible
            mListOfTranslationsAvaible.append(QPair<QTranslator*, QString>(translator, langPrefix));
            //if the current language is the machine language, changing the language:
            if(langPrefix == systemLang) {
#ifdef QT_DEBUG
            qDebug() << "The language of the machine is known: now loading \""+systemLang+"\" translation";
#endif
                setLanguage(systemLang);
            }
        }
    }
}

TranslationApp::~TranslationApp()
{
    for(int i = 0; i<mListOfTranslationsAvaible.size(); i++)
        if(mListOfTranslationsAvaible[i].first != nullptr) {
            delete mListOfTranslationsAvaible[i].first;
            mListOfTranslationsAvaible[i].first = nullptr;
        }
}

QString TranslationApp::tr()
{
    return "";
}

void TranslationApp::setLanguage(const QString &language)
{
    for(int i = 0; i<mListOfTranslationsAvaible.size(); i++)
    {
        if(mListOfTranslationsAvaible[i].second == language)
        {
            //remove the old translator and load the new
            qApp->removeTranslator(currentTranslator);
            qApp->installTranslator(mListOfTranslationsAvaible[i].first);
            currentTranslator = mListOfTranslationsAvaible[i].first;
            emit languageChanged();
            return;
        }
    }
    //default language
    qApp->removeTranslator(currentTranslator);
    emit languageChanged();
}

QVariantList TranslationApp::getAvaibleLanguages() const
{
    //Retrieving the list of languages loaded by gostcrypt
    QVariantList list;
    QVariantList subList;
    subList.append("English");
    subList.append("en");
    list.append(subList);
    subList.clear();
    for(auto it : mListOfTranslationsAvaible) {
        subList.append(QLocale(it.second).nativeLanguageName());
        subList.append(it.second);
        list.append(subList);
        subList.clear();
    }
    return list;
}
