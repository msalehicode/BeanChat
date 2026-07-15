pragma Singleton

import QtQuick
import BeanChatClient 1.0

QtObject
{
    function statusColor(status)
    {
        switch (status)
        {
        case Presence.Online:
            return "lime"

        case Presence.Idle:
            return "orange"

        case Presence.Away:
            return "yellow"

        case Presence.Busy:
            return "red"

        case Presence.DoNotDisturb:
            return "#ff0066"

        case Presence.Offline:
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
            case Relationship.None: return "#acbbd9"
            case Relationship.Friend: return "lime"
        }
    }

    function statusText(status)
    {
        switch (status)
        {
        case Presence.Online:
            return "Online"

        case Presence.Idle:
            return "Idle"

        case Presence.Away:
            return "Away"

        case Presence.Busy:
            return "Busy"

        case Presence.DoNotDisturb:
            return "Do Not Disturb"

        case Presence.Offline:
        default:
            return "Offline"
        }
    }
}
