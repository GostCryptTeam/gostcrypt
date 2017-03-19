/*!
 * \file Window.h
 * \brief Class for GostCrypt's UI
 * \author Antoine Hébert, Louis Béclair, William Lardier
 * \version 0.1
 * \date 15/07/2017
 *
 * GUI for GostCrypt 2.0
 *
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QDesktopWidget>
#include <QLabel>
#include <QString>
#include <QPushButton>
#include <QFile>
#include <QFontDatabase>
#include <QFont>
#include <QGridLayout>
#include <QFontMetrics>
#include <QFrame>
#include <QGroupBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QComboBox>
#include <QAbstractScrollArea>
#include <QMap>
#include <QListWidget>
#include <QTableWidget>
#include <QStandardItemModel>
#include <QScrollArea>

#include "Button.h"
#include "VolumeList.h"
#include "GraphicUserInterface.h"
#include "OpenVolumeDialog.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

#define UI_RATIO 1.41
#define HEIGHT 530
#define WIDTH HEIGHT*UI_RATIO

namespace UI {
	class Window;
}

	class Window : public QMainWindow
	{
		Q_OBJECT

	public:
		//! Constructor
		explicit Window(/*QWidget *parent = 0*/);
		//! Destructor
		~Window();

    public slots:
        // void AddVolume(); //! TODO

	protected :
		void ResizeEvent(QResizeEvent* event);
		void CreateVolumeWidget(
			const QString& volumeLetter,
			const QString& path,
			const VolumeType& type,
			const VolumeAlgorithm& algorithm,
			const unsigned long long& size
		);

	private slots:
		void DebugSlot(QString path);
        void OpenVolume();

	private:
        void ConnectSignals();
        QLabel mSidebar; /*!< left-sidebar with white background */
        QLabel mTitle; /*!< "GostCrypt volume creation Wizard" */
        int mGridCpt; /*!< Main Window's height */
		int mWidth; /*!< Main Window's width */
		int mHeight; /*!< Main Window's height */
		QLabel * mLogo; /*!< Label where is draw the logo */
		QLabel * mTextHistory; /*!< "Never save history" */
		QLabel * mLock; /*!< Green lock in 'mVolume' group */

		Button * mNewVolume; /*!< sends signal to create a new volume */
        Button * mMount; /*!< Mounts selected volume */
        Button * mAutoMount; /*!< Mounts all loaded volumes */
        Button * mDismountAll; /*!< Dismount all volumes */
        Button * mOpenVolume; /*!< Open an existing volume */

		QString mStyleSheet; /*!< stylesheet */
        QFont mFont; /*!< text font */
		QFrame * mSeparator; /*!< gray line */

        QGroupBox * mButtonVolume; /*!< Volume section */
        QGroupBox * mButtonMount; /*!< Mount volume section */
        QCheckBox * mSaveHistory; /*!< Allow user (or not) to save the history
                                         of previously loaded path(s) */
        QGridLayout * mGrid; /*!< Grid that contains volume groupbox elements */
		QGridLayout * mButtonGrid; /*!< TODO */
		QGridLayout * mButtonMountGrid; /*!< TODO */
		QComboBox * mVolumePath; /*!< Grid that contains volume groupbox elements */

		QGroupBox * mVolumeProperty; /*!< Volume property section */
		QMap<QString, VolumeList*> mVolumeItems; /*!< All Widget Volume items */
		QVBoxLayout * mVolumesWidget; /*!< Widget with all volumes */
        QWidget * mVolumeWidget;
        QGridLayout * mVolumeLayout;
        QScrollArea * mVolumeScroll;

        GraphicUserInterface mSlots; /*!< Slot manager */
        OpenVolumeDialog * mOpenVolumeDialogWindow; /*!< Dialog to open a volume file */
	};


#endif // WINDOW_H
