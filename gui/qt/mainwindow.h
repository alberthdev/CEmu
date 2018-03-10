#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ipc.h"
#include "searchwidget.h"
#include "cemuopts.h"
#include "lcdwidget.h"
#include "romselection.h"
#include "emuthread.h"
#include "keyhistory.h"
#include "dockwidget.h"
#include "keypad/qtkeypadbridge.h"
#include "debugger/hexwidget.h"
#include "png.h"
#include "../../core/vat.h"
#include "../../core/debug/debug.h"
#include "../../core/debug/disasm.h"

#include <QtWidgets/QProgressBar>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QFileDialog>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include <QtGui/QTextCursor>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTranslator>
#include <QtCore/QStandardPaths>

#ifdef PNG_WRITE_APNG_SUPPORTED
class RecordingThread : public QThread {
    Q_OBJECT
protected:
    virtual void run() Q_DECL_OVERRIDE;
public:
    QString m_filename;
    bool m_optimize;
signals:
    void done();
};
#endif

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(CEmuOpts &opts, QWidget *p = Q_NULLPTR);
    ~MainWindow();
    void setup();
    bool isInitialized();
    bool isReload();
    bool isResetAll();

public slots:
    // Console
    void console(const QString &str, const QColor &colorFg = Qt::black, const QColor &colorBg = Qt::white, int type = EmuThread::ConsoleNorm);
    void console(int type, const char *str, int size = -1);
    void consoleStr(int type);
    void consoleSubmission();

    // Saved/Restored State
    void savedEmu(bool success);

    // ROM Image setting
    void setRom(const QString &name);

    // Other
    void saveEmu();
    void restoreFromFile();
    void saveToFile();
    void exportRom();
    void exportWindowConfig();
    void importWindowConfig();
    void setImagePath();
#ifdef PNG_WRITE_APNG_SUPPORTED
    void updateAnimatedControls();
#endif

    // Debugging
    void debuggerGUIDisable();
    void debuggerGUIEnable();

    // Sending keys
    void sendASMKey();

    // LCD Popouts
    void newMemoryVisualizer();

private slots:
    void emuTimerSlot();
    void fpsTimerSlot();

signals:
    // LCD
    void updateFrameskip(int value);
    void updateMode(bool state);

protected:
    // Misc.
    virtual void changeEvent(QEvent* event) Q_DECL_OVERRIDE;
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;
    virtual void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    // Drag & Drop
    virtual void dropEvent(QDropEvent*) Q_DECL_OVERRIDE;
    virtual void dragEnterEvent(QDragEnterEvent*) Q_DECL_OVERRIDE;

private:
    enum consoleColors {
        CONSOLE_ESC,
        CONSOLE_BRACKET,
        CONSOLE_PARSE,
        CONSOLE_BGCOLOR,
        CONSOLE_FGCOLOR,
        CONSOLE_EQUALS,
        CONSOLE_ENDVAL
    };

    enum breakpointIndex {
        BREAK_LABEL_LOC=0,
        BREAK_ADDR_LOC,
        BREAK_ENABLE_LOC,
        BREAK_REMOVE_LOC
    };

    enum watchpointIndex {
        WATCH_LABEL_LOC=0,
        WATCH_ADDR_LOC,
        WATCH_SIZE_LOC,
        WATCH_VALUE_LOC,
        WATCH_READ_LOC,
        WATCH_WRITE_LOC,
        WATCH_REMOVE_LOC
    };

    enum portIndex {
        PORT_ADDR_LOC=0,
        PORT_VALUE_LOC,
        PORT_READ_LOC,
        PORT_WRITE_LOC,
        PORT_FREEZE_LOC,
        PORT_REMOVE_LOC
    };

    enum opIndex {
        OP_ADDRESS=0,
        OP_NUMBER,
        OP_DATA,
        OP_DATASTRING
    };

    enum vatIndex {
        VAT_ADDRESS=0,
        VAT_VAT_ADDRESS,
        VAT_SIZE,
        VAT_NAME,
        VAT_TYPE
    };

    enum varIndex {
        VAR_NAME=0,
        VAR_LOCATION,
        VAR_TYPE,
        VAR_SIZE,
        VAR_PREVIEW
    };

    enum recentIndex {
        RECENT_SELECT=0,
        RECENT_LOAD,
        RECENT_PATH,
    };

    enum slotIndex {
        SLOT_NAME=0,
        SLOT_LOAD,
        SLOT_SAVE,
        SLOT_EDIT,
        SLOT_REMOVE
    };

    enum memIndex {
        MEM_FLASH=0,
        MEM_RAM,
        MEM_MEM
    };

    // Language
    enum {
        TRANSLATE_INIT,
        TRANSLATE_UPDATE,
        TRANSLATE_ONLY
    };

    // Fullscreen modes
    enum {
        FULLSCREEN_NONE,
        FULLSCREEN_ALL,
        FULLSCREEN_LCD
    };

    void translateExtras(int init);
    void debuggerStep(int mode);

    // Save/Restore
    void saveToPath(const QString &path);
    bool restoreFromPath(const QString &path);

    // Actions
    void switchTranslator(const QString &lang);
    bool runSetup();
    void screenshot();
    void screenshotSave(const QString &nameFilter, const QString &defaultSuffix, const QString &temppath);
#ifdef PNG_WRITE_APNG_SUPPORTED
    void recordAPNG();
    void saveAnimated(QString &filename);
#endif
    void setFrameskip(int value);
    void setOptimizeRecording(bool state);
    void checkForUpdates(bool forceInfoBox);
    void showAbout();
    void batteryIsCharging(bool checked);
    void batteryChangeStatus(int value);
    void setPortableConfig(bool state);
    void setAutoSaveState(bool state);
    void changeSnapshotPath();

    // Debugger
    void breakpointGUIAdd();
    void watchpointGUIAdd();
    void debuggerGUIPopulate();
    void debuggerGUISetState(bool state);

    void debuggerInstall();
    void debuggerRaise();
    void debuggerLeave();
    void debuggerUpdateChanges();
    void debuggerChangeState();
    void debuggerExecuteCommand(uint32_t debugAddress, uint8_t command);
    void debuggerProcessCommand(int reason, uint32_t input);

    void portRemoveSelected();
    void portRemoveRow(int row);

    void portUpdate(int currRow);
    void watchpointUpdate(int row);

    void portSetPreviousAddress(QTableWidgetItem *item);
    void breakpointSetPreviousAddress(QTableWidgetItem *item);
    void watchpointSetPreviousAddress(QTableWidgetItem *item);

    void portDataChanged(QTableWidgetItem *item);
    void breakpointDataChanged(QTableWidgetItem *item);
    void watchpointDataChanged(QTableWidgetItem *item);

    void updateDisasm();
    void updateDisasmAddr(int sentBase, bool newPane);
    void drawNextDisasmLine();
    void drawNextStackLine();
    void scrollDisasm(int value);
    void scrollStack(int value);
    bool getAdlState(int state);

    void stepIn();
    void stepOver();
    void stepNext();
    void stepOut();

    void updateTIOSView();
    void updateStack();

    void gotoPressed();
    void slotAddNew();
    void slotAdd(QString &name, QString &path);
    void slotRemove();
    void slotEdit();
    void slotSave();
    void slotLoad();
    void saveSlotInfo();
    void setSlotInfo();
    int slotGet(QObject *obj, int col);

    void consoleContextMenu(const QPoint &posa);
    void disasmContextMenu(const QPoint &posa);
    void variablesContextMenu(const QPoint &posa);
    void vatContextMenu(const QPoint &posa);
    void opContextMenu(const QPoint &posa);
    void memContextMenu(const QPoint &posa);
    void memoryContextMenu(const QPoint &posa, uint32_t addr);
    void removeAllSentVars();
    void removeSentVars();
    void deselectAllVars();
    void selectAllVars();
    void resendContextMenu(const QPoint &);

    void setDebugIgnoreBreakpoints(bool state);
    void setDebugResetTrigger(bool state);
    void setDebugSoftCommands(bool state);
    void setFocusSetting(bool state);

    void breakpointRemoveAddress(uint32_t address);
    void watchpointRemoveAddress(uint32_t address);

    void debuggerZeroClockCounter();
    void gotoDisasmAddr(uint32_t address);
    void gotoMemAddr(uint32_t address);

    void setDataCol(bool state);
    void setMenuBarState(bool state);
    void setDockBoundaries(bool state);

    void handleCtrlClickText(QPlainTextEdit *edit);
    void handleCtrlClickLine(QLineEdit *edit);

    void forceEnterDebug();

    // For linking to the buttons
    void breakpointSlotAdd();
    void watchpointSlotAdd();
    void portSlotAdd();

    // Removal from widgets
    void breakpointRemoveSelected();
    void watchpointRemoveSelected();
    void breakpointRemoveRow(int row);
    void watchpointRemoveRow(int row);

    // Get labels
    QString watchpointNextLabel();
    QString breakpointNextLabel();

    // Adding watchpoints from disassembly
    void watchpointReadGUIAdd();
    void watchpointWriteGUIAdd();
    void watchpointReadWriteGUIAdd();

    // Debugging files
    void debuggerImportFile(const QString &filename);
    void debuggerExportFile(const QString &filename);
    QString debuggerGetFile(bool save);
    void debuggerImport();
    void debuggerExport();
    void setAutoEquates(bool enable);

    // Create memory views
    void addMemoryDock(const QString &title, int bytes, bool ascii);

    // Creating bootable images
    bool checkForCEmuBootImage();
    void exportCEmuBootImage();
    bool loadCEmuBootImage(const QString &bootImagePath);
    void resetSettingsIfLoadedCEmuBootableImage();

    // MAIN IMPLEMENTATION ROUTINES
    bool portAdd(uint16_t port, unsigned int mask);
    bool breakpointAdd(const QString &label, uint32_t address, bool enabled, bool toggle);
    bool watchpointAdd(const QString &label, uint32_t address, uint8_t len, unsigned int mask, bool toggle);

    void screenContextMenu(const QPoint &posa);
    void updateLabels();
    void equatesAddDialog();
    void equatesAddFile(const QString &fileName);
    void equatesAddEquate(const QString &name, uint32_t address);
    void equatesClear();
    void equatesRefresh();
    void selectKeypadColor();
    void setKeypadColor(unsigned int color);
    void setCalcSkinTopFromType();
    void changeCalcID();

    // Speed
    void setEmuSpeed(int value);
    void setThrottle(int mode);
    void showEmuSpeed(int speed);
    void showFpsSpeed(double emuFps, double guiFps);

    // Console
    void showStatusMsg(const QString &str);
    void consoleOutputChanged();

    // Settings
    void adjustScreen();
    void setDebugPath();
    void setSkinToggle(bool enable);
    void setLcdScale(int state);
    void setGuiSkip(int value);
    void setLcdSpi(bool state);
    void setLcdDma(bool state);
    void setAlwaysOnTop(int state);
    void setAutoCheckForUpdates(int state);
    void setSpaceDisasm(bool state);
    void setUIDocks(bool firstRun);
    void updateDocks();
    void toggleUIEditMode();
    void toggleFullscreen();
    void setSaveDebug(bool state);
    void saveMiscSettings();
    void setPreRevisionI(bool state);
    void saveSettings();

    // Linking
    QStringList showVariableFileDialog(QFileDialog::AcceptMode mode, const QString &name_filter, const QString &defaultSuffix);
    void selectFiles();
    void lockedEmu(int req);
    void changeVariableState();
    void variableDoubleClicked(QTableWidgetItem *item);
    void launchPrgm(const calc_var_t *prgm);
    void saveSelectedFile();
    void saveSelectedFiles();
    void resendFiles();
    void receiveChangeState();

    // Recent Files
    void setRecentInfo();
    void saveRecentInfo();
    void setRecentSave(bool state);

    // Autotester
    void dispAutotesterError(int errCode);
    int openJSONConfig(const QString &jsonPath);
    void prepareAndOpenJSONConfig();
    void reloadJSONConfig();
    void launchTest();
    void updateCRCParamsFromPreset(int comboBoxIdx);
    void refreshCRC();

    // Hex Editor
    void flashUpdate();
    void flashGotoPressed();
    void flashSyncPressed();
    void ramUpdate();
    void ramGotoPressed();
    void ramSyncPressed();
    void updateMemoryViews();
    void memUpdateEdit(HexWidget *edit, bool force = false);
    void memGotoEdit(HexWidget *edit);
    void memGoto(HexWidget *edit, uint32_t address);
    void memSearchEdit(HexWidget *edit);
    void memSyncEdit(HexWidget *edit);
    void memAsciiToggle(HexWidget *edit);
    void memDocksUpdate();

    // Others
    void syncHexWidget(HexWidget *edit);
    void setStatusInterval(int value);

    // Keypad
    void keymapChanged();
    void setKeymap(const QString &value);

    // Font
    void setFont(int fontSize);

    // Reset
    int loadEmu(bool image);
    void resetCalculator();

    // Versioning
    void setVersion();
    void checkVersion();
    bool isFirstRun();

    // Init
    void setUIEditMode(bool mode);
    void optSend(CEmuOpts &o);

    // Misc
    QString getAddressString(const QString &string, bool *ok);
    void optLoadFiles(CEmuOpts &o);
    void optAttemptLoad(CEmuOpts &o);
    void pauseEmu(Qt::ApplicationState state);
    void setMemoryDocks();
    void setMemoryState();

    // LCD
    void updateLcd(double emuFps);

    // State items
    void resetCEmu();
    void resetGui();

    // Key History
    void toggleKeyHistory();
    void closedKeyHistory();

    // Clipboard
    void saveScreenToClipboard();

    // IPC
    bool ipcSetup();
    void ipcSpawnRandom();
    void ipcCloseOthers();
    void ipcReceived();
    void ipcChangeID();
    void ipcHandleCommandlineReceive(QDataStream &stream);

#ifdef _WIN32
    // Win32 Console Toggle
    void toggleConsole();
    void installToggleConsole();
#endif

    // Redistribute Docks
    DockWidget *redistributeFindDock(const QPoint &pos);
    bool redistributeDocks(const QPoint &pos, const QPoint &offset,
                           Qt::CursorShape cursorShape,
                           int (QSize::*dimension)() const,
                           Qt::Orientation orientation);

    // Members
    Ui::MainWindow *ui = Q_NULLPTR;
    EmuThread emu;
    CEmuOpts opts;
    InterCom com;

    unsigned int m_watchGUIMask = DBG_MASK_NONE;

    QTranslator m_appTranslator;
    QLabel m_speedLabel;
    QLabel m_fpsLabel;
    QLabel m_msgLabel;
    QTextCursor m_disasmOffset;
    bool m_disasmOffsetSet;
    bool m_fromPane;
    int32_t m_addressPane;
    uint32_t m_stackAddr;

    QString m_searchStr;
    int m_searchMode = SEARCH_MODE_HEX;

    QDir m_dir;
    QStringList m_equateFiles;

    bool m_uiEditMode;
    bool m_portable = false;
    bool m_nativeConsole = false;
    bool m_shutdown = false;
    bool m_recording = false;

    uint32_t m_prevBreakAddr = 0;
    uint32_t m_prevWatchAddr = 0;
    uint32_t m_prevDisasmAddr = 0;
    uint16_t m_prevPortAddr = 0;
    QPalette m_cBack, m_cNone;

    QShortcut *m_shortcutStepIn;
    QShortcut *m_shortcutStepOver;
    QShortcut *m_shortcutStepNext;
    QShortcut *m_shortcutStepOut;
    QShortcut *m_shortcutDebug;
    QShortcut *m_shortcutFullscreen;
    QShortcut *m_shortcutAsm;
    QShortcut *m_shortcutResend;

    QAction *m_actionToggleUI;
    QAction *m_actionAddMemory;

    QIcon m_iconRun, m_iconStop;
    QIcon m_iconSave, m_iconLoad;
    QIcon m_iconEdit, m_iconRemove;
    QIcon m_iconSearch, m_iconGoto;
    QIcon m_iconSync, m_iconAddMem;
    QIcon m_iconAscii, m_iconUiEdit;
    QTextCharFormat m_consoleFormat;

    QString m_gotoAddr;
    QString m_flashGotoAddr;
    QString m_RamGotoAddr;
    QString m_memGotoAddr;

    QString m_settingsPath;
    QMenu *m_menuDocks;
    QMenu *m_menuDebug;

    KeyHistory *m_windowKeys = Q_NULLPTR;

    bool m_isSendingRom = false;
    QString m_dragRom;

    bool m_needReload = false;
    bool m_needFullReset = false;
    bool m_windowLoading = false;
    bool m_keepSetup = false;
    bool m_guiAdd = false;
    bool m_initPassed = true;
    bool m_windowVisible = false;
    bool m_useDataCol;
    bool m_useSoftCom = false;
    bool m_pauseOnFocus;
    bool m_loadedBootImage = false;
    bool m_optimizeRecording;
    bool m_activatedPortable = false;
    bool m_ignoreDmaCycles;
    int m_fullscreen = FULLSCREEN_NONE;

    QProgressBar *m_progressBar;
    QStringList m_docksMemory;
    QList<int> m_docksMemoryBytes;
    QList<bool> m_docksMemoryAscii;
    QSettings *m_settings = Q_NULLPTR;
    HexWidget *m_memoryWidget = Q_NULLPTR;

    QString m_pathRom;
    QString m_pathImage;
    QTimer m_timerEmu;
    QTimer m_timerFps;
    bool m_timerEmuTriggerable = true;
    bool m_timerFpsTriggerable = true;
    bool m_timerFpsTriggered = false;

    static const char *m_varExtensions[];

    // Settings definitions
    static const QString SETTING_DEBUGGER_TEXT_SIZE;
    static const QString SETTING_DEBUGGER_ADD_DISASM_SPACE;
    static const QString SETTING_DEBUGGER_RESTORE_ON_OPEN;
    static const QString SETTING_DEBUGGER_SAVE_ON_CLOSE;
    static const QString SETTING_DEBUGGER_RESET_OPENS;
    static const QString SETTING_DEBUGGER_ENABLE_SOFT;
    static const QString SETTING_DEBUGGER_DATA_COL;
    static const QString SETTING_DEBUGGER_IMAGE_PATH;
    static const QString SETTING_DEBUGGER_FLASH_BYTES;
    static const QString SETTING_DEBUGGER_RAM_BYTES;
    static const QString SETTING_DEBUGGER_FLASH_ASCII;
    static const QString SETTING_DEBUGGER_RAM_ASCII;
    static const QString SETTING_DEBUGGER_BREAK_IGNORE;
    static const QString SETTING_DEBUGGER_IGNORE_DMA;
    static const QString SETTING_DEBUGGER_AUTO_EQUATES;
    static const QString SETTING_DEBUGGER_PRE_I;
    static const QString SETTING_SCREEN_FRAMESKIP;
    static const QString SETTING_SCREEN_SCALE;
    static const QString SETTING_SCREEN_SKIN;
    static const QString SETTING_SCREEN_SPI;
    static const QString SETTING_KEYPAD_KEYMAP;
    static const QString SETTING_KEYPAD_COLOR;
    static const QString SETTING_WINDOW_STATE;
    static const QString SETTING_WINDOW_GEOMETRY;
    static const QString SETTING_WINDOW_SEPARATOR;
    static const QString SETTING_WINDOW_MENUBAR;
    static const QString SETTING_WINDOW_MEMORY_DOCKS;
    static const QString SETTING_WINDOW_MEMORY_DOCK_BYTES;
    static const QString SETTING_WINDOW_MEMORY_DOCK_ASCII;
    static const QString SETTING_CAPTURE_FRAMESKIP;
    static const QString SETTING_CAPTURE_OPTIMIZE;
    static const QString SETTING_SLOT_NAMES;
    static const QString SETTING_SLOT_PATHS;
    static const QString SETTING_IMAGE_PATH;
    static const QString SETTING_ROM_PATH;
    static const QString SETTING_STATUS_INTERVAL;
    static const QString SETTING_FIRST_RUN;
    static const QString SETTING_UI_EDIT_MODE;
    static const QString SETTING_PAUSE_FOCUS;
    static const QString SETTING_SAVE_ON_CLOSE;
    static const QString SETTING_RESTORE_ON_OPEN;
    static const QString SETTING_EMUSPEED;
    static const QString SETTING_AUTOUPDATE;
    static const QString SETTING_ALWAYS_ON_TOP;
    static const QString SETTING_CURRENT_DIR;
    static const QString SETTING_ENABLE_WIN_CONSOLE;
    static const QString SETTING_RECENT_SAVE;
    static const QString SETTING_RECENT_PATHS;
    static const QString SETTING_RECENT_SELECT;

    static const QString SETTING_KEYPAD_CEMU;
    static const QString SETTING_KEYPAD_TILEM;
    static const QString SETTING_KEYPAD_WABBITEMU;
    static const QString SETTING_KEYPAD_JSTIFIED;

    static const QString SETTING_PREFERRED_LANG;
    static const QString SETTING_VERSION;

    static const QString SETTING_DEFAULT_FILE;
    static const QString SETTING_DEFAULT_ROM_FILE;
    static const QString SETTING_DEFAULT_IMAGE_FILE;
    static const QString SETTING_DEFAULT_DEBUG_FILE;
    static const QString TXT_YES;
    static const QString TXT_NO;

    QString TITLE_DEBUG;
    QString TITLE_DOCKS;

    QString TXT_MEM_DOCK;

    QString TXT_CONSOLE;
    QString TXT_SETTINGS;
    QString TXT_VARIABLES;
    QString TXT_CAPTURE;
    QString TXT_STATE;
    QString TXT_KEYPAD;

    QString TXT_DEBUG_CONTROL;
    QString TXT_CPU_STATUS;
    QString TXT_DISASSEMBLY;
    QString TXT_MEMORY;
    QString TXT_TIMERS;
    QString TXT_BREAK_WATCH;
    QString TXT_OS_VIEW;
    QString TXT_MISC;
    QString TXT_AUTOTESTER;

    QString MSG_INFORMATION;
    QString MSG_WARNING;
    QString MSG_ERROR;
    QString MSG_ADD_MEMORY;
    QString MSG_EDIT_UI;

#ifdef _WIN32
    QAction *actionToggleConsole;
    QString TXT_TOGGLE_CONSOLE;
#endif
};

#endif
