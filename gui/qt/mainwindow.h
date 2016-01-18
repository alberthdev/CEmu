#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QFileDialog>
#include <QtCore/QSettings>
#include <QTextCursor>

#include "lcdwidget.h"
#include "romselection.h"
#include "emuthread.h"
#include "../../core/vat.h"
#include "../../core/debug/debug.h"
#include "../../core/debug/disasm.h"
#include "qhexedit/qhexedit.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *p = 0);
    ~MainWindow();

public slots:
    // Misc.
    void closeEvent(QCloseEvent*) override;

    //Drag & Drop
    void dropEvent(QDropEvent*) override;
    void dragEnterEvent(QDragEnterEvent*) override;

    // Console
    void consoleStr(QString);

signals:
    // Debugging
    void debuggerChangedState(bool);
    void triggerEmuSendState();

    // Linking
    void setSendState(bool);
    void sendVariable(std::string);
    void setReceiveState(bool);
    void setDebugStepMode();
    void setDebugStepOverMode();
    void setDebugStepOutMode();

    // Speed
    void changedEmuSpeed(int);
    void changedThrottleMode(bool);

private:
    // Actions
    bool runSetup(void);
    void screenshot(void);
    void screenshotGIF(void);
    void recordGIF(void);
    void checkForUpdates(bool);
    void showAbout(void);
    void setUIMode(bool);

    // Debugger
    void raiseDebugger();
    void updateDebuggerChanges();
    void populateDebugWindow();
    void setDebuggerState(bool);
    void changeDebuggerState();
    void processDebugCommand(int, uint32_t);
    void portMonitorCheckboxToggled(QTableWidgetItem *);
    void pollPort();
    void deletePort();
    void updatePortData(int);
    void changePortData(QTableWidgetItem*);
    void deleteBreakpoint();
    void breakpointCheckboxToggled(QTableWidgetItem *);
    void drawNextDisassembleLine();
    void stepPressed();
    void stepOverPressed();
    void stepOutPressed();
    void updateStackView();
    void updateDisasmView(const int, const bool);
    void gotoPressed();
    void setBreakpointAddress();
    void disasmContextMenu(const QPoint &);
    bool addBreakpoint();

    // Others
    void resetCalculator();
    void addEquateFile();
    void clearEquateFile();

    // Speed
    void changeEmulatedSpeed(int);
    void changeThrottleMode(int);
    void showActualSpeed(int);

    // Console
    void clearConsole(void);
    void showStatusMsg(QString);

    // Settings
    void changeLCDRefresh(int);
    void alwaysOnTop(int);
    void autoCheckForUpdates(int);
    void popoutLCD();

    // Linking
    QStringList showVariableFileDialog(QFileDialog::AcceptMode mode);
    void sendFiles(QStringList fileNames);
    void selectFiles();
    void refreshVariableList();
    void saveSelected();

    // Hex Editor
    void flashUpdate();
    void flashGotoPressed();
    void flashSearchPressed();
    void flashSyncPressed();
    void ramUpdate();
    void ramGotoPressed();
    void ramSearchPressed();
    void ramSyncPressed();
    void memUpdate();
    void memGotoPressed();
    void memSearchPressed();
    void memSyncPressed();
    void syncHexView(int, QHexEdit *);
    void searchEdit(QHexEdit *);

    // Keypad
    void keymapChanged();
    void changeKeymap(const QString &);

    // Font
    void setFont(int);

    QLabel status_label;
    QString getAddressString(bool &, QString);

    Ui::MainWindow *ui = nullptr;
    QSettings *settings = nullptr;
    QDockWidget *dock_debugger = nullptr;
    QTextCursor disasm_offset;
    bool detached_state = false;
    bool disasm_offset_set;
    bool from_pane;
    int address_pane;
    int mem_hex_size;

    QDir current_dir;
    EmuThread emu;
    LCDWidget detached_lcd;

    bool debugger_on = false;
    bool in_recieving_mode = false;

    QList<calc_var_t> vars;
};

// Used as global instance by EmuThread and Debugger class
extern MainWindow *main_window;

#endif
