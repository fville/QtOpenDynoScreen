#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QPushButton>
#include <QSlider>
#include <QDial>
#include <QLCDNumber>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QDoubleSpinBox>
#include <QtDesigner/QFormBuilder>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerxml.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qsciapis.h>
#include <Qsci/qscistyle.h>
#include <QMessageBox>
#include "PropertyBrowser/qtpropertymanager.h"
#include "PropertyBrowser/qtvariantproperty.h"
#include "PropertyBrowser/qttreepropertybrowser.h"
#include "AnalogWidgets/manometer.h"
#include "AnalogWidgets/counter.h"
#include "AnalogWidgets/potentiometer.h"
#include "AnalogWidgets/thermometer.h"
#include "AnalogWidgets/led.h"
#include "AnalogWidgets/wallclock.h"
#include <QFileDialog>
#include <QDomDocument>
#include <QXmlFormatter>
#include "typeconverters.h"
#include "controlFactory.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    editMode(false)
{
    ui->setupUi(this);
    QMainWindow::centralWidget()->layout()->setContentsMargins(0, 0, 0, 0);
    //Create the rubber band selector
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

    jsLexer = new QsciLexerJavaScript(this);
    scriptEdit = new QsciScintilla(this);

    xmlLexer = new QsciLexerXML(this);
    xmlEdit = new QsciScintilla(this);

    QsciAPIs *api = new QsciAPIs(jsLexer);
    api->add("util");
    api->add("util.MessageBox");
    api->add("screen");
    //api->add("sOmethingElse");
    //Compile the api for use in the lexer
    api->prepare();


    scriptEdit->autoCompleteFromAll();
     //textEdit->autocomp
    scriptEdit->setAutoCompletionSource(QsciScintilla::AcsAPIs);
    scriptEdit->setAutoCompletionThreshold(1);
    scriptEdit->setLexer(jsLexer);
    scriptEdit->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    scriptEdit->setMarginLineNumbers(0,true);
    scriptEdit->setMarginWidth(0,30);
    scriptEdit->setAutoIndent(true);
    scriptEdit->setFolding(QsciScintilla::BoxedTreeFoldStyle );



    xmlEdit->setLexer(xmlLexer);
    xmlEdit->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    xmlEdit->setMarginLineNumbers(0,true);
    xmlEdit->setMarginWidth(0,30);
    xmlEdit->setAutoIndent(true);
    xmlEdit->setFolding(QsciScintilla::BoxedTreeFoldStyle );


    ui->verticalLayout_2->addWidget(scriptEdit);
    ui->verticalLayout_3->addWidget(xmlEdit);
    //textEdit->setLayout(ui->verticalLayout_2);

    QFile fOutput("out.ui");
    fOutput.open(QIODevice::WriteOnly);
    Q_ASSERT(fOutput.isOpen());
    //save .ui file
    QFormBuilder builder;
    builder.save(&fOutput,this);
    //close file
    fOutput.close();


    //ui->screen->children().count()
    debugger.attachTo(&engine);
    QScriptValue result = engine.evaluate("(1+2)*3.23232");
    qDebug() << "Result as float:" << result.toNumber();

    QScriptValue ret = engine.evaluate("var x=0.0; \nfor(var  i=0; i<1000; i++) \n{\n x += i * 1.23 + 4.56; \n  \n}\n print(x);  function add(a, b) { for(var  i=0; i<1000; i++) \n{\n x += i * 1.23 + 4.56; \n  \n}     return x= x+ a + b;}");
    qDebug() << "script returned " << ret.toInteger();

    QScriptValue add = engine.globalObject().property("add");
    qDebug() << add.call(QScriptValue(), QScriptValueList() << 1 << 2).toNumber(); // 3

    QScriptValue qsScreen = engine.newQObject(ui->screen);// interpreter.newQObject(dialog);
    engine.globalObject().setProperty("screen",qsScreen);
   // qsScript.setProperty("dialog", qsDialog);

    QScriptValue qsFunctions = engine.newQObject(&sFunctions);
    engine.globalObject().setProperty("util",qsFunctions);

//    variantManager = new QtVariantPropertyManager(ui->screen);
//    QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(ui->screen);
//    propertyEditor = new QtTreePropertyBrowser();
//    propertyEditor->setFactoryForManager(variantManager, variantFactory);
//    propertyEditor->show();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    if(!editMode) return;
    contextPoint = event->globalPos();

    QMenu menu(this);

    QMenu *editMenu = menu.addMenu("Edit");
    editMenu->addAction(ui->actionDelete);
    editMenu->addAction(ui->actionCopy);
    editMenu->addAction(ui->actionPaste);

    QMenu *controlMenu = menu.addMenu("Native Controls");
    controlMenu->addAction(ui->actionPushButton);
    controlMenu->addAction(ui->actionTextbox);
    controlMenu->addAction(ui->actionLabel);
    controlMenu->addAction(ui->actionSpinBox);
    controlMenu->addAction(ui->actionSlider);

    QMenu *gaugeMenu = menu.addMenu("Gauges and Dials");
    gaugeMenu->addAction(ui->actionManometer);
    gaugeMenu->addAction(ui->actionThermometer);
    gaugeMenu->addAction(ui->actionPotentiometer);
    gaugeMenu->addAction(ui->actionDial);
    gaugeMenu->addAction(ui->actionMeter);

    QMenu *lcdMenu = menu.addMenu("LCDs");
    lcdMenu->addAction(ui->actionLCD);
    lcdMenu->addAction(ui->actionCounter);

    QMenu *indMenu = menu.addMenu("LEDs and Indicators");
    indMenu->addAction(ui->actionLED);


    menu.addAction(ui->actionDelete);
    menu.addAction(ui->actionDial);
   // menu.addAction()

    menu.exec(event->globalPos());


}

void MainWindow::on_actionEdit_Mode_triggered(bool checked)
{
    editMode = checked;
    qDebug() << "Edit mode =" << checked;
}


void MainWindow::selectWidgets()
{
    const QWidgetList l = ui->screen->findChildren<QWidget*>();
    QListIterator <QWidget*> it(l);
    const QRect selRect(mapToGlobal(m_currRect.topLeft()), m_currRect.size());
    while (it.hasNext()) {
        QWidget *w = it.next();
        if (w->isVisibleTo(this)) {
            const QPoint p = w->mapToGlobal(QPoint(0,0));
            const QRect r(p, w->size());
            if (r.intersects(selRect) && !r.contains(selRect) )
            {
                if(w->parent() == ui->screen)
                {
                    selectWidget(w);
                }
                qDebug() << "Found item " << w->objectName();
            }
        }
    }
}

void MainWindow::selectWidget(QWidget *w)
{
    WidgetSelection *rc;
    rc = new WidgetSelection(this);
    rc->setWidget(w);
    m_selectionPool.push_back(rc);
}

void MainWindow::clearSelectedWidgets()
{
    const SelectionPool::iterator mend = m_selectionPool.end();
    for (SelectionPool::iterator it = m_selectionPool.begin(); it != mend; ++it) {
        (*it)->setWidget(0);
    }
    qDeleteAll(m_selectionPool);
    m_selectionPool.clear();
}


void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(!editMode) return;
    if (Qt::RightButton == event->button()) return;
    clearSelectedWidgets();

    rubbndStartPoint = event->pos();

    rubberBand->setGeometry(QRect(rubbndStartPoint, QSize()));
    rubberBand->show();
    m_currRect = QRect();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(!editMode) return;
    QRect r(rubbndStartPoint, event->pos());
    r = r.normalized();
    m_currRect =r;
    rubberBand->setGeometry(QRect(rubbndStartPoint, event->pos()).normalized());//Area Bounding

}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(!editMode) return;
    selectWidgets();
    rubberBand->hide();// hide on mouse Release
    //delete rubberBand;
}

int sliderCnt=1;
void MainWindow::on_actionSlider_triggered()
{
    QSlider *slider = new QSlider(Qt::Horizontal, ui->screen);
    QString objName = QString("slider%1").arg(sliderCnt);
    sliderCnt++;
    slider->setObjectName(objName);
    slider->move( (ui->screen->mapFromGlobal(contextPoint)));
    slider->show();
}

int dialCnt=1;
void MainWindow::on_actionDial_triggered()
{
    QDial *dial = new QDial(ui->screen);
    QString objName = QString("dial%1").arg(dialCnt);
    dialCnt++;
    dial->setObjectName(objName);
    dial->move( (ui->screen->mapFromGlobal(contextPoint)));
    dial->show();
}

int pbCnt=1;
void MainWindow::on_actionPushButton_triggered()
{
    QPushButton *button = new QPushButton(ui->screen);
    QString objName = QString("button%1").arg(pbCnt);
    pbCnt++;
    button->setObjectName(objName);
    button->setText("New Button");
    button->move( (ui->screen->mapFromGlobal(contextPoint)));
    button->show();
}

int lcdCnt=1;
void MainWindow::on_actionLCD_triggered()
{
    QLCDNumber *lcd = new QLCDNumber(ui->screen);
    QString objName = QString("lcd%1").arg(lcdCnt);
    lcdCnt++;
    lcd->setObjectName(objName);
    lcd->move( (ui->screen->mapFromGlobal(contextPoint)));
    lcd->display(100);
    lcd->show();

}

int labelCnt=1;
void MainWindow::on_actionLabel_triggered()
{
    QLabel *label = new QLabel("New label" ,ui->screen);
    QString objName = QString("label%1").arg(labelCnt);
    labelCnt++;
    label->setObjectName(objName);
    label->move( (ui->screen->mapFromGlobal(contextPoint)));
    label->show();
}

int spinBoxCnt=1;
void MainWindow::on_actionSpinBox_triggered()
{
    QDoubleSpinBox *spinbox = new QDoubleSpinBox(ui->screen);
    QString objName = QString("spinbox%1").arg(spinBoxCnt);
    spinBoxCnt++;
    spinbox->setObjectName(objName);
    spinbox->move( (ui->screen->mapFromGlobal(contextPoint)));
    spinbox->show();
}

int txtBoxCnt=1;
void MainWindow::on_actionTextbox_triggered()
{
    QLineEdit *textbox = new QLineEdit("New text box", ui->screen);
    QString objName = QString("textbox%1").arg(txtBoxCnt);
    txtBoxCnt++;
    textbox->setObjectName(objName);
    textbox->move( (ui->screen->mapFromGlobal(contextPoint)));
    textbox->show();
}

int ledCnt=1;
void MainWindow::on_actionLED_triggered()
{
    QLedIndicator *led = new QLedIndicator(ui->screen);
    QString objName = QString("led%1").arg(ledCnt);
    ledCnt++;
    led->setObjectName(objName);
    led->move( (ui->screen->mapFromGlobal(contextPoint)));
    led->show();
}

int meterCnt =1;
void MainWindow::on_actionMeter_triggered()
{
    QScale *scale = new QScale(ui->screen);
    QString objName = QString("meter%1").arg(meterCnt);
    meterCnt++;
    scale->setObjectName(objName);
    scale->move( (ui->screen->mapFromGlobal(contextPoint)));
    scale->show();
}

void MainWindow::on_actionDelete_triggered()
{
    const SelectionPool::iterator mend = m_selectionPool.end();
    for (SelectionPool::iterator it = m_selectionPool.begin(); it != mend; ++it) {
        QWidget* current = (*it)->widget();
        delete current;
        //delete (*it)->widget();
        //(*it)->widget() = 0;
        (*it)->setWidget(0);
    }
    qDeleteAll(m_selectionPool);
    m_selectionPool.clear();
}

void MainWindow::on_actionAlign_Lefts_triggered()
{
    const SelectionPool::iterator mend = m_selectionPool.end();
    for (SelectionPool::iterator it = m_selectionPool.begin(); it != mend; ++it) {
        (*it)->widget()->move(0,0);
    }
}

void MainWindow::on_actionCall_Script_triggered()
{
    QMessageBox msgbox;
    try
    {
        //Update the script if it was modified
        if(scriptEdit->isModified())
        {
            QScriptValue disc = engine.globalObject().property("UnInitialize");
            QVariant discValue = disc.call(QScriptValue(), QScriptValueList() ).toVariant();

            engine.evaluate(scriptEdit->text());

            QScriptValue con = engine.globalObject().property("Initialize");
            QVariant conValue = con.call(QScriptValue(), QScriptValueList() ).toVariant();

            scriptEdit->setModified(false);
        }
     }
    catch(...)
    {
        QString msg = QString("Script error");
        msgbox.setText(msg);
        msgbox.exec();
    }
}

int manometerCnt =1;
void MainWindow::on_actionManometer_triggered()
{
    ManoMeter *manometer = new ManoMeter(ui->screen);
    QString objName = QString("manometer%1").arg(manometerCnt);
    manometerCnt++;
    manometer->setObjectName(objName);
    manometer->move( (ui->screen->mapFromGlobal(contextPoint)));
    manometer->show();

}

int thermoCnt =1;
void MainWindow::on_actionThermometer_triggered()
{
    ThermoMeter *thermo = new ThermoMeter(ui->screen);
    QString objName = QString("thermometer%1").arg(thermoCnt);
    thermoCnt++;
    thermo->setObjectName(objName);
    thermo->move( (ui->screen->mapFromGlobal(contextPoint)));
    thermo->show();

}

int counterCnt=1;
void MainWindow::on_actionCounter_triggered()
{
    Counter *counter = new Counter(ui->screen);
    QString objName = QString("counter%1").arg(counterCnt);
    counterCnt++;
    counter->setObjectName(objName);
    counter->move( (ui->screen->mapFromGlobal(contextPoint)));
    counter->show();

}

int potCnt=1;
void MainWindow::on_actionPotentiometer_triggered()
{
    PotentioMeter *pot = new PotentioMeter(ui->screen);
    QString objName = QString("potentiometer%1").arg(potCnt);
    potCnt++;
    pot->setObjectName(objName);
    pot->move( (ui->screen->mapFromGlobal(contextPoint)));
    pot->show();

}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Dyno Screen File"),
                                                    "",
                                                    tr("Screen (*.screen)"));

    if (fileName.isEmpty())
        return;

    //Clear the screen and open file
    RemoveAllScreenWidgets();
    OpenScreenFile(fileName);

    if(scriptEdit->text().length() > 1)
    {
        engine.evaluate(scriptEdit->text());
        QScriptValue disc = engine.globalObject().property("Initialize");
        QVariant discValue = disc.call(QScriptValue(), QScriptValueList() ).toVariant();
        scriptEdit->setModified(false);
    }
}




void MainWindow::OpenScreenFile(QString fileName)
{

    QString xmlContent = "";
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while (!file.atEnd()) {
        xmlContent += file.readLine();
    }
    file.close();

    xmlEdit->setText(xmlContent);



    QDomDocument screenDoc;
    screenDoc.setContent(xmlContent);
    QDomElement docElement = screenDoc.documentElement();
    this->resize(QSizeConverter::ConvertFromString(docElement.attribute("Size", "640,480")));
    qDebug() << "Document Element = \n" << docElement.nodeName();
    qDebug() << "CHILDREN" << docElement.childNodes().count();
    QDomNode node = docElement.firstChild();

    int i=0;
    while(!node.isNull())
    {
        i++;
        if(i>30) break;
        if(node.isElement())
        {
            QDomElement element = node.toElement();
            qDebug() << "ELEMENT" << element.tagName();
            if(element.tagName() == "Controls")
            {
                QDomNodeList controls = element.childNodes();
                int controlCount = controls.count();
                for(int i=0; i<controlCount; i++)
                {
                    QDomNode controlNode = controls.at(i);
                    QDomElement control = controlNode.toElement();


                    ControlFactory::CreateWidget(ui->screen,
                                                 ControlFactory::GetTypeFromString(
                                                     control.attribute("Type")),
                                                 control);
                }
            }
            else if(element.tagName() == "Script")
            {
                if(node.firstChild().isCDATASection())
                {
                    QDomNode scripNode = node.firstChild();
                   // QDomElement scriptEl = scripNode.toElement();
                    QDomCDATASection script = scripNode.toCDATASection();
                    qDebug() << "Script has a CDATA section";
                    qDebug() << script.data();
                    scriptEdit->setText(script.data());
                }
            }
            qDebug() << "CHILDREN" << element.childNodes().count();
            qDebug() << "ELEMENT ATTRIBUTE NAME:" << element.attribute("name", "not set");
        }

        if(node.isText())
        {
            QDomText text = node.toText();
            qDebug() << text.data();
        }

        node = node.nextSiblingElement();
        qDebug() << "Node name" << node.nodeName();
    }
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Open Dyno Screen File"),
                                                    "",
                                                    tr("Screen (*.screen)"));

    if (fileName.isEmpty())
        return;

    QString xmlContent = "";
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;


    QDomDocument doc("OpenDynoScreen");
    QDomElement root = doc.createElement("OpenDynoScreen");
    root.setAttribute("Size", QSizeConverter::ConvertToString(this->size()));
    doc.appendChild(root);

    QDomElement controlsEl = doc.createElement("Controls");
    root.appendChild(controlsEl);


    QList<QWidget*> children= ui->screen->findChildren<QWidget*>();
    int cnt=children.count();
    for(int i=0;i<cnt;i++)
    {
        if(children.at(i)->parent() == ui->screen)
        {
            QDomElement controlEl = doc.createElement("Control");
            controlEl.setAttribute("Type", children.at(i)->metaObject()->className());
            controlEl.setAttribute("Name", children.at(i)->objectName());
            controlEl.setAttribute("Size", QSizeConverter::ConvertToString(children.at(i)->size()) );
            controlEl.setAttribute("Location", QPointConverter::ConvertToString(children.at(i)->pos()) );
            controlEl.setAttribute("Font", QFontConverter::ConvertToString(children.at(i)->font()) );
            controlsEl.appendChild(controlEl);
        }
    }

    QDomElement script = doc.createElement("Script");
    root.appendChild(script);
    QDomCDATASection cdata = doc.createCDATASection("Script");
    cdata.setData(scriptEdit->text());
    script.appendChild(cdata);



//    QDomText t = doc.createTextNode("Hello World");
//    tag.appendChild(t);

    QString xml = doc.toString();
    xmlEdit->setText(xml);

    file.write(xml.toUtf8().constData());
    file.close();

//    QDomDocument screenDoc("OpenDynoScren");
//    QDomElement root = screenDoc.createElement("OpenDynoScreen");
//    root.set
//    screenDoc.appendChild(root);

//        elem.setAttribute("Image", ImageName);
//        elem.setAttribute("Name", Description);
//        elem.setAttribute("ID", ID);
//        elem.setAttribute("TargetClass", ClassType);
//        elem.setAttribute("Type", TypeToString());
//        QDomElement physEl = doc.createElement("Physics");
//        for(int i=0;i<BodyShapes.count();i++)
//        {
//            physEl.appendChild(BodyShapes[i]->ToXMLNode(doc));
//        }
//        elem.appendChild(physEl);
//        return elem;


}

void MainWindow::RemoveAllScreenWidgets()
{
    //Remove any selections
    clearSelectedWidgets();

    //Delete all of the widgets
    QList<QWidget*> children= ui->screen->findChildren<QWidget*>();
    int cnt=children.count();
    for(int i=0;i<cnt;i++)
    {
        if(children.at(i)->parent() == ui->screen)
        {
        children.at(i)->setParent(0);
        delete children.at(i);
        }
    }
}

//void Asset::Load(const QDomElement& node)
//{
//    ImageName =  node.attribute("Image");
//    Bitmap.load(GetResourceFolder() + QDir::separator() + ImageName);
//    Description =  node.attribute("Name");
//    ID =  node.attribute("ID");
//    ClassType =  node.attribute("TargetClass");
//    QDomNodeList shapes = node.firstChildElement("Physics").childNodes();
//    for(int i=0;i<shapes.count();i++)
//    {
//        BodyShape* s = BodyShape::Load(shapes.item(i).toElement());
//        BodyShapes << s;
//    }
//    IsLoaded = true;
//}


void MainWindow::on_tabWidget_currentChanged(int index)
{
    Q_UNUSED(index)
    clearSelectedWidgets();
}
