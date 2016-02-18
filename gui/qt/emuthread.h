#ifndef EMUTHREAD_H
#define EMUTHREAD_H

#include <QtCore/QThread>

#include <chrono>

#include "../../core/asic.h"
#include "../../core/debug/debug.h"

class EmuThread : public QThread {
    Q_OBJECT
public:
    explicit EmuThread(QObject *p = 0);

    void doStuff();
    void throttleTimerWait();

    std::string rom;
    volatile bool waitForLink;

signals:
    // Debugger
    void debuggerEntered();
    void sendDebugCommand(int, uint32_t);
    void debugInputRequested(bool);

    // I/O
    void consoleChar(char);
    void consoleStr(QString);
    void exited(int);

    // Status
    void actualSpeedChanged(int);
    void isBusy(bool busy);

    // Save/Restore state
    void saved(bool);
    void restored(bool);

    // Stopped/Started
    void started(bool);
    void stopped();

public slots:
    virtual void run() override;
    bool stop();
    void resetTriggered();

    // Debugging
    void setDebugMode(bool);
    void setDebugStepInMode();
    void setDebugStepOverMode();
    void setDebugStepNextMode();
    void setDebugStepOutMode();
    void debuggerInput(QString);

    // Linking
    void setSendState(bool);
    void setReceiveState(bool);

    // Speed
    void changeEmuSpeed(int);
    void changeThrottleMode(bool);

    // Save/Restore
    bool restore(QString);
    void save(QString);
    void saveRomImage(QString);

private:
    void setActualSpeed(int);

    int speed, actualSpeed;
    bool enterDebugger = false;
    bool enterSendState = false;
    bool enterReceiveState = false;
    bool throttleOn = true;
    std::chrono::steady_clock::time_point lastTime;
    std::string debugInput,imagePath,exportRomPath;
    volatile bool saveImage = false;
    volatile bool saveRom = false;
    volatile bool doRestore = false;
};

// For friends
extern EmuThread *emu_thread;

#endif
