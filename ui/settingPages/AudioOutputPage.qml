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
                text: "Audio Output"

                color: "white"

                font.pixelSize: 28
                font.bold: true
            }

            Label
            {
                text: "Configure your output"

                color: "#B5BAC1"

                font.pixelSize: 14
            }

            ///////////////////////////////////////////////////////
            // OUTPUT DEVICE
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: "#2B2D31"

                border.color: "#404249"

                implicitHeight: content.implicitHeight + 32

                Column
                {
                    id: content

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    Label
                    {
                        text: "Output Device"

                        color: "white"

                        font.pixelSize: 18
                        font.bold: true
                    }

                    Label
                    {
                        text: "Choose where incoming voice will be played."

                        color: "#B5BAC1"

                        wrapMode: Text.WordWrap
                    }

                    SettingCombo
                    {
                        id:audioOutputCurrentDevice
                        width: parent.width

                        model: speaker.audioOutputNames

                        currentIndex: speaker.currentAudioOutput

                        onActivated:
                        {
                            speaker.currentAudioOutput = currentIndex
                            settings.setValue
                            (
                                "AudioOutput/Device",
                                speaker.audioOutputId(currentIndex)
                            )
                        }
                    }

                    Label
                    {
                        text: "Interface Effects Volume"

                        color: "white"

                        font.bold: true
                    }

                    SettingSlider
                    {
                        id:effectVolume
                        width: parent.width

                        from: 0
                        to: 1

                        value: soundManager.volume

                        onMoved:
                        {
                            soundManager.volume = value
                            settings.setValue("AudioOutput/EffectsVolume", value)
                        }
                    }
                }
            }
        }
    }
}
