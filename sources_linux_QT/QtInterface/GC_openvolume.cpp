#include <QDebug>
#include "GC_openvolume.h"

GC_OpenVolume::GC_OpenVolume(QWidget* parent, const QString& style)
    : QDialog(parent), mGC_styleSheet(style)
{
    setFixedSize(600,140);
    /* Embedded font */
    QFontDatabase::addApplicationFont(":/ressources/gs_font.ttf");
    mGC_font = QFont("Caviar Dreams", 10, 1);

    /*! GroupBox volume (load file) */
    mGC_Volume = new QGroupBox(this);
    mGC_Volume->setFont(mGC_font);
    mGC_Volume->setObjectName(QString::fromUtf8("group-noborder"));
    //mGC_Volume->setTitle(QString(tr("Ouvrir un volume")));
    mGC_Volume->setGeometry(QRect(0,0,600,140));


    /*! GroupBox volume layout */
    mGC_grid = new QGridLayout(mGC_Volume);

    /*! lock image */
    mGC_lock = new QLabel(mGC_Volume);
    mGC_lock->setObjectName(QString::fromUtf8("lock"));
    mGC_lock->setPixmap(QPixmap(
                           QString::fromUtf8(":/ressources/lock.gif")
                          ));
    mGC_lock->setAlignment(Qt::AlignHCenter);

    /*!< Volume selection Combobox in mGC_Volume QGroupbox*/
    mGC_volumePath = new QComboBox(mGC_Volume);
    mGC_volumePath->setGeometry(QRect(0, 0, 321, 31));
    QSizePolicy* expandPolicy = new QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    mGC_volumePath->setSizePolicy(*expandPolicy);
    mGC_volumePath->setObjectName(QString::fromUtf8("combo"));

    /*!< Buttons related to Volume*/
    mGC_selectFile = new GC_button(mGC_Volume, tr("Ouvrir...").toUtf8(), &mGC_styleSheet, eGC_fullGreen);
    mGC_selectFile->setFont(mGC_font);

    mGC_volumeTools = new GC_button(mGC_Volume, tr("Outils du volume").toUtf8(), &mGC_styleSheet, eGC_fullGreen);
    mGC_volumeTools->setFont(mGC_font);

    mGC_selectDevice = new GC_button(mGC_Volume, tr("Périphériques...").toUtf8(), &mGC_styleSheet, eGC_fullGreen);
    mGC_selectDevice->setFont(mGC_font);

    mGC_saveHistory = new QCheckBox(mGC_lock);
    mGC_saveHistory->setChecked(true);

    mGC_textHistory = new QLabel(mGC_Volume);
    mGC_textHistory->setText(tr("Ne jamais sauvegarder l'historique"));
    mGC_textHistory->setFont(mGC_font);
    mGC_textHistory->setObjectName("title");

    /*********** OK / Cancel ***********/
    mGC_ok  = new GC_button(mGC_Volume, tr("Ouvrir le volume").toUtf8(), &mGC_styleSheet, eGC_verif);
    mGC_ok->setFont(mGC_font);
    mGC_ok->setDisabled(true);

    mGC_cancel = new GC_button(mGC_Volume, tr("Fermer").toUtf8(), &mGC_styleSheet, eGC_verif);
    mGC_cancel->setFont(mGC_font);

    /*********** add widgets ***********/

    mGC_grid->addWidget(mGC_lock, 0,0,2,2);
    mGC_grid->addWidget(mGC_volumePath,0,2,1,9);
    mGC_grid->addWidget(mGC_selectFile,0,11,1,2);
    mGC_grid->addWidget(mGC_saveHistory,1,2,1,1);
    mGC_grid->addWidget(mGC_textHistory,1,3,1,3);
    mGC_grid->addWidget(mGC_volumeTools,1,8,1,2);
    mGC_grid->addWidget(mGC_selectDevice,1,10,1,3);
    mGC_grid->addWidget(mGC_ok,7,4,2,2);
    mGC_grid->addWidget(mGC_cancel,7,6,2,3);

    setWindowTitle(tr("Ouvrir un volume"));

    /*********** password part **********/
    mGC_passwordTxt = new QLabel(mGC_Volume);
    mGC_passwordTxt->setText(tr("Mot de passe:"));
    mGC_passwordTxt->hide();
    mGC_grid->addWidget(mGC_passwordTxt,2,1,1,2);
    mGC_passwordTxt->setFont(mGC_font);
    mGC_passwordTxt->setObjectName("password");

    mGC_password = new QLineEdit(mGC_Volume);
    mGC_password->setEchoMode(QLineEdit::Password);
    mGC_password->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);
    mGC_password->hide();
    mGC_grid->addWidget(mGC_password,2,3,1,9);
    mGC_password->setObjectName("password");

    mGC_cachePwd = new QCheckBox(tr("Mettre le mot de passe/keyfile en mémoire").toUtf8(), mGC_Volume);
    mGC_cachePwd->hide();
    mGC_grid->addWidget(mGC_cachePwd,3,3,1,9);
    mGC_cachePwd->setFont(mGC_font);
    mGC_cachePwd->setObjectName("password");

    mGC_displayPwd = new QCheckBox(tr("Afficher le mot de passe").toUtf8(), mGC_Volume);
    mGC_displayPwd->hide();
    mGC_grid->addWidget(mGC_displayPwd,4,3,1,6);
    mGC_displayPwd->setFont(mGC_font);
    mGC_displayPwd->setObjectName("password");

    mGC_useKeyfile = new QCheckBox(tr("Utiliser les Keyfiles").toUtf8(), mGC_Volume);
    mGC_useKeyfile->hide();
    mGC_grid->addWidget(mGC_useKeyfile,5,3,1,6);
    mGC_useKeyfile->setFont(mGC_font);
    mGC_useKeyfile->setObjectName("password");

    mGC_keyFiles = new GC_button(mGC_Volume, tr("Keyfiles...").toUtf8(), &mGC_styleSheet, eGC_fullGreen);
    mGC_keyFiles->setFont(mGC_font);
    mGC_grid->addWidget(mGC_keyFiles,4,9,1,3);
    mGC_keyFiles->hide();

    mGC_mountOption = new GC_button(mGC_Volume, tr("Options de montage...").toUtf8(), &mGC_styleSheet, eGC_fullGreen);
    mGC_mountOption->setFont(mGC_font);
    mGC_grid->addWidget(mGC_mountOption,5,9,1,3);
    mGC_mountOption->hide();

    /*********** connect widgets ********/
    connect(mGC_selectFile, SIGNAL(Clicked()), this, SLOT(open()));
    connect(mGC_cancel, SIGNAL(Clicked()), this, SLOT(quit()));
}

GC_OpenVolume::~GC_OpenVolume()
{
    delete mGC_lock;
    delete mGC_volumePath;
    delete mGC_selectFile;
    delete mGC_volumeTools;
    delete mGC_selectDevice;
    delete mGC_saveHistory;
    delete mGC_textHistory;
    delete mGC_ok;
    delete mGC_cancel;
    delete mGC_passwordTxt;
    delete mGC_password;
    delete mGC_cachePwd;
    delete mGC_displayPwd;
    delete mGC_useKeyfile;
    delete mGC_keyFiles;
    delete mGC_mountOption;

    delete mGC_grid;
    delete mGC_Volume;
}

void GC_OpenVolume::open()
{
    QString directory =
           QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Find Files"), QDir::currentPath()));

    if(directory != "")
    {
         mGC_volumePath->addItem(directory);
         if(!mGC_saveHistory->isChecked())
         {
             //! TODO : save path
         }
         setFixedSize(600,260);
         mGC_Volume->setGeometry(QRect(0,0,600,260));
         mGC_passwordTxt->show();
         mGC_password->show();
         mGC_cachePwd->show();
         mGC_displayPwd->show();
         mGC_useKeyfile->show();
         mGC_keyFiles->show();
         mGC_mountOption->show();
    }

    qDebug() << directory;
}

void GC_OpenVolume::quit()
{
    close();
}
