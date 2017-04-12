#ifndef DLNARENDERER_H
#define DLNARENDERER_H

#include "SOAPActionManager.h"
#include "DLNAPlaybackInfo.h"
#include "DLNARendererIcon.h"

#include <QtNetwork>

class DLNARenderer : public QObject
{
    Q_OBJECT
public:
    explicit DLNARenderer(QUrl url, QObject *parent = 0);
    
    QUrl getUrl();
    QString getControlUrl();
    QString getName();
    DLNARendererIcon icon;
    
    void setName(const QString & name);
    void setControlUrl(const QString & name);
    // DLNA functions
    void setPlaybackUrl(const QUrl & url, const QFileInfo & fileInfo);
    void setNextPlaybackUrl(const QUrl & url);
    void playPlayback();
    void stopPlayback();
    void pausePlayback();
    void nextPlayback();
    void previousPlayback();
    void seekPlayback(QTime time);
    void queryPlaybackInfo();
private:
    SOAPActionManager *sam;
    QUrl serverUrl, fullcontrolUrl;
    QString serverName, controlUrl;
signals:
    void receivePlaybackInfo(DLNAPlaybackInfo*);
};

#endif // DLNARENDERER_H
