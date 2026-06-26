import QtQuick
import QtQuick.Controls

Item
{
    id: root

    property real currentLevel: 0
    property real threshold: 0.15

    property bool showThreshold: true

    implicitWidth: 420
    implicitHeight: 22

    Rectangle
    {
        anchors.fill: parent

        radius: height / 2

        color: "#1E1F22"

        border.color: "#404249"

        Rectangle
        {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter

            width: Math.min(parent.width,
                            parent.width * (root.currentLevel / 0.5))

            height: parent.height

            radius: parent.radius

            color:
            {
                if(root.currentLevel < root.threshold)
                    return "#ED4245"

                return "#23A559"
            }

            Behavior on width
            {
                NumberAnimation
                {
                    duration: 70
                }
            }
        }

        Rectangle
        {
            visible: root.showThreshold

            width: 4
            height: parent.height + 8

            radius: 2

            x: parent.width *
               (root.threshold / 0.5)
               - width / 2

            y: -4

            color: "#FFFFFF"
        }
    }
}
