#ifndef QMLBRIDGE_H
#define QMLBRIDGE_H

#include <QtCore/QObject>
#include <QtQml/QtQml>

#include "emuthread.h"

class QMLBridge : public QObject
{
    Q_OBJECT
public:
    explicit QMLBridge(QObject *parent = 0);
    ~QMLBridge();

    Q_INVOKABLE void keypadStateChanged(unsigned int keymap_id, bool state);
    Q_INVOKABLE void registerNButton(unsigned int keymap_id, QVariant button);

private:
    QSettings settings;
};

void notifyKeypadStateChanged(unsigned int row, unsigned int col, bool state);
QObject *qmlBridgeFactory(QQmlEngine *engine, QJSEngine *scriptEngine);


#endif
