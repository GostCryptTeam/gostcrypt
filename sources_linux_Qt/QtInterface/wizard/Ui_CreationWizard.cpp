/*!
 * \file Ui_CreationWizard.cpp
 * \brief GUI pages for the creation of a new volume.
 * \author The GostCrypt developpment team
 * \version 0.1
 * \date April 20th 2015
 *
 */

#include "Ui_CreationWizard.h"
using namespace std;


void Ui_Creation::CreateVolumeWizard()
{
    Infos = new InfoVolumeCreation;
    step = -1;
    /*!< Window Creation Wizard*/
    VolumeWizard = new QWidget;
    VolumeWizard->setDisabled(false);
    VolumeWizard->setFixedSize(820, 550);
    VolumeWizard->setWindowTitle(QString::fromUtf8(GetValueFromKey(xml, string("INTRO_TITLE")).c_str()));

    /*!< Poster of Creation Wizard*/
    Poster = new QLabel(VolumeWizard);
    Poster->setPixmap(QPixmap("Ressources/GostCrypt_Wizard2.png"));

    /*!< Help QButton*/
    Help = new QPushButton(VolumeWizard);
    Help->setGeometry(QRect(400, 510, 100, 30));
    Help->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDHELP")).c_str()));
    QObject::connect(Help, SIGNAL(pressed()), this, SLOT(Help_url()));

    /*!< Previous QButton*/
    Previous = new QPushButton(VolumeWizard);
    Previous->setGeometry(QRect(505, 510, 100, 30));
    Previous->setText(QString::fromUtf8("< ") + QString::fromUtf8(GetValueFromKey(xml, std::string("PREV")).c_str()));
    QObject::connect(Previous, SIGNAL(pressed()), this, SLOT(Previous_Choice()));

    /*!< Next QButton*/
    Next = new QPushButton(VolumeWizard);
    Next->setGeometry(QRect(610, 510, 100, 30));
    Next->setText(QString::fromUtf8(GetValueFromKey(xml, std::string("NEXT")).c_str()) + QString::fromUtf8(" >"));
    QObject::connect(Next, SIGNAL(pressed()), this, SLOT(Next_Choice()));

    /*!< Cancel QButton*/
    Cancel = new QPushButton(VolumeWizard);
    Cancel->setGeometry(QRect(715, 510, 100, 30));
    Cancel->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("CANCEL")).c_str()));
    QObject::connect(Cancel, SIGNAL(pressed()), VolumeWizard, SLOT(close()));

    /*!< QGroupbox for file of partition container choice*/
    File_Partition_Choice = new QGroupBox(VolumeWizard);
    Creation_Select = new QButtonGroup(File_Partition_Choice);
    RadioCreate1 = new QRadioButton(File_Partition_Choice);
    RadioCreate2 = new QRadioButton(File_Partition_Choice);
    Description1 = new QLabel(File_Partition_Choice);
    Description2 = new QLabel(File_Partition_Choice);

    Standard_or_Hidden = new QGroupBox(VolumeWizard);
    RadioCreatestandard = new QRadioButton(Standard_or_Hidden);
    RadioCreatehidden = new QRadioButton(Standard_or_Hidden);
    Descriptionstandard = new QLabel(Standard_or_Hidden);
    Descriptionhidden = new QLabel(Standard_or_Hidden);
    standard_or_Hidden_Select = new QButtonGroup(Standard_or_Hidden);

    /*!< QGroupbox for file Location*/
    LocationFile = new QGroupBox(VolumeWizard);
    SaveHistory = new QCheckBox(LocationFile);
    SelectFile = new QPushButton(LocationFile);
    SelectVolume = new QComboBox(LocationFile);
    LocationDescription = new QLabel(LocationFile);

    /*!< QGroupbox for Device selection*/
    LocationDevice = new QGroupBox(VolumeWizard);
    treeView = new QTreeWidget(LocationDevice);

    /*!< QGroupboxes for encryption and hash alogithms selection*/
    Encryption_Algorithm = new QGroupBox(VolumeWizard);
    Hash_Algorithm = new QGroupBox(VolumeWizard);
    EncryptChoice = new QComboBox(Encryption_Algorithm);
    AlgoDescription = new QLabel(Encryption_Algorithm);
    HashChoice = new QComboBox(Hash_Algorithm);
    HashDescription = new QLabel(Hash_Algorithm);

    /*!< QGroupbox for volume size selection*/
    VolumeSize = new QGroupBox(VolumeWizard);
    Size = new QTextEdit(VolumeSize);
    RadioSize1 = new QRadioButton(VolumeSize);
    RadioSize2 = new QRadioButton(VolumeSize);
    RadioSize3 = new QRadioButton(VolumeSize);
    SizeDescription = new QLabel(VolumeSize);
    Size_Select = new QButtonGroup(VolumeSize);

    /*!< QGroupbox for Password entry*/
    VolumePassword=new QGroupBox(VolumeWizard);
    Password=new QLineEdit(VolumePassword);
    Confirm=new QLineEdit(VolumePassword);
    PasswordDescription=new QLabel(VolumePassword);
    Pass=new QLabel(VolumePassword);
    ConfirmText=new QLabel(VolumePassword);

    FileSystem = new QGroupBox(VolumeWizard);
    type = new QComboBox(FileSystem);
    QuickFormat = new QCheckBox(FileSystem);
    Format_description = new QLabel(FileSystem);

    Keys = new QGroupBox(VolumeWizard);
    DescriptionPool = new QLabel(Keys);
    DescriptionHeader = new QLabel(Keys);
    DescriptionMaster = new QLabel(Keys);
    loadBar = new QGroupBox(VolumeWizard);
    Load = new QProgressBar(loadBar);
    Description_format = new QLabel(Keys);

    Creation_completed = new QGroupBox(VolumeWizard);
    Final_description = new QLabel(Creation_completed);

    /*!< Call of the first step*/
    Next_Choice();
    /*!< show the Creation wizard win#include <fstream> dow*/
    VolumeWizard->show();
}


void Ui_Creation::Previous_Choice()
{
    /*!< Decrease the step and actualize the page*/
    step-=2;
    Next_Choice();
}

void Ui_Creation::Next_Choice()
{
    /*!< Increase the step and actualize the page*/
    /*!< Add informations in the Volume information structure*/
    /*!< Enable the right QGroupbox and disable the others*/

    LocationFile->setDisabled(true);
    LocationFile->setVisible(false);
    File_Partition_Choice->setDisabled(true);
    File_Partition_Choice->setVisible(false);
    Standard_or_Hidden->setDisabled(true);
    Standard_or_Hidden->setVisible(false);
    LocationDevice->setDisabled(true);
    LocationDevice->setVisible(false);
    Encryption_Algorithm->setDisabled(true);
    Encryption_Algorithm->setVisible(false);
    Hash_Algorithm->setDisabled(true);
    Hash_Algorithm->setVisible(false);
    VolumeSize->setDisabled(true);
    VolumeSize->setVisible(false);
    VolumePassword->setDisabled(true);
    VolumePassword->setVisible(false);
    FileSystem->setDisabled(true);
    FileSystem->setVisible(false);
    Keys->setDisabled(true);
    loadBar->setDisabled(true);
    Keys->setVisible(false);
    loadBar->setVisible(false);
    Creation_completed->setDisabled(true);
    Creation_completed->setVisible(false);

    Previous->setDisabled(false);
    Previous->setVisible(true);
    step++;

    switch (step)
    {
    case 0:
        Previous->setDisabled(true);
        Previous->setVisible(false);
        File_Partition_Choice->setDisabled(false);
        File_Partition_Choice->setVisible(true);
        File_or_Partiton();//1
        break;
    case 1:
        Standard_or_Hidden->setDisabled(false);
        Standard_or_Hidden->setVisible(true);
        Infos->IsFileContainer = RadioCreate1->isChecked();
        System_Type();//2
        break;
    case 2:
        Infos->IsHidden = Standard_or_Hidden->isChecked();
        if(Infos->IsFileContainer)
        {
            LocationFile->setDisabled(false);
            LocationFile->setVisible(true);
            File_Location();//3
        }
        else
        {
            LocationDevice->setDisabled(false);
            LocationDevice->setVisible(true);
            Device_Location();//3
        }
        break;
    case 3:
        if(Infos->IsFileContainer)
        {
            Infos->path = (char *)calloc(strlen(SelectVolume->currentText().toStdString().c_str())+1,sizeof(char));
            if(Infos->path)
            {
                strcpy(Infos->path,SelectVolume->currentText().toStdString().c_str());
            }
        }
        Encryption_Algorithm->setDisabled(false);
        Encryption_Algorithm->setVisible(true);
        Hash_Algorithm->setDisabled(false);
        Hash_Algorithm->setVisible(true);
        Algos_Choices();
        break;
    case 4:
        Infos->Algo = EncryptChoice->currentIndex();
        Infos->Hash = HashChoice->currentIndex();
        VolumeSize->setDisabled(false);
        VolumeSize->setVisible(true);
        Volume_Size();
        break;
    case 5:
        Infos->VolumeSize = Size->toPlainText().toLongLong();
        if(RadioSize1->isChecked())
            Infos->VolumeSize *= 1024;
        if(RadioSize2->isChecked())
            Infos->VolumeSize *= 1048576;
        if(RadioSize3->isChecked())
            Infos->VolumeSize *= 1073741824;
        VolumePassword->setDisabled(false);
        VolumePassword->setVisible(true);
        Volume_Password();
        break;
    case 6:
        FileSystem->setDisabled(false);
        FileSystem->setVisible(true);
        Format_Options();
        break;
    case 7:
        Infos->FileSystem = type->currentIndex();
        Keys->setDisabled(false);
        loadBar->setDisabled(false);
        Keys->setVisible(true);
        loadBar->setVisible(true);
        Volume_Format();
        break;
    case 8:
        Keys->setDisabled(false);
        loadBar->setDisabled(false);
        Keys->setVisible(true);
        loadBar->setVisible(true);
        Load->setRange(0, 100);
        Load->setValue(75);
		Load->setFormat(QString::fromUtf8("%p%"));
        break;
    case 9:
        Creation_completed->setDisabled(false);
        Creation_completed->setVisible(true);
        End();
        break;
    }
}

void Ui_Creation::Help_url()
{
	QDesktopServices::openUrl(QUrl(QString::fromUtf8("https://www.gostcrypt.org/wiki/doku.php"), QUrl::TolerantMode));
}
