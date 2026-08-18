import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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
                    color: "#3A3D44"
                }

                // Boost area: 100% -> 200%
                Rectangle
                {
                    anchors.verticalCenter: parent.verticalCenter

                    x: parent.width * 0.5
                    width: parent.width * 0.5
                    height: 6

                    radius: 3
                    color: "#D94A4A"
                }

                // 100% divider
                Rectangle
                {
                    x: parent.width * 0.5 - 1

                    anchors.verticalCenter: parent.verticalCenter

                    width: 2
                    height: 10

                    color: "#FFFFFF"
                }

                // Current value
                Rectangle
                {
                    anchors.verticalCenter: parent.verticalCenter

                    x: 0
                    width: parent.width * slider.visualPosition
                    height: 6

                    radius: 3
                    color: slider.value > 100 ? "#FF5555" : "#B5BAC1"
                }
            }
        }

        Label
        {
            Layout.preferredWidth: 75

            horizontalAlignment: Text.AlignRight

            color: slider.value > 100
                   ? "#FF5555"
                   : "#B5BAC1"

            text:
            {
                if (slider.value > 100)
                    return "Boost " + Math.round(slider.value) + "%"

                return Math.round(slider.value) + "%"
            }
        }
    }
}
