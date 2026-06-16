#ifndef MYVIDEOITEM_H
#define MYVIDEOITEM_H

#include <QQuickItem>
#include <QVideoFrame>
#include <QSGSimpleTextureNode>
#include <QMutex>

class MyVideoItem : public QQuickItem
{
    Q_OBJECT

public:
    MyVideoItem();

public slots:
    void setFrame(const QVideoFrame &frame);

protected:
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    QVideoFrame m_frame;
    QMutex m_mutex;
};

#endif // MYVIDEOITEM_H


