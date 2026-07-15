import QtQuick
import QtQuick.Controls

ItemDelegate
{
    id: root

    property color color: "white"

    implicitWidth: 190
    implicitHeight: 38

    background: Rectangle
    {
        radius: 6
        color: root.hovered ? "#4752C4" : "transparent"
    }

    contentItem: Row
    {
        spacing: 10

        Rectangle
        {
            width: 10
            height: 10
            radius: 5
            color: root.color

            anchors.verticalCenter: parent.verticalCenter
        }

        Text
        {
            text: root.text
            color: "white"
            font.pixelSize: 14
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
