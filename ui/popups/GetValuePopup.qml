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

    width: 400
    height: 350

    anchors.centerIn: Overlay.overlay

    // Parameters
    property string titleText: "Password Required"
    property string fieldTitle: "PASSWORD"
    property string descriptionText: "Enter password for channel:"
    property string valueFieldPlaceHolder: "Enter Password"
    property string currentValue: ""
    property string acceptButtonText: "Join"

    //to assign function to action
    property var onOk: null

    onOpened:
    {
        valueField.forceActiveFocus()
    }
    onClosed:
    {
        //reset values
        root.titleText= "Password Required"
        root.fieldTitle= "PASSWORD"
        root.descriptionText= "Enter password for channel:"
        root.valueFieldPlaceHolder= "Enter Password"
        root.acceptButtonText= "Join"
        root.onOk=null
        valueField.clear()
    }

    background: Rectangle
    {
        color: Theme.current.background
        radius: 8
        border.color: Theme.current.input
    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Text
        {
            text: root.titleText
            color: Theme.current.text
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: parent.width
            elide: Text.ElideRight
        }

        Text
        {
            text: root.descriptionText
            color: Theme.current.textSecondary
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: parent.width
            elide: Text.ElideRight
        }


        Item { Layout.fillHeight: true }

        Text
        {
            text: root.fieldTitle
            color: Theme.current.textSecondary
            font.bold: true
            font.pixelSize: 12
        }

        TextField
        {
            id: valueField

            Layout.fillWidth: true

            // echoMode: TextInput.Password
            placeholderText: root.valueFieldPlaceHolder
            placeholderTextColor: Theme.current.text
            text: root.currentValue
            color: Theme.current.text

            onAccepted: acceptButton.clicked()

            background: Rectangle
            {
                radius: 4
                color: Theme.current.input
                border.color: parent.activeFocus ? Theme.current.accent : Theme.current.inputBorder
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
                id: acceptButton

                text: root.acceptButtonText
                Layout.fillWidth: true

                onClicked:
                {
                    // root.valueEntered()

                    if (onOk)
                        onOk(valueField.text)

                    root.close()
                }

                background: Rectangle
                {
                    radius: 4

                    color: acceptButton.down
                           ?  Theme.current.accentHover
                           : Theme.current.accent

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
        }
    }
}
