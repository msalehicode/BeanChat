import QtQuick
import QtQuick.Controls

import "../constants/"

ItemDelegate
{
    id: root

    property color color: Theme.current.text

    implicitWidth: 190
    implicitHeight: 38

    background: Rectangle
    {
        radius: 6
        color: root.hovered ? Theme.current.accentHover : "transparent"
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
            color: Theme.current.text
            font.pixelSize: 14
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
