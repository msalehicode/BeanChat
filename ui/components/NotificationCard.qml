import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../managers"
import "../constants"
Rectangle
{
    id: root

    property string text: ""
    property int type: 0

    color: Theme.current.surface
    radius: 8

    border.width: 1
    border.color: Theme.current.border

    width: 450
    implicitHeight: Math.max(56, message.implicitHeight + 24)

    Rectangle
    {
        width: 5
        radius: 8

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        color:
        {
            switch(root.type)
            {
                case NotificationTypes.info:
                    return Theme.current.info

                case NotificationTypes.success:
                    return Theme.current.success

                case NotificationTypes.warning:
                    return Theme.current.warning

                case NotificationTypes.error:
                    return Theme.current.danger

                default:
                    return Theme.current.info
            }
        }
    }

    RowLayout
    {
        anchors.fill: parent

        anchors.leftMargin: 16
        anchors.rightMargin: 10

        spacing: 12

        Label
        {
            font.pixelSize: 20

            color:
            {
                switch(root.type)
                {
                    case NotificationTypes.info:
                        return Theme.current.info

                    case NotificationTypes.success:
                        return Theme.current.success

                    case NotificationTypes.warning:
                        return Theme.current.warning

                    case NotificationTypes.error:
                        return Theme.current.danger

                    default:
                        return Theme.current.text
                }
            }

            text:
            {
                switch(root.type)
                {
                case NotificationTypes.info: return "ℹ"
                case NotificationTypes.success: return "✔"
                case NotificationTypes.warning: return "⚠"
                case NotificationTypes.error: return "✖"
                default: return ""
                }
            }
        }

        Label
        {
            id: message

            Layout.fillWidth: true

            color: Theme.current.text

            text: root.text

            wrapMode: Text.WordWrap

            font.pixelSize: 15
        }

        Rectangle
        {
            id: closeButton

            width: 28
            height: 28
            radius: 14

            color: mouse.containsMouse
                   ? Theme.current.hover
                   : "transparent"

            Text
            {
                anchors.centerIn: parent

                text: "✕"

                color: mouse.containsMouse
                       ? Theme.current.iconActive
                       : Theme.current.iconMuted

                font.pixelSize: 15
            }

            MouseArea
            {
                id: mouse

                anchors.fill: parent

                hoverEnabled: true

                cursorShape: Qt.PointingHandCursor

                onClicked: NotificationManager.close()
            }
        }
    }
}
