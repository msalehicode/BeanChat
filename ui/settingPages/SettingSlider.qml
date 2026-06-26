import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item
{
    id: root

    property alias from: slider.from
    property alias to: slider.to
    property alias value: slider.value
    property alias stepSize: slider.stepSize

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

            onValueChanged:
                root.moved(value)
        }

        Label
        {
            Layout.preferredWidth: 45

            horizontalAlignment: Text.AlignRight

            color: "#B5BAC1"

            text:
            {
                if (slider.to === slider.from)
                    return "0%"

                return Math.round(
                    (slider.value - slider.from) /
                    (slider.to - slider.from) * 100
                ) + "%"
            }
        }
    }
}
