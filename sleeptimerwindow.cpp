#include "sleeptimerwindow.h"
#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

SleepTimerWindow::SleepTimerWindow(QWidget* parent)
    : QDialog(parent), timer(new QTimer(this)), remainingTime(0), isTimerActive(false) {
    setWindowTitle("Sleep Timer");
    resize(300, 30);

    dropdown = new QComboBox(this);
    dropdown->addItems({"3 Sekunden", "5 Minuten", "10 Minuten", "15 Minuten", "30 Minuten", "1 Stunde"});
    dropdown->setFixedHeight(30);
    dropdown->setFocusPolicy(Qt::NoFocus);

    button = new QPushButton("Starten", this);
    button->setFixedHeight(30);
    button->setFixedWidth(80);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    mainLayout->addWidget(dropdown, 6); // Stretch-Faktor 5
    mainLayout->addWidget(button, 1);  // Stretch-Faktor 1

    setLayout(mainLayout);

    MainWindow* mainWindow = qobject_cast<MainWindow*>(parent);

    mainWindow->addClass(this, "SleepTimerWindow");
    mainWindow->addClass(button, "SleeptimerStart");

    connect(button, &QPushButton::clicked, this, [this, mainWindow]() {
        if (!isTimerActive) {
            remainingTime = dropdownToMilliseconds(dropdown->currentText());
            if (remainingTime > 0) {
                timer->start(1000);
                isTimerActive = true;

                if (mainWindow) {
                    mainWindow->removeClass(button, "SleeptimerStart");
                    mainWindow->addClass(button, "SleeptimerStopp");
                }
                button->setText("Stoppen");

                qDebug() << "Timer gestartet für" << remainingTime / 1000 << "Sekunden.";
            }
        } else {
            stopTimer(false);
        }
    });

    connect(timer, &QTimer::timeout, this, [this]() {
        remainingTime -= 1000;
        if (remainingTime <= 0) {
            stopTimer(true);
        } else {
            emit remainingTimeUpdated(remainingTime / 1000);
        }
    });
}

SleepTimerWindow::~SleepTimerWindow() = default;

int SleepTimerWindow::dropdownToMilliseconds(const QString& text) const {
    static const QMap<QString, int> timeMap = {
        {"3 Sekunden", 3 * 1000},
        {"5 Minuten", 5 * 60 * 1000},
        {"10 Minuten", 10 * 60 * 1000},
        {"15 Minuten", 15 * 60 * 1000},
        {"30 Minuten", 30 * 60 * 1000},
        {"1 Stunde", 60 * 60 * 1000}
    };
    return timeMap.value(text, 0);
}

int SleepTimerWindow::getRemainingTime() const {
    return remainingTime / 1000;
}

void SleepTimerWindow::stopTimer(bool expired) {
    timer->stop();
    remainingTime = 0;
    isTimerActive = false;

    MainWindow* mainWindow = qobject_cast<MainWindow*>(parent());
    if (mainWindow) {
        mainWindow->addClass(button, "SleeptimerStart");
        mainWindow->removeClass(button, "SleeptimerStopp");
    }
    button->setText("Starten");

    emit remainingTimeUpdated(0);

    if (expired) {
        emit timerExpired();
        qDebug() << "Timer abgelaufen.";
    } else {
        qDebug() << "Timer gestoppt.";
    }
}
