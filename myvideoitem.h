#ifndef MYVIDEOITEM_H
#define MYVIDEOITEM_H

#include <QQuickItem>
#include <QVideoFrame>
#include <QSGSimpleTextureNode>
#include <QSGSimpleRectNode>
#include <QMutex>
#include "videosink.h"
#include <QThread>

class MyVideoItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(VideoSink* sink READ sink WRITE setSink NOTIFY sinkChanged)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
public:
    MyVideoItem();
    void releaseResources() override;
    ~MyVideoItem();
    VideoSink *sink() const;
    void setSink(VideoSink *sink);

    void onImageChanged();


    qreal radius() const { return m_radius; }

    void setRadius(qreal radius)
    {
        if (qFuzzyCompare(m_radius, radius))
            return;

        m_radius = radius;
        update();

        emit radiusChanged();
    }
protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

signals:
    void sinkChanged();
    void radiusChanged();
private:
    VideoSink *m_sink = nullptr;
    QImage m_image;
    QSGTexture *m_texture = nullptr;
    qreal m_radius=0;
};

#endif // MYVIDEOITEM_H


