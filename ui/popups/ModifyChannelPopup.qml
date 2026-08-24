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
        color: Theme.current.background
        radius: 8
        border.color: Theme.current.input
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
            color: Theme.current.background

            Column
            {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 6

                Text
                {
                    text: "Channel Settings"
                    color: Theme.current.text
                    font.pixelSize: 22
                    font.bold: true
                }

                Text
                {
                    text: "Edit your voice channel settings."
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
                    text: "CHANNEL NAME"
                    color: Theme.current.textSecondary
                    font.bold: true
                    font.pixelSize: 11
                }

                TextField
                {
                    id: channelNameField

                    Layout.fillWidth: true
                    height: 40
                    onAccepted: saveButton.clicked()

                    color: Theme.current.text
                    placeholderText: "General"
                    placeholderTextColor: Theme.current.text

                    background: Rectangle
                    {
                        radius: 6
                        color: Theme.current.input

                        border.width: channelNameField.activeFocus ? 2 : 1
                        border.color: channelNameField.activeFocus
                                      ? Theme.current.accent
                                      : Theme.current.inputBorder
                    }
                }

                Label
                {
                    text: "CHANNEL PASSWORD"
                    color: Theme.current.textSecondary
                    font.bold: true
                    font.pixelSize: 11
                }

                TextField
                {
                    id: passwordField

                    Layout.fillWidth: true
                    height: 40
                    onAccepted: saveButton.clicked()

                    color: Theme.current.text

                    placeholderText: "Leave empty to keep current password"
                    placeholderTextColor: Theme.current.text

                    echoMode: TextInput.Password
                    onFocusChanged:
                    {
                        if(focus)
                            text=""
                    }

                    background: Rectangle
                    {
                        radius: 6
                        color: Theme.current.input

                        border.width: passwordField.activeFocus ? 2 : 1
                        border.color: passwordField.activeFocus
                                      ? Theme.current.accent
                                      : Theme.current.inputBorder
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

                            color: saveChatsCheck.checked ? Theme.current.accent : Theme.current.input
                            border.color: Theme.current.accent

                            Text
                            {
                                anchors.centerIn: parent
                                text: "✓"
                                visible: saveChatsCheck.checked
                                color: Theme.current.text
                                font.bold: true
                                font.pixelSize: 12
                            }
                        }

                        Text
                        {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "Save chat history"
                            color: Theme.current.text
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
                               ? Theme.current.accentPressed
                               : Theme.current.danger

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
                    id: cancelButton

                    text: "Cancel"

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
                    id: saveButton

                    text: "Save"

                    enabled: channelNameField.text.trim().length > 0

                    onClicked:
                    {
                        if(enabled)
                        {
                            root.saveClicked()
                            root.close()
                        }
                    }

                    background: Rectangle
                    {
                        radius: 4

                        color: !saveButton.enabled
                               ? Theme.current.textDisabled
                               : saveButton.down
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
                }
            }
        }
    }
}
