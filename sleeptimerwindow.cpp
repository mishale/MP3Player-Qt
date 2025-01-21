#include "sleeptimerwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

SleepTimerWindow::SleepTimerWindow(QWidget* parent)
    : QDialog(parent), timer(new QTimer(this)), remainingTime(0) {
    setWindowTitle("Sleep Timer");
    resize(300, 200);

    dropdown = new QComboBox(this);
    dropdown->addItems({"3 Sekunden", "5 Minuten", "10 Minuten", "15 Minuten", "30 Minuten", "1 Stunde"});

    checkbox = new QCheckBox("Sleeptimer aktivieren", this);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QHBoxLayout* controlLayout = new QHBoxLayout;

    controlLayout->addWidget(dropdown);
    controlLayout->addWidget(checkbox);
    mainLayout->addLayout(controlLayout);
    setLayout(mainLayout);

    connect(checkbox, &QCheckBox::toggled, this, [this](bool checked) {
        if (checked) {
            remainingTime = dropdownToMilliseconds(dropdown->currentText());
            if (remainingTime > 0) {
                timer->start(1000);
                qDebug() << "Timer gestartet für" << remainingTime / 1000 << "Sekunden.";
            } else {
                checkbox->setChecked(false);
            }
        } else {
            stopTimer(true);
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
    checkbox->setChecked(false);
    emit remainingTimeUpdated(0);

    if (expired) {
        emit timerExpired();
        qDebug() << "Timer abgelaufen.";
    } else {
        qDebug() << "Timer gestoppt.";
    }
}
