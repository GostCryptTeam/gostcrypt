/*!
 * \file Window.cpp
 * \brief Main cpp file for the GostCrypt's UI
 * \author Antoine Hébert, Louis Béclair, William Lardier
 * \version 0.1
 * \date 14/07/2017
 *
 * GUI for GostCrypt 2.0
 *
 */

#include "Window.h"

Window::Window(/*QWidget *parent*/) :
    mSidebar(this),
    mTitle(this),
	mGridCpt(0)
{
    /*********** Size & style ***********/
    QIcon favicon;
    /* Embedded font */
    QFontDatabase::addApplicationFont(":/ressources/gs_font.ttf");
    mFont = QFont("Caviar Dreams", 10, 1);

    /*! Set window dimensions according to the resolution */
    QRect dim;
    dim.setHeight(530);
    dim.setWidth(dim.height() * UI_RATIO);
    this->setMinimumWidth(dim.width());
    this->setMinimumHeight(dim.height());
    mWidth = dim.width();
    mHeight = dim.height();
	//setFixedSize(QSize(dim.width(), dim.height()));

    /*!< Load QSS stylesheet file*/
    QFile file(":/ressources/style.qss");
    file.open(QFile::ReadOnly);
    mStyleSheet = QLatin1String(file.readAll());
    setStyleSheet(mStyleSheet);

    /*! Change background color */
    this->setObjectName( "window" );

    /*! Disable maximize button */
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint/* | Qt::MSWindowsFixedSizeDialogHint*/);
    /*! Load and set the appication favicon */
    favicon.addFile(QString::fromUtf8(":/ressources/favicon.gif"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(favicon);

    /*********** Widgets ***********/
    /*! sidebar with white background */
    mSidebar.setObjectName("sidebar");
    mSidebar.setGeometry(0,0,170,dim.height());

    /*! Load and draw the application logo*/
    mLogo = new QLabel(this);
    mLogo->setObjectName(QString::fromUtf8("logo"));
    mLogo->setGeometry(QRect(15, 15, 141, 48));
    mLogo->setPixmap(QPixmap(
                           QString::fromUtf8(":/ressources/logo_gostcrypt.gif")
                          ));

    /*! New Volume button */
    mNewVolume = new Button(this, tr("creer un volume").toUtf8(), &mStyleSheet, eLeftGreen);
    mNewVolume->setGeometry(mWidth/3.5, 35, mWidth-(mWidth/3.5+10), 7);
    mNewVolume->setFont(mFont);

    /*! title */
    QString title = QString(tr("GostCrypt Volume Creation Wizard :"));
    mTitle.setText(title);
    mTitle.setObjectName("title");
    mTitle.setFont(mFont);
    QFontMetrics fm(mFont);
    mTitle.setGeometry(200,18,fm.width(title),fm.height());

    /*! gray line separator */
    mSeparator = new QFrame(this);
    mSeparator->setObjectName(QString::fromUtf8("line"));
    mSeparator->setGeometry(QRect(180, 50, this->width()-190, 1));
    mSeparator->setFrameShape(QFrame::HLine);
    mSeparator->setFrameShadow(QFrame::Sunken);

    /********* Volume Property ********/
    mVolumeProperty = new QGroupBox(this);
    mVolumeProperty->setFont(mFont);
    mVolumeProperty->setObjectName(QString::fromUtf8("group"));
    mVolumeProperty->setProperty("white", true);
    mVolumeProperty->setTitle(QString(tr("Propriété des volumes")));
    mVolumeProperty->setGeometry(QRect(180,217,this->width()-190, this->height()*0.5));

	mVolumeWidget = new QWidget;
	
	mVolumeLayout = new QGridLayout(mVolumeWidget);
	mVolumeWidget->setLayout(mVolumeLayout);

	mVolumeScroll = new QScrollArea(this);
	mVolumeScroll->setWidgetResizable(true);
	mVolumeScroll->setObjectName(QString::fromUtf8("volumes"));
	mVolumeScroll->setWidget(mVolumeWidget);
	mVolumeScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	mVolumeScroll->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	mVolumeScroll->setStyleSheet("background-color:transparent;");
	
	//add widgets
	mVolumeScroll->setGeometry(QRect(185, 240, this->width() - 200, this->height()*0.5-25));
    CreateVolumeWidget("E", "C:/Program Files (x86)/MesVolumes/volume.*", VolumeType::eNormal, VolumeAlgorithm::eGost, 5);
    CreateVolumeWidget("F", "C:/Program Files (x86)/MesVolumes/volume.*", VolumeType::eOther, VolumeAlgorithm::eGost, 541665);
    CreateVolumeWidget("G", "C:/Program Files (x86)/MesVolumes/volume.*", VolumeType::eOther, VolumeAlgorithm::eGrasshopper, 545455402);
    CreateVolumeWidget("H", "C:/Program Files (x86)/MesVolumes/volume.*", VolumeType::eNormal, VolumeAlgorithm::eGost, 15478955035);
    CreateVolumeWidget("I", "C:/Program Files (x86)/MesVolumes/volume.*", VolumeType::eNormal, VolumeAlgorithm::eGost, 5);
    CreateVolumeWidget("J", "C:/Program Files (x86)/MesVolumes/volume.*", VolumeType::eNormal, VolumeAlgorithm::eGost, 5);
    CreateVolumeWidget("K", "C:/Program Files (x86)/MesVolumes/volume.*", VolumeType::eNormal, VolumeAlgorithm::eGost, 5);
    CreateVolumeWidget("L", "C:/Program Files (x86)/MesVolumes/volume.*", VolumeType::eNormal, VolumeAlgorithm::eGost, 5);
    CreateVolumeWidget("M", "C:/Program Files (x86)/MesVolumes/volume.*", VolumeType::eNormal, VolumeAlgorithm::eGost, 5);
    CreateVolumeWidget("O", "C:/Program Files (x86)/MesVolumes/volume.*", VolumeType::eNormal, VolumeAlgorithm::eGost, 5);
    CreateVolumeWidget("P", "C:/Program Files (x86)/MesVolumes/volume.*", VolumeType::eNormal, VolumeAlgorithm::eGost, 5);
    CreateVolumeWidget("Q", "C:/Program Files (x86)/MesVolumes/volume.*", VolumeType::eNormal, VolumeAlgorithm::eGost, 5);

	//! Buttons
    mMount = new Button(this, tr("Monter le volume"), &mStyleSheet, eFullGreen);
    mMount->setFont(mFont);

    mAutoMount = new Button(this, tr("Monter Automatiquement"), &mStyleSheet, eFullGreen);
    mAutoMount->setFont(mFont);

    mDismountAll = new Button(this, tr("Démonter tous les volumes"), &mStyleSheet, eFullGreen);
    mDismountAll->setFont(mFont);

    mOpenVolume = new Button(this, tr("Ouvrir un volume"), &mStyleSheet, eFullGreen);
    mOpenVolume->setFont(mFont);

	//! Containers for buttons
	mButtonVolume = new QGroupBox(this);
	mButtonVolume->setGeometry(QRect(180, 170, this->width() - 190, 40));
	mButtonVolume->setStyleSheet("border:0px");

    mButtonGrid = new QGridLayout(mButtonVolume);
    mButtonGrid->addWidget(mMount, 0, 0);
    mButtonGrid->addWidget(mOpenVolume, 0, 1);

	mButtonMount = new QGroupBox(this);
	mButtonMount->setGeometry(QRect(180, this->height()-50, this->width() - 190, 40));
	mButtonMount->setStyleSheet("border:0px");

	mButtonMountGrid = new QGridLayout(mButtonMount);
    mButtonMountGrid->addWidget(mAutoMount, 0, 0);
    //mButtonMountGrid->addWidget(mDismount, 0, 1);
    mButtonMountGrid->addWidget(mDismountAll, 0, 2);
	
    ConnectSignals();
}

Window::~Window()
{

	delete mLogo;
    delete mSeparator;
}

void Window::ConnectSignals()
{
    QObject::connect(mMount, SIGNAL(Clicked()), &mSlots, SLOT(receiveMount()));
    QObject::connect(mAutoMount, SIGNAL(Clicked()), &mSlots, SLOT(receiveAutoMount()));
    QObject::connect(mDismountAll, SIGNAL(Clicked()), &mSlots, SLOT(receiveDismountAll()));
    QObject::connect(mOpenVolume, SIGNAL(Clicked()), this, SLOT(OpenVolume()));
}

void Window::ResizeEvent(QResizeEvent* event) {
#ifdef QT_DEBUG
    qDebug() << event;
#endif
    mWidth = this->width();
    mHeight = this->height();
    mSidebar.setGeometry(0,0,170,this->height());
    mNewVolume->setGeometry(mWidth/3.5, 35, mWidth-(mWidth/3.5+10), 7);
    mSeparator->setGeometry(QRect(180, 50, this->width()-190, 1));
    mVolumeProperty->setGeometry(QRect(180,180,this->width()-190,this->height()*0.5));
	mVolumeScroll->setGeometry(QRect(185, 200, this->width() - 200, this->height()*0.5 - 25));
}

void Window::CreateVolumeWidget(
		const QString& volumeLetter,
		const QString& path,
		const VolumeType& type,
		const VolumeAlgorithm& algorithm,
		const unsigned long long& size
	)
{
	if (mVolumeItems[volumeLetter])
		delete mVolumeItems[volumeLetter];
	mVolumeItems[volumeLetter] = new VolumeList(volumeLetter, path, type, algorithm, size, WIDTH-241);
	mVolumeLayout->addWidget(mVolumeItems[volumeLetter],mGridCpt/2, mGridCpt%2+1);
#ifdef QT_DEBUG
	qDebug() << mGridCpt / 2 << "," << mGridCpt % 2 + 1 << volumeLetter;
#endif
    QObject::connect(mVolumeItems[volumeLetter], SIGNAL(Clicked(QString)), &mSlots, SLOT(receive(QString)));
	mGridCpt++;
}

void Window::DebugSlot(QString path)
{
#ifdef QT_DEBUG
	qDebug() << path;
#endif
}

void Window::OpenVolume()
{
    mOpenVolumeDialogWindow = new OpenVolumeDialog(this, mStyleSheet);
    mOpenVolumeDialogWindow->setAttribute(Qt::WA_DeleteOnClose);
    mOpenVolumeDialogWindow->setModal(true);
    mOpenVolumeDialogWindow->show();
}
