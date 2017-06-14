#include "Location.h"


void Ui_Creation::File_Location()
{
    /*!< QGroupbox of file location*/
    LocationFile->setGeometry(QRect(400, 0, 415, 400));
    LocationFile->setTitle(QString::fromUtf8(GetValueFromKey(xml,std::string("FILE_TITLE")).c_str()));

    /*!< QCheckbox history*/
    SaveHistory->setGeometry(QRect(50, 70, 180, 25));
    SaveHistory->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDC_NO_HISTORY")).c_str()));
    SaveHistory->setChecked(true);
    /*!< QButton to Select a file in the system*/
    SelectFile->setGeometry(QRect(250, 40, 120, 31));
    SelectFile->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("IDM_SELECT_FILE")).c_str()));
    QObject::connect(SelectFile, SIGNAL(clicked()), this , SLOT(select_file()));
    /*!< QCombobox to Select a file in the system*/
    SelectVolume->setGeometry(QRect(20, 40, 220, 31));
    SelectVolume->clear();
    /*!< Description of file location*/
    LocationDescription->setGeometry(QRect(10, 120, 390, 250));
    LocationDescription->setText(QString::fromUtf8(GetValueFromKey(xml,std::string("FILE_HELP")).c_str()));
    LocationDescription->setWordWrap(true);
}

void Ui_Creation::Device_Location()
{
     /*!< Enable the right QGroupbox and disable the others*/
    LocationDevice->setGeometry(QRect(400, 0, 400, 360));
    LocationDevice->setTitle(QString::fromUtf8(GetValueFromKey(xml,string("IDC_SELECT_DEVICE")).c_str()));
    treeView->setGeometry(QRect(20, 50, 360, 300));

    QStringList labels;
    labels <<tr("")<<tr("Name")<<tr("MountPoint")<<tr("Size");
    treeView->setHeaderLabels(labels);
    treeView->header()->resizeSection(0, 00);
    treeView->header()->resizeSection(1, 70);
    treeView->header()->resizeSection(2, 200);
    treeView->header()->resizeSection(3, 50);
    treeView->clear();
    Devices_Info info;
    info = Getinfodevices();

    for(int i = 0; i<info.nbparts;i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(treeView);

        item->setText(1,QString::fromUtf8(info.part[i].name));
        item->setText(2,QString::fromUtf8(info.part[i].mountpoint));
        if(info.part[i].size >= 1024 && info.part[i].size < 1048576)
            item->setText(3,QString(QString::number(info.part[i].size/1024,'f',2) + "  " + QString::fromUtf8(GetValueFromKey(xml,std::string("KB")).c_str())));
        if(info.part[i].size >= 1048576 && info.part[i].size < 1073741824)
            item->setText(3,QString(QString::number(info.part[i].size/1048576,'f',2) + "  " + QString::fromUtf8(GetValueFromKey(xml,std::string("MB")).c_str())));
        if(info.part[i].size >= 1073741824 && info.part[i].size < 1099511627776)
            item->setText(3,QString(QString::number(info.part[i].size/1073741824,'f',2) + "  " + QString::fromUtf8(GetValueFromKey(xml,std::string("GB")).c_str())));
        if(info.part[i].size >= 1099511627776)
            item->setText(3,QString(QString::number(info.part[i].size/1099511627776,'f',2) + "  " + QString::fromUtf8(GetValueFromKey(xml,std::string("TB")).c_str())));
        QIcon icon;
        icon.addFile(QString::fromUtf8("Ressources/device.png"), QSize(), QIcon::Normal, QIcon::Off);
        item->setIcon(1,icon);
    }
    return;
}

void Ui_Creation::select_file()
{
    QString filter = QString::fromUtf8("GostCrypt Volume (*.*)");
    SelectVolume->addItem(QFileDialog::getOpenFileName(0, QString::fromUtf8(GetValueFromKey(xml,string("IDC_SELECT_FILE")).c_str()), QDir::homePath(), filter));

}
