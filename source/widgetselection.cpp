/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "widgetselection.h"

#include <QtWidgets/QMenu>
#include <QtWidgets/QWidget>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QStylePainter>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QStyleOptionToolButton>
#include <QtWidgets/QApplication>

#include <QtCore/QVariant>
#include <QtCore/qdebug.h>

#include <algorithm>

QT_BEGIN_NAMESPACE

enum { debugWidgetSelection = 0 };


// ----------- WidgetHandle

WidgetHandle::WidgetHandle(QWidget *parent, WidgetHandle::Type t, WidgetSelection *s) :
    InvisibleWidget(parent),
    m_widget(0),
    m_type(t),
    m_formWindow( parent),
    m_sel(s),
    m_active(true)
{
    setMouseTracking(false);
    setAutoFillBackground(true);

    setBackgroundRole(m_active ? QPalette::Text : QPalette::Dark);

    setFixedSize(6, 6);

    updateCursor();
}

void WidgetHandle::updateCursor()
{
#ifndef QT_NO_CURSOR
    if (!m_active) {
        setCursor(Qt::ArrowCursor);
        return;
    }

    switch (m_type) {
    case LeftTop:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case Top:
        setCursor(Qt::SizeVerCursor);
        break;
    case RightTop:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case Right:
        setCursor(Qt::SizeHorCursor);
        break;
    case RightBottom:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case Bottom:
        setCursor(Qt::SizeVerCursor);
        break;
    case LeftBottom:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case Left:
        setCursor(Qt::SizeHorCursor);
        break;
    case Move:
        setCursor(Qt::OpenHandCursor);
        break;
    default:
        Q_ASSERT(0);
    }
#endif
}

void WidgetHandle::setActive(bool a)
{
    m_active = a;
    setBackgroundRole(m_active ? QPalette::Text : QPalette::Dark);
    updateCursor();
}

void WidgetHandle::setWidget(QWidget *w)
{
    m_widget = w;
}

void WidgetHandle::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    if (m_type != Move)
    {
        p.setPen( Qt::blue );
        p.drawRect(0, 0, width() - 1, height() - 1);
    }
    else
    {

        p.setBrush(QBrush(QColor(201,201,201)));
        p.setPen( Qt::blue );
        p.drawRect(0, 0,m_widget->width()/2 -1, m_widget->height()/2 -1);
    }
}

void WidgetHandle::mousePressEvent(QMouseEvent *e)
{
    e->accept();

    if (!(m_widget && e->button() == Qt::LeftButton))
        return;

    if (!(m_active))
        return;

    QWidget *container = m_widget->parentWidget();

    m_origPressPos = container->mapFromGlobal(e->globalPos());
    m_geom = m_origGeom = m_widget->geometry();
}

void WidgetHandle::mouseMoveEvent(QMouseEvent *e)
{
    if (!(m_widget && m_active && e->buttons() & Qt::LeftButton))
        return;

    e->accept();

    QWidget *container = m_widget->parentWidget();

    const QPoint rp = container->mapFromGlobal(e->globalPos());
    const QPoint d = rp - m_origPressPos;

    const QRect pr = container->rect();

    switch (m_type) {

    case LeftTop: {
        if (rp.x() > pr.width() - 2 * width() || rp.y() > pr.height() - 2 * height())
            return;

        int w = m_origGeom.width() - d.x();
        m_geom.setWidth(w);
      //  w = grid.widgetHandleAdjustX(w);

        int h = m_origGeom.height() - d.y();
        m_geom.setHeight(h);
     //   h = grid.widgetHandleAdjustY(h);

        const int dx = m_widget->width() - w;
        const int dy = m_widget->height() - h;

        trySetGeometry(m_widget, m_widget->x() + dx, m_widget->y() + dy, w, h);
    } break;

    case Top: {
        if (rp.y() > pr.height() - 2 * height())
            return;

        int h = m_origGeom.height() - d.y();
        m_geom.setHeight(h);
       // h = grid.widgetHandleAdjustY(h);

        const int dy = m_widget->height() - h;
        trySetGeometry(m_widget, m_widget->x(), m_widget->y() + dy, m_widget->width(), h);
    } break;

    case RightTop: {
        if (rp.x() < 2 * width() || rp.y() > pr.height() - 2 * height())
            return;

        int h = m_origGeom.height() - d.y();
        m_geom.setHeight(h);
      //  h = grid.widgetHandleAdjustY(h);

        const int dy = m_widget->height() - h;

        int w = m_origGeom.width() + d.x();
        m_geom.setWidth(w);
       // w = grid.widgetHandleAdjustX(w);

        trySetGeometry(m_widget, m_widget->x(), m_widget->y() + dy, w, h);
    } break;

    case Right: {
        if (rp.x() < 2 * width())
            return;

        int w = m_origGeom.width() + d.x();
        m_geom.setWidth(w);
       // w = grid.widgetHandleAdjustX(w);

        tryResize(m_widget, w, m_widget->height());
    } break;

    case RightBottom: {
        if (rp.x() < 2 * width() || rp.y() < 2 * height())
            return;

        int w = m_origGeom.width() + d.x();
        m_geom.setWidth(w);
       // w = grid.widgetHandleAdjustX(w);

        int h = m_origGeom.height() + d.y();
        m_geom.setHeight(h);
       // h = grid.widgetHandleAdjustY(h);

        tryResize(m_widget, w, h);
    } break;

    case Bottom: {
        if (rp.y() < 2 * height())
            return;

        int h = m_origGeom.height() + d.y();
        m_geom.setHeight(h);
       // h = grid.widgetHandleAdjustY(h);

        tryResize(m_widget, m_widget->width(), h);
    } break;

    case LeftBottom: {
        if (rp.x() > pr.width() - 2 * width() || rp.y() < 2 * height())
            return;

        int w = m_origGeom.width() - d.x();
        m_geom.setWidth(w);
       // w = grid.widgetHandleAdjustX(w);

        int h = m_origGeom.height() + d.y();
        m_geom.setHeight(h);
       // h = grid.widgetHandleAdjustY(h);

        int dx = m_widget->width() - w;

        trySetGeometry(m_widget, m_widget->x() + dx, m_widget->y(), w, h);
    } break;

    case Left: {
        if (rp.x() > pr.width() - 2 * width())
            return;

        int w = m_origGeom.width() - d.x();
        m_geom.setWidth(w);
        //w = grid.widgetHandleAdjustX(w);

        const int dx = m_widget->width() - w;

        trySetGeometry(m_widget, m_widget->x() + dx, m_widget->y(), w, m_widget->height());
    } break;

    case Move: {
        int xPos = rp.x() - ( m_origPressPos.x() -m_origGeom.x() );
        int yPos = rp.y() - ( m_origPressPos.y() -m_origGeom.y() );
        trySetGeometry(m_widget, xPos , yPos , m_widget->width(), m_widget->height());
    } break;

    default: break;

    } // end switch

    m_sel->updateGeometry();

    //m_formWindow->updateChildSelections(m_widget);
}

void WidgetHandle::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton || !m_active)
        return;

    e->accept();

}

void WidgetHandle::trySetGeometry(QWidget *w, int x, int y, int width, int height)
{
    int minw = w->minimumSize().width();
    minw = qMax(minw, 16);

    int minh = w->minimumSize().height();
    minh = qMax(minh, 16);

    if (qMax(minw, width) > w->maximumWidth() ||
         qMax(minh, height) > w->maximumHeight())
        return;

    if (width < minw && x != w->x())
        x -= minw - width;

    if (height < minh && y != w->y())
        y -= minh - height;

    w->setGeometry(x, y, qMax(minw, width), qMax(minh, height));
}

void WidgetHandle::tryResize(QWidget *w, int width, int height)
{
    int minw = w->minimumSize().width();
    minw = qMax(minw, 16);

    int minh = w->minimumSize().height();
    minh = qMax(minh, 16);

    w->resize(qMax(minw, width), qMax(minh, height));
}

// ------------------ WidgetSelection

WidgetSelection::WidgetSelection(QWidget *parent)   :
    m_widget(0),
    m_formWindow(parent)
{
    for (int i = WidgetHandle::LeftTop; i < WidgetHandle::TypeCount; ++i)
        m_handles[i] = new WidgetHandle(m_formWindow, static_cast<WidgetHandle::Type>(i), this);
    hide();
    dragActive = false;
}

WidgetSelection::~WidgetSelection()
{
    for (int i = WidgetHandle::LeftTop; i < WidgetHandle::TypeCount; ++i)
    {
        delete m_handles[i];
        m_handles[i]=0;
    }
}

void WidgetSelection::setWidget(QWidget *w)
{
    if (m_widget != 0)
        m_widget->removeEventFilter(this);

    if (w == 0) {
        hide();
        m_widget = 0;
        return;
    }

    m_widget = w;

    m_widget->installEventFilter(this);

    updateActive();

    updateGeometry();
    show();
}

void WidgetSelection::updateActive()
{

    for (int i = WidgetHandle::LeftTop; i < WidgetHandle::TypeCount; ++i)
        if (WidgetHandle *h = m_handles[i]) {
            h->setWidget(m_widget);
          //  h->setActive(active[i]);
        }
}

bool WidgetSelection::isUsed() const
{
    return m_widget != 0;
}

void WidgetSelection::updateGeometry()
{
    if (!m_widget || !m_widget->parentWidget())
        return;

    QPoint p = m_widget->parentWidget()->mapToGlobal(m_widget->pos());
    p = m_formWindow->mapFromGlobal(p);
    const QRect r(p, m_widget->size());

    const int w = 6;
    const int h = 6;

    for (int i = WidgetHandle::LeftTop; i < WidgetHandle::TypeCount; ++i) {
        WidgetHandle *hndl = m_handles[ i ];
        if (!hndl)
            continue;
        switch (i) {
        case WidgetHandle::LeftTop:
            hndl->move(r.x() - w / 2, r.y() - h / 2);
            break;
        case WidgetHandle::Top:
            hndl->move(r.x() + r.width() / 2 - w / 2, r.y() - h / 2);
            break;
        case WidgetHandle::RightTop:
            hndl->move(r.x() + r.width() - w / 2, r.y() - h / 2);
            break;
        case WidgetHandle::Right:
            hndl->move(r.x() + r.width() - w / 2, r.y() + r.height() / 2 - h / 2);
            break;
        case WidgetHandle::RightBottom:
            hndl->move(r.x() + r.width() - w / 2, r.y() + r.height() - h / 2);
            break;
        case WidgetHandle::Bottom:
            hndl->move(r.x() + r.width() / 2 - w / 2, r.y() + r.height() - h / 2);
            break;
        case WidgetHandle::LeftBottom:
            hndl->move(r.x() - w / 2, r.y() + r.height() - h / 2);
            break;
        case WidgetHandle::Left:
            hndl->move(r.x() - w / 2, r.y() + r.height() / 2 - h / 2);
            break;
        case WidgetHandle::Move:
            hndl->setFixedSize(m_widget->width() /2 , m_widget->height() /2);
            hndl->move(r.x()+  m_widget->width() /4   , r.y() + m_widget->height() /4);
            break;
        default:
            break;
        }
    }
}

void WidgetSelection::hide()
{
    for (int i = WidgetHandle::LeftTop; i < WidgetHandle::TypeCount; ++i) {
        WidgetHandle *h = m_handles[ i ];
        if (h)
            h->hide();
    }
}

void WidgetSelection::show()
{
    for (int i = WidgetHandle::LeftTop; i < WidgetHandle::TypeCount; ++i) {
        WidgetHandle *h = m_handles[ i ];
        if (h) {
            h->show();
            h->raise();
        }
    }
}

void WidgetSelection::update()
{
    for (int i = WidgetHandle::LeftTop; i < WidgetHandle::TypeCount; ++i) {
        WidgetHandle *h = m_handles[ i ];
        if (h)
            h->update();
    }
}

QWidget *WidgetSelection::widget() const
{
    return m_widget;
}


bool WidgetSelection::eventFilter(QObject *object, QEvent *event)
{
    bool retVal = false;
    if (object != widget())
        return false;

    switch (event->type()) {
        default: break;


        case QEvent::MouseButtonPress:
             updateGeometry();
             dragActive = true;
             qDebug() << "dragActive =" << dragActive;
            // retVal = true;
            break;
        case QEvent::MouseButtonRelease:
             updateGeometry();
             dragActive = false;
             qDebug() << "dragActive =" << dragActive;
            // retVal = true;
            break;
        case QEvent::Move:
        case QEvent::Resize:
        qDebug() << "moving";
            updateGeometry();
            break;
        case QEvent::ZOrderChange:
            show();
            break;
    } // end switch

    return retVal;
}


QT_END_NAMESPACE
