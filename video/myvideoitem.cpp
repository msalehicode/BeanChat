#include "myvideoitem.h"


MyVideoItem::MyVideoItem()
{
    setFlag(ItemHasContents, true);
#if D_PRINT_MYVIDEOITEM_INFO
    qDebug()
        << "MyVideoItem thread"
        << thread();
#endif
}

void MyVideoItem::releaseResources()
{
#if D_PRINT_MYVIDEOITEM_INFO
    qDebug() << "releaseResources";
#endif
}

MyVideoItem::~MyVideoItem()
{
#if D_PRINT_MYVIDEOITEM_INFO
    qDebug() << "~MyVideoItem" << this;
#endif
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

#if D_PRINT_MYVIDEOITEM_INFO
    qDebug()
    << "paint thread"
    << QThread::currentThread();

    qDebug()
    << "paint"
    << this
    << m_sink;
#endif

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

#if D_PRINT_MYVIDEOITEM_INFO
    qDebug() << "videoItem image:"
        << img.size()
        << img.format()
        << img.isDetached();
#endif

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
