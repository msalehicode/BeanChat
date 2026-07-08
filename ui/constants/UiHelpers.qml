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

    function relationColor(relation)
    {
        switch(relation)
        {
            case Relationship.None: return "white"
            case Relationship.Blocked: return "#FF7070"
            case Relationship.Friend: return "lime"
        }
    }

    function isTalkingColor(isMuted=false,relation=Relationship.None)
    {
        if(isMuted)
            return "#FF0000"

        switch(relation)
        {
            case Relationship.Blocked: return "#FF7070"
            case Relationship.None: return "yellow"
            case Relationship.Friend: return "lime"
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
