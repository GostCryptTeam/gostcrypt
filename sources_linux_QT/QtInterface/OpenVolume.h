#ifndef GC_OPENVOLUME_H
#define GC_OPENVOLUME_H
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
#include "GC_button.h"

class GC_OpenVolume : public QDialog
{
    Q_OBJECT

public:
    GC_OpenVolume(QWidget *parent = 0, const QString& style = "");
    ~GC_OpenVolume();

private slots:
    void open();
    void quit();

private:
    QString mGC_styleSheet; /*!< stylesheet */
    QFont mGC_font; /*!< Font used in the buttons */
    QGroupBox * mGC_Volume; /*!< Table of widgets */
    QGridLayout * mGC_grid; /*!< Layout that contains child widgets */
    QLabel * mGC_lock; /*!< GostCrypt logo */
    QLabel * mGC_textHistory; /*!< Text related to mGC_saveHistory */

    QLabel * mGC_passwordTxt;
    QLineEdit * mGC_password;
    QCheckBox * mGC_cachePwd;
    QCheckBox * mGC_displayPwd;
    QCheckBox * mGC_useKeyfile;
    GC_button * mGC_keyFiles;
    GC_button * mGC_mountOption;

    QComboBox * mGC_volumePath; /*!< file path */
    QCheckBox * mGC_saveHistory; /*!< save the previous path of openned files */
    GC_button * mGC_selectFile; /*!< sends signal to select a new volume file */
    GC_button * mGC_volumeTools; /*!< sends signal to open the volume tools window */
    GC_button * mGC_selectDevice; /*!< sends signal to let the user to choose the device */
    GC_button * mGC_ok; /*!< Open Volume button */
    GC_button * mGC_cancel; /*!< Cancel button */
};

#endif // GC_OPENVOLUME_H
