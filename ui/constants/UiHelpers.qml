pragma Singleton

import QtQuick
import BeanChatClient 1.0

QtObject
{
    function statusColor(status)
    {
        switch (status)
        {
        case ClientUser.Online:
            return "lime"

        case ClientUser.Idle:
            return "orange"

        case ClientUser.Busy:
            return "red"

        case ClientUser.DoNotDisturb:
            return "#ff0066"

        case ClientUser.Offline:
        default:
            return "gray"
        }
    }

    function statusText(status)
    {
        switch (status)
        {
        case ClientUser.Online:
            return "Online"

        case ClientUser.Idle:
            return "Idle"

        case ClientUser.Busy:
            return "Busy"

        case ClientUser.DoNotDisturb:
            return "Do Not Disturb"

        case ClientUser.Offline:
        default:
            return "Offline"
        }
    }
}
