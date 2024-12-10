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
        ":/styles/QLabel.qss",
        ":/styles/QListWidget.qss",
        ":/styles/QPushButton.qss",
        ":/styles/QToolButton.qss",
        ":/styles/QSlider.qss"
    };

    loadCombinedStylesheet(stylesheetFiles);

    widgetsCreated = false;
    bibliothek = new Playlist("Bibliothek");
    allPlaylists = new PlaylistManager();
    queue = new Queue();
    ui->playlists->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->songList->setContextMenuPolicy(Qt::CustomContextMenu);

    initPlayer();
    connect(ui->selectDirButton, &QPushButton::clicked, this, &MainWindow::selectDirectory);
    connect(ui->songList, &QListWidget::itemDoubleClicked, this, &MainWindow::startSong);
    connect(ui->playlists, &QListWidget::itemClicked, this, &MainWindow::getPlaylistOnClick);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::pause);
    connect(ui->newPlaylist, &QPushButton::clicked, this, &MainWindow::createPlaylistUI);
    connect(ui->playlists, &QListWidget::customContextMenuRequested,
            this, &MainWindow::showContextMenuPlaylist);
    connect(ui->songList, &QListWidget::customContextMenuRequested,
            this, &MainWindow::showContextMenuSongs);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::printSongList);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &MainWindow::changeVolume);
    connect(ui->loopSongBtn, &QPushButton::clicked, this, &MainWindow::handleLoop);

    addClass(ui->prevButton, "soundControlButton");
    addClass(ui->pauseButton, "soundControlButton");
    addClass(ui->nextButton, "soundControlButton");
    addClass(ui->progressBar, "progressBar");

    addClass(ui->songTitle, "SongName");
    addClass(ui->songAuthor, "SongInterpret");

    ui->pauseButton->setIcon(QIcon(":/icons/play.png"));
    addClass(ui->pauseButton, "playButton");

    //ui->loopSongBtn->setIcon(QIcon(":/icons/playlist-loop.png"));
    ui->shuffleButton->setIcon(QIcon(":/icons/shuffle.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadCombinedStylesheet(const QStringList &stylesheetFiles) {
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

void MainWindow::initPlayer()
{
    mediaPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(1);
    ui->volumeSlider->setRange(0,100);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::updateSliderRange);
    connect(ui->progressBar, &QSlider::sliderMoved, this, &MainWindow::setSongPosition);
}


void MainWindow::selectDirectory()
{

    QString fileName = QFileDialog::getOpenFileName(this, "Wähle eine MP3-Datei aus", QString(), "MP3-Dateien (*.mp3)");

    if (ui->playlists->findItems("Bibliothek", Qt::MatchExactly).isEmpty())
    {
        ui->playlists->addItem(bibliothek->getName());
        if(allPlaylists->getPlaylistByName("Bibliothek") == nullptr)
        {
            allPlaylists->addPlaylist(bibliothek);
        }
    }

    if (!fileName.isEmpty())
    {
        Song* song = new Song(fileName);
        bibliothek->addSong(song);
        //queue->addSong(song);
        ui->playlists->setCurrentItem(ui->playlists->findItems("Bibliothek", Qt::MatchExactly).first());
        catchMetaData(song);
        //displayPlaylist(bibliothek);
    }
}


void MainWindow::displayPlaylist(Playlist* playlist)
{
    ui->songList->clear();
    QList<Song*> allSongs = playlist->getSongs();
    for (Song* s : allSongs)
    {
        QList<QString> metaList = s->getCachedMetaData();
        ui->songList->addItem(metaList.at(0));
    }

}

void MainWindow::catchMetaData(Song* song)
{
    QString fileName = song->getFilePath();
    QMediaPlayer* tempPlayer = new QMediaPlayer(this);
    tempPlayer->setSource(fileName);
    connect(tempPlayer, &QMediaPlayer::mediaStatusChanged, this, [this, song, tempPlayer](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia) {
            QList<QString> metaList = song->getMetaData(tempPlayer->metaData());
            ui->songList->addItem(metaList.at(0));
            tempPlayer->deleteLater();
        }
    });
}


void MainWindow::displayMetaData(Song* song)
{
    // QList<QString> metaList = song->getCachedMetaData();
    ui->songTitle->setText(song->getTitle());
    ui->songAuthor->setText(song->getAuthor());
}

void MainWindow::buildQueue(Song* song, Playlist* playlist)
{
    queue->clear();
    for(Song* s : playlist->getSongs())
    {
        queue->addSong(s);
    }
    while(queue->getCurrentSong() != song && queue->getCurrentSong() != nullptr)
    {
        queue->forwards();
    }
}

void MainWindow::startSong(QListWidgetItem *item)
{
    Song* song = getSongByGUI(item);
    Playlist* playlist = getPlaylistByGUI(ui->playlists->currentItem());

    buildQueue(song, playlist);
    mediaPlayer->setSource(queue->getCurrentSong()->getFilePath());
    displayMetaData(song);
    mediaPlayer->play();

    ui->pauseButton->setIcon(QIcon(":/icons/pause.png"));
    removeClass(ui->pauseButton, "playButton");
    addClass(ui->pauseButton, "pauseButton");

    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::handleSongFinish);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::updateSliderPosition);
    connect(ui->nextButton, &QPushButton::clicked, this, &MainWindow::playNextSong);
    connect(ui->prevButton, &QPushButton::clicked, this, &MainWindow::playPrevSong);
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
        removeClass(ui->pauseButton, "pauseButton");
        addClass(ui->pauseButton, "playButton");

    }
    else
    {
        mediaPlayer->play();
        ui->pauseButton->setIcon(QIcon(":/icons/pause.png"));
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
            Playlist* p = new Playlist(nameInput->text());
            ui->playlists->addItem(p->getName());
            allPlaylists->addPlaylist(p);
            delete nameInput;
            delete createButton;
            widgetsCreated = false;
        });
    }
}

void MainWindow::showContextMenuPlaylist(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);
    QAction *actionDeletePlaylist = new QAction("Playlist löschen", this);
    connect(actionDeletePlaylist, &QAction::triggered, this, &MainWindow::deletePlaylist);

    QAction *actionAddSong = new QAction("Song hinzufügen", this);
    connect(actionAddSong, &QAction::triggered, this, &MainWindow::addSongToPlaylist);

    contextMenu.addAction(actionDeletePlaylist);
    contextMenu.addAction(actionAddSong);

    contextMenu.exec(ui->playlists->mapToGlobal(pos));
}

void MainWindow::showContextMenuSongs(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);
    QAction *actionDeleteSong = new QAction("Song löschen", this);
    connect(actionDeleteSong, &QAction::triggered, this, &MainWindow::deleteSong);

    contextMenu.addAction(actionDeleteSong);

    QMenu *subMenu = new QMenu("zu Playlist hinzufügen", &contextMenu);
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
    QString songToAdd = ui->songList->currentItem()->text();
    Playlist* fromPlaylist = getPlaylistByGUI(ui->playlists->currentItem());
    QList<Song*> allSongs = fromPlaylist->getSongs();
    for(Song* s : allSongs)
    {
        if(s->getTitle() == songToAdd)
        {
            playlist->addSong(s);
        }
    }
}
void MainWindow::addSongToPlaylist()
{
    QListWidgetItem *selectedItem = ui->playlists->currentItem();
    QString playlistName = selectedItem->text();
    QList<Playlist*> everyPlaylist = allPlaylists->getPlaylists();
    for (Playlist* p : everyPlaylist)
    {
        if(p->getName() == playlistName)
        {
            QString fileName = QFileDialog::getOpenFileName(this, "Wähle eine MP3-Datei aus", QString(), "MP3-Dateien (*.mp3)");
            Song* song = new Song(fileName);
            ui->songList->addItem(song->getFilePath());
            p->addSong(song);
            buildQueue(queue->getCurrentSong(), p);
            displayPlaylist(p);
        }
    }

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
                qDebug() << p->getName();
                return p;
            }
        }
        return nullptr;

    }
}


void MainWindow::handleLoop()
{
    if(!isSongLooped && isPlaylistLooped)
    {
        ui->loopSongBtn->setText("kein Loop");
        isPlaylistLooped = false;
        return;
    }
    if(isSongLooped && !isPlaylistLooped)
    {
        ui->loopSongBtn->setText("Playlist Loop");
        mediaPlayer->setLoops(1);
        isSongLooped = false;
        isPlaylistLooped = true;
        return;
    }
    if(!isSongLooped && !isPlaylistLooped)
    {
        ui->loopSongBtn->setText("Song Loop");
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
    else
    {
        Playlist* p = allPlaylists->getPlaylistByName(ui->playlists->currentItem()->text());
        QString songText = selectedItem->text();
        for(Song* s : p->getSongs())
        {
            if(s->getTitle() == songText)
            {
                qDebug() << s->getFilePath();
                return s;
            }
        }
        return nullptr;
    }
}
void MainWindow::playNextSong()
{
    if(!isSongLooped)
    {
        if(!queue->isAtEnd())
        {
            queue->forwards();
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
    if(duration > 0 && position >= duration && ui->songList->count() > 1)
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
    for (Song* s : playlist->getSongs())
    {
        if(s->getTitle() == selectedItem->text())
        {
            playlist->deleteSong(s);
            delete ui->songList->takeItem(ui->songList->row(ui->songList->currentItem()));
            disconnect(mediaPlayer, nullptr, this, nullptr);
            //buildQueue(queue->getCurrentSong(), playlist);
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

void MainWindow::printSongList() {
    for (int i = 0; i < ui->songList->count(); ++i) {
        QListWidgetItem* item = ui->songList->item(i);
    }
    exportSongListToJson();
}

void MainWindow::exportSongListToJson() {
    QJsonArray songArray;
    for (int i = 0; i < ui->songList->count(); ++i) {
        QListWidgetItem* item = ui->songList->item(i);
        QJsonObject songObject;
        songObject["filePath"] = item->text();
        songArray.append(songObject);
    }

    QJsonDocument doc(songArray);
    QString directoryPath = "../../json";
    QDir dir(directoryPath);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create directory for writing JSON file.";
            return;
        }
    }
    QFile file(dir.filePath("songFilePaths.json"));
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Failed to open file for writing.");
        return;
    }

    file.write(doc.toJson());
    file.close();
    qDebug() << "Song file paths exported to JSON.";
}

void MainWindow::addClass(QWidget* widget, const QString& classToAdd) {
    QString currentClasses = widget->property("class").toString();
    if (!currentClasses.split(' ').contains(classToAdd)) {
        currentClasses += " " + classToAdd;
        widget->setProperty("class", currentClasses.trimmed());
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
    }
}

void MainWindow::removeClass(QWidget* widget, const QString& classToRemove) {
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
