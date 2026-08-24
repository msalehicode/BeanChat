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
    height: 480

    anchors.centerIn: Overlay.overlay

    background: Rectangle
    {
        color: Theme.current.background
        radius: 8
        border.color: Theme.current.surface3
    }


    property int serverId:-1;
    property int dbIndex: -1
    property string currentName: ""
    property string currentIp: ""
    property string currentPort: ""

    property alias serverName: nameField.text
    property alias ip: ipField.text
    property alias port: portField.text

    signal saveClicked()


    enter: Transition
    {
        ParallelAnimation
        {
            NumberAnimation
            {
                property: "opacity"
                from: 0
                to: 1
                duration: 120
            }

            NumberAnimation
            {
                property: "scale"
                from: 0.97
                to: 1.0
                duration: 120
            }
        }
    }

    exit: Transition
    {
        NumberAnimation
        {
            property: "opacity"
            to: 0
            duration: 100
        }
    }

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 0

        //
        // Header
        //
        Rectangle
        {
            Layout.fillWidth: true
            height: 82
            color: Theme.current.background

            Column
            {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 6

                Text
                {
                    text: "Modify Server"
                    color: Theme.current.text
                    font.pixelSize: 22
                    font.bold: true
                }

                Text
                {
                    text: "Update this server's information."
                    color: Theme.current.textSecondary
                    font.pixelSize: 13
                }
            }
        }

        //
        // Body
        //
        Item
        {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout
            {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.topMargin: 8
                anchors.bottomMargin: 16

                spacing: 14

                Label
                {
                    text: "SERVER NAME"
                    color: Theme.current.textSecondary
                    font.bold: true
                    font.pixelSize: 11
                }

                TextField
                {
                    id: nameField

                    Layout.fillWidth: true
                    height: 40

                    color: Theme.current.text
                    placeholderText: "BeanChat Server"
                    placeholderTextColor: Theme.current.text
                    text:currentName

                    background: Rectangle
                    {
                        radius: 6
                        color: Theme.current.surface3
                        border.width: nameField.activeFocus ? 2 : 1
                        border.color: nameField.activeFocus
                                      ? Theme.current.accent
                                      : Theme.current.inputBorder
                    }
                }

                Label
                {
                    text: "IP ADDRESS"
                    color: Theme.current.textSecondary
                    font.bold: true
                    font.pixelSize: 11
                }

                TextField
                {
                    id: ipField

                    Layout.fillWidth: true
                    height: 40

                    color: Theme.current.text
                    text: currentIp
                    background: Rectangle
                    {
                        radius: 6
                        color: Theme.current.surface3
                        border.width: ipField.activeFocus ? 2 : 1
                        border.color: ipField.activeFocus
                                      ? Theme.current.accent
                                      : Theme.current.inputBorder
                    }
                }

                Label
                {
                    text: "PORT"
                    color: Theme.current.textSecondary
                    font.bold: true
                    font.pixelSize: 11
                }

                TextField
                {
                    id: portField

                    Layout.fillWidth: true
                    height: 40

                    color: Theme.current.text
                    text:Number(currentPort)
                    // inputMethodHints: Qt.ImhDigitsOnly

                    background: Rectangle
                    {
                        radius: 6
                        color: Theme.current.surface3
                        border.width: portField.activeFocus ? 2 : 1
                        border.color: portField.activeFocus
                                      ? Theme.current.accent
                                      : Theme.current.inputBorder
                    }
                }

                Item
                {
                    Layout.fillHeight: true
                }
            }
        }

        //
        // Footer
        //
        Rectangle
        {
            Layout.fillWidth: true
            height: 64

            color: Theme.current.surface

            RowLayout
            {
                anchors.fill: parent
                anchors.margins: 16

                Item
                {
                    Layout.fillWidth: true
                }

                Button
                {
                    text: "Cancel"

                    onClicked: root.close()

                    background: Rectangle
                    {
                        radius: 4
                        color: cancelButton.down
                               ? Theme.current.pressed
                               : "transparent"

                        border.width: 1
                        border.color: Theme.current.border
                    }

                    contentItem: Text
                    {
                        text: parent.text
                        color: Theme.current.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    id: cancelButton
                }

                Button
                {
                    text: "Save Changes"

                    onClicked:
                    {
                        root.saveClicked()
                        root.close()
                    }

                    background: Rectangle
                    {
                        radius: 4
                        color: saveButton.down
                               ? Theme.current.accentHover
                               : Theme.current.accent
                    }

                    contentItem: Text
                    {
                        text: parent.text
                        color: Theme.current.text
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    id: saveButton
                }
            }
        }
    }
}
