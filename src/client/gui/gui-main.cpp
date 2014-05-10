/**
 * @file gui-main.cpp
 *
 * @brief hlavny modul pre implementaciu grafickeho rozhrania aplikacie
 *
 * @author Stefan Marticek
 * @author Oliver Nemcek
 */

#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    try
    {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        a.exec();
    }
    catch( std::exception &e )
    {
        QMessageBox msgBox;
        msgBox.setText("Exception catched. Probably connection error:");
        msgBox.setInformativeText(e.what());
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStandardButtons(QMessageBox::Abort);
    }

    return 0;
}
