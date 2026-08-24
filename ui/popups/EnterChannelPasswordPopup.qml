import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import "../constants/"

Popup
{
    id: root

    modal: true
    focus: true

    width: 400
    height: 350

    anchors.centerIn: Overlay.overlay

    // Parameters
    property string channelName: ""
    property int channelId: -1
    property bool isTextChannel: false

    signal passwordEntered(int channelId, string password)

    //to assign function to action
    property var onOk: null


    background: Rectangle
    {
        color: Theme.current.background
        radius: 8
        border.color: Theme.current.input
    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Text
        {
            text: "Password Required"
            color: Theme.current.text
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text
        {
            text: "Enter password for channel:"
            color: Theme.current.textSecondary
            Layout.alignment: Qt.AlignHCenter
        }


        Text
        {
            text: root.channelName
            color: Theme.current.accent
            font.bold: true
            font.pixelSize: 18

            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: 250
            elide: Text.ElideRight
        }

        Item { Layout.fillHeight: true }

        Text
        {
            text: "PASSWORD"
            color: Theme.current.textSecondary
            font.bold: true
            font.pixelSize: 12
        }

        TextField
        {
            id: passwordField

            Layout.fillWidth: true

            echoMode: TextInput.Password
            placeholderText: "Enter channel password"
            placeholderTextColor: Theme.current.text

            color: Theme.current.text

            onAccepted: joinButton.clicked()

            background: Rectangle
            {
                radius: 4
                color: Theme.current.input
                border.color: parent.activeFocus ? Theme.current.accent : Theme.current.inputBorder
            }
        }

        Item { Layout.fillHeight: true }

        RowLayout
        {
            Layout.fillWidth: true

            Button
            {
                id:cancelButton
                text: "Cancel"
                Layout.fillWidth: true

                onClicked: root.close()

                background: Rectangle
                {
                    radius: 4

                    color: cancelButton.down
                           ? Theme.current.pressed
                           : "transparent"

                    border.width: 1
                    border.color: Theme.current.borderLight
                }

                contentItem: Text
                {
                    text: parent.text
                    color: Theme.current.text

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Button
            {
                id: joinButton

                text: "Join"
                Layout.fillWidth: true

                onClicked:
                {
                    root.passwordEntered(
                        root.channelId,
                        passwordField.text
                    )

                    if (onOk)
                        onOk(passwordField.text)

                    root.close()
                }

                background: Rectangle
                {
                    radius: 4

                    color: joinButton.down
                           ?  Theme.current.accentHover
                           : Theme.current.accent

                    border.width: 1
                    border.color: Theme.current.borderLight
                }

                contentItem: Text
                {
                    text: parent.text
                    color: Theme.current.text

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }

    onOpened:
    {
        passwordField.clear()
        passwordField.forceActiveFocus()
    }
}
