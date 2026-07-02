import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Popup
{
    id: root
    modal: true
    focus: true

    width: 420
    height: 480

    anchors.centerIn: Overlay.overlay
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle
    {
        color: "#313338"
        radius: 8
        border.color: "#1e1f22"
    }

    function connectToServer()
    {

    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Text
        {
            text: "Connect to Server"
            color: "white"
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text
        {
            text: "Enter the server address and port."
            color: "#b5bac1"
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
        }

        Item { Layout.fillHeight: true }

        Text
        {
            text: "SERVER IP"
            color: "#b5bac1"
            font.bold: true
            font.pixelSize: 12
        }

        TextField
        {
            id: serverIp
            Layout.fillWidth: true
            placeholderText: "127.0.0.1"
            placeholderTextColor: "white"
            color: "white"
            text:""
            onAccepted: buttonConnect.clicked()

            background: Rectangle
            {
                radius: 4
                color: "#1e1f22"
                border.color: parent.activeFocus ? "#5865f2" : "#111214"
            }
        }

        Text
        {
            text: "PORT"
            color: "#b5bac1"
            font.bold: true
            font.pixelSize: 12
        }

        TextField
        {
            id: serverPort

            Layout.fillWidth: true

            // placeholderText: "9987"
            // placeholderTextColor: "white"
            text: "9987"

            validator: IntValidator
            {
                bottom: 1
                top: 65535
            }

            color: "white"

            background: Rectangle
            {
                radius: 4
                color: "#1e1f22"
                border.color: parent.activeFocus ? "#5865f2" : "#111214"
            }
        }


        CheckBox
        {
            id: saveThisServerStatus

            checked: true

            indicator: Item {}    // Hide the default indicator

            contentItem: Row
            {
                spacing: 10

                Rectangle
                {
                    width: 18
                    height: 18
                    radius: 4

                    anchors.verticalCenter: parent.verticalCenter

                    color: saveThisServerStatus.checked ? "#5865F2" : "#1E1F22"
                    border.color: "#5865F2"

                    Text
                    {
                        anchors.centerIn: parent
                        text: "✓"
                        visible: saveThisServerStatus.checked
                        color: "white"
                        font.bold: true
                        font.pixelSize: 12
                    }
                }

                Text
                {
                    anchors.verticalCenter: parent.verticalCenter

                    text: "ADD TO MY SERVERS"
                    color: "white"
                    font.pixelSize: 14
                }
            }
        }


        Item { Layout.fillHeight: true }

        RowLayout
        {
            Layout.fillWidth: true

            Button
            {
                id:buttonCancel
                text: "Cancel"

                Layout.fillWidth: true

                onClicked: root.close()

                background: Rectangle
                {
                    radius: 4

                    color: buttonCancel.down
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
                id:buttonConnect
                text: "Connect"

                Layout.fillWidth: true

                onClicked:
                {
                    user.connectToServer( saveThisServerStatus.checked, serverIp.text, serverPort.text)
                    root.close()
                }

                background: Rectangle
                {
                    radius: 4

                    color: !buttonConnect.enabled
                           ? "#444"
                           : buttonConnect.down
                             ? "#4752C4"
                             : "#5865F2"
                }

                contentItem: Text
                {
                    text: parent.text
                    color: "white"
                    font.bold: true

                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}


