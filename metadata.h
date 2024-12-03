#ifndef METADATA_H
#define METADATA_H
#include <QString>
#include <QMediaPlayer>

class MetaData
{
public:
    MetaData(const QString title, const QString& artist, const QString& album, const int& year, const int& duration);

    QString getTitle() const;
    QString getArtist() const;
    QString getAlbum() const;
    int getYear() const;
    int getDuration() const;

private:
    QString title;
    QString artist;
    QString album;
    int year;
    int duration;
};

#endif // METADATA_H
