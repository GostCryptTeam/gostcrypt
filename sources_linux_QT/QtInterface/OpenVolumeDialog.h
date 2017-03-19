#ifndef OPENVOLUME_H
#define OPENVOLUME_H
#include <QDialog>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QFontDatabase>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include "Button.h"

class OpenVolumeDialog : public QDialog
{
    Q_OBJECT

public:
    OpenVolumeDialog(QWidget *parent = 0, const QString& style = "");
    ~OpenVolumeDialog();

private slots:
    void open();
    void quit();

private:
    QString mStyleSheet; /*!< stylesheet */
    QFont mFont; /*!< Font used in the buttons */
    QGroupBox * mVolume; /*!< Table of widgets */
    QGridLayout * mGrid; /*!< Layout that contains child widgets */
    QLabel * mLock; /*!< GostCrypt logo */
    QLabel * mTextHistory; /*!< Text related to mSaveHistory */

    QLabel * mPasswordTxt;
    QLineEdit * mPassword;
    QCheckBox * mCachePwd;
    QCheckBox * mDisplayPwd;
    QCheckBox * mUseKeyfile;
    Button * mKeyFiles;
    Button * mMountOption;

    QComboBox * mVolumePath; /*!< file path */
    QCheckBox * mSaveHistory; /*!< save the previous path of openned files */
    Button * mSelectFile; /*!< sends signal to select a new volume file */
    Button * mVolumeTools; /*!< sends signal to open the volume tools window */
    Button * mSelectDevice; /*!< sends signal to let the user to choose the device */
    Button * mOk; /*!< Open Volume button */
    Button * mCancel; /*!< Cancel button */
};

#endif // OPENVOLUME_H