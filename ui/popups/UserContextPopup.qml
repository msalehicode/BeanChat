import QtQuick
import QtQuick.Controls

import "../constants/"

Popup {
    id: root

    property int userId: 0
    property string username: ""
    property var clientUser: null

    modal: false
    focus: true
    padding: 0

    width: 270
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle {
        radius: 8
        color: Theme.current.background
        border.color: Theme.current.border
        border.width: 1
    }

    Column {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        //-----------------------------------------
        // Username
        //-----------------------------------------

        Text {
            text: root.username
            color: Theme.current.text
            font.pixelSize: 16
            font.bold: true
            elide: Text.ElideRight
        }

        Rectangle {
            width: parent.width
            height: 1
            color: Theme.current.border
        }

        //-----------------------------------------
        // Volume
        //-----------------------------------------

        Text {
            text: "Volume: " +
                  (root.clientUser
                       ? Math.round(root.clientUser.volume)
                       : 100) + "%"
            color: Theme.current.textSecondary
            font.pixelSize: 12
        }

        Slider {
            id: volumeSlider

            width: parent.width
            height: 50
            from: 0
            to: 100

            value: root.clientUser
                   ? root.clientUser.volume
                   : 100

            onMoved:
            {
                if (root.clientUser)
                {
                    logger.action("changing user's voice volume")
                    root.clientUser.volume = value
                    relationshipManager.setVoiceVolume(root.clientUser.identity, value)
                }
            }

            background: Rectangle
            {
                x: volumeSlider.leftPadding
                y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2

                width: volumeSlider.availableWidth
                height: 4

                radius: 2
                color: Theme.current.borderLight

                Rectangle {
                    width: volumeSlider.visualPosition * parent.width
                    height: parent.height
                    radius: parent.radius
                    color: Theme.current.accent
                }
            }

            handle: Rectangle {

                width: volumeSlider.pressed ? 16 : 14
                height: width

                radius: width / 2

                color: Theme.current.text

                border.width: 1
                border.color: Theme.current.borderLight

                x: volumeSlider.leftPadding
                   + volumeSlider.visualPosition
                   * (volumeSlider.availableWidth - width)

                y: volumeSlider.topPadding
                   + volumeSlider.availableHeight / 2
                   - height / 2

                Behavior on width {
                    NumberAnimation {
                        duration: 80
                    }
                }

                Behavior on height {
                    NumberAnimation {
                        duration: 80
                    }
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 1
            color: Theme.current.border
        }

        //-----------------------------------------
        // Local mute
        //-----------------------------------------

        Button {
            id: muteButton

            width: parent.width
            height: 42

            text: root.clientUser &&
                  root.clientUser.localMuted
                  ? "Unmute"
                  : "Mute"

            onClicked:
            {
                if (root.clientUser)
                {
                    logger.action("localMute/unmute user")
                    root.clientUser.localMuted = !root.clientUser.localMuted
                    relationshipManager.setMuted(root.clientUser.identity, root.clientUser.localMuted)
                }
            }

            contentItem: Text {
                text: muteButton.text
                color: Theme.current.text
                font.pixelSize: 14
                font.weight: Font.DemiBold

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                radius: 8

                color: muteButton.down
                       ? Theme.current.pressed
                       : muteButton.hovered
                         ? Theme.current.hover
                         : Theme.current.surface

                border.width: 1
                border.color: Theme.current.border

                Behavior on color {
                    ColorAnimation { duration: 120 }
                }
            }
        }
        Rectangle {
            width: parent.width
            height: 1
            color: Theme.current.border
        }

    }
}
