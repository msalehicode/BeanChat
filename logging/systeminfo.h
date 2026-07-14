#pragma once

#include <QString>

class SystemInfo
{
public:
    Q_DISABLE_COPY_MOVE(SystemInfo)

    // Returns the complete diagnostic report ready for logging.
    static QString diagnosticReport();

    //==========================================================================
    // Application
    //==========================================================================

    // static QString applicationVersion();
    static QString applicationBuildType();
    static QString qtVersion();

    //==========================================================================
    // Operating System
    //==========================================================================

    static QString operatingSystemName();
    static QString operatingSystemVersion();

    static QString kernelType();
    static QString kernelVersion();

    static QString architecture();
    static QString hostName();

    //==========================================================================
    // Hardware
    //==========================================================================

    static QString cpuModel();
    static int logicalCpuCount();

    static quint64 totalMemory();
    static QString totalMemoryString();

    //==========================================================================
    // Display
    //==========================================================================

    static QString primaryScreenResolution();
    static QString primaryScreenScale();

    //==========================================================================
    // Helpers
    //==========================================================================

    static QString formatBytes(quint64 bytes);

private:
    SystemInfo() = delete;
    ~SystemInfo() = delete;
};
