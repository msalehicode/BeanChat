import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Dialogs
import "../settingPages"
import "../popups"
Page
{
    id: profilePage

    Material.theme: Material.Dark
    Material.accent: "#5865F2"

    background: Rectangle
    {
        color: "#313338"
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

                color: "white"

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

                color: "#2B2D31"
                border.color: "#404249"

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

                        color: "white"

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

                            color: "#1E1F22"

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

                                color: "white"

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

                                    color: "white"

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
                                           ? "darkred"
                                           : removeAvatarButton.enabled
                                             ? "red"
                                             : "transparent"

                                    border.width: 1
                                    border.color: "#555"
                                }

                                contentItem: Text
                                {
                                    text: parent.text

                                    color: "white"

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

                color: "#2B2D31"
                border.color: "#404249"

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

                        color: "white"

                        font.pixelSize: 18
                        font.bold: true
                    }

                    TextField
                    {
                        id: usernameField

                        width: parent.width

                        placeholderText: "Username"
                        placeholderTextColor: "#72767D"

                        text: user.myUsername

                        color: "white"

                        background: Rectangle
                        {
                            radius: 6

                            color: "#1E1F22"
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

                color: "#2B2D31"
                border.color: "#404249"

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

                        color: "white"

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

                        color: "#B5BAC1"

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
                                       ? "#3F4147"
                                       : "transparent"

                                border.width: 1
                                border.color: "#555"
                            }

                            contentItem: Text
                            {
                                text: parent.text

                                color: "white"

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
                               ? "#4752C4"
                               : "#5865F2"
                    }

                    contentItem: Text
                    {
                        text: parent.text
                        color: "white"
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
