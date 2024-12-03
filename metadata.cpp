#include "metadata.h"

MetaData::MetaData(const QString title, const QString& artist, const QString& album, const int& year, const int& duration)
    : title(title), artist(artist), album(album), year(year), duration(duration){}

QString MetaData::getArtist() const
{
    return artist;
}

QString MetaData::getTitle() const
{
    return title;
}

QString MetaData::getAlbum() const
{
    return album;
}

int MetaData::getYear() const
{
    return year;
}

int MetaData::getDuration() const
{
    return duration;
}
