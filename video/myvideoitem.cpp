#include "myvideoitem.h"

#include "logging/loggingcategories.h"

MyVideoItem::MyVideoItem()
{
    setFlag(ItemHasContents, true);
}

void MyVideoItem::releaseResources()
{
    qCInfo(_videoitem) << "MyVideoItem release resources";
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
    qCInfo(_videoitem) << "set sink";
    if(m_sink == sink)
    {
        qCWarning(_videoitem) << "set sink ignored, it's the same";
        return;
    }

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


    // if (img.isNull()) //<- this check causes crash!
    // {
    //     qCWarning(_videoitem) << "image is null!";
    //     node->setTexture(nullptr);
    //     return node;
    // }

#if D_PRINT_MYVIDEOITEM_INFO
    qDebug() << "videoItem image:"
        << img.size()
        << img.format()
        << img.isDetached();
#endif

    //apply radius to image, (it has heavy for CPU lets comment it for now)
    // QImage rounded(img.size(), QImage::Format_ARGB32_Premultiplied);
    // rounded.fill(Qt::transparent);
    // QPainter p(&rounded);
    // p.setRenderHint(QPainter::Antialiasing);
    // QPainterPath path;
    // path.addRoundedRect(
    //     QRectF(0,0,img.width(),img.height()),
    //     m_radius,
    //     m_radius);
    // p.setClipPath(path);
    // p.drawImage(0,0,img);
    // p.end();

    if (!window())
        return node;

    node->setTexture(window()->createTextureFromImage(img));

    node->setRect(boundingRect());

    return node;
}
