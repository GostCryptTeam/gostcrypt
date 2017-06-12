#include "Size.h"


void Ui_Creation::Volume_Size()
{
    /*!< QGroupbox for Volume Size*/
    VolumeSize->setGeometry(QRect(400, 0, 400, 360));
    VolumeSize->setTitle(QString::fromUtf8(GetValueFromKey(xml,std::string("SIZE_TITLE")).c_str()));

    /*!< Text edit to write the size*/
    Size->setGeometry(QRect(20, 50, 220, 30));

    /*!< QRadioButton for the choice of size (kB, MB, GB)*/
    RadioSize1->setGeometry(QRect(250, 50, 50, 25));
    RadioSize1->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("KB")).c_str()));
    RadioSize2->setGeometry(QRect(300, 50, 50, 25));
    RadioSize2->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("MB")).c_str()));
    RadioSize2->setChecked(true);
    RadioSize3->setGeometry(QRect(350, 50, 45, 25));
    RadioSize3->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("GB")).c_str()));

    /*!< addQRadioButton in Creation_Select to choose only one option*/
    Size_Select->addButton(RadioSize1);
    Size_Select->addButton(RadioSize2);
    Size_Select->addButton(RadioSize3);

    SizeDescription->setGeometry(QRect(20, 80, 350, 250));
    SizeDescription->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("SIZE_HELP")).c_str()));
    SizeDescription->setWordWrap(true);
}
