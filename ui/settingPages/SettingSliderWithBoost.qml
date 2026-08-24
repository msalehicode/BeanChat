import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../constants/"

Item
{
    id: root

    property alias value: slider.value

    signal moved(real value)

    implicitHeight: 40
    implicitWidth: 400

    RowLayout
    {
        anchors.fill: parent
        spacing: 12

        Slider
        {
            id: slider

            Layout.fillWidth: true

            from: 0
            to: 200

            onMoved:
                root.moved(value)

            background: Item
            {
                implicitHeight: 6

                Rectangle
                {
                    anchors.verticalCenter: parent.verticalCenter

                    width: parent.width
                    height: 6

                    radius: 3
                    color: Theme.current.border
                }

                // Boost area: 100% -> 200%
                Rectangle
                {
                    anchors.verticalCenter: parent.verticalCenter

                    x: parent.width * 0.5
                    width: parent.width * 0.5
                    height: 6

                    radius: 3
                    color: Theme.current.danger
                }

                // 100% divider
                Rectangle
                {
                    x: parent.width * 0.5 - 1

                    anchors.verticalCenter: parent.verticalCenter

                    width: 2
                    height: 10

                    color: Theme.current.text
                }

                // Current value
                Rectangle
                {
                    anchors.verticalCenter: parent.verticalCenter

                    x: 0
                    width: parent.width * slider.visualPosition
                    height: 6

                    radius: 3
                    color: slider.value > 100
                           ? Theme.current.danger
                           : Theme.current.icon
                }
            }
        }

        Label
        {
            Layout.preferredWidth: 75

            horizontalAlignment: Text.AlignRight

            color: slider.value > 100
                   ? Theme.current.danger
                   : Theme.current.textSecondary

            text:
            {
                if (slider.value > 100)
                    return "Boost " + Math.round(slider.value) + "%"

                return Math.round(slider.value) + "%"
            }
        }
    }
}
