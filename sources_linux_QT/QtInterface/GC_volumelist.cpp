#include "GC_volumelist.h"
#include <QHBoxLayout>
#include <QFontDatabase>
#include <QDebug>
#define UI_RATIO 1.41

/*! truncate the path text */
QString truncate(QString str) {
	if (str.length() > 20)
		return str.left(20) + QString("...");
	else
		return str;
}

/*! converts byte to MB, GB, KB */
QString formatSize(unsigned long long sizeInByte) {

	if (sizeInByte < 1024) return QString(QString("<font color=#6e9f45>")
		+ QString::number(sizeInByte)
		+ QString("</font>") 
		+ QString(" B"));

	else if (sizeInByte < 1048576) return QString("<font color=#6e9f45>") 
		+ QString::number((float)sizeInByte / (float)1024, 'f', 1) 
		+ QString("</font>") 
		+ QString(" KB");

	else if (sizeInByte < 1073741824) return QString("<font color=#6e9f45>") 
		+ QString::number((float)sizeInByte / (float)1048576, 'f', 1) 
		+ QString("</font>") 
		+ QString(" MB");

	else return QString("<font color=#6e9f45>") 
		+ QString::number((float)sizeInByte / (float)1073741824, 'f', 1)
		+ QString("</font>") 
		+ QString(" GB");

}

GC_VolumeList::GC_VolumeList(
	const QString& volumeLetter,
	const QString& path,
	const eGC_TYPE& type,
	const eGC_ALGO& algorithm,
	const unsigned long long& size,
	const int& width) :
	m_volumeLetter(volumeLetter),
	m_path(path),
	m_type(type),
	m_algorithm(algorithm),
	m_size(size)
{
	QFontDatabase::addApplicationFont(":/ressources/gs_font.ttf");
	this->setAttribute(Qt::WA_Hover);
	this->setMouseTracking(true);
	this->setStyleSheet("QLabel {background-color:#ececec;}" +
				QString("#grayColor {color: #777777;}") +
				QString("#grayColorDark {color:#3c3c3c}") +
				QString("#volumeItem {border-radius: 45px;}") +
				QString("#disk { background: transparent; }"));

	/*! Gray background with border radius */
	m_bkGray = new QLabel(this);
	this->setMinimumHeight(90);
	m_bkGray->resize(width/2, 90);
	m_bkGray->setObjectName(QString::fromUtf8("volumeItem"));	

	/*! Loading disk image */
	m_diskLabel = new QLabel(this);
	m_diskLabel->setGeometry(20, 5, 55, 59);
	m_diskLabel->setPixmap(QPixmap(
		QString::fromUtf8(":/ressources/disk.gif")
	));
	m_diskLabel->setObjectName(QString::fromUtf8("disk"));

	/*! Showing the volume letter */
	QString volume = volumeLetter + QString::fromUtf8(":/");
	m_volumeLetterLabel = new QLabel(this);
	m_volumeLetterLabel->setFont(QFont("Caviar Dreams", 15, 5));
	m_volumeLetterLabel->setText(volume.toUpper());
	m_volumeLetterLabel->setAlignment(Qt::AlignCenter);
	m_volumeLetterLabel->setGeometry(/*21*/25, 55, 54, 25);
	m_volumeLetterLabel->setObjectName(QString::fromUtf8("grayColor"));

	/*! Volume path */
	m_pathLabel = new QLabel(this);
	m_pathLabel->setFont(QFont("Caviar Dreams", 10, 5));
	m_pathLabel->setAlignment(Qt::AlignLeft);
	m_pathLabel->setGeometry(80, 10, 140, 50);
	//m_pathLabel->setStyleSheet("QLabel {background-color:#000eee;}");
	m_pathLabel->setWordWrap(false);
	m_pathLabel->setText(truncate(m_path));
	m_pathLabel->setToolTip(QString(m_path));
	m_pathLabel->setObjectName(QString::fromUtf8("grayColor"));

	/*! Size of volume */
	m_sizeLabel = new QLabel(this);
	m_sizeLabel->setFont(QFont("Caviar Dreams", 15, 10));
	m_sizeLabel->setText(formatSize(m_size));
	m_sizeLabel->setGeometry(80, 27, 140, 20);
	m_sizeLabel->setAlignment(Qt::AlignCenter);
	m_sizeLabel->setObjectName(QString::fromUtf8("grayColorDark"));

	/*! Type of Algorithm */
	m_algorithmLabel = new QLabel(this);
	m_algorithmLabel->setFont(QFont("Caviar Dreams", 12, 10));
    m_algorithmLabel->setText((m_algorithm == eGC_GOST) ? QString("GOST 28147-89") : QString("GOST Grasshopper"));
	m_algorithmLabel->setGeometry(80, 49, 140, 20);
	m_algorithmLabel->setAlignment(Qt::AlignCenter);
	m_algorithmLabel->setObjectName(QString::fromUtf8("grayColorDark"));

	/*! Type of volume */
	m_typeLabel = new QLabel(this);
	m_typeLabel->setFont(QFont("Caviar Dreams", 10, 10));
	QString lType = QObject::tr("Type : ");
	QString lResult = (m_type == eGC_NORMAL)
		? QObject::tr("Normal") :
		QObject::tr("Autre");
	m_typeLabel->setText(lType + lResult);
	m_typeLabel->setGeometry(80, 67, 140, 20);
	m_typeLabel->setAlignment(Qt::AlignCenter);
	m_typeLabel->setObjectName(QString::fromUtf8("grayColor"));

#ifdef QT_DEBUG
	qDebug() << "Creation of " << m_path << "volume succeed";
#endif
}

GC_VolumeList::~GC_VolumeList()
{
	delete m_bkGray;
	delete m_diskLabel;
}

void GC_VolumeList::mousePressEvent(QMouseEvent * event)
{
#ifdef QT_DEBUG
	qDebug() << "Mouse pressed";
#endif
	emit Clicked(m_volumeLetter);
}

void GC_VolumeList::enterEvent(QEvent * event)
{
#ifdef QT_DEBUG
	qDebug() << Q_FUNC_INFO << this->objectName();
#endif
	m_bkGray->setStyleSheet("#volumeItem {border: 1px solid #000000;}");
	QWidget::enterEvent(event);
	
}

void GC_VolumeList::leaveEvent(QEvent * event)
{
#ifdef QT_DEBUG
	qDebug() << Q_FUNC_INFO << this->objectName();
#endif
	m_bkGray->setStyleSheet("#volumeItem {border: 0px; background: #ececec}");
	QWidget::leaveEvent(event);
}
