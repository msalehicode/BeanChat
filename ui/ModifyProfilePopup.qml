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

    width: 520
    height: 620

    anchors.centerIn: Overlay.overlay

    Material.theme: Material.Dark

    background: Rectangle
    {
        radius: 12
        color: "#313338"

        border.width: 1
        border.color: "#1E1F22"
    }

    property string avatarSource: ""


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
        anchors.margins: 24

        spacing: 18

        Label
        {
            text: "User Profile"

            font.pixelSize: 24
            font.bold: true

            color: "white"
        }

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
                        text: "Change Avatar"

                        onClicked: avatarDialog.open()
                    }

                    Button
                    {
                        text: "Remove Avatar"

                        enabled: profilePopup.hasAvatar

                        onClicked:
                        {
                            profilePopup.avatarSource = ""

                            // tell backend
                            // profileManager.removeAvatar()
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

        RowLayout
        {
            Layout.alignment: Qt.AlignRight

            spacing: 12

            Button
            {
                text: "Cancel"

                onClicked:
                    profilePopup.close()
            }

            Button
            {
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

                    //check if image modified
                    if(profilePopup.imageHasChanged)
                        user.updateMyProfile(usernameField.text, identityField.text, profilePopup.avatarSource);
                    else
                        user.updateMyProfile(usernameField.text, identityField.text);

                    console.log( "update user info: " , usernameField.text," " , identityField.text, " " ,profilePopup.avatarSource)

                    //reset for later use.
                    profilePopup.imageHasChanged=false;
                    profilePopup.avatarSource="";

                    profilePopup.close()
                }
            }
        }
    }


}
