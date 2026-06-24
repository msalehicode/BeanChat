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
            color: "white"
            text:"localhost" //for tests
            onAccepted: buttonConnect.clicked()

            background: Rectangle
            {
                radius: 4
                color: "#1e1f22"
                border.color: parent.activeFocus ? "#5865f2" : "#111214"
            }

            Button
            {
                text:"clear"
                onClicked: serverIp.clear()
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
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

            placeholderText: "9987"
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

        RowLayout
        {
            Text
            {
                text: "ADD TO MY SERVERS"
                color: "#b5bac1"
                font.bold: true
                font.pixelSize: 12
            }
            CheckBox
            {
                id:saveThisServerStatus
                checked: true
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
            }
        }
    }
}


