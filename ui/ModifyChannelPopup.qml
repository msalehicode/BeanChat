import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Popup
{
    id: root

    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    width: 440
    height: 450

    anchors.centerIn: Overlay.overlay

    property int targetChannelId:-1;
    property string initialChannelName: ""
    property string initialChannelPassword: ""
    property bool initialSaveChats: true

    property alias channelName: channelNameField.text
    property alias channelPassword: passwordField.text
    property alias saveChats: saveChatsCheck.checked

    signal saveClicked()
    signal deleteClicked()

    background: Rectangle
    {
        color: "#313338"
        radius: 8
        border.color: "#1E1F22"
    }

    onOpened:
    {
        channelNameField.text = initialChannelName
        passwordField.text = initialChannelPassword
        saveChatsCheck.checked = initialSaveChats

        channelNameField.forceActiveFocus()
    }

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
                    text: "Channel Settings"
                    color: "white"
                    font.pixelSize: 22
                    font.bold: true
                }

                Text
                {
                    text: "Edit your voice channel settings."
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
                    text: "CHANNEL NAME"
                    color: "#B5BAC1"
                    font.bold: true
                    font.pixelSize: 11
                }

                TextField
                {
                    id: channelNameField

                    Layout.fillWidth: true
                    height: 40
                    onAccepted: saveButton.clicked()

                    color: "white"
                    placeholderText: "General"

                    background: Rectangle
                    {
                        radius: 6
                        color: "#1E1F22"

                        border.width: channelNameField.activeFocus ? 2 : 1
                        border.color: channelNameField.activeFocus
                                      ? "#5865F2"
                                      : "#202225"
                    }
                }

                Label
                {
                    text: "CHANNEL PASSWORD"
                    color: "#B5BAC1"
                    font.bold: true
                    font.pixelSize: 11
                }

                TextField
                {
                    id: passwordField

                    Layout.fillWidth: true
                    height: 40
                    onAccepted: saveButton.clicked()

                    color: "white"

                    placeholderText: "Leave empty to keep current password"

                    echoMode: TextInput.Password
                    onFocusChanged:
                    {
                        if(focus)
                            text=""
                    }

                    background: Rectangle
                    {
                        radius: 6
                        color: "#1E1F22"

                        border.width: passwordField.activeFocus ? 2 : 1
                        border.color: passwordField.activeFocus
                                      ? "#5865F2"
                                      : "#202225"
                    }
                }

                CheckBox
                {
                    id: saveChatsCheck

                    indicator: Item {}

                    contentItem: Row
                    {
                        spacing: 10

                        Rectangle
                        {
                            width: 18
                            height: 18
                            radius: 4

                            anchors.verticalCenter: parent.verticalCenter

                            color: saveChatsCheck.checked ? "#5865F2" : "#1E1F22"
                            border.color: "#5865F2"

                            Text
                            {
                                anchors.centerIn: parent
                                text: "✓"
                                visible: saveChatsCheck.checked
                                color: "white"
                                font.bold: true
                                font.pixelSize: 12
                            }
                        }

                        Text
                        {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Save chat history"
                            color: "white"
                            font.pixelSize: 14
                        }
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
                    id: deleteButton

                    text: "Delete"

                    onClicked:
                    {
                        root.deleteClicked()
                        root.close()
                    }

                    background: Rectangle
                    {
                        radius: 4

                        color: deleteButton.down
                               ? "darkred"
                               : "red"

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
                    id: cancelButton

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
                }

                Button
                {
                    id: saveButton

                    text: "Save"

                    enabled: channelNameField.text.trim().length > 0

                    onClicked:
                    {
                        root.saveClicked()
                        root.close()
                    }

                    background: Rectangle
                    {
                        radius: 4

                        color: !saveButton.enabled
                               ? "#444"
                               : saveButton.down
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
}
