#include "Container_Type.h"


void Ui_Creation::File_or_Partiton()
{
    Previous->setDisabled(true);
    Previous->setVisible(false);
    RadioCreate1->setChecked(true);

    /*!< QGroupbox file or patition choice parameters*/
    File_Partition_Choice->setGeometry(QRect(400, 0, 405, 360));
    File_Partition_Choice->setTitle(QString::fromUtf8(GetValueFromKey(xml,std::string("IDD_VOL_CREATION_WIZARD_DLG")).c_str()));

    /*!< QRadioButton for the choice*/
    RadioCreate1->setGeometry(QRect(5, 80, 350, 25));
    RadioCreate1->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDC_FILE_CONTAINER")).c_str()));
    RadioCreate2->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDC_DEVICE_TRANSFORM_MODE_FORMAT")).c_str()));
    RadioCreate2->setGeometry(QRect(5, 250, 350, 25));

    /*!< addQRadioButton in Creation_Select to choose only one option*/
    Creation_Select->addButton(RadioCreate1);
    Creation_Select->addButton(RadioCreate2);

    /*!< Description of the choices*/
    Description1->setGeometry(QRect(25, 100, 360, 60));
    Description1->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDT_FILE_CONTAINER")).c_str()));
    Description1->setWordWrap(true);
    Description2->setGeometry(QRect(25, 285, 360, 60));
    Description2->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDT_NON_SYS_DEVICE")).c_str()));
    Description2->setWordWrap(true);
}

/*!
 * \fn void Ui_Creation::System_Type()
 * \brief First step of creation : The choice between a standard container or an hidden container
 *
 */
void Ui_Creation::System_Type()
{
    RadioCreatestandard->setChecked(true);

    /*!< QGroupbox standard or hidden choice parameters*/
    Standard_or_Hidden->setGeometry(QRect(400, 0, 405, 500));
    Standard_or_Hidden->setTitle(QString::fromUtf8(GetValueFromKey(xml,std::string("VOLUME_TYPE_TITLE")).c_str()));

    /*!< Redfinition of QRadioButtons for the choice*/
    RadioCreatestandard->setGeometry(QRect(5, 80, 350, 25));
    RadioCreatestandard->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDC_STD_VOL")).c_str()));
    RadioCreatehidden->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("FORMAT_HIDVOL_TITLE")).c_str()));
    RadioCreatehidden->setGeometry(QRect(5, 250, 350, 25));
    /*!< Description of the choices*/
    Descriptionstandard->setGeometry(QRect(25, 120, 370, 45));
    Descriptionstandard->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("NORMAL_VOLUME_TYPE_HELP")).c_str()));
    Descriptionstandard->setWordWrap(true);
    Descriptionhidden->setGeometry(QRect(25, 290, 370, 130));
    Descriptionhidden->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("HIDDEN_VOLUME_TYPE_HELP")).c_str()));
    Descriptionhidden->setWordWrap(true);

    standard_or_Hidden_Select->addButton(RadioCreatestandard);
    standard_or_Hidden_Select->addButton(RadioCreatehidden);
}
