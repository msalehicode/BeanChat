import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Dialogs

Popup
{
    id: profilePopup

    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    width: 520
    height: 620

    anchors.centerIn: Overlay.overlay

    background: Rectangle
    {
        color: "#313338"
        radius: 8
        border.color: "#1E1F22"
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

    property string avatarSource: ""
    property string usernameText: ""
    property string identityText: ""

    // local usage
    property bool hasAvatar: avatarSource !== ""
    property bool imageHasChanged: false

    FileDialog
    {
        id: avatarDialog

        title: "Select Profile Picture"

        nameFilters:
            [
            "Images (*.png)"
        ]

        onAccepted:
        {
            profilePopup.avatarSource =
                    selectedFile.toString()

            imageHasChanged=true;
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
                    text: "User Profile"

                    color: "white"

                    font.pixelSize: 22
                    font.bold: true
                }

                Text
                {
                    text: "Update your username, identity and avatar."

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
                id: bodyLayout

                anchors.fill: parent

                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.topMargin: 10
                anchors.bottomMargin: 16

                spacing: 16

                Rectangle
                {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 140

                    radius: 10
                    color: "#2B2D31"

                    RowLayout
                    {
                        anchors.fill: parent
                        anchors.margins: 16

                        spacing: 16

                        Rectangle
                        {
                            width: 96
                            height: 96

                            radius: width / 2

                            color: "#1E1F22"

                            clip: true

                            Image
                            {
                                anchors.fill: parent

                                source: profilePopup.avatarSource

                                fillMode: Image.PreserveAspectCrop

                                visible:
                                    profilePopup.avatarSource !== ""
                            }

                            Text
                            {
                                anchors.centerIn: parent

                                visible:
                                    profilePopup.avatarSource === ""

                                text: "?"
                                color: "white"

                                font.pixelSize: 36
                            }

                        }

                        ColumnLayout
                        {
                            Layout.fillWidth: true

                            Button
                            {
                                id: changeAvatarButton
                                text: "Change Avatar"

                                onClicked: avatarDialog.open()


                                background: Rectangle
                                {
                                    radius: 4

                                    color: changeAvatarButton.down
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
                                id:removeAvatarButton
                                text: "Remove Avatar"

                                enabled: profilePopup.hasAvatar

                                onClicked:
                                {
                                    profilePopup.avatarSource = ""

                                    // tell backend
                                    // profileManager.removeAvatar()
                                }

                                background: Rectangle
                                {
                                    radius: 4

                                    color: removeAvatarButton.enabled ? "red" :
                                            removeAvatarButton.down? "darkred" : "transparent"

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

                Label
                {
                    text: "USERNAME"
                    color: "#B5BAC1"
                    font.bold: true
                }

                TextField
                {
                    id: usernameField

                    Layout.fillWidth: true

                    placeholderText: "Username"
                    placeholderTextColor: "white"

                    text: user.myUsername
                    color: "white"

                    background: Rectangle
                    {
                        radius: 6
                        color: "#1E1F22"
                    }
                }

                Label
                {
                    text: "IDENTITY"
                    color: "#B5BAC1"
                    font.bold: true
                }

                TextField
                {
                    id: identityField

                    Layout.fillWidth: true

                    placeholderText: "Identity"
                    placeholderTextColor: "white"
                    text:user.myIdentity

                    color: "white"

                    background: Rectangle
                    {
                        radius: 6
                        color: "#1E1F22"
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
                    id: cancelButton

                    text: "Cancel"

                    onClicked: profilePopup.close()

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

                    highlighted: true

                    onClicked:
                    {
                        // backend call:
                        //
                        // profileManager.saveProfile(
                        //     usernameField.text,
                        //     identityField.text,
                        //     profilePopup.avatarSource
                        // )


                        if(profilePopup.imageHasChanged)
                            user.updateMyProfile(
                                        usernameField.text,
                                        identityField.text,
                                        profilePopup.avatarSource)
                        else
                            user.updateMyProfile(
                                        usernameField.text,
                                        identityField.text)

                        //reset for later use.
                        profilePopup.imageHasChanged = false
                        profilePopup.avatarSource = ""

                        profilePopup.close()
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
                }
            }
        }
    }
}

