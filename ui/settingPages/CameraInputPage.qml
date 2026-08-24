import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import "../constants/"


Page
{
    id: root

    Material.theme: Material.Dark
    Material.accent: Theme.current.accent

    background: Rectangle
    {
        color: Theme.current.background
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

                color: Theme.current.text

                font.pixelSize: 28
                font.bold: true
            }

            Label
            {
                text: "Configure your camera."

                color: Theme.current.textSecondary

                font.pixelSize: 14
            }


            ///////////////////////////////////////////////////////
            // CAMERA
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: Theme.current.surface
                border.color: Theme.current.border

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

                        color: Theme.current.text

                        font.pixelSize: 18
                        font.bold: true
                    }

                    Label
                    {
                        text: "Choose which camera BeanChat should use."

                        color: Theme.current.textSecondary

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
