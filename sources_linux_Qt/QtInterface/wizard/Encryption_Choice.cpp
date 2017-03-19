#include "Encryption_Choice.h"


void Ui_Creation::Algos_Choices()
{
    /*!< QGroupbox of the encryption algorithm choice*/
    Encryption_Algorithm->setGeometry(QRect(400, 0, 400, 350));
    Encryption_Algorithm->setTitle(QString::fromUtf8(GetValueFromKey(xml,std::string("ENCRYPTION_ALGORITHM")).c_str()));

    /*!< QGroupbox of the hash algorithm choice*/
    Hash_Algorithm->setGeometry(QRect(400, 350, 400, 150));
    Hash_Algorithm->setTitle(QString::fromUtf8(GetValueFromKey(xml,std::string("IDT_HASH_ALGO")).c_str()));

    /*!< QCombobox to choose an encryption alogrithm*/
    EncryptChoice->setGeometry(QRect(15, 45, 350, 30));
    EncryptChoice->clear();
    EncryptChoice->addItems(QStringList() << QString::fromUtf8("GOST 28174-89") << QString::fromUtf8("Grasshopper"));

    /*!< QCombobox to choose an encryption alogrithm*/
    HashChoice->setGeometry(QRect(15, 45, 350, 30));
    HashChoice->clear();
    HashChoice->addItems(QStringList() << QString::fromUtf8("GOST R 34-11 2012") << QString::fromUtf8("GOST R 34-11 1994") << QString::fromUtf8("Whirlpool"));

    /*!< Encryption and hash description*/
    AlgoDescription->setGeometry(QRect(20, 75, 370, 250));
    AlgoDescription->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("GOST_HELP")).c_str()));
    /*HashDescription->setGeometry(QRect(20, 80, 370, 60));
    HashDescription->setText(QString::fromUtf8("GOST R 34.11-94 : Original Hash function (256 bits)\nGOST R 34.11-2012 : New version (512 bits)\nWhirlpool : Based on AES (512 bits)"));*/
    AlgoDescription->setWordWrap(true);

}

