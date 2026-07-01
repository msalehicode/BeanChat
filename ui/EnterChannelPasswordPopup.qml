import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

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

    signal passwordEntered(int channelId, string password)

    //to assign function to action
    property var onOk: null


    background: Rectangle
    {
        color: "#313338"
        radius: 8
        border.color: "#1e1f22"
    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Text
        {
            text: "Password Required"
            color: "white"
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text
        {
            text: "Enter password for channel:"
            color: "#b5bac1"
            Layout.alignment: Qt.AlignHCenter
        }


        Text
        {
            text: root.channelName
            color: "#5865f2"
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
            color: "#b5bac1"
            font.bold: true
            font.pixelSize: 12
        }

        TextField
        {
            id: passwordField

            Layout.fillWidth: true

            echoMode: TextInput.Password
            placeholderText: "Enter channel password"
            placeholderTextColor: "white"

            color: "white"

            onAccepted: joinButton.clicked()

            background: Rectangle
            {
                radius: 4
                color: "#1e1f22"
                border.color: parent.activeFocus ? "#5865f2" : "#111214"
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
                           ? "#3F4147"
                           : "transparent"

                    border.width: 1
                    border.color: "#555"
                }

                contentItem: Text
                {
                    text: parent.text
                    color: "white"

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
                           ?  "#4752C4"
                           : "#5865F2"

                    border.width: 1
                    border.color: "#555"
                }

                contentItem: Text
                {
                    text: parent.text
                    color: "white"

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
