#include <QGuiApplication>
#include <QClipboard>

class ClipboardHelper : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE void copy(const QString &text)
    {
        QGuiApplication::clipboard()->setText(text);
    }
};
