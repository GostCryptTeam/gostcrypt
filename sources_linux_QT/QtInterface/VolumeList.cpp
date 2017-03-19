#include "VolumeList.h"
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

VolumeList::VolumeList(
	const QString& volumeLetter,
	const QString& path,
	const VolumeType& type,
	const VolumeAlgorithm& algorithm,
	const unsigned long long& size,
	const int& width) :
	mVolumeLetter(volumeLetter),
	mPath(path),
	mType(type),
	mAlgorithm(algorithm),
	mSize(size)
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
	mBkGray = new QLabel(this);
	this->setMinimumHeight(90);
	mBkGray->resize(width/2, 90);
	mBkGray->setObjectName(QString::fromUtf8("volumeItem"));	

	/*! Loading disk image */
	mDiskLabel = new QLabel(this);
	mDiskLabel->setGeometry(20, 5, 55, 59);
	mDiskLabel->setPixmap(QPixmap(
		QString::fromUtf8(":/ressources/disk.gif")
	));
	mDiskLabel->setObjectName(QString::fromUtf8("disk"));

	/*! Showing the volume letter */
	QString volume = volumeLetter + QString::fromUtf8(":/");
	mVolumeLetterLabel = new QLabel(this);
	mVolumeLetterLabel->setFont(QFont("Caviar Dreams", 15, 5));
	mVolumeLetterLabel->setText(volume.toUpper());
	mVolumeLetterLabel->setAlignment(Qt::AlignCenter);
	mVolumeLetterLabel->setGeometry(/*21*/25, 55, 54, 25);
	mVolumeLetterLabel->setObjectName(QString::fromUtf8("grayColor"));

	/*! Volume path */
	mPathLabel = new QLabel(this);
	mPathLabel->setFont(QFont("Caviar Dreams", 10, 5));
	mPathLabel->setAlignment(Qt::AlignLeft);
	mPathLabel->setGeometry(80, 10, 140, 50);
	//mPathLabel->setStyleSheet("QLabel {background-color:#000eee;}");
	mPathLabel->setWordWrap(false);
	mPathLabel->setText(truncate(mPath));
	mPathLabel->setToolTip(QString(mPath));
	mPathLabel->setObjectName(QString::fromUtf8("grayColor"));

	/*! Size of volume */
	mSizeLabel = new QLabel(this);
	mSizeLabel->setFont(QFont("Caviar Dreams", 15, 10));
	mSizeLabel->setText(formatSize(mSize));
	mSizeLabel->setGeometry(80, 27, 140, 20);
	mSizeLabel->setAlignment(Qt::AlignCenter);
	mSizeLabel->setObjectName(QString::fromUtf8("grayColorDark"));

	/*! Type of Algorithm */
	mAlgorithmLabel = new QLabel(this);
	mAlgorithmLabel->setFont(QFont("Caviar Dreams", 12, 10));
    mAlgorithmLabel->setText((mAlgorithm == VolumeAlgorithm.eGost) ? QString("GOST 28147-89") : QString("GOST Grasshopper"));
	mAlgorithmLabel->setGeometry(80, 49, 140, 20);
	mAlgorithmLabel->setAlignment(Qt::AlignCenter);
	mAlgorithmLabel->setObjectName(QString::fromUtf8("grayColorDark"));

	/*! Type of volume */
	mTypeLabel = new QLabel(this);
	mTypeLabel->setFont(QFont("Caviar Dreams", 10, 10));
	QString lType = QObject::tr("Type : ");
	QString lResult = (mType == VolumeType.eNORMAL)
		? QObject::tr("Normal") :
		QObject::tr("Autre");
	mTypeLabel->setText(lType + lResult);
	mTypeLabel->setGeometry(80, 67, 140, 20);
	mTypeLabel->setAlignment(Qt::AlignCenter);
	mTypeLabel->setObjectName(QString::fromUtf8("grayColor"));

#ifdef QT_DEBUG
	qDebug() << "Creation of " << mPath << "volume succeed";
#endif
}

VolumeList::~VolumeList()
{
	delete mBkGray;
	delete mDiskLabel;
}

void VolumeList::mousePressEvent(QMouseEvent * event)
{
#ifdef QT_DEBUG
	qDebug() << "Mouse pressed";
#endif
	emit Clicked(mVolumeLetter);
}

void VolumeList::enterEvent(QEvent * event)
{
#ifdef QT_DEBUG
	qDebug() << Q_FUNC_INFO << this->objectName();
#endif
	mBkGray->setStyleSheet("#volumeItem {border: 1px solid #000000;}");
	QWidget::enterEvent(event);
	
}

void VolumeList::leaveEvent(QEvent * event)
{
#ifdef QT_DEBUG
	qDebug() << Q_FUNC_INFO << this->objectName();
#endif
	mBkGray->setStyleSheet("#volumeItem {border: 0px; background: #ececec}");
	QWidget::leaveEvent(event);
}
