#ifndef SCRIPTFUNCTIONS_H
#define SCRIPTFUNCTIONS_H

#include <QObject>

class ScriptFunctions : public QObject
{
    Q_OBJECT
public:
    explicit ScriptFunctions(QObject *parent = 0);

signals:

public slots:
    void MessageBox(QString message);
};

#endif // SCRIPTFUNCTIONS_H
