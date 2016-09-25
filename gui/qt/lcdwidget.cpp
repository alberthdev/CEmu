#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QMenu>
#include <QDrag>
#include <QApplication>

#include "lcdwidget.h"
#include "qtframebuffer.h"
#include "sendinghandler.h"
#include "../../core/lcd.h"
#include "../../core/link.h"

LCDWidget::LCDWidget(QWidget *p) : QWidget(p) {
    lcdState = &lcd;
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(repaint()));

    setAcceptDrops(true);
    refreshRate(60);    // Default rate is 60 FPS
}

LCDWidget::~LCDWidget(){}

void LCDWidget::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    paintFramebuffer(&painter, lcdState);
    if(in_drag) {
        QRect left = painter.window();
        QRect right = painter.window();
        left.setRight(left.right()/2);
        right.setLeft(left.right());
        painter.fillRect(left, QColor(200, 0, 0, 128));
        painter.fillRect(right, QColor(0, 200, 0, 128));
        painter.setPen(Qt::white);
        painter.drawText(painter.window(), Qt::AlignCenter, QObject::tr("Archive     RAM     "));
    }
}

void LCDWidget::refreshRate(int newrate) {
    refreshTimer.stop();
    refreshTimer.setInterval(1000 / newrate);
    refreshTimer.start();
}

void LCDWidget::setLCD(lcd_state_t *lcdS) {
    lcdState = lcdS;
}

void LCDWidget::dropEvent(QDropEvent *e) {
    in_drag = false;
    sending_handler.dropOccured(e, (e->pos().x() < this->width()/2) ? LINK_ARCH : LINK_RAM);
}

void LCDWidget::dragEnterEvent(QDragEnterEvent *e) {
    in_drag = true;
    sending_handler.dragOccured(e);
}

void LCDWidget::dragLeaveEvent(QDragLeaveEvent *e) {
    in_drag = false;
}
