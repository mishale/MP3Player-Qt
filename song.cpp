#include "song.h"
#include <QMediaMetaData>

Song::Song(const QString& filePath)
    : filePath(filePath), metaList(), metaDataLoaded(false) {}

QString Song::getFilePath() const
{
    return filePath;
}

QList<QString> Song::getMetaData(const QMediaMetaData& metaData)
{
    if (!metaDataLoaded)
    {
        if (!metaData.isEmpty()) {
            QString title = metaData.value(QMediaMetaData::Title).toString();
            if (title.isEmpty()) {
                metaList.append("Titel nicht vorhanden: " + filePath);
            } else {
                metaList.append(title);
            }

            QStringList authors = metaData.value(QMediaMetaData::ContributingArtist).toStringList();
            if (authors.isEmpty()) {
                metaList.append("Interpret nicht vorhanden");
            } else {
                metaList.append(authors.join(", "));
            }

            QVariant durationVar = metaData.value(QMediaMetaData::Duration);
            QString duration = durationVar.isValid() ? durationVar.toString() : "";
            if (duration.isEmpty()) {
                metaList.append("Duration nicht vorhanden");
            } else {
                metaList.append(duration);
            }

            QString date = metaData.value(QMediaMetaData::Date).toString();
            if (date.isEmpty()) {
                metaList.append("Date nicht vorhanden");
            } else {
                metaList.append(date);
            }
        }
        metaDataLoaded = true;
    }
    return metaList;
}

QList<QString> Song::getCachedMetaData() const
{
    return metaList;
}

QString Song::getTitle() const
{
    return metaList.size() > 0 ? metaList.at(0) : "";
}

QString Song::getAuthor() const
{
    return metaList.size() > 1 ? metaList.at(1) : "";
}

QString Song::getDuration() const
{
    return metaList.size() > 2 ? metaList.at(2) : "";
}

QString Song::getDate() const
{
    return metaList.size() > 3 ? metaList.at(3) : "";
}
