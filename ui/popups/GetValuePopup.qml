import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

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
            text: root.titleText
            color: "white"
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: parent.width
            elide: Text.ElideRight
        }

        Text
        {
            text: root.descriptionText
            color: "#b5bac1"
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: parent.width
            elide: Text.ElideRight
        }


        Item { Layout.fillHeight: true }

        Text
        {
            text: root.fieldTitle
            color: "#b5bac1"
            font.bold: true
            font.pixelSize: 12
        }

        TextField
        {
            id: valueField

            Layout.fillWidth: true

            // echoMode: TextInput.Password
            placeholderText: root.valueFieldPlaceHolder
            placeholderTextColor: "white"
            text: root.currentValue
            color: "white"

            onAccepted: acceptButton.clicked()

            background: Rectangle
            {
                radius: 4
                color: "#1e1f22"
                border.color: parent.activeFocus ? "#5865f2" : "#111214"
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
                           ?  "#4752C4"
                           : "#5865F2"

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
        }
    }
}
