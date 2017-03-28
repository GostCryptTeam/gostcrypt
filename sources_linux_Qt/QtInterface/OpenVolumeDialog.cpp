#include <QDebug>
#include "OpenVolumeDialog.h"

OpenVolumeDialog::OpenVolumeDialog(GraphicUserInterface * aSignal,
                                   QWidget* parent,
                                   const QString& style)
    : QDialog(parent), mStyleSheet(style)
{
    setFixedSize(600,140);
    /* Embedded font */
    QFontDatabase::addApplicationFont(":/ressources/gs_font.ttf");
    mFont = QFont("Caviar Dreams", 10, 1);

    mSlots = aSignal;

    /*! GroupBox volume (load file) */
    mVolume = new QGroupBox(this);
    mVolume->setFont(mFont);
    mVolume->setObjectName(QString::fromUtf8("group-noborder"));
    //mVolume->setTitle(QString(tr("Ouvrir un volume")));
    mVolume->setGeometry(QRect(0,0,600,140));

    /*! GroupBox volume layout */
    mGrid = new QGridLayout(mVolume);

    /*! lock image */
    mLock = new QLabel(mVolume);
    mLock->setObjectName(QString::fromUtf8("lock"));
    mLock->setPixmap(QPixmap(
                           QString::fromUtf8(":/ressources/lock.gif")
                          ));
    mLock->setAlignment(Qt::AlignHCenter);

    /*!< Volume selection Combobox in mVolume QGroupbox*/
    mVolumePath = new QComboBox(mVolume);
    mVolumePath->setGeometry(QRect(0, 0, 321, 31));
    QSizePolicy* expandPolicy = new QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    mVolumePath->setSizePolicy(*expandPolicy);
    mVolumePath->setObjectName(QString::fromUtf8("combo"));

    /*!< Buttons related to Volume*/
    mSelectFile = new Button(mVolume, tr("Ouvrir...").toUtf8(), &mStyleSheet, eFullGreen);
    mSelectFile->setFont(mFont);

    mVolumeTools = new Button(mVolume, tr("Outils du volume").toUtf8(), &mStyleSheet, eFullGreen);
    mVolumeTools->setFont(mFont);

    mSelectDevice = new Button(mVolume, tr("Périphériques...").toUtf8(), &mStyleSheet, eFullGreen);
    mSelectDevice->setFont(mFont);

    mSaveHistory = new QCheckBox(mLock);
    mSaveHistory->setChecked(true);

    mTextHistory = new QLabel(mVolume);
    mTextHistory->setText(tr("Ne jamais sauvegarder l'historique"));
    mTextHistory->setFont(mFont);
    mTextHistory->setObjectName("title");

    /*********** OK / Cancel ***********/
    mOk  = new Button(mVolume, tr("Ouvrir le volume").toUtf8(), &mStyleSheet, eVerif);
    mOk->setFont(mFont);
    mOk->setDisabled(true);

    mCancel = new Button(mVolume, tr("Fermer").toUtf8(), &mStyleSheet, eVerif);
    mCancel->setFont(mFont);

    /*********** add widgets ***********/

    mGrid->addWidget(mLock, 0,0,2,2);
    mGrid->addWidget(mVolumePath,0,2,1,9);
    mGrid->addWidget(mSelectFile,0,11,1,2);
    mGrid->addWidget(mSaveHistory,1,2,1,1);
    mGrid->addWidget(mTextHistory,1,3,1,3);
    mGrid->addWidget(mVolumeTools,1,8,1,2);
    mGrid->addWidget(mSelectDevice,1,10,1,3);
    mGrid->addWidget(mOk,7,4,2,2);
    mGrid->addWidget(mCancel,7,6,2,3);

    setWindowTitle(tr("Ouvrir un volume"));

    /*********** password part **********/
    mPasswordTxt = new QLabel(mVolume);
    mPasswordTxt->setText(tr("Mot de passe:"));
    mPasswordTxt->hide();
    mGrid->addWidget(mPasswordTxt,2,1,1,2);
    mPasswordTxt->setFont(mFont);
    mPasswordTxt->setObjectName("password");

    mPassword = new QLineEdit(mVolume);
    mPassword->setEchoMode(QLineEdit::Password);
    mPassword->setInputMethodHints(Qt::ImhHiddenText| Qt::ImhNoPredictiveText|Qt::ImhNoAutoUppercase);
    mPassword->hide();
    mGrid->addWidget(mPassword,2,3,1,9);
    mPassword->setObjectName("password");

    mCachePwd = new QCheckBox(tr("Mettre le mot de passe/keyfile en mémoire").toUtf8(), mVolume);
    mCachePwd->hide();
    mGrid->addWidget(mCachePwd,3,3,1,9);
    mCachePwd->setFont(mFont);
    mCachePwd->setObjectName("password");

    mDisplayPwd = new QCheckBox(tr("Afficher le mot de passe").toUtf8(), mVolume);
    mDisplayPwd->hide();
    mGrid->addWidget(mDisplayPwd,4,3,1,6);
    mDisplayPwd->setFont(mFont);
    mDisplayPwd->setObjectName("password");

    mUseKeyfile = new QCheckBox(tr("Utiliser les Keyfiles").toUtf8(), mVolume);
    mUseKeyfile->hide();
    mGrid->addWidget(mUseKeyfile,5,3,1,6);
    mUseKeyfile->setFont(mFont);
    mUseKeyfile->setObjectName("password");

    mKeyFiles = new Button(mVolume, tr("Keyfiles...").toUtf8(), &mStyleSheet, eFullGreen);
    mKeyFiles->setFont(mFont);
    mGrid->addWidget(mKeyFiles,4,9,1,3);
    mKeyFiles->hide();

    mMountOption = new Button(mVolume, tr("Options de montage...").toUtf8(), &mStyleSheet, eFullGreen);
    mMountOption->setFont(mFont);
    mGrid->addWidget(mMountOption,5,9,1,3);
    mMountOption->hide();

    /*********** connect widgets ********/
    connect(mSelectFile, SIGNAL(Clicked()), this, SLOT(open()));
    connect(mCancel, SIGNAL(Clicked()), this, SLOT(quit()));
    //! Enable the Open volume button on password
    connect(mPassword, SIGNAL(textEdited(QString)), this, SLOT(enableButton()));
    //! Emit volume infos
    connect(mOk, SIGNAL(Clicked()), this, SLOT(createVolume()));
    connect(this, SIGNAL(openVolume(QString, QString)), mSlots, SLOT(receiveMount(QString, QString)));
}

OpenVolumeDialog::~OpenVolumeDialog()
{
    delete mLock;
    delete mVolumePath;
    delete mSelectFile;
    delete mVolumeTools;
    delete mSelectDevice;
    delete mSaveHistory;
    delete mTextHistory;
    delete mOk;
    delete mCancel;
    delete mPasswordTxt;
    delete mPassword;
    delete mCachePwd;
    delete mDisplayPwd;
    delete mUseKeyfile;
    delete mKeyFiles;
    delete mMountOption;

    delete mGrid;
    delete mVolume;
}

void OpenVolumeDialog::open()
{
    mPath = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Find Files"),
                                                                  QDir::currentPath()));

    if(mPath != "")
    {
         mVolumePath->addItem(mPath);
         if(!mSaveHistory->isChecked())
         {
             //! TODO : save path
         }
         setFixedSize(600,260);
         mVolume->setGeometry(QRect(0,0,600,260));
         mPasswordTxt->show();
         mPassword->show();
         mCachePwd->show();
         mDisplayPwd->show();
         mUseKeyfile->show();
         mKeyFiles->show();
         mMountOption->show();
        // mOk->setDisabled(false);
    }

    qDebug() << mPath;
}

void OpenVolumeDialog::enableButton()
{
    if(mPassword->text() != "")
        mOk->setDisabled(false);
    else
        mOk->setDisabled(true);
}

void OpenVolumeDialog::createVolume()
{
    emit openVolume(mPath, mPassword->text());
    close();
}

void OpenVolumeDialog::quit()
{
    close();
}
