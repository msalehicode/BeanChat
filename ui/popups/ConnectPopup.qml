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
        color: Theme.current.background
        radius: 8
        border.color: Theme.current.border
    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Text
        {
            text: "Connect to Server"
            color: Theme.current.text
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        Text
        {
            text: "Enter the serverCode or Address and Port to connect"
            color: Theme.current.textSecondary
            font.pixelSize: 14
            Layout.alignment: Qt.AlignHCenter
        }

        Item { Layout.fillHeight: true }

        Text
        {
            text: "SERVER CODE"
            color: Theme.current.textSecondary
            font.bold: true
            font.pixelSize: 12
        }

        TextField
        {
            id: serverCodeField

            Layout.fillWidth: true

            placeholderText: "ABCD-EFGH-IJKL"
            placeholderTextColor: Theme.current.inputPlaceholder
            property bool isDecoded:false;

            color: Theme.current.text

            background: Rectangle
            {
                radius: 4
                color: Theme.current.input
                border.color: parent.activeFocus
                              ? Theme.current.accent
                              : Theme.current.inputBorder
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
                color: Theme.current.border
            }

            Text
            {
                text: "OR"
                color: Theme.current.textSecondary
                font.bold: true
            }

            Rectangle
            {
                Layout.fillWidth: true
                height: 4
                color: Theme.current.border
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
                    color: Theme.current.textSecondary
                    font.bold: true
                    font.pixelSize: 12
                }

                TextField
                {
                    id: serverIp

                    Layout.fillWidth: true

                    placeholderText: "example.com or 0.0.0.0"
                    placeholderTextColor: Theme.current.inputPlaceholder
                    color: Theme.current.text

                    onAccepted: buttonConnect.clicked()

                    background: Rectangle
                    {
                        radius: 4
                        color: Theme.current.input
                        border.color: parent.activeFocus
                                      ? Theme.current.accent
                                      : Theme.current.inputBorder
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
                    color: Theme.current.textSecondary
                    font.bold: true
                    font.pixelSize: 12
                }

                TextField
                {
                    id: serverPort

                    Layout.fillWidth: true

                    text: "9987"
                    placeholderText: "9987"
                    placeholderTextColor: Theme.current.inputPlaceholder

                    validator: IntValidator
                    {
                        bottom: 1
                        top: 65535
                    }

                    color: Theme.current.text

                    onAccepted: buttonConnect.clicked()

                    onTextChanged:
                    {
                        buttonConnect.enabled=true
                    }

                    background: Rectangle
                    {
                        radius: 4
                        color: Theme.current.input
                        border.color: parent.activeFocus ?
                                          Theme.current.accent
                                        : Theme.current.inputBorder
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

                    color: saveThisServerStatus.checked ? Theme.current.accent : Theme.current.input
                    border.color: Theme.current.accent

                    Text
                    {
                        anchors.centerIn: parent
                        text: "✓"
                        visible: saveThisServerStatus.checked
                        color: Theme.current.selectionText
                        font.bold: true
                        font.pixelSize: 12
                    }
                }

                Text
                {
                    anchors.verticalCenter: parent.verticalCenter

                    text: "ADD TO MY SERVERS"
                    color: Theme.current.text
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
                           ? Theme.current.hover
                           : "transparent"

                border.width: 1
                border.color: Theme.current.borderLight

                Layout.alignment: Qt.AlignVCenter


                Text
                {
                    anchors.centerIn: parent
                    text: "Cancel"
                    color: Theme.current.text
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
                       ? Theme.current.textDisabled
                       : connectArea.containsMouse ? Theme.current.accentHover : Theme.current.accentHover

                border.width: 1
                border.color: Theme.current.borderLight

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
                    color: Theme.current.text
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


