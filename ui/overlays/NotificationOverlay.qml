import QtQuick
import QtQuick.Controls

import "../managers"
import "../components"
import "../constants"

Item
{
    id: root

    anchors.fill: parent


    NotificationCard
    {
        id: notification

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 20

        visible: NotificationManager.currentNotification !== null

        opacity: 0

        y: -20

        text: NotificationManager.currentNotification ?
                  NotificationManager.currentNotification.text : ""

        type: NotificationManager.currentNotification ?
                  NotificationManager.currentNotification.type : NotificationTypes.info
    }

    Timer
    {
        id: hideTimer

        repeat: false

        onTriggered:
        {
            hideAnimation.start()
        }
    }

    Connections
    {
        target: NotificationManager

        function onCurrentNotificationChanged()
        {
            if(NotificationManager.currentNotification === null)
                return

            notification.opacity = 0
            notification.y = -20

            showAnimation.start()

            hideTimer.interval =
                    NotificationManager.currentNotification.duration

            hideTimer.start()
        }
    }

    ParallelAnimation
    {
        id: showAnimation

        NumberAnimation
        {
            target: notification

            property: "opacity"

            from: 0
            to: 1

            duration: 180
        }

        NumberAnimation
        {
            target: notification

            property: "y"

            from: -20
            to: 20

            duration: 180
        }
    }

    ParallelAnimation
    {
        id: hideAnimation

        NumberAnimation
        {
            target: notification

            property: "opacity"

            from: 1
            to: 0

            duration: 180
        }

        NumberAnimation
        {
            target: notification

            property: "y"

            from: 20
            to: -20

            duration: 180
        }

        onFinished:
        {
            NotificationManager.close()
        }
    }
}
