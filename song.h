#ifndef SONG_H
#define SONG_H
#include <QString>
#include <QMediaMetaData>
#include <QList>

class Song
{
public:
    Song(const QString& filePath);

    QString getFilePath() const;
    QList<QString> getMetaData(const QMediaMetaData& metaData) const;
    bool operator==(const Song& other) const {
        return this->filePath == other.filePath;
    }

private:

    QString filePath;


};

#endif // SONG_H
