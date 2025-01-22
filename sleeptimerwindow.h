#ifndef SLEEPTIMERWINDOW_H
#define SLEEPTIMERWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QPushButton>
#include <QComboBox>

class SleepTimerWindow : public QDialog {
    Q_OBJECT

public:
    explicit SleepTimerWindow(QWidget* parent = nullptr);
    ~SleepTimerWindow();

    int getRemainingTime() const;

signals:
    void remainingTimeUpdated(int remainingSeconds); // Signal für verbleibende Zeit
    void timerExpired(); // Signal, wenn der Timer abläuft

private:
    QTimer* timer;           // Timer für den Sleeptimer
    QPushButton* button;     // Button zum Starten/Stoppen
    QComboBox* dropdown;     // Dropdown-Menü für Zeitwahl
    int remainingTime;       // Verbleibende Zeit in Millisekunden
    bool isTimerActive;      // Status des Timers

    int dropdownToMilliseconds(const QString& text) const;
    void stopTimer(bool expired);
};

#endif // SLEEPTIMERWINDOW_H
