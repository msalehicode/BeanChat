import QtQuick
import QtQuick.Controls

Item
{
    id: root

    property bool checked: false
    signal toggled(bool checked)

    implicitWidth: 42
    implicitHeight: 24

    MouseArea
    {
        anchors.fill: parent

        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        onClicked:
        {
            root.checked = !root.checked
            root.toggled(root.checked)
        }
    }

    Rectangle
    {
        id: track

        anchors.fill: parent

        radius: height / 2

        color: root.checked ? "#23A559" : "#4E5058"

        Behavior on color
        {
            ColorAnimation
            {
                duration: 150
            }
        }
    }

    Rectangle
    {
        id: thumb

        width: 18
        height: 18

        radius: 9

        anchors.verticalCenter: parent.verticalCenter

        x: root.checked
           ? parent.width - width - 3
           : 3

        color: "white"

        Behavior on x
        {
            NumberAnimation
            {
                duration: 150
                easing.type: Easing.OutCubic
            }
        }
    }
}
