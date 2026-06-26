import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Page
{
    id: root

    Material.theme: Material.Dark
    Material.accent: "#5865F2"

    background: Rectangle
    {
        color: "#313338"
    }

    Flickable
    {
        anchors.fill: parent

        clip: true

        contentWidth: width
        contentHeight: settingsColumn.implicitHeight + 40

        ScrollBar.vertical: ScrollBar {}

        Column
        {
            id: settingsColumn

            width: Math.min(parent.width - 80, 820)

            anchors.horizontalCenter: parent.horizontalCenter

            spacing: 24

            topPadding: 32
            bottomPadding: 40

            ///////////////////////////////////////////////////////
            // Header
            ///////////////////////////////////////////////////////

            Label
            {
                text: "Video"

                color: "white"

                font.pixelSize: 28
                font.bold: true
            }

            Label
            {
                text: "Configure your camera."

                color: "#B5BAC1"

                font.pixelSize: 14
            }


            ///////////////////////////////////////////////////////
            // CAMERA
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: "#2B2D31"

                border.color: "#404249"

                implicitHeight: cameraColumn.implicitHeight + 32

                Column
                {
                    id: cameraColumn

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    Label
                    {
                        text: "Camera"

                        color: "white"

                        font.pixelSize: 18
                        font.bold: true
                    }

                    Label
                    {
                        text: "Choose which camera BeanChat should use."

                        color: "#B5BAC1"

                        wrapMode: Text.WordWrap
                    }

                    SettingCombo
                    {
                        width: parent.width

                        model: camera.cameraInputsNames

                        currentIndex: camera.currentCameraInput

                        onActivated:
                        {
                            camera.currentCameraInput = currentIndex
                            settings.setValue
                            (
                                "CameraInput/Device",
                                camera.cameraIntputId(currentIndex)
                            )
                        }
                    }
                }
            }

        }
    }
}
