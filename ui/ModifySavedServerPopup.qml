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


    property int serverId:-1;
    property int dbIndex: -1
    property string currentName: ""
    property string currentIp: ""
    property string currentPort: ""

    property alias serverName: nameField.text
    property alias ip: ipField.text
    property alias port: portField.text

    signal saveClicked()

    // background: Rectangle
    // {
    //     radius: 10
    //     color: "#313338"
    //     border.color: "#1E1F22"
    //     border.width: 1
    // }




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
            color: "#313338"

            Column
            {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 6

                Text
                {
                    text: "Modify Server"
                    color: "white"
                    font.pixelSize: 22
                    font.bold: true
                }

                Text
                {
                    text: "Update this server's information."
                    color: "#B5BAC1"
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
                    color: "#B5BAC1"
                    font.bold: true
                    font.pixelSize: 11
                }

                TextField
                {
                    id: nameField

                    Layout.fillWidth: true
                    height: 40

                    color: "white"
                    placeholderText: "BeanChat Server"
                    text:currentName

                    background: Rectangle
                    {
                        radius: 6
                        color: "#1E1F22"
                        border.width: nameField.activeFocus ? 2 : 1
                        border.color: nameField.activeFocus
                                      ? "#5865F2"
                                      : "#202225"
                    }
                }

                Label
                {
                    text: "IP ADDRESS"
                    color: "#B5BAC1"
                    font.bold: true
                    font.pixelSize: 11
                }

                TextField
                {
                    id: ipField

                    Layout.fillWidth: true
                    height: 40

                    color: "white"
                    text: currentIp
                    background: Rectangle
                    {
                        radius: 6
                        color: "#1E1F22"
                        border.width: ipField.activeFocus ? 2 : 1
                        border.color: ipField.activeFocus
                                      ? "#5865F2"
                                      : "#202225"
                    }
                }

                Label
                {
                    text: "PORT"
                    color: "#B5BAC1"
                    font.bold: true
                    font.pixelSize: 11
                }

                TextField
                {
                    id: portField

                    Layout.fillWidth: true
                    height: 40

                    color: "white"
                    text:Number(currentPort)
                    // inputMethodHints: Qt.ImhDigitsOnly

                    background: Rectangle
                    {
                        radius: 6
                        color: "#1E1F22"
                        border.width: portField.activeFocus ? 2 : 1
                        border.color: portField.activeFocus
                                      ? "#5865F2"
                                      : "#202225"
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

            color: "#2B2D31"

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

                    id: saveButton
                }
            }
        }
    }
}
