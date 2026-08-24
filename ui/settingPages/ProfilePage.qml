import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Dialogs
import "../settingPages"
import "../popups"
import "../constants/"

Page
{
    id: profilePage

    Material.theme: Material.Dark
    Material.accent: Theme.current.accent

    background: Rectangle
    {
        color: Theme.current.background
    }

    property string avatarSource: ""
    property string usernameText: ""
    property string identityText: ""

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
            profilePage.avatarSource =
                    selectedFile.toString()

            profilePage.imageHasChanged = true
        }
    }

    Flickable
    {
        anchors.fill: parent
        clip: true

        contentWidth: width
        contentHeight: settingsColumn.implicitHeight + 40

        ScrollBar.vertical: ScrollBar {}

        Column
        {
            id: settingsColumn

            width: Math.min(parent.width - 80, 820)

            anchors.horizontalCenter: parent.horizontalCenter

            spacing: 10

            topPadding: 23
            bottomPadding: 40

            Label
            {
                text: "User Profile"

                color: Theme.current.text

                font.pixelSize: 28
                font.bold: true
            }

            //
            // Avatar
            //

            Rectangle
            {
                width: parent.width

                radius: 10

                color: Theme.current.surface
                border.color: Theme.current.border

                implicitHeight: avatarContent.implicitHeight + 32

                Column
                {
                    id: avatarContent

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    Label
                    {
                        text: "Profile Picture"

                        color: Theme.current.text

                        font.pixelSize: 18
                        font.bold: true
                    }

                    Row
                    {
                        width: parent.width

                        spacing: 20

                        Rectangle
                        {
                            width: 96
                            height: 96

                            radius: width / 2

                            color: Theme.current.input

                            clip: true

                            Image
                            {
                                anchors.fill: parent

                                source: user.myAvatarPath

                                fillMode: Image.PreserveAspectCrop

                                visible: user.myAvatarPath !== ""
                            }

                            Text
                            {
                                anchors.centerIn: parent

                                visible: user.myAvatarPath === ""

                                text: "?"

                                color: Theme.current.text

                                font.pixelSize: 36
                            }
                        }

                        Column
                        {
                            spacing: 10

                            anchors.verticalCenter:
                                parent.verticalCenter

                            Button
                            {
                                id: changeAvatarButton

                                text: "Change Avatar"

                                onClicked:
                                    avatarDialog.open()

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

                                    color: Theme.current.text

                                    horizontalAlignment:
                                        Text.AlignHCenter

                                    verticalAlignment:
                                        Text.AlignVCenter
                                }
                            }

                            Button
                            {
                                id: removeAvatarButton

                                text: "Remove Avatar"

                                enabled: profilePage.hasAvatar

                                onClicked:
                                {
                                    profilePage.avatarSource = ""

                                    // tell backend
                                    // profileManager.removeAvatar()
                                }

                                background: Rectangle
                                {
                                    radius: 4

                                    color: removeAvatarButton.down
                                           ? Theme.current.danger
                                           : removeAvatarButton.enabled
                                             ? Theme.current.danger
                                             : "transparent"

                                    border.width: 1
                                    border.color: Theme.current.danger
                                }

                                contentItem: Text
                                {
                                    text: parent.text

                                    color: Theme.current.text

                                    horizontalAlignment:
                                        Text.AlignHCenter

                                    verticalAlignment:
                                        Text.AlignVCenter
                                }
                            }
                        }
                    }
                }
            }

            //
            // Username
            //

            Rectangle
            {
                width: parent.width

                radius: 10

                color: Theme.current.surface
                border.color: Theme.current.border

                implicitHeight: usernameContent.implicitHeight + 32

                Column
                {
                    id: usernameContent

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    Label
                    {
                        text: "Username"

                        color: Theme.current.text

                        font.pixelSize: 18
                        font.bold: true
                    }

                    TextField
                    {
                        id: usernameField

                        width: parent.width

                        placeholderText: "Username"
                        placeholderTextColor: Theme.current.inputPlaceholder

                        text: user.myUsername

                        color: Theme.current.text

                        background: Rectangle
                        {
                            radius: 6

                            color: Theme.current.input
                        }
                    }
                }
            }

            //
            // Identity
            //

            Rectangle
            {
                width: parent.width

                radius: 10

                color: Theme.current.surface
                border.color: Theme.current.border

                implicitHeight: identityContent.implicitHeight + 32

                Column
                {
                    id: identityContent

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    Label
                    {
                        text: "Identity"

                        color: Theme.current.text

                        font.pixelSize: 18
                        font.bold: true
                    }

                    Label
                    {
                        width: parent.width

                        text:
                            !user.isConnectedToServer
                            ? "Select your identity."
                            : "NOTE: Identity cannot be modified while connected to a server."

                        color: Theme.current.textSecondary

                        wrapMode: Text.WordWrap
                    }

                    SettingCombo
                    {
                        id: identityCombobox
                        width: parent.width
                        model: identityManager.identityNames
                        currentIndex: identityManager.currentIdentityIndex
                        enabled: !user.isConnectedToServer

                        onActivated:
                        {
                            identityManager.setCurrentIdentityIndex(currentIndex)
                        }
                    }

                    Row
                    {
                        width: parent.width
                        spacing: 8
                        visible: identityCombobox.enabled

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
                                border.color: Theme.current.danger
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
                                getValuePopup.titleText = "Rename Identity"
                                getValuePopup.fieldTitle = "NEW NAME"
                                getValuePopup.descriptionText = "enter a new name for " + identityManager.identityNames[identityCombobox.currentIndex]
                                getValuePopup.valueFieldPlaceHolder = "enter a new name"

                                getValuePopup.currentValue = identityManager.identityNames[identityCombobox.currentIndex]

                                getValuePopup.acceptButtonText = "Rename"

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
                                       ? Theme.current.pressed
                                       : "transparent"

                                border.width: 1
                                border.color: Theme.current.borderLight
                            }

                            contentItem: Text
                            {
                                text: parent.text

                                color: Theme.current.text

                                horizontalAlignment:
                                    Text.AlignHCenter

                                verticalAlignment:
                                    Text.AlignVCenter
                            }
                        }

                        Button
                        {
                            id: createIdentity

                            text: "Create Identity"

                            onClicked:
                            {
                                getValuePopup.titleText = "Create Identity"
                                getValuePopup.fieldTitle = "NAME"
                                getValuePopup.descriptionText = "enter a name for new identity"
                                getValuePopup.valueFieldPlaceHolder = "enter a unique name"
                                getValuePopup.acceptButtonText = "Create"

                                getValuePopup.onOk = function(identityName)
                                {
                                    identityManager.createIdentity(identityName)
                                }

                                getValuePopup.open()
                            }

                            background: Rectangle
                            {
                                radius: 4

                                color: createIdentity.down
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
                    }
                }
            }

            //
            // Save
            //

            Row
            {
                width: parent.width

                spacing: 10

                layoutDirection: Qt.RightToLeft

                Button
                {
                    id: saveButton
                    text: "Save"
                    highlighted: true
                    onClicked:
                    {
                        if (profilePage.imageHasChanged)
                        {
                            user.updateMyProfile(usernameField.text, profilePage.avatarSource)
                        }
                        else
                        {
                            user.updateMyProfile( usernameField.text)
                        }

                        profilePage.imageHasChanged = false
                        profilePage.avatarSource = ""
                    }

                    background: Rectangle
                    {
                        radius: 4
                        color: saveButton.down
                               ? Theme.current.accentPressed
                               : Theme.current.accent
                    }

                    contentItem: Text
                    {
                        text: parent.text
                        color: Theme.current.text
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment:Text.AlignVCenter
                    }
                }
            }
        }
    }
    GetValuePopup
    {
        id:getValuePopup
    }
}
