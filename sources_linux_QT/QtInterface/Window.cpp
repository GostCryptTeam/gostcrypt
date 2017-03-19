/*!
 * \file mGC_window.cpp
 * \brief Main cpp file for the GostCrypt's UI
 * \author Antoine Hébert, Louis Béclair, William Lardier
 * \version 0.1
 * \date 14/07/2017
 *
 * GUI for GostCrypt 2.0
 *
 */

#include "GC_window.h"

GC_window::GC_window(/*QWidget *parent*/) :
    mGC_sidebar(this),
    mGC_title(this),
	mGC_gridCpt(0)
{
    /*********** Size & style ***********/
    QIcon favicon;
    /* Embedded font */
    QFontDatabase::addApplicationFont(":/ressources/gs_font.ttf");
    mGC_font = QFont("Caviar Dreams", 10, 1);

    /*! Set window dimensions according to the resolution */
    QRect dim;
    dim.setHeight(530);
    dim.setWidth(dim.height() * UI_RATIO);
    this->setMinimumWidth(dim.width());
    this->setMinimumHeight(dim.height());
    mGC_width = dim.width();
    mGC_height = dim.height();
	//setFixedSize(QSize(dim.width(), dim.height()));

    /*!< Load QSS stylesheet file*/
    QFile file(":/ressources/GC_style.qss");
    file.open(QFile::ReadOnly);
    mGC_styleSheet = QLatin1String(file.readAll());
    setStyleSheet(mGC_styleSheet);

    /*! Change background color */
    this->setObjectName( "window" );

    /*! Disable maximize button */
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint/* | Qt::MSWindowsFixedSizeDialogHint*/);
    /*! Load and set the appication favicon */
    favicon.addFile(QString::fromUtf8(":/ressources/favicon.gif"), QSize(), QIcon::Normal, QIcon::Off);
    this->setWindowIcon(favicon);

    /*********** Widgets ***********/
    /*! sidebar with white background */
    mGC_sidebar.setObjectName("sidebar");
    mGC_sidebar.setGeometry(0,0,170,dim.height());

    /*! Load and draw the application logo*/
    mGC_logo = new QLabel(this);
    mGC_logo->setObjectName(QString::fromUtf8("logo"));
    mGC_logo->setGeometry(QRect(15, 15, 141, 48));
    mGC_logo->setPixmap(QPixmap(
                           QString::fromUtf8(":/ressources/logo_gostcrypt.gif")
                          ));

    /*! New Volume button */
    mGC_newVolume = new GC_button(this, tr("creer un volume").toUtf8(), &mGC_styleSheet, eGC_leftGreen);
    mGC_newVolume->setGeometry(mGC_width/3.5, 35, mGC_width-(mGC_width/3.5+10), 7);
    mGC_newVolume->setFont(mGC_font);

    /*! title */
    QString title = QString(tr("GostCrypt Volume Creation Wizard :"));
    mGC_title.setText(title);
    mGC_title.setObjectName("title");
    mGC_title.setFont(mGC_font);
    QFontMetrics fm(mGC_font);
    mGC_title.setGeometry(200,18,fm.width(title),fm.height());

    /*! gray line separator */
    mGC_separator = new QFrame(this);
    mGC_separator->setObjectName(QString::fromUtf8("line"));
    mGC_separator->setGeometry(QRect(180, 50, this->width()-190, 1));
    mGC_separator->setFrameShape(QFrame::HLine);
    mGC_separator->setFrameShadow(QFrame::Sunken);

    /********* Volume Property ********/
    mGC_VolumeProperty = new QGroupBox(this);
    mGC_VolumeProperty->setFont(mGC_font);
    mGC_VolumeProperty->setObjectName(QString::fromUtf8("group"));
    mGC_VolumeProperty->setProperty("white", true);
    mGC_VolumeProperty->setTitle(QString(tr("Propriété des volumes")));
    mGC_VolumeProperty->setGeometry(QRect(180,217,this->width()-190, this->height()*0.5));

	mGC_VolumeWidget = new QWidget;
	
	mGC_VolumeLayout = new QGridLayout(mGC_VolumeWidget);
	mGC_VolumeWidget->setLayout(mGC_VolumeLayout);

	mGC_VolumeScroll = new QScrollArea(this);
	mGC_VolumeScroll->setWidgetResizable(true);
	mGC_VolumeScroll->setObjectName(QString::fromUtf8("volumes"));
	mGC_VolumeScroll->setWidget(mGC_VolumeWidget);
	mGC_VolumeScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	mGC_VolumeScroll->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	mGC_VolumeScroll->setStyleSheet("background-color:transparent;");
	
	//add widgets
	mGC_VolumeScroll->setGeometry(QRect(185, 240, this->width() - 200, this->height()*0.5-25));
	CreateVolumeWidget("E", "C:/Program Files (x86)/MesVolumes/volume.*", eGC_NORMAL, eGC_GOST, 5);
	CreateVolumeWidget("F", "C:/Program Files (x86)/MesVolumes/volume.*", eGC_OTHER, eGC_GOST, 541665);
	CreateVolumeWidget("G", "C:/Program Files (x86)/MesVolumes/volume.*", eGC_OTHER, eGC_GHOPPER, 545455402);
	CreateVolumeWidget("H", "C:/Program Files (x86)/MesVolumes/volume.*", eGC_NORMAL, eGC_GOST, 15478955035);
	CreateVolumeWidget("I", "C:/Program Files (x86)/MesVolumes/volume.*", eGC_NORMAL, eGC_GOST, 5);
	CreateVolumeWidget("J", "C:/Program Files (x86)/MesVolumes/volume.*", eGC_NORMAL, eGC_GOST, 5);
	CreateVolumeWidget("K", "C:/Program Files (x86)/MesVolumes/volume.*", eGC_NORMAL, eGC_GOST, 5);
	CreateVolumeWidget("L", "C:/Program Files (x86)/MesVolumes/volume.*", eGC_NORMAL, eGC_GOST, 5);
	CreateVolumeWidget("M", "C:/Program Files (x86)/MesVolumes/volume.*", eGC_NORMAL, eGC_GOST, 5);
	CreateVolumeWidget("O", "C:/Program Files (x86)/MesVolumes/volume.*", eGC_NORMAL, eGC_GOST, 5);
	CreateVolumeWidget("P", "C:/Program Files (x86)/MesVolumes/volume.*", eGC_NORMAL, eGC_GOST, 5);
	CreateVolumeWidget("Q", "C:/Program Files (x86)/MesVolumes/volume.*", eGC_NORMAL, eGC_GOST, 5);

	//! Buttons
    mGC_Mount = new GC_button(this, tr("Monter le volume"), &mGC_styleSheet, eGC_fullGreen);
    mGC_Mount->setFont(mGC_font);

    mGC_AutoMount = new GC_button(this, tr("Monter Automatiquement"), &mGC_styleSheet, eGC_fullGreen);
    mGC_AutoMount->setFont(mGC_font);

    mGC_DismountAll = new GC_button(this, tr("Démonter tous les volumes"), &mGC_styleSheet, eGC_fullGreen);
    mGC_DismountAll->setFont(mGC_font);

    mGC_OpenVolume = new GC_button(this, tr("Ouvrir un volume"), &mGC_styleSheet, eGC_fullGreen);
    mGC_OpenVolume->setFont(mGC_font);

	//! Containers for buttons
	mGC_ButtonVolume = new QGroupBox(this);
	mGC_ButtonVolume->setGeometry(QRect(180, 170, this->width() - 190, 40));
	mGC_ButtonVolume->setStyleSheet("border:0px");

    mGC_ButtonGrid = new QGridLayout(mGC_ButtonVolume);
    mGC_ButtonGrid->addWidget(mGC_Mount, 0, 0);
    mGC_ButtonGrid->addWidget(mGC_OpenVolume, 0, 1);

	mGC_ButtonMount = new QGroupBox(this);
	mGC_ButtonMount->setGeometry(QRect(180, this->height()-50, this->width() - 190, 40));
	mGC_ButtonMount->setStyleSheet("border:0px");

	mGC_ButtonMountGrid = new QGridLayout(mGC_ButtonMount);
    mGC_ButtonMountGrid->addWidget(mGC_AutoMount, 0, 0);
    //mGC_ButtonMountGrid->addWidget(mDismount, 0, 1);
    mGC_ButtonMountGrid->addWidget(mGC_DismountAll, 0, 2);
	
    ConnectSignals();
}

GC_window::~GC_window()
{

	delete mGC_logo;
    delete mGC_separator;
}

void GC_window::ConnectSignals()
{
    QObject::connect(mGC_Mount, SIGNAL(Clicked()), &mGC_slots, SLOT(GC_receiveMount()));
    QObject::connect(mGC_AutoMount, SIGNAL(Clicked()), &mGC_slots, SLOT(GC_receiveAutoMount()));
    QObject::connect(mGC_DismountAll, SIGNAL(Clicked()), &mGC_slots, SLOT(GC_receiveDismountAll()));
    QObject::connect(mGC_OpenVolume, SIGNAL(Clicked()), this, SLOT(OpenVolume()));
}

void GC_window::ResizeEvent(QResizeEvent* event) {
#ifdef QT_DEBUG
    qDebug() << event;
#endif
    mGC_width = this->width();
    mGC_height = this->height();
    mGC_sidebar.setGeometry(0,0,170,this->height());
    mGC_newVolume->setGeometry(mGC_width/3.5, 35, mGC_width-(mGC_width/3.5+10), 7);
    mGC_separator->setGeometry(QRect(180, 50, this->width()-190, 1));
    mGC_VolumeProperty->setGeometry(QRect(180,180,this->width()-190,this->height()*0.5));
	mGC_VolumeScroll->setGeometry(QRect(185, 200, this->width() - 200, this->height()*0.5 - 25));
}

void GC_window::CreateVolumeWidget(
		const QString& volumeLetter,
		const QString& path,
		const eGC_TYPE& type,
		const eGC_ALGO& algorithm,
		const unsigned long long& size
	)
{
	if (mGC_VolumeItems[volumeLetter])
		delete mGC_VolumeItems[volumeLetter];
	mGC_VolumeItems[volumeLetter] = new GC_VolumeList(volumeLetter, path, type, algorithm, size, WIDTH-241);
	mGC_VolumeLayout->addWidget(mGC_VolumeItems[volumeLetter],mGC_gridCpt/2, mGC_gridCpt%2+1);
#ifdef QT_DEBUG
	qDebug() << mGC_gridCpt / 2 << "," << mGC_gridCpt % 2 + 1 << volumeLetter;
#endif
    QObject::connect(mGC_VolumeItems[volumeLetter], SIGNAL(Clicked(QString)), &mGC_slots, SLOT(GC_receive(QString)));
	mGC_gridCpt++;
}

void GC_window::DebugSlot(QString path)
{
#ifdef QT_DEBUG
	qDebug() << path;
#endif
}

void GC_window::OpenVolume()
{
    mGC_OpenVolumeWindow = new GC_OpenVolume(this, mGC_styleSheet);
    mGC_OpenVolumeWindow->setAttribute(Qt::WA_DeleteOnClose);
    mGC_OpenVolumeWindow->setModal(true);
    mGC_OpenVolumeWindow->show();
}
