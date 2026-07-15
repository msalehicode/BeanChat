import QtQuick
import QtQuick.Controls
import BeanChatClient 1.0

Popup
{
    id: root

    signal statusSelected(int status)

    width: 200
    padding: 6

    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle
    {
        radius: 8
        color: "#111214"
        border.color: "#2b2d31"
        border.width: 1
    }

    enter: Transition
    {
        ParallelAnimation
        {
            NumberAnimation
            {
                property: "opacity"
                from: 0
                to: 1
                duration: 120
            }

            NumberAnimation
            {
                property: "scale"
                from: 0.95
                to: 1.0
                duration: 120
                easing.type: Easing.OutCubic
            }
        }
    }

    exit: Transition
    {
        ParallelAnimation
        {
            NumberAnimation
            {
                property: "opacity"
                to: 0
                duration: 100
            }

            NumberAnimation
            {
                property: "scale"
                to: 0.95
                duration: 100
            }
        }
    }

    Column
    {
        spacing: 2

        StatusItem
        {
            text: "🟢 Online"
            color: "#3ba55c"

            onClicked:
            {
                root.statusSelected(Presence.Online)
                root.close()
            }
        }

        StatusItem
        {
            text: "🌙 Away"
            color: "#faa61a"

            onClicked:
            {
                root.statusSelected(Presence.Away)
                root.close()
            }
        }

        StatusItem
        {
            text: "⛔ Busy"
            color: "#ed4245"

            onClicked:
            {
                root.statusSelected(Presence.Busy)
                root.close()
            }
        }

        StatusItem
        {
            text: "🔕 Do Not Disturb"
            color: "#ed4245"

            onClicked:
            {
                root.statusSelected(Presence.DoNotDisturb)
                root.close()
            }
        }

        StatusItem
        {
            text: "👻 Invisible"
            color: "#747f8d"

            onClicked:
            {
                root.statusSelected(Presence.Offline)
                root.close()
            }
        }
    }
}
