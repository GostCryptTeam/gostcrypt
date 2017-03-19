#ifndef UI_CREATIONWIZARD_H
#define UI_CREATIONWIZARD_H
/*!
 * \file Ui_CreationWizard.h
 * \brief GUI pages for the creation of a new volume.
 * \author The GostCrypt developpment team
 * \version 0.1
 * \date April 20th 2015
 *
 */
#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QTreeView>
#include <QMainWindow>
#include <QRadioButton>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QScrollArea>
#include <QStatusBar>
#include <QWidget>
#include <QDialog>
#include <QTextBlock>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QDebug>
#include <QObject>
#include <QTextBlock>
#include <QTextLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QTextEdit>
#include <QLineEdit>
#include <QListView>
#include <QTreeWidget>
#include <QProgressBar>
#include "GC_window.h"
#include <QLatin1String>
#include <inttypes.h>
#include <math.h>
#include <fstream>
#include <string.h>
#include <QProgressBar>
//#include "../Language/XML.h"
#include "Size.h"
#include "Password.h"
#include "Location.h"
#include "Format.h"
#include "Encryption_Choice.h"
#include "Container_Type.h"



QT_BEGIN_NAMESPACE


/**
 * \struct InfoVolumeCreation
 * \brief Information for Volume Creation
 *
 * Structure which contains every informations needs to
 * create au Gostrcypt Volume with the Creation Wizard
 */
typedef struct v_Info_Volume_Creation
{
    bool IsFileContainer;
    bool IsHidden;
    char *path;
    int Hash; //GOST94 = 0,GOST212 = 1, Whirpool = 2
    int Algo; //GOST89 = 0,GRASSHOPPER = 1
    unsigned long long VolumeSize;
    int FileSystem; //FAT = 0, EXT2 = 1, EXT3 = 2, EXT4 = 3
}InfoVolumeCreation;

/*extern "C"{
int Get_Info_Device(const char * path, Devices_Info *info);
}*/

/*! \class Ui_Creation
   * \brief class with any variables and functions for the Creation Wizard
   *
   */
class Ui_Creation : public QObject
{

    Q_OBJECT

public :

    QLabel *Poster;
    QWidget *VolumeWizard;

    QPushButton *Help;
    QPushButton *Previous;
    QPushButton *Next;
    QPushButton *Cancel;

    QRadioButton *RadioCreate1;
    QRadioButton *RadioCreate2;
    QGroupBox *File_Partition_Choice;
    QLabel *Description1;
    QLabel *Description2;
    QButtonGroup *Creation_Select;

    QRadioButton *RadioCreatestandard;
    QRadioButton *RadioCreatehidden;
    QGroupBox *Standard_or_Hidden;
    QLabel *Descriptionstandard;
    QLabel *Descriptionhidden;
    QButtonGroup *standard_or_Hidden_Select;

    QLabel *LocationDescription;

    QGroupBox* LocationFile;
    QCheckBox *SaveHistory;
    QPushButton *SelectFile;
    QComboBox *SelectVolume;

    QGroupBox *LocationDevice;
    InfoVolumeCreation *Infos;
    QWidget *SelectionDevice;
    QTreeWidget *treeView;

    QComboBox *EncryptChoice;
    QLabel *AlgoDescription;
    QLabel *HashDescription;
    QComboBox *HashChoice;
    QGroupBox *Encryption_Algorithm;
    QGroupBox *Hash_Algorithm;

    QGroupBox *VolumeSize;
    QRadioButton *RadioSize1;
    QRadioButton *RadioSize2;
    QRadioButton *RadioSize3;
    QLabel *SizeDescription;
    QTextEdit *Size;
    QButtonGroup *Size_Select;

    QGroupBox *VolumePassword;
    QLineEdit *Password;
    QLineEdit *Confirm;
    QLabel *PasswordDescription;
    QLabel *Pass;
    QLabel *ConfirmText;

    QGroupBox * FileSystem;
    QComboBox * type;
    QCheckBox *QuickFormat;
    QLabel *Format_description;

    QGroupBox *Keys;
    QGroupBox *loadBar;
    QLabel *DescriptionPool;
    QLabel *DescriptionHeader;
    QLabel *DescriptionMaster;
    QLabel *Description_format;
    QProgressBar * Load;

    QGroupBox *Creation_completed;
    QLabel *Final_description;

    char step;


public slots:

    /*!
     * \fn void CreateVolumeWizard()
     * \brief Create the Creation Wizard Pages. The Creation Wizard is an unic page where the QGroupBox are replacing inf unction of the step. All information are add the InfoVolumeCreation structure
     *
     */
    void CreateVolumeWizard();
    /*!
     * \fn void Previous_Choice()
     * \brief Previous Button actions
     *
     */
    void File_or_Partiton();
    /*!
     * \fn void Next_Choice()
     * \brief Next Button actions
     *
     */
    void Next_Choice();
    /*!
     * \fn void File_or_Partiton()
     * \brief First step of creation : The choice between a file container or a partition volume
     *
     */
    void Previous_Choice();
    /*!
     * \fn void Help_url()
     * \brief Connection to the wikipage of Gostcrypt
     *
     */
    void Help_url();

    void System_Type();

    void File_Location();

    void Device_Location();

    void Algos_Choices();

    void Volume_Size();

    void Volume_Password();

    void select_file();

    void Format_Options();

    void Volume_Format();

    void End();

};


QT_END_NAMESPACE
#endif // UI_CREATIONWIZARD_H
