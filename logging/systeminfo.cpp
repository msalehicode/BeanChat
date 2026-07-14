#include "systeminfo.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QGuiApplication>
#include <QScreen>
#include <QSysInfo>
#include <QTextStream>
#include <QThread>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#include <unistd.h>
#include <QFile>
#endif

#ifdef Q_OS_ANDROID
#include <unistd.h>
#endif

// #include "version.h"     // APP_VERSION


QString SystemInfo::diagnosticReport()
{
    QString report;

    QTextStream stream(&report);

    stream << "==================================================\n";
    stream << "BeanChat Diagnostic Log\n";
    stream << "==================================================\n\n";

    stream << "Generated        : "
           << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
           << "\n\n";

    stream << "Application\n";
    stream << "-----------\n";
    stream << "build Version    : " << APP_VERSION << '\n';
    stream << "Build            : " << applicationBuildType() << '\n';
    stream << "Qt               : " << qtVersion() << "\n\n";

    stream << "System\n";
    stream << "------\n";
    stream << "OS               : " << operatingSystemName() << '\n';
    stream << "OS Version       : " << operatingSystemVersion() << '\n';
    stream << "Kernel           : " << kernelType() << '\n';
    stream << "Kernel Version   : " << kernelVersion() << '\n';
    stream << "Architecture     : " << architecture() << '\n';
    stream << "Hostname         : " << hostName() << "\n\n";

    stream << "Hardware\n";
    stream << "--------\n";
    stream << "CPU              : " << cpuModel() << '\n';
    stream << "Logical Cores    : " << logicalCpuCount() << '\n';
    stream << "RAM              : " << totalMemoryString() << "\n\n";

    stream << "Display\n";
    stream << "-------\n";
    stream << "Resolution       : " << primaryScreenResolution() << '\n';
    stream << "Scale            : " << primaryScreenScale() << '\n';

    stream << "\n==================================================\n";

    return report;
}


// QString SystemInfo::applicationVersion()
// {
//     return APP_VERSION;
// }

QString SystemInfo::applicationBuildType()
{
#ifdef QT_DEBUG
    return "Debug";
#else
    return "Release";
#endif
}

QString SystemInfo::qtVersion()
{
    return QT_VERSION_STR;
}

QString SystemInfo::operatingSystemName()
{
    return QSysInfo::prettyProductName();
}

QString SystemInfo::operatingSystemVersion()
{
    return QSysInfo::productVersion();
}

QString SystemInfo::kernelType()
{
    return QSysInfo::kernelType();
}

QString SystemInfo::kernelVersion()
{
    return QSysInfo::kernelVersion();
}

QString SystemInfo::architecture()
{
    return QSysInfo::currentCpuArchitecture();
}

QString SystemInfo::hostName()
{
    return QSysInfo::machineHostName();
}

int SystemInfo::logicalCpuCount()
{
    return QThread::idealThreadCount();
}


QString SystemInfo::primaryScreenResolution()
{
    const QScreen *screen = QGuiApplication::primaryScreen();

    if (!screen)
        return "Unknown";

    const QSize size = screen->size();

    return QString("%1 x %2")
        .arg(size.width())
        .arg(size.height());
}

QString SystemInfo::primaryScreenScale()
{
    const QScreen *screen = QGuiApplication::primaryScreen();

    if (!screen)
        return "Unknown";

    return QString("%1%")
        .arg(qRound(screen->devicePixelRatio() * 100.0));
}


quint64 SystemInfo::totalMemory()
{
#ifdef Q_OS_WIN

    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);

    if (GlobalMemoryStatusEx(&memoryStatus))
        return memoryStatus.ullTotalPhys;

    return 0;

#elif defined(Q_OS_LINUX)

    struct sysinfo info;

    if (sysinfo(&info) == 0)
        return static_cast<quint64>(info.totalram) * info.mem_unit;

    return 0;

#elif defined(Q_OS_ANDROID)

    const long pageCount = sysconf(_SC_PHYS_PAGES);
    const long pageSize  = sysconf(_SC_PAGE_SIZE);

    if (pageCount > 0 && pageSize > 0)
        return static_cast<quint64>(pageCount) * pageSize;

    return 0;

#else

    return 0;

#endif
}

QString SystemInfo::totalMemoryString()
{
    return formatBytes(totalMemory());
}

QString SystemInfo::formatBytes(quint64 bytes)
{
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;
    constexpr double TB = GB * 1024.0;

    if (bytes >= TB)
    {
        return QString("%1 TB")
        .arg(bytes / TB, 0, 'f', 2);
    }

    if (bytes >= GB)
    {
        return QString("%1 GB")
        .arg(bytes / GB, 0, 'f', 2);
    }

    if (bytes >= MB)
    {
        return QString("%1 MB")
        .arg(bytes / MB, 0, 'f', 2);
    }

    if (bytes >= KB)
    {
        return QString("%1 KB")
        .arg(bytes / KB, 0, 'f', 2);
    }

    return QString("%1 B").arg(bytes);
}

QString SystemInfo::cpuModel()
{
#ifdef Q_OS_WIN

    HKEY hKey;

    constexpr LPCWSTR keyPath =
        L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                      keyPath,
                      0,
                      KEY_READ,
                      &hKey) != ERROR_SUCCESS)
    {
        return "Unknown";
    }

    wchar_t buffer[256];
    DWORD bufferSize = sizeof(buffer);

    const LONG result =
        RegQueryValueExW(hKey,
                         L"ProcessorNameString",
                         nullptr,
                         nullptr,
                         reinterpret_cast<LPBYTE>(buffer),
                         &bufferSize);

    RegCloseKey(hKey);

    if (result == ERROR_SUCCESS)
        return QString::fromWCharArray(buffer).trimmed();

    return "Unknown";

#elif defined(Q_OS_LINUX)

    QFile cpuInfo("/proc/cpuinfo");

    if (!cpuInfo.open(QIODevice::ReadOnly | QIODevice::Text))
        return "Unknown";

    while (!cpuInfo.atEnd())
    {
        const QString line =
            QString::fromUtf8(cpuInfo.readLine()).trimmed();

        if (line.startsWith("model name"))
        {
            const int index = line.indexOf(':');

            if (index != -1)
                return line.mid(index + 1).trimmed();
        }

        if (line.startsWith("Hardware"))
        {
            const int index = line.indexOf(':');

            if (index != -1)
                return line.mid(index + 1).trimmed();
        }
    }

    return architecture();

#elif defined(Q_OS_ANDROID)

    //
    // Android intentionally hides lots of hardware details.
    // Returning the architecture is usually the most reliable
    // cross-device fallback.
    //

    return architecture();

#else

    return "Unknown";

#endif
}
