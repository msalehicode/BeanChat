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

    background: Rectangle
    {
        color: Theme.current.background
        radius: 8
        border.color: Theme.current.input
    }

    onClosed: clear()

    property alias channelName: channelNameField.text
    property alias channelPassword: passwordField.text
    property alias saveChats: saveChatsCheck.checked
    property alias isVoiceChannel: channelTypeCheck.checked

    function clear()
    {
        channelNameField.clear()
        passwordField.clear()
        channelNameField.focus=true
    }

    signal createClicked()

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
                    text: "Create Channel"
                    color: Theme.current.text
                    font.pixelSize: 22
                    font.bold: true
                }

                Text
                {
                    text: "Create a new voice channel."
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
                    onAccepted: createButton.clicked()
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
                    text: "CHANNEL PASSWORD (OPTIONAL)"
                    color: Theme.current.textSecondary
                    font.bold: true
                    font.pixelSize: 11
                }

                TextField
                {
                    id: passwordField

                    Layout.fillWidth: true
                    height: 40
                    onAccepted: createButton.clicked()

                    color: Theme.current.text
                    placeholderText: "Leave empty for an unlocked channel"
                    placeholderTextColor: Theme.current.text

                    echoMode: TextInput.Password

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


                CheckBox
                {
                    id: channelTypeCheck

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

                            color: channelTypeCheck.checked ? Theme.current.accent : Theme.current.input
                            border.color: Theme.current.accent

                            Text
                            {
                                anchors.centerIn: parent
                                text: "✓"
                                visible: channelTypeCheck.checked
                                color: Theme.current.text
                                font.bold: true
                                font.pixelSize: 12
                            }
                        }

                        Text
                        {
                            anchors.verticalCenter: parent.verticalCenter

                            text: "Is Voice Channel?"
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
                    id: cancelButton

                    text: "Cancel"

                    onClicked: root.close()

                    background: Rectangle
                    {
                        radius: 4

                        color: cancelButton.down
                               ? Theme.current.hover
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
                    id: createButton

                    text: "Create"

                    enabled: channelNameField.text.trim().length > 0

                    onClicked:
                    {
                        if(enabled)
                        {
                            root.createClicked()
                            root.close()
                        }
                    }

                    background: Rectangle
                    {
                        radius: 4

                        color: !createButton.enabled
                               ? Theme.current.textDisabled
                               : createButton.down
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
