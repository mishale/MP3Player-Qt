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
#include <QTextStream>
#include <QMediaMetaData>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mediaPlayer(new QMediaPlayer(this))
    , audioOutput(new QAudioOutput(this))
{
    ui->setupUi(this);
    widgetsCreated = false;
    bibliothek = new Playlist("Bibliothek");
    allPlaylists = new PlaylistManager();
    newPlaylist = new Playlist("");
    queue = new Queue();
    ui->playlists->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->songList->setContextMenuPolicy(Qt::CustomContextMenu);

    qDebug() << "Test";

    initPlayer();
    connect(ui->selectDirButton, &QPushButton::clicked, this, &MainWindow::selectDirectory);
    connect(ui->songList, &QListWidget::itemDoubleClicked, this, &MainWindow::startSong);
    connect(ui->playlists, &QListWidget::itemClicked, this, &MainWindow::showPlaylist);
    connect(ui->pauseButton, &QPushButton::clicked, this, &MainWindow::pause);
    connect(ui->newPlaylist, &QPushButton::clicked, this, &MainWindow::createPlaylistUI);
    connect(ui->playlists, &QListWidget::customContextMenuRequested,
            this, &MainWindow::showContextMenuPlaylist);
    connect(ui->songList, &QListWidget::customContextMenuRequested,
            this, &MainWindow::showContextMenuSongs);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::printSongList);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &MainWindow::changeVolume);
    addClass(ui->prevButton, "soundControlButton");
    addClass(ui->pauseButton, "soundControlButton");
    addClass(ui->nextButton, "soundControlButton");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initPlayer()
{
    mediaPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(1);
    ui->volumeSlider->setRange(0,100);
}

void MainWindow::selectDirectory()
{

    QString fileName = QFileDialog::getOpenFileName(this, "Wähle eine MP3-Datei aus", QString(), "MP3-Dateien (*.mp3)");

    if (ui->playlists->findItems("Bibliothek", Qt::MatchExactly).isEmpty())
    {
        ui->playlists->addItem(bibliothek->getName());
        allPlaylists->addPlaylist(*bibliothek);
    }

    if (!fileName.isEmpty())
    {
        Song* song = new Song(fileName);
        bibliothek->addSong(*song);
        queue->addSong(*song);
        mediaPlayer->setSource(fileName);
        connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [this, &song, fileName](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::LoadedMedia) {
                QList<QString> metaList = song->getMetaData(mediaPlayer->metaData());
                if(metaList.at(0) == "")
                {
                    ui->songList->addItem("Kein Titel vorhanden " + fileName);
                }
                else
                {
                    ui->songList->addItem(metaList.at(0));
                }
            }
        });
    }
}

void MainWindow::searchMP3Files(const QString &directoryPath)
{
    QDir dir(directoryPath);

    QStringList filters;
    filters << "*.mp3";
    dir.setNameFilters(filters);

    QFileInfoList fileList = dir.entryInfoList(QDir::Files);

    for (const QFileInfo &fileInfo : fileList) {
        ui->songList->addItem(fileInfo.fileName());
    }
}

void MainWindow::startSong(QListWidgetItem *item)
{
    QString filePath = item->text();
    mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    mediaPlayer->play();
    connect(ui->nextButton, QPushButton::clicked, this, [=]()
            {
                queue->forwards();
                Song currentSong = queue->getCurrentSong();
                mediaPlayer->setSource(currentSong.getFilePath());
                mediaPlayer->play();
            });

    connect(ui->prevButton, QPushButton::clicked, this, [=]()
            {
                queue->backwards();
                Song currentSong = queue->getCurrentSong();

                mediaPlayer->setSource(currentSong.getFilePath());
                mediaPlayer->play();
            });
}

void MainWindow::showPlaylist(QListWidgetItem *playlist)
{
    ui->songList->clear();
    QString playlistName = playlist->text();
    QList<Playlist> everyPlaylist = allPlaylists->getPlaylists();
    for (const Playlist& pp : everyPlaylist)
    {
        qDebug() << pp.getName();
        qDebug() << "Playlistname " << playlistName;
        if(pp.getName() == playlistName)
        {
            qDebug() << "geht in if rein";
            QList<Song> allSongs = pp.getSongs();
            for (const Song &s : allSongs)
            {
                qDebug() << s.getFilePath();
                ui->songList->addItem(s.getFilePath());
            }
            //break;
        }
    }
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
            newPlaylist->changeName(nameInput->text());
            ui->playlists->addItem(newPlaylist->getName());
            allPlaylists->addPlaylist(*newPlaylist);
            //qDebug() << playlist->getName();
            /*          QList<Playlist> everyPlaylist = allPlaylists->getPlaylists();
            for (const Playlist& p : everyPlaylist)
            {
                qDebug() << p.getName();
            }
*/
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
    QList<Playlist> playlists = allPlaylists->getPlaylists();

    for (const Playlist &playlist : playlists) {
        QAction *actionToPlaylist = new QAction(playlist.getName(), this);
        connect(actionToPlaylist, &QAction::triggered, this, [this, playlist]() {
            addSongToPlaylist();
            qDebug() << "zu" << playlist.getName() << "hinzugefügt";
        });
        subMenu->addAction(actionToPlaylist);
    }
    contextMenu.addMenu(subMenu);
    contextMenu.exec(ui->songList->mapToGlobal(pos));
}

void MainWindow::addSongToPlaylist()
{
    QListWidgetItem *selectedItem = ui->playlists->currentItem();
    QString playlistName = selectedItem->text();
    QList<Playlist> everyPlaylist = allPlaylists->getPlaylists();
    for (Playlist& p : everyPlaylist)
    {
        if(p.getName() == playlistName)
        {
            QString fileName = QFileDialog::getOpenFileName(this, "Wähle eine MP3-Datei aus", QString(), "MP3-Dateien (*.mp3)");
            Song song(fileName);
            //ui->songList->addItem(QFileInfo(fileName).fileName()); // Die ausgewählte Datei zur Liste hinzufügen
            ui->songList->addItem(song.getFilePath());
            p.addSong(song);
            QList<Song> psongs = newPlaylist->getSongs();
            for (const Song& psong : psongs)
            {
                qDebug() << psong.getFilePath();
            }
            /*           QList <Song> psongs = p.getSongs();
            for(const Song &psong : psongs)
            {
                qDebug() << psong.getFilePath();
            }
*/
        }
    }

}
void MainWindow::deletePlaylist()
{
    QListWidgetItem *selectedItem = ui->playlists->currentItem();
    if (!selectedItem) {
        qWarning() << "No item selected for deletion.";
        return;
    }
    else
    {
        QString playlistName = selectedItem->text();
        QList<Playlist> everyPlaylist = allPlaylists->getPlaylists();
        for (const Playlist& p : everyPlaylist)
        {
            if(p.getName() == playlistName)
            {
                allPlaylists->deletePlaylist(p);
            }
        }
        delete ui->playlists->takeItem(ui->playlists->row(selectedItem));
    }
}

void MainWindow::deleteSong()
{
    QListWidgetItem *selectedItem = ui->songList->currentItem();
    if (!selectedItem) {
        qWarning() << "No item selected for deletion.";
        return;
    }
}

void MainWindow::printSongList() {
    for (int i = 0; i < ui->songList->count(); ++i) {
        QListWidgetItem* item = ui->songList->item(i);
        qDebug() << item->text();
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

