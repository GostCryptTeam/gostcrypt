#include <iostream>
#include <string>
#include "modelclass.h"
using namespace std;

QString ModelClass::getText() const {
    return _test;
}

void ModelClass::setText(const QString& txt) {
    _test = txt;
    qDebug() << "Texte issu du code c++ : " << _test;
    emit testChanged();
}

/*void ModelClass::testSignal() {
    //cout << "Le bouton cliqué est : " << _test.toStdString();
    qDebug("Test");
}*/

