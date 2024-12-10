#include "song.h"
#include <QMediaMetaData>

Song::Song(const QString& filePath)
    : filePath(filePath), metaList(), metaDataLoaded(false){}

QString Song::getFilePath() const
{
    return filePath;
}

QList<QString> Song::getMetaData(const QMediaMetaData& metaData)
{
    if(!metaDataLoaded)
    {
        if (!metaData.isEmpty()) {
            metaList.append(metaData.stringValue(QMediaMetaData::Title));
            if(metaList.at(0) == "")
            {
                metaList[0] = "Titel nicht vorhanden " + filePath;
            }

            metaList.append(metaData.stringValue(QMediaMetaData::Author));
            if(metaList.at(1) == "")
            {
                metaList[1] = "Interpret nicht vorhanden";
            }

            metaList.append(metaData.stringValue(QMediaMetaData::Duration));
            if(metaList.at(2) == "")
            {
                metaList[2] = "Duration nicht vorhanden";
            }

            metaList.append(metaData.stringValue(QMediaMetaData::Date));
            if(metaList.at(3) == "")
            {
                metaList[3] = "Date nicht vorhanden";
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
    return  metaList.at(0);
}

QString Song::getAuthor() const
{
    return metaList.at(1);
}

QString Song::getDuration() const
{
    return metaList.at(2);
}

QString Song::getDate() const
{
    return metaList.at(3);
}
