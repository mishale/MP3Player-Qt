#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDir>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>
#include <QDebug>
#include <QMenu>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QLabel>
#include <QTextStream>
#include <QMediaMetaData>
#include <QFontDatabase>
#include <algorithm>
#include <random>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mediaPlayer(new QMediaPlayer(this))
    , audioOutput(new QAudioOutput(this))
{
    ui->setupUi(this);

    QFontDatabase::addApplicationFont(":/fonts/Gilroy-Heavy.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Gilroy-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Gilroy-Medium.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Gilroy-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Gilroy-Light.ttf");


    QStringList stylesheetFiles = {
        ":/styles/QMainWindow.qss",
        ":/styles/QToolTip.qss",
        ":/styles/QLabel.qss",
        ":/styles/QListWidget.qss",
        ":/styles/QPushButton.qss",
        ":/styles/QToolButton.qss",
        ":/styles/QSlider.qss"
    };

    connect(ui->newPlaylist,        &QPushButton::clicked,                      this, &MainWindow::createPlaylistUI);
    connect(ui->playlists,          &QListWidget::itemClicked,                  this, &MainWindow::getPlaylistOnClick);
    connect(ui->playlists,          &QListWidget::customContextMenuRequested,   this, &MainWindow::showContextMenuPlaylist);

    connect(ui->sleepTimer,         &QPushButton::clicked,                      this, &MainWindow::setUpSleepTimer);
    connect(ui->shuffleButton,      &QPushButton::clicked,                      this, &MainWindow::shuffle);
    connect(ui->prevButton,         &QPushButton::clicked,                      this, &MainWindow::playPrevSong);
    connect(ui->pauseButton,        &QPushButton::clicked,                      this, &MainWindow::pause);
    connect(ui->nextButton,         &QPushButton::clicked,                      this, &MainWindow::playNextSong);
    connect(ui->loopSongBtn,        &QPushButton::clicked,                      this, &MainWindow::handleLoop);
    connect(ui->volumeSlider,       &QSlider::valueChanged,                     this, &MainWindow::changeVolume);

    connect(ui->progressBar,        &QSlider::sliderMoved,                      this, &MainWindow::setSongPosition);

    connect(mediaPlayer,            &QMediaPlayer::positionChanged,             this, &MainWindow::handleSongFinish);
    connect(mediaPlayer,            &QMediaPlayer::positionChanged,             this, &MainWindow::updateSliderPosition);
    connect(mediaPlayer,            &QMediaPlayer::durationChanged,             this, &MainWindow::updateSliderRange);

    connect(ui->selectDirButton,    &QPushButton::clicked,                      this, &MainWindow::selectDirectory);
    connect(ui->songList,           &QListWidget::itemDoubleClicked,            this, &MainWindow::startSong);
    connect(ui->songList,           &QListWidget::customContextMenuRequested,   this, &MainWindow::showContextMenuSongs);

    addClass(ui->playlists,     "playlistListWidget");
    addClass(ui->songList,      "songListWidget");

    addClass(ui->shuffleButton, "soundControlButton");
    addClass(ui->prevButton,    "soundControlButton");
    addClass(ui->pauseButton,   "soundControlButton");
    addClass(ui->pauseButton,   "pauseButton");
    addClass(ui->nextButton,    "soundControlButton");
    addClass(ui->loopSongBtn,   "soundControlButton");

    addClass(ui->progressBar,   "progressBar");

    addClass(ui->songTitle,     "SongName");
    addClass(ui->songAuthor,    "SongInterpret");

    addClass(ui->sleepTimer, "SleepTimer");

    ui->pauseButton     ->setIcon(QIcon(":/icons/pause.png"));
    ui->loopSongBtn     ->setIcon(QIcon(":/icons/loop_gray_dark.png"));
    ui->shuffleButton   ->setIcon(QIcon(":/icons/shuffle_gray_dark.png"));

    widgetsCreated = false;
    allPlaylists = new PlaylistManager();
    bibliothek = new Playlist("Bibliothek");
    currentPlaylist = bibliothek;
    allPlaylists->addPlaylist(bibliothek);

    queue = new Queue();

    ui->playlists->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->playlists->addItem(bibliothek->getName());
    ui->songList->setContextMenuPolicy(Qt::CustomContextMenu);

    initPlayer();

    //importPlaylistsFromJson();

    loadCombinedStylesheet(stylesheetFiles);

    sleeptimerwindow = new SleepTimerWindow(this);
    ui->sleepTimer->setToolTip("Sleeptimer");
    connect(sleeptimerwindow, &SleepTimerWindow::remainingTimeUpdated, this, [this](int remainingSeconds) {
        int minutes = remainingSeconds / 60;
        int seconds = remainingSeconds % 60;

        QString timeText = QString("%1:%2")
                               .arg(minutes, 2, 10, QChar('0')) // Minuten zweistellig
                               .arg(seconds, 2, 10, QChar('0')); // Sekunden zweistellig

        ui->sleepTimer->setText(timeText); // Text auf den Button setzen
    });

    connect(sleeptimerwindow, &SleepTimerWindow::timerExpired, this, [this]() {
        mediaPlayer->pause();
        ui->pauseButton->setIcon(QIcon(":/icons/play.png"));
        ui->pauseButton->setToolTip("Play");
        removeClass(ui->pauseButton, "pauseButton");
        addClass(ui->pauseButton, "playButton");
        printColored("Counter zuende", "rot", "", false);
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadCombinedStylesheet(const QStringList &stylesheetFiles)
{
    QString combinedStylesheet;
    foreach (const QString &path, stylesheetFiles) {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            combinedStylesheet += in.readAll() + "\n";  // Kombiniere die Inhalte
            file.close();
        } else {
            qDebug() << "Cannot open stylesheet file:" << path;
        }
    }

    this->setStyleSheet(combinedStylesheet);  // Setze das kombinierte Stylesheet
}

void MainWindow::setUpSleepTimer() {
    sleeptimerwindow->setModal(true); // Fenster als modal setzen
    sleeptimerwindow->exec();
    printColored("Sleeptimer clicked", "gelb", "", false);
}

void MainWindow::initPlayer()
{
    mediaPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(1);
    ui->volumeSlider->setRange(0,100);
}

bool MainWindow::IsSongInPlaylist(QString filePath, Playlist* playlist)
{
    if (playlist->getSongs().isEmpty()) {
        return false;
    }

    for (Song* song : playlist->getSongs()) {
        if (song->getFilePath() == filePath) {
            return true;
        }
    }

    return false;
}

void MainWindow::selectDirectory()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Wähle eine MP3-Datei aus", QString(), "MP3-Dateien (*.mp3)");

    if (fileName.isEmpty()) {
        return;
    }

    if (IsSongInPlaylist(fileName, bibliothek)) {
        printColored("keine doppelten Songs in Playlist erlaubt", "rot", "", false);
        return;
    }

    Song* song = new Song(fileName);
    catchMetaData(song);
    bibliothek->addSong(song);
    bibliothek->printSongs();
    ui->playlists->setCurrentItem(ui->playlists->findItems("Bibliothek", Qt::MatchExactly).first());

    exportSongListToJson();
    importPlaylistsFromJson();
}


void MainWindow::displayPlaylist(Playlist* playlist)
{
    printColored("Funktionsaufruf: displayPlaylist()", "gelb", "", false);
    ui->songList->clear();
    QList<Song*> allSongs = playlist->getSongs();
    if (!allSongs.isEmpty())
    {
        for (Song* s : allSongs) {
            addSongToUIList(s);
        }
    }
    playlist->printSongs();
}

void MainWindow::addSongToUIList(Song* s)
{
    // Widget für den Song erstellen
    QWidget* itemWidget = new QWidget(ui->songList);

    QLabel* TitleLabel = new QLabel(s->getTitle(), itemWidget);
    addClass(TitleLabel, "TitleLabel");

    QLabel* AuthorLabel = new QLabel(s->getAuthor(), itemWidget); // Autor hinzufügen
    addClass(AuthorLabel, "AuthorLabel");

    QString durationStr = s->getDuration(); // Dauer als QString
    QString formattedDuration;
    if (!durationStr.isEmpty()) {
        qint64 durationMs = durationStr.toLongLong();
        int minutes = durationMs / 60000;
        int seconds = (durationMs % 60000) / 1000;

        formattedDuration = QString("%1:%2")
                                .arg(minutes, 2, 10, QChar('0')) // Minuten, zweistellig
                                .arg(seconds, 2, 10, QChar('0')); // Sekunden, zweistellig
    } else {
        formattedDuration = "Dauer unbekannt";
    }

    QLabel* DurationLabel = new QLabel(formattedDuration, itemWidget);
    addClass(DurationLabel, "DurationLabel");

    // Layout für Titel und Autor (ohne Abstand)
    QHBoxLayout* titleAuthorLayout = new QHBoxLayout();
    titleAuthorLayout->setContentsMargins(0, 0, 0, 0);
    titleAuthorLayout->setSpacing(0); // Kein Abstand zwischen den Widgets
    titleAuthorLayout->addWidget(TitleLabel);
    titleAuthorLayout->addWidget(AuthorLabel);

    // Hauptlayout für die gesamte Zeile
    QHBoxLayout* layout = new QHBoxLayout(itemWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addLayout(titleAuthorLayout); // Titel und Autor als Gruppe hinzufügen
    layout->addStretch();
    layout->addWidget(DurationLabel);

    itemWidget->setLayout(layout);

    QListWidgetItem* listItem = new QListWidgetItem(ui->songList);
    listItem->setSizeHint(itemWidget->sizeHint());

    // Speichere den Song-Zeiger in den Listeneintrag
    listItem->setData(Qt::UserRole, QVariant::fromValue(reinterpret_cast<void*>(s)));

    // Verbinde das Widget mit dem Listeneintrag
    ui->songList->setItemWidget(listItem, itemWidget);
}


void MainWindow::catchMetaData(Song* song)
{
    QString fileName = song->getFilePath();
    QMediaPlayer* tempPlayer = new QMediaPlayer(this);
    tempPlayer->setSource(fileName);
    connect(tempPlayer, &QMediaPlayer::mediaStatusChanged, this, [this, song, tempPlayer](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia) {
            QList<QString> metaList = song->getMetaData(tempPlayer->metaData());
            //ui->songList->addItem
            addSongToUIList(song);
            tempPlayer->deleteLater();
        }
    });
}



void MainWindow::displayMetaData(Song* song)
{
    song->getCachedMetaData();
    ui->songTitle->setText(song->getTitle());
    ui->songAuthor->setText(song->getAuthor());
}

void MainWindow::buildQueue(Song* song, Playlist* playlist)
{
    queue->clear();
    currentPlaylist = playlist;
    if(!currentPlaylist->getSongs().isEmpty())
    {
        for(Song* s : currentPlaylist->getSongs())
        {
            queue->addSong(s);
        }
        while(queue->getCurrentSong() != song && queue->getCurrentSong() != nullptr)
        {
            queue->forwards();
        }
    }
}

void MainWindow::startSong(QListWidgetItem *item)
{
    Song* song = getSongByGUI(item);
    Playlist* playlist = getPlaylistByGUI(ui->playlists->currentItem());
    currentPlaylist = playlist;
    buildQueue(song, playlist);
    mediaPlayer->setSource(queue->getCurrentSong()->getFilePath());
    displayMetaData(song);
    mediaPlayer->play();
}

void MainWindow::getPlaylistOnClick(QListWidgetItem *playlist)
{
    displayPlaylist(getPlaylistByGUI(playlist));
}

void MainWindow::pause()
{
    if(mediaPlayer->isPlaying())
    {
        mediaPlayer->pause();
        ui->pauseButton->setIcon(QIcon(":/icons/play.png"));
        ui->pauseButton->setToolTip("Play");
        removeClass(ui->pauseButton, "pauseButton");
        addClass(ui->pauseButton, "playButton");

    }
    else
    {
        mediaPlayer->play();
        ui->pauseButton->setIcon(QIcon(":/icons/pause.png"));
        ui->pauseButton->setToolTip("Pause");
        removeClass(ui->pauseButton, "playButton");
        addClass(ui->pauseButton, "pauseButton");
    }
}

void MainWindow::createPlaylistUI()
{
    if(!widgetsCreated)
    {
        QLineEdit *nameInput = new QLineEdit();
        nameInput->setPlaceholderText("Name: ");
        QPushButton *createButton = new QPushButton("Playlist erstellen");
        ui->leftVerticalLayout->addWidget(nameInput);
        ui->leftVerticalLayout->addWidget(createButton);
        widgetsCreated = true;
        connect(createButton, &QPushButton::clicked, this, [=]() {
            if(allPlaylists->findPlaylistByName(nameInput->text()) == nullptr)
            {
                Playlist* p = new Playlist(nameInput->text());
                ui->playlists->addItem(p->getName());
                allPlaylists->addPlaylist(p);
                delete nameInput;
                delete createButton;
                widgetsCreated = false;
                exportSongListToJson();
            }
            else
            {
                qWarning() << "Es exisitert bereits eine Playlist mit diesem Namen, bitte nimm einen anderen.";
            }
        });
    }
}

void MainWindow::showContextMenuPlaylist(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);
    QAction *actionDeletePlaylist = new QAction("Playlist löschen", this); // Action Playlist löschen
    connect(actionDeletePlaylist, &QAction::triggered, this, &MainWindow::deletePlaylist);

    contextMenu.addAction(actionDeletePlaylist);

    contextMenu.exec(ui->playlists->mapToGlobal(pos));
}

void MainWindow::showContextMenuSongs(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);
    QAction *actionDeleteSong = new QAction("Song löschen", this); // Action Song löschen
    connect(actionDeleteSong, &QAction::triggered, this, &MainWindow::deleteSong);

    contextMenu.addAction(actionDeleteSong);

    QMenu *subMenu = new QMenu("zu Playlist hinzufügen", &contextMenu); // Action zur Playlist hinzufügen
    QList<Playlist*> playlists = allPlaylists->getPlaylists();

    for (Playlist* playlist : playlists) {
        QAction *actionToPlaylist = new QAction(playlist->getName(), this);
        connect(actionToPlaylist, &QAction::triggered, this, [this, playlist]() {
            fromLibToPlaylist(playlist);
        });
        subMenu->addAction(actionToPlaylist);
    }
    contextMenu.addMenu(subMenu);
    contextMenu.exec(ui->songList->mapToGlobal(pos));
}

void MainWindow::fromLibToPlaylist(Playlist* playlist)
{
    if (!playlist) {
        qWarning() << "Zielplaylist ist ungültig";
        return;
    }

    QListWidgetItem* selectedItem = ui->songList->currentItem();
    if (!selectedItem) {
        qWarning() << "Kein Song ausgewählt!";
        return;
    }

    Song* songToAdd = reinterpret_cast<Song*>(selectedItem->data(Qt::UserRole).value<void*>());
    if (!songToAdd) {
        qWarning() << "Konnte den Song aus dem Listeneintrag nicht laden!";
        return;
    }

    Playlist* fromPlaylist = getPlaylistByGUI(ui->playlists->currentItem());
    if (!fromPlaylist || fromPlaylist == playlist) {
        qWarning() << (fromPlaylist ? "Start- und Zielplaylist dürfen nicht identisch sein!" : "Konnte die Quell-Playlist nicht ermitteln!");
        return;
    }

    if (!fromPlaylist->getSongs().contains(songToAdd)) {
        qWarning() << "Der ausgewählte Song befindet sich nicht in der Quell-Playlist!";
        return;
    }

    if (IsSongInPlaylist(songToAdd->getFilePath(), playlist)) {
        qWarning() << "Song ist bereits in der Zielplaylist vorhanden!";
        return;
    }

    playlist->addSong(songToAdd);
    exportSongListToJson();
    qDebug() << "Song erfolgreich hinzugefügt:" << songToAdd->getTitle();
}


Playlist* MainWindow::getPlaylistByGUI(QListWidgetItem *selectedItem)
{
    if (!selectedItem) {
        qWarning() << "No item selected.";
        return nullptr;
    }
    else
    {
        QString playlistName = selectedItem->text();
        for (Playlist* p : allPlaylists->getPlaylists())
        {
            if(p->getName() == playlistName)
            {
                return p;
            }
        }
        return nullptr;
    }
}

void MainWindow::handleLoop()
{
    if(!isSongLooped && isPlaylistLooped) // Playlist Loop
    {
        ui->loopSongBtn->setIcon(QIcon(":/icons/loop_gray_dark.png"));
        ui->loopSongBtn->setToolTip("Wiedergabe stoppt, wenn die Playlist durchgelaufen ist.");
        isPlaylistLooped = false;
        return;
    }
    if(isSongLooped && !isPlaylistLooped) // kein Loop
    {
        ui->loopSongBtn->setIcon(QIcon(":/icons/loop.png"));
        ui->loopSongBtn->setToolTip("Playlist wird nach dem letzten Song neu gestartet.");
        mediaPlayer->setLoops(1);
        isSongLooped = false;
        isPlaylistLooped = true;
        return;
    }

    if(!isSongLooped && !isPlaylistLooped) // Song Loop
    {
        ui->loopSongBtn->setIcon(QIcon(":/icons/song-loop.png"));
        ui->loopSongBtn->setToolTip("Aktueller Song wird in Endlosschleife wiedergegeben.");
        mediaPlayer->setLoops(QMediaPlayer::Infinite);
        isSongLooped = true;
        return;
    }
}


Song* MainWindow::getSongByGUI(QListWidgetItem *selectedItem)
{
    if (!selectedItem) {
        return nullptr;
    }

    // Lade den Song-Zeiger direkt aus den Daten des Listeneintrags
    QVariant songData = selectedItem->data(Qt::UserRole);
    if (songData.isValid())
    {
        return reinterpret_cast<Song*>(songData.value<void*>());
    }

    // Falls keine Daten gefunden wurden, gib nullptr zurück
    return nullptr;
}

void MainWindow::shuffle()
{
    isShuffled = !isShuffled;
    if(isShuffled)
    {
        ui->shuffleButton->setIcon(QIcon(":/icons/shuffle.png"));
        ui->loopSongBtn->setToolTip("Shuffle aktiviert.");
        Playlist* tmpPlaylist = currentPlaylist;
        std::random_device rd;
        std::mt19937 g(rd());
        if(currentPlaylist && !tmpPlaylist->getSongs().isEmpty())
        {
            auto songs = tmpPlaylist->getSongs();
            std::vector<Song*> songVector(songs.begin(), songs.end());
            std::shuffle(songVector.begin(), songVector.end(), g);
            buildQueue(queue->getCurrentSong(), tmpPlaylist);
        }
    }
    else
    {
        ui->shuffleButton->setIcon(QIcon(":/icons/shuffle_gray_dark.png"));
        ui->loopSongBtn->setToolTip("Shuffle deaktiviert.");
        buildQueue(getSongByGUI(ui->songList->currentItem()), currentPlaylist);
    }
}

void MainWindow::playNextSong()
{
    if(!isSongLooped) // wenn Song nicht geloopt
    {
        if(!queue->isAtEnd())
        {
            queue->forwards();
            displayMetaData(queue->getCurrentSong());
            mediaPlayer->setSource(queue->getCurrentSong()->getFilePath());
            mediaPlayer->play();
        }
        else // wenn letzter Song
        {
            if(isPlaylistLooped) // Playlist neu von vorne
            {
                displayMetaData(queue->getFirst());
                mediaPlayer->setSource(queue->getFirst()->getFilePath());
                mediaPlayer->play();
                buildQueue(queue->getFirst(),getPlaylistByGUI(ui->playlists->currentItem()));
            }
        }
    }
    else // wenn Song geloopt
    {
        mediaPlayer->setPosition(0);
        mediaPlayer->play();
    }
}

void MainWindow::playPrevSong()
{
    if(!isSongLooped)
    {
        if(!queue->isAtStart())
        {
            queue->backwards();
            displayMetaData(queue->getCurrentSong());
            mediaPlayer->setSource(queue->getCurrentSong()->getFilePath());
            mediaPlayer->play();
        }
    }
    else
    {
        mediaPlayer->setPosition(0);
        mediaPlayer->play();
    }
}

void MainWindow::handleSongFinish(qint64 position)
{
    qint64 duration = mediaPlayer->duration();
    if(duration > 0 && position >= duration && ui->songList->count() > 1) // wenn aktuelle Position = Länge des Songs
    {
        playNextSong();
    }
}

void MainWindow::deletePlaylist()
{
    Playlist* playlist = getPlaylistByGUI(ui->playlists->currentItem());
    if(playlist->getName() == "Bibliothek")
    {
        playlist->clearSongs();
    }
    else
    {
        allPlaylists->deletePlaylist(playlist);
    }
    ui->songList->clear();
    delete ui->playlists->takeItem(ui->playlists->row(ui->playlists->currentItem()));

}

void MainWindow::deleteSong()
{
    Playlist* playlist;
    if(!(ui->playlists->currentItem()))
    {
        playlist = bibliothek;
    }
    else
    {
        playlist = getPlaylistByGUI(ui->playlists->currentItem());
    }

    QListWidgetItem *selectedItem = ui->songList->currentItem();

    Song* songToRemove = reinterpret_cast<Song*>(selectedItem->data(Qt::UserRole).value<void*>());
    if (!songToRemove) {
        qWarning() << "Konnte den Song aus dem Listeneintrag nicht laden!";
        return;
    }

    for (Song* s : playlist->getSongs())
    {
        if(s == songToRemove)
        {
            playlist->deleteSong(s);
            delete ui->songList->takeItem(ui->songList->row(ui->songList->currentItem()));
            disconnect(mediaPlayer, nullptr, this, nullptr);
            if(playlist->getSongs().isEmpty())
            {
                queue->clear();
            }
            else
            {
                buildQueue(queue->getCurrentSong(), playlist);
            }
            displayPlaylist(playlist);
            break;
        }
    }
}

void MainWindow::updateSliderPosition(qint64 position)
{
    ui->progressBar->setValue(static_cast<int>(position));
}

void MainWindow::setSongPosition(int position)
{
    mediaPlayer->setPosition(static_cast<qint64>(position));
}

void MainWindow::updateSliderRange(qint64 duration)
{
    ui->progressBar->setRange(0, static_cast<int>(duration));
}

void MainWindow::printSongList()
{
    for (int i = 0; i < ui->songList->count(); ++i) {
        QListWidgetItem* item = ui->songList->item(i);
    }
    exportSongListToJson();
}

void MainWindow::exportSongListToJson()
{
    QDir dir("../../json");
    if (!dir.exists() && !dir.mkpath(".")) {
        qDebug() << "Verzeichnis kann nicht erstellt werden:" << dir.absolutePath();
        return;
    }

    QJsonArray playlistArray;
    for (const auto* playlist : allPlaylists->getPlaylists()) {
        QJsonObject playlistObj;
        playlistObj["name"] = playlist->getName();

        QJsonArray songsArray;
        for (const auto* song : playlist->getSongs()) {
            songsArray.append(song->getFilePath());
        }
        playlistObj["songs"] = songsArray;
        playlistArray.append(playlistObj);
    }

    QFile file(dir.filePath("playlists.json"));
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Datei kann nicht geschrieben werden:" << file.fileName();
        return;
    }
    file.write(QJsonDocument(playlistArray).toJson());
    file.close();

    if (playlistArray.isEmpty()) {
        qDebug() << "Keine Playlists zum Exportieren gefunden.";
    } else {
        qDebug() << "Playlists erfolgreich exportiert nach" << file.fileName();
        for (const auto* playlist : allPlaylists->getPlaylists()) {
            qDebug() << "- " << playlist->getName();
            for (const auto* song : playlist->getSongs()) {
                qDebug() << "  •" << song->getFilePath();
            }
        }
    }
}

void MainWindow::importPlaylistsFromJson()
{
    QString filePath = "../../json/playlists.json";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "JSON-Datei kann nicht geöffnet werden:" << filePath;
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isArray()) {
        qDebug() << "Ungültiges JSON-Format in der Datei.";
        return;
    }

    QJsonArray playlistArray = doc.array();
    for (const QJsonValue& playlistValue : playlistArray) {
        QJsonObject playlistObj = playlistValue.toObject();
        QString playlistName = playlistObj["name"].toString();
        if (playlistName.isEmpty()) continue;
        // Überprüfe, ob die Playlist bereits existiert
        Playlist* playlist = allPlaylists->findPlaylistByName(playlistName);
        if (!playlist) {
            // Erstelle die Playlist, falls sie nicht existiert
            playlist = new Playlist(playlistName);
            allPlaylists->addPlaylist(playlist);
            ui->playlists->addItem(playlist->getName());
            exportSongListToJson();
            qDebug() << "Playlist wurde nicht gefunden. Sie wurde neu erstellt.";
        } else {
            qDebug() << "Playlist wurde gefunden.";
        }

        QJsonArray songsArray = playlistObj["songs"].toArray();
        for (const QJsonValue& songValue : songsArray) {
            QString filePath = songValue.toString();
            if (filePath.isEmpty()) continue;

            // Überprüfe, ob der Song bereits in der Playlist ist
            if (!playlist->containsSong(filePath)) {
                // Erstelle den Song und füge ihn hinzu
                Song* song = new Song(filePath);
                bibliothek->addSong(song);
                playlist->addSong(song);

                // Verarbeite die Metadaten des Songs
                catchMetaData(song);
            }
        }
    }

    ui->songList->clear();

    qDebug() << "Playlists erfolgreich aus JSON importiert.";
}

void MainWindow::addClass(QWidget* widget, const QString& classToAdd)
{
    QString currentClasses = widget->property("class").toString();
    if (!currentClasses.split(' ').contains(classToAdd)) {
        currentClasses += " " + classToAdd;
        widget->setProperty("class", currentClasses.trimmed());
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
    }
}

void MainWindow::removeClass(QWidget* widget, const QString& classToRemove)
{
    QStringList classes = widget->property("class").toString().split(' ');
    classes.removeAll(classToRemove);
    widget->setProperty("class", classes.join(' ').trimmed());
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
}

void MainWindow::changeVolume(int value)
{
    // Logarithmische Skala für natürliche Lautstärkeempfindung
    double volume = pow(value / 100.0, 2);
    audioOutput->setVolume(volume);
}

void MainWindow::printColored(const QString& text, const QString& textColor, const QString& backgroundColor = "", bool bold = false) {
    // ANSI-Farbcodes
    QMap<QString, int> colorCodes = {
        {"schwarz", 30},
        {"rot", 31},
        {"grün", 32},
        {"gelb", 33},
        {"blau", 34},
        {"magenta", 35},
        {"cyan", 36},
        {"weiß", 37}
    };

    int textCode = colorCodes.value(textColor.toLower(), 37); // Standard: Weiß
    QString backgroundCode = backgroundColor.isEmpty() ? ""
                                                       : QString::number(colorCodes.value(backgroundColor.toLower(), 40) + 10);

    QString style = bold ? "1" : "0"; // "1" für Fett, "0" für Normal

    QString formattedText;
    if (backgroundCode.isEmpty()) {
        // Nur Textfarbe und Stil
        formattedText = QString("\033[%1;%2m%3\033[0m").arg(style).arg(textCode).arg(text);
    } else {
        // Text- und Hintergrundfarbe
        formattedText = QString("\033[%1;%2;%3m%4\033[0m").arg(style).arg(textCode).arg(backgroundCode).arg(text);
    }

    QTextStream out(stdout);
    out << formattedText << Qt::endl;
}
