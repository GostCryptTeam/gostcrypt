#include "Format.h"


void Ui_Creation::Format_Options()
{

    FileSystem->setGeometry(QRect(400, 0, 400, 500));
    FileSystem->setTitle(QString::fromUtf8(GetValueFromKey(xml,std::string("FORMAT_TITLE")).c_str()));

    Format_description->setGeometry(QRect(10, 130, 380, 280));
    Format_description->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("FORMAT_DEVICE_FOR_ADVANCED_ONLY")).c_str()));
    Format_description->setWordWrap(true);
    type->setGeometry(QRect(90, 40, 221, 31));
    type->clear();
    type->addItem(QString::fromUtf8("FAT"));
    type->addItem(QString::fromUtf8("Linux EXT2"));
    type->addItem(QString::fromUtf8("Linux EXT3"));
    type->addItem(QString::fromUtf8("Linux EXT4"));

    QuickFormat->setGeometry(QRect(120, 80, 180, 25));
    QuickFormat->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDC_QUICKFORMAT")).c_str()));
    QuickFormat->setChecked(true);
}

void Ui_Creation::Volume_Format()
{
    Next->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("FORMAT")).c_str()));
     /*!< QGroupbox of the encryption algorithm choice*/
    Keys->setGeometry(QRect(400, 0, 400, 350));
    Keys->setTitle(QString::fromUtf8("Keys"));
    DescriptionPool->setGeometry(QRect(20, 50, 100, 20));
    DescriptionPool->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDT_RANDOM_POOL")).c_str()));

    DescriptionHeader->setGeometry(QRect(20, 70, 100, 20));
    DescriptionHeader->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDT_HEADER_KEY")).c_str()));

    DescriptionMaster->setGeometry(QRect(20, 90, 100, 20));
    DescriptionMaster->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDT_MASTER_KEY")).c_str()));

    Description_format->setGeometry(QRect(10, 120, 380,150));
    Description_format->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDT_COLLECTING_RANDOM_DATA_NOTE")).c_str()));
    Description_format->setWordWrap(true);

    /*!< QGroupbox of the hash algorithm choice*/
    loadBar->setGeometry(QRect(400, 350, 400, 150));
    //loadBar->setTitle(QString::fromUtf8("Loading"));
    Load->setGeometry(QRect(20, 40, 360, 31));
}

void Ui_Creation::End()
{
    Next->setText(QString::fromUtf8("Finish"));
    Previous->setDisabled(true);
    Previous->setVisible(false);

    Creation_completed->setGeometry(QRect(400, 0, 400, 360));
    Creation_completed->setTitle(QString::fromUtf8(GetValueFromKey(xml,std::string("FORMAT_FINISHED_TITLE")).c_str()));
    Final_description->setGeometry(QRect(5, 120, 380, 100));
    Final_description->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("FORMAT_FINISHED_HELP")).c_str()));
    Final_description->setWordWrap(true);
    QObject::connect(Next, SIGNAL(pressed()), VolumeWizard, SLOT(close()));

}
