pragma Singleton

import QtQuick
import "../constants/"

QtObject
{
    // Queue of pending notifications
    property var queue: []

    // Currently displayed notification
    property bool showing: false
    property var currentNotification: null


    function show(notification)
    {
        // Ignore duplicate notifications with the same id
        if (notification.id !== undefined)
        {
            if (currentNotification && currentNotification.id === notification.id)
                return

            for (let i = 0; i < queue.length; ++i)
            {
                if (queue[i].id === notification.id)
                    return
            }
        }

        queue.push(notification)

        if (currentNotification === null)
            next()
    }

    function next()
    {
        if (queue.length === 0)
        {
            currentNotification = null
            showing = false
            return
        }

        currentNotification = queue.shift()
        showing = true
    }

    function close()
    {
        currentNotification = null
        showing = false
        next()
    }

    // Convenience functions

    function info(text, duration, id)
    {
        show({
            id: id,
            type: NotificationTypes.info,
            text: text,
            duration: duration || 2500
        })
    }

    function success(text, duration, id)
    {
        show({
            id: id,
            type: NotificationTypes.success,
            text: text,
            duration: duration || 2500
        })
    }

    function warning(text, duration, id)
    {
        show({
            id: id,
            type: NotificationTypes.warning,
            text: text,
            duration: duration || 3500
        })
    }

    function error(text, duration, id)
    {
        show({
            id: id,
            type: NotificationTypes.error,
            text: text,
            duration: duration || 5000
        })
    }
}
