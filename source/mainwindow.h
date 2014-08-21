#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRubberBand>
#include <QMouseEvent>
#include "widgetselection.h"
#include "LedIndicator/qledindicator.h"
#include "QScale/qscale.h"
#include <QtScript/QScriptEngine>
#include <QtScriptTools/QScriptEngineDebugger>
#include "scriptfunctions.h"


namespace Ui {

class MainWindow;
}
class QsciScintilla;
class QsciLexerJavaScript;
class QsciLexerXML;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void OpenScreenFile(QString fileName);
    void RemoveAllScreenWidgets();
    ~MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void on_actionEdit_Mode_triggered(bool checked);

    void on_actionSlider_triggered();

    void on_actionDial_triggered();

    void on_actionPushButton_triggered();

    void on_actionLCD_triggered();

    void on_actionLabel_triggered();

    void on_actionSpinBox_triggered();

    void on_actionTextbox_triggered();

    void on_actionLED_triggered();

    void on_actionMeter_triggered();

    void on_actionDelete_triggered();

    void on_actionAlign_Lefts_triggered();

    void on_actionCall_Script_triggered();

    void on_actionManometer_triggered();

    void on_actionThermometer_triggered();

    void on_actionCounter_triggered();

    void on_actionPotentiometer_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void selectWidgets();
    void selectWidget(QWidget *);
    void clearSelectedWidgets();

    bool editMode;

    QRubberBand *rubberBand;
    QPoint rubbndStartPoint;
    QPoint contextPoint;
    WidgetSelection *selection;
    QRect m_currRect;
    QsciScintilla *scriptEdit;
    QsciScintilla *xmlEdit;
    QsciLexerXML *xmlLexer;
    QsciLexerJavaScript *jsLexer;


    QScriptEngine engine;
    QScriptEngineDebugger debugger;
    ScriptFunctions sFunctions;

    typedef QList<WidgetSelection *> SelectionPool;
    SelectionPool m_selectionPool;

    class QtVariantPropertyManager *variantManager;
    class QtTreePropertyBrowser *propertyEditor;
};

#endif // MAINWINDOW_H
