#include "myvideoitem.h"
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QImage>
#include <QSGImageNode>
#include <QPainter>
#include <QPainterPath>

MyVideoItem::MyVideoItem()
{
    setFlag(ItemHasContents, true);
    qDebug()
        << "MyVideoItem thread"
        << thread();
}

void MyVideoItem::releaseResources()
{
    qDebug() << "releaseResources";
}

MyVideoItem::~MyVideoItem()
{
    qDebug() << "~MyVideoItem" << this;
}


VideoSink *MyVideoItem::sink() const
{
    return m_sink;
}

void MyVideoItem::setSink(
    VideoSink *sink)
{
    if(m_sink == sink)
        return;

    if(m_sink)
        disconnect(
            m_sink,
            nullptr,
            this,
            nullptr);

    m_sink = sink;

    if(m_sink)
    {
        connect(
            m_sink,
            &VideoSink::frameChanged,
            this,
            &MyVideoItem::update, Qt::QueuedConnection);
    }

    emit sinkChanged();
}

void MyVideoItem::onImageChanged()
{
    update();
}


QSGNode *MyVideoItem::updatePaintNode(
    QSGNode *oldNode,
    UpdatePaintNodeData *)
{
    qDebug()
    << "paint thread"
    << QThread::currentThread();

    qDebug()
    << "paint"
    << this
    << m_sink;


    auto *node =
        static_cast<QSGSimpleTextureNode *>(oldNode);

    if (!node)
    {
        node = new QSGSimpleTextureNode();
        node->setOwnsTexture(true);
    }

    if (!m_sink)
        return node;

    QImage img = m_sink->image();
    qDebug()
        << img.size()
        << img.format()
        << img.isDetached();


    //apply radius to image
    QImage rounded(img.size(), QImage::Format_ARGB32_Premultiplied);
    rounded.fill(Qt::transparent);

    QPainter p(&rounded);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(
        QRectF(0,0,img.width(),img.height()),
        m_radius,
        m_radius);

    p.setClipPath(path);
    p.drawImage(0,0,img);
    p.end();

    // if (img.isNull()) //this cause crash!
    //     return node;

    if (!window())
        return node;

    //test image
    // QImage image(
    //     320,
    //     180,
    //     QImage::Format_RGB32);
    // image.fill(Qt::green);
    // image = image.mirrored(true, false); // Optional mirror like webcam preview

    node->setTexture(
        window()->createTextureFromImage(rounded));

    node->setRect(boundingRect());

    return node;
}
