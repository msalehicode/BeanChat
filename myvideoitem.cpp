#include "myvideoitem.h"
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QImage>

MyVideoItem::MyVideoItem()
{
    setFlag(ItemHasContents, true);
}

void MyVideoItem::setFrame(const QVideoFrame &frame)
{
    {
        QMutexLocker locker(&m_mutex);
        m_frame = frame;
    }

    update();
}


QSGNode *MyVideoItem::updatePaintNode(QSGNode *oldNode,
                                      UpdatePaintNodeData *)
{
    auto *node = static_cast<QSGSimpleTextureNode *>(oldNode);

    if (!node) {
        node = new QSGSimpleTextureNode();
        node->setOwnsTexture(true);
    }

    QVideoFrame frame(m_frame);

    if (!frame.isValid())
        return node;

    QImage image = frame.toImage();

    if (image.isNull())
        return node;

    image = image.scaled(
        640,
        360,
        Qt::KeepAspectRatio,
        Qt::FastTransformation);

    // Flip horizontally
    image = image.mirrored(true, false);


    qDebug() << "FRAME IMAGE:" << image.size()
             << image.format()
             << image.isNull();

    QSGTexture *texture =
        window()->createTextureFromImage(image);

    node->setTexture(texture);
    node->setRect(boundingRect());

    return node;
}
