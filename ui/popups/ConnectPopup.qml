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
    height: 550

    anchors.centerIn: Overlay.overlay
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    onClosed:
    {
        //reset inputs
        serverCodeField.clear()
        serverIp.clear()
        serverPort.text="9987"
        saveThisServerStatus.checked;
        buttonConnect.enabled=false
    }

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
            text: "Connect to Server"
            color: "white"
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text
        {
            text: "Enter the serverCode or Address and Port to connect"
            color: "#b5bac1"
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
        }

        Item { Layout.fillHeight: true }

        Text
        {
            text: "SERVER CODE"
            color: "#b5bac1"
            font.bold: true
            font.pixelSize: 12
        }

        TextField
        {
            id: serverCodeField

            Layout.fillWidth: true

            placeholderText: "ABCD-EFGH-IJKL"
            placeholderTextColor: "#888"
            property bool isDecoded:false;

            color: "white"

            background: Rectangle
            {
                radius: 4
                color: "#1e1f22"
                border.color: parent.activeFocus ? "#5865F2" : "#111214"
            }
            onAccepted: serverCodeField.isDecoded===true ? buttonConnect.clicked() : null

            onTextChanged:
            {
                let result = serverCode.decode(text);

                serverCodeField.isDecoded=result.ok
                if (result.ok)
                {
                    serverIp.text = result.ip;
                    serverPort.text = result.port.toString();
                    buttonConnect.enabled=true
                }
            }
        }

        Item { Layout.fillHeight: true }

        //or seperator
        RowLayout
        {
            Layout.fillWidth: true
            spacing: 10

            Rectangle
            {
                Layout.fillWidth: true
                height: 4
                color: "#444"
            }

            Text
            {
                text: "OR"
                color: "#b5bac1"
                font.bold: true
            }

            Rectangle
            {
                Layout.fillWidth: true
                height: 4
                color: "#444"
            }
        }

        Item { Layout.fillHeight: true }


        //============================= enter ip and port or domain
        RowLayout
        {
            Layout.fillWidth: true
            spacing: 12

            ColumnLayout
            {
                Layout.fillWidth: true
                spacing: 4

                Text
                {
                    text: "ADDRESS"
                    color: "#b5bac1"
                    font.bold: true
                    font.pixelSize: 12
                }

                TextField
                {
                    id: serverIp

                    Layout.fillWidth: true

                    placeholderText: "example.com or 0.0.0.0"
                    placeholderTextColor: "#888"
                    color: "white"

                    onAccepted: buttonConnect.clicked()

                    background: Rectangle
                    {
                        radius: 4
                        color: "#1e1f22"
                        border.color: parent.activeFocus ? "#5865f2" : "#111214"
                    }

                    onTextChanged:
                    {
                        buttonConnect.enabled=true
                    }
                }
            }

            ColumnLayout
            {
                Layout.preferredWidth: 90
                spacing: 4

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

                    text: "9987"
                    placeholderText: "9987"
                    placeholderTextColor: "#888"

                    validator: IntValidator
                    {
                        bottom: 1
                        top: 65535
                    }

                    color: "white"

                    onAccepted: buttonConnect.clicked()

                    onTextChanged:
                    {
                        buttonConnect.enabled=true
                    }

                    background: Rectangle
                    {
                        radius: 4
                        color: "#1e1f22"
                        border.color: parent.activeFocus ? "#5865f2" : "#111214"
                    }
                }
            }
        }
        Item { Layout.fillHeight: true }


        //actions
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

        RowLayout
        {
            Layout.fillWidth: true

            Rectangle
            {
                id:buttonCancel
                Layout.fillWidth: true
                height: 38

                radius: 8

                color: cancelArea.containsMouse
                           ? "#3F4147"
                           : "transparent"

                border.width: 1
                border.color: "#555"

                Layout.alignment: Qt.AlignVCenter


                Text
                {
                    anchors.centerIn: parent
                    text: "Cancel"
                    color: "white"
                    font.bold: true
                }

                MouseArea
                {
                    id:cancelArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: root.close()
                }
            }


            Rectangle
            {
                id:buttonConnect
                Layout.fillWidth: true
                height: 38

                radius: 8
                enabled: false

                color: !buttonConnect.enabled
                       ? "#444"
                       : connectArea.containsMouse ? "#4752C4" : "#5865F2"

                border.width: 1
                border.color: "#555"

                Layout.alignment: Qt.AlignVCenter

                function clicked()
                {
                    if(!buttonConnect.enabled)
                        return;
                    user.connectToServer( saveThisServerStatus.checked, serverIp.text, serverPort.text)
                    root.close()
                }

                Text
                {
                    anchors.centerIn: parent
                    text: "Connect"
                    color: "white"
                    font.bold: true
                }

                MouseArea
                {
                    id:connectArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked:
                    {
                        buttonConnect.clicked()
                    }
                }
            }

        }
    }
}


