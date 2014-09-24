#ifndef GOOGLESPEECH_H
#define GOOGLESPEECH_H

#include "googlespeech_globals.h"
#include <QString>
#include <QObject>
#include <QtMultimedia>

class GOOGLESPEECHSHARED_EXPORT GoogleSpeech: QObject
{
    Q_OBJECT
private:
    QString m_language;
    QMediaPlaylist *m_playlist;
    QMediaPlayer *m_player;
    QString m_url;
public:
    GoogleSpeech(QString language);
    void setLanguage(QString language);
    Q_INVOKABLE void speech(QString text);

private slots:
    void errorSlot();
    void state(QMediaPlayer::State state);
signals:
    void stopped();
    void error();
};

#endif // GoogleSpeech_H
