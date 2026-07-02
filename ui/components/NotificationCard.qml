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

    color: "#2b2d31"
    radius: 8

    border.width: 1
    border.color: "#3a3d45"

    // width: Math.min(450, implicitWidth)
    width:  450
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
                case NotificationTypes.info: return "#3498db"
                case NotificationTypes.success: return "#2ecc71"
                case NotificationTypes.warning: return "#f1c40f"
                case NotificationTypes.error: return "#e74c3c"
                default: return "#3498db"
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
                case NotificationTypes.info: return "#3498db"
                case NotificationTypes.success: return "#2ecc71"
                case NotificationTypes.warning: return "#f1c40f"
                case NotificationTypes.error: return "#e74c3c"
                default: return "white"
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

            color: "white"

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

            color: mouse.containsMouse ? "#3b3d43" : "transparent"

            Text
            {
                anchors.centerIn: parent

                text: "✕"

                color: mouse.containsMouse ? "white" : "#b9bbbe"

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
