#ifndef CONTROLFACTORY_H
#define CONTROLFACTORY_H

#include <QtCore>
#include <QDebug>
#include <QtWidgets>
#include <QDomDocument>



class ControlFactory
{
public:
    ControlFactory();

    enum ControlType
    {
        Unknown,
        OpenDynoPushButton,
        OpenDynoLabel,
        OpenDynoTextBox,
        OpenDynoSpinBox,
        OpenDynoSlider,
        OpenDynoThermometer,
        OpenDynoEncoder,
        OpenDynoLCD,
        OpenDynoCounter,
        OpenDynoLED,
        OpenDynoMeter,
        OpenDynoDial,
        OpenDynoDialGauge,
        OpenDynoBarGauge
    };

    //The parent should always be the ui screen widget
    static QWidget* CreateWidget(QWidget* parent, ControlType type , const QDomElement &control)
    {
        QWidget* widget =0;

        switch(type)
        {
            case OpenDynoLabel:
            {
                QLabel* temp = new QLabel(parent);
                temp->setText("New Label");
                widget = temp;
            }
                break;
            case OpenDynoPushButton:
            {
                QPushButton* temp = new QPushButton(parent);
                temp->setText("New Button");
                widget = temp;
            }
                break;
            case OpenDynoDialGauge:
            {
                widget = new ManoMeter(parent);
            }
                break;
            case OpenDynoTextBox:
            {
                widget = new QLineEdit(parent);
            }
                break;
            case OpenDynoSpinBox:
            {
                widget = new QDoubleSpinBox(parent);
            }
                break;
            case OpenDynoSlider:
            {
                QSlider* temp = new QSlider(parent);
                temp->setOrientation(Qt::Horizontal);
                widget = temp;
            }
                break;
            case OpenDynoThermometer:
            {
                widget = new ThermoMeter(parent);
            }
                break;
            case OpenDynoEncoder:
            {
                widget = new PotentioMeter(parent);
            }
                break;
            case OpenDynoLCD:
            {
                widget = new QLCDNumber(parent);
            }
                break;
            case OpenDynoLED:
            {
                widget = new QLedIndicator(parent);
            }
                break;
            case OpenDynoCounter:
            {
                widget = new Counter(parent);
            }
                break;
            case OpenDynoMeter:
            {
                widget = new QScale(parent);
            }
                break;
            case OpenDynoDial:
            {
                widget = new QDial(parent);
            }
                break;
            case Unknown:
            default:
                break;
        }

        if(widget != 0)
        {
            if(control.attributes().count() > 0)
            {
                widget->setObjectName(control.attribute("Name"));
                widget->move(QPointConverter::ConvertFromString(control.attribute("Location", "0,0")));
                widget->resize(QSizeConverter::ConvertFromString(control.attribute("Size", "50,50")));
                widget->setFont(QFontConverter::ConvertFromString(control.attribute("Font")));
                widget->show();
            }
        }
        return widget;
    }

    static ControlType GetTypeFromString(QString type )
    {
        ControlType retType = Unknown;

        if(type == "QLabel") retType = OpenDynoLabel;
        if(type == "QPushButton") retType = OpenDynoPushButton;
        if(type == "ManoMeter") retType = OpenDynoDialGauge;
        if(type == "QLineEdit") retType = OpenDynoTextBox;
        if(type == "QDoubleSpinBox") retType = OpenDynoSpinBox;
        if(type == "QSlider") retType = OpenDynoSlider;
        if(type == "ThermoMeter") retType = OpenDynoThermometer;
        if(type == "PotentioMeter") retType = OpenDynoEncoder;
        if(type == "QLCDNumber") retType = OpenDynoLCD;
        if(type == "QLedIndicator") retType = OpenDynoLED;
        if(type == "Counter") retType = OpenDynoCounter;
        if(type == "QDial") retType = OpenDynoDial;
        if(type == "QScale") retType = OpenDynoMeter;

        return retType;
    }

};

#endif // CONTROLFACTORY_H
