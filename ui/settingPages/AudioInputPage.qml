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
                text: "Audio Input"

                color: "white"

                font.pixelSize: 28
                font.bold: true
            }

            Label
            {
                text: "Configure your input"

                color: "#B5BAC1"

                font.pixelSize: 14
            }

            ///////////////////////////////////////////////////////
            // INPUT DEVICE
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: "#2B2D31"

                border.color: "#404249"

                implicitHeight: inputColumn.implicitHeight + 32

                Column
                {
                    id: inputColumn

                    anchors.fill: parent

                    anchors.margins: 16

                    spacing: 14

                    Label
                    {
                        text: "Input Device"

                        color: "white"

                        font.pixelSize: 18
                        font.bold: true
                    }

                    Label
                    {
                        text: "Choose the microphone BeanChat should use."

                        color: "#B5BAC1"

                        wrapMode: Text.WordWrap
                    }

                    SettingCombo
                    {
                        width: parent.width

                        model: microphone.audioInputNames

                        currentIndex: microphone.currentAudioInput

                        onActivated:
                        {
                            microphone.currentAudioInput = currentIndex
                            settings.setValue
                            (
                                "AudioInput/Device",
                                microphone.audioInputId(currentIndex)
                            )
                        }
                    }

                    Label
                    {
                        text: "Microphone Level"

                        color: "white"

                        font.bold: true
                    }

                    MicLevelMeter
                    {
                        width: parent.width

                        currentLevel: microphone.currentVolume

                        threshold: microphone.volumeGateThreshold

                        showThreshold: microphone.volumeGateStatus
                    }
                }
            }
            ///////////////////////////////////////////////////////
            // NOISE SUPPRESSION
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: "#2B2D31"

                border.color: "#404249"

                implicitHeight: noiseColumn.implicitHeight + 32

                Column
                {
                    id: noiseColumn

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    Label
                    {
                        text: "Noise Suppression"

                        color: "white"

                        font.pixelSize: 18
                        font.bold: true
                    }

                    Label
                    {
                        text: "Reduce keyboard clicks and background sounds."

                        color: "#B5BAC1"

                        wrapMode: Text.WordWrap
                    }

                    Item
                    {
                        width: parent.width
                        height: 28

                        SettingSwitch
                        {
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter

                            checked: microphone.rnnoiseStatus

                            onToggled:
                            {
                                microphone.rnnoiseStatus = checked
                                settings.setValue("AudioInput/NoiseSuppression", checked)
                            }
                        }
                    }

                    SettingSlider
                    {
                        visible: microphone.rnnoiseStatus

                        width: parent.width

                        from: 0
                        to: 1

                        value: microphone.rnnoiseValue

                        onMoved:
                        {
                            microphone.rnnoiseValue = value
                            settings.setValue("AudioInput/NoiseSuppressionValue", value)
                        }
                    }
                }
            }

            ///////////////////////////////////////////////////////
            // INPUT SENSITIVITY
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: "#2B2D31"

                border.color: "#404249"

                implicitHeight: gateColumn.implicitHeight + 32

                Column
                {
                    id: gateColumn

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    Label
                    {
                        text: "Input Sensitivity"

                        color: "white"

                        font.pixelSize: 18
                        font.bold: true
                    }

                    Label
                    {
                        text: "Automatically transmit when your microphone reaches the selected level."

                        color: "#B5BAC1"

                        wrapMode: Text.WordWrap
                    }

                    Item
                    {
                        width: parent.width
                        height: 28

                        SettingSwitch
                        {
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter

                            checked: microphone.volumeGateStatus

                            onToggled:
                            {
                                microphone.volumeGateStatus = checked
                                microphone.pushToTalkStatus = !checked
                                settings.setValue("AudioInput/VolumeGate", checked)
                                settings.setValue("AudioInput/PushToTalk", !checked)
                            }
                        }
                    }

                    MicLevelMeter
                    {
                        visible: microphone.volumeGateStatus

                        width: parent.width

                        currentLevel: microphone.currentVolume

                        threshold: microphone.volumeGateThreshold
                    }

                    SettingSlider
                    {
                        visible: microphone.volumeGateStatus

                        width: parent.width

                        from: 0
                        to: 0.5

                        value: microphone.volumeGateThreshold

                        onMoved:
                        {
                            microphone.volumeGateThreshold = value
                            settings.setValue("AudioInput/VolumeGateThreshold", value)
                        }
                    }
                }
            }

            ///////////////////////////////////////////////////////
            // PUSH TO TALK
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: "#2B2D31"

                border.color: "#404249"

                implicitHeight: pttColumn.implicitHeight + 32

                Column
                {
                    id: pttColumn

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    Label
                    {
                        text: "Push To Talk"

                        color: "white"

                        font.pixelSize: 18
                        font.bold: true
                    }

                    Label
                    {
                        text: "Transmit only while holding your selected hotkey."

                        color: "#B5BAC1"

                        wrapMode: Text.WordWrap
                    }

                    Item
                    {
                        width: parent.width
                        height: 28

                        SettingSwitch
                        {
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter

                            checked: microphone.pushToTalkStatus

                            onToggled:
                            {
                                microphone.pushToTalkStatus = checked
                                microphone.volumeGateStatus = !checked
                                settings.setValue("AudioInput/PushToTalk", checked)
                                settings.setValue("AudioInput/VolumeGate", !checked)
                            }
                        }
                    }

                    HotkeyCapture
                    {
                        width: parent.width

                        key: microphone.pushToTalkKey

                        keyString: microphone.pushToTalkKeyString

                        onKeySelected: function(k)
                        {
                            microphone.pushToTalkKey = k
                            microphone.pushToTalkModifiers = 0 //for now modifiers (combined keys are disabled)

                            microphone.initialPushToTalkHotkey()
                            settings.setValue("AudioInput/PushToTalkHotkey", k)
                        }
                    }
                }
            }


            ///////////////////////////////////////////////////////
            // Footer
            ///////////////////////////////////////////////////////

            Label
            {
                width: parent.width

                horizontalAlignment: Text.AlignHCenter

                color: "#80848E"

                font.pixelSize: 12

                text: "Note: hotkey and noise suppersion may don't work properly at the time."
            }

            Item
            {
                width: 1
                height: 24
            }
        }
    }
}
