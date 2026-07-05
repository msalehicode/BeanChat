import QtQuick
import QtQuick.Controls

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
        color: "#313338"
        border.color: "#1E1F22"
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
            color: "white"
            font.pixelSize: 16
            font.bold: true
            elide: Text.ElideRight
        }

        Rectangle {
            width: parent.width
            height: 1
            color: "#404249"
        }

        //-----------------------------------------
        // Volume
        //-----------------------------------------

        Text {
            text: "Volume: " +
                  (root.clientUser
                       ? Math.round(root.clientUser.volume)
                       : 100) + "%"
            color: "#B5BAC1"
            font.pixelSize: 12
        }

        Slider {
            id: volumeSlider

            width: parent.width

            from: 0
            to: 100

            value: root.clientUser
                   ? root.clientUser.volume
                   : 100

            onMoved: {
                if (root.clientUser)
                    root.clientUser.volume = value
            }

            background: Rectangle {
                x: volumeSlider.leftPadding
                y: volumeSlider.topPadding + volumeSlider.availableHeight / 2 - height / 2

                width: volumeSlider.availableWidth
                height: 4

                radius: 2
                color: "#4E5058"

                Rectangle {
                    width: volumeSlider.visualPosition * parent.width
                    height: parent.height
                    radius: parent.radius
                    color: "#5865F2"
                }
            }

            handle: Rectangle {

                width: volumeSlider.pressed ? 16 : 14
                height: width

                radius: width / 2

                color: "white"

                border.width: 1
                border.color: "#DCDDDE"

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
            color: "#404249"
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
                  ? "Unmute for me"
                  : "Mute for me"

            onClicked: {
                if (root.clientUser)
                    root.clientUser.localMuted =
                            !root.clientUser.localMuted
            }

            contentItem: Text {
                text: muteButton.text
                color: "#F2F3F5"
                font.pixelSize: 14
                font.weight: Font.DemiBold

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                radius: 8

                color: muteButton.down
                       ? "#6D6F78"
                       : muteButton.hovered
                         ? "#555861"
                         : "#43464D"

                border.width: 1
                border.color: "#2B2D31"

                Behavior on color {
                    ColorAnimation { duration: 120 }
                }
            }
        }
        Rectangle {
            width: parent.width
            height: 1
            color: "#404249"
        }

    }
}
