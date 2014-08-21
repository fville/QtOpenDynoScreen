#include "scriptfunctions.h"
#include <QMessageBox>

ScriptFunctions::ScriptFunctions(QObject *parent) :
    QObject(parent)
{
}

void ScriptFunctions::MessageBox(QString message)
{
   QMessageBox msgbox;
   msgbox.setText(message);
   msgbox.exec();

}
