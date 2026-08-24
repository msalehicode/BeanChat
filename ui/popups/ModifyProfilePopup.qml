import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Dialogs
import "../settingPages"

import "../constants/"

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
        color: Theme.current.background
        radius: 8
        border.color: Theme.current.surface3
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

            color: Theme.current.background

            Column
            {
                anchors.fill: parent
                anchors.margins: 20

                spacing: 6

                Text
                {
                    text: "User Profile"

                    color: Theme.current.text

                    font.pixelSize: 22
                    font.bold: true
                }

                Text
                {
                    text: "Update your username, identity and avatar."

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
                    color: Theme.current.surface

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

                            color: Theme.current.surface3

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
                                color: Theme.current.text

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

                                    color: removeAvatarButton.enabled ? Theme.current.danger :
                                            removeAvatarButton.down? Theme.current.danger : "transparent"

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

                Label
                {
                    text: "USERNAME"
                    color: Theme.current.textSecondary
                    font.bold: true
                }

                TextField
                {
                    id: usernameField

                    Layout.fillWidth: true

                    placeholderText: "Username"
                    placeholderTextColor: Theme.current.text

                    text: user.myUsername
                    color: Theme.current.text

                    background: Rectangle
                    {
                        radius: 6
                        color: Theme.current.surface3
                    }
                }

                Label
                {
                    text: !user.isConnectedToServer ? "IDENTITY" : "IDENTITY (CANNOT MODIFY WHEN YOU ARE CONNECTED)"
                    color: Theme.current.textSecondary
                    font.bold: true
                }
                SettingCombo
                {
                    id:identityCombobox
                    width: parent.width
                    height: parent.width
                    model: identityManager.identityNames
                    currentIndex: identityManager.currentIdentityIndex
                    enabled: !user.isConnectedToServer
                    onActivated:
                    {
                        identityManager.setCurrentIdentityIndex(currentIndex)
                        // settings.setValue
                        // (
                        //     "IDENTITITITITI",
                        //     speaker.audioOutputId(currentIndex)
                        // )
                    }
                }
                Row
                {
                    id:rowManagerIdentities
                    visible: identityCombobox.enabled
                    width: 70
                    height: parent.height
                    Layout.alignment: Qt.AlignRight
                    Button
                    {
                        id: deleteIdentity
                        text: "Delete Current Identity"
                        onClicked: identityManager.removeCurrentIdentity()
                        background: Rectangle
                        {
                            radius: 4
                            color: deleteIdentity.down
                                   ? Theme.current.danger
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
                        id: renameIdentity
                        text: "Rename Identity"
                        onClicked:
                        {
                            getValuePopup.titleText="Rename Identity"
                            getValuePopup.fieldTitle="NEW NAME"
                            getValuePopup.descriptionText="enter a new name for "+ identityManager.identityNames[identityCombobox.currentIndex]
                            getValuePopup.valueFieldPlaceHolder="enter a new name"
                            getValuePopup.currentValue=identityManager.identityNames[identityCombobox.currentIndex]
                            getValuePopup.acceptButtonText="Rename"
                            getValuePopup.onOk = function(identityName)
                            {
                                identityManager.renameCurrentIdentity(identityName)
                            }
                            getValuePopup.open()
                        }

                        background: Rectangle
                        {
                            radius: 4
                            color: renameIdentity.down
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
                        id: createIdentity
                        text: "Create Identity"
                        onClicked:
                        {
                            getValuePopup.titleText="Create Identity"
                            getValuePopup.fieldTitle="NAME"
                            getValuePopup.descriptionText="enter a name for new identity"
                            getValuePopup.valueFieldPlaceHolder="enter a name unique name"
                            getValuePopup.acceptButtonText="Create"
                            getValuePopup.onOk  = function(identityName)
                            {
                                identityManager.createIdentity(identityName)
                            }
                            getValuePopup.open()
                        }

                        background: Rectangle
                        {
                            radius: 4
                            color: createIdentity.down
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

                    onClicked: profilePopup.close()

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
                            user.updateMyProfile(usernameField.text, profilePopup.avatarSource)
                        else
                            user.updateMyProfile(usernameField.text)

                        //reset for later use.
                        profilePopup.imageHasChanged = false
                        profilePopup.avatarSource = ""

                        profilePopup.close()
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
                }
            }
        }
    }
}

