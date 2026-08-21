import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Page
{
    id: root

    Material.theme: Material.Dark
    Material.accent: "#5865F2"

    background: Rectangle
    {
        color: "#313338"
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

            spacing: 24

            topPadding: 32
            bottomPadding: 40

            ///////////////////////////////////////////////////////
            // Header
            ///////////////////////////////////////////////////////

            Label
            {
                text: "Update Configuration"

                color: "white"

                font.pixelSize: 28
                font.bold: true
            }

            Label
            {
                text: "Configure updates"

                color: "#B5BAC1"

                font.pixelSize: 14
            }

            ///////////////////////////////////////////////////////
            // CHECK FOR UPDATES ON LAUNCH
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10
                color: "#2B2D31"
                border.color: "#404249"

                implicitHeight: checkForUpdatesColumn.implicitHeight + 32

                Column
                {
                    id: checkForUpdatesColumn

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    RowLayout
                    {
                        width: parent.width

                        Label
                        {
                            text: "Check for updates"

                            color: "white"

                            font.pixelSize: 18
                            font.bold: true

                            Layout.fillWidth: true
                        }

                        SettingSwitch
                        {
                            checked: user.checkUpdate

                            onToggled:
                            {
                                user.checkUpdate=checked
                            }
                        }
                    }

                    Label
                    {
                        width: parent.width

                        text: "On app launch check for latest updates and show you notification if an update is available."

                        color: "#B5BAC1"

                        wrapMode: Text.WordWrap
                    }
                }
            }

            ///////////////////////////////////////////////////////
            // UPDATE REPOSITORY URL
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10
                color: "#2B2D31"
                border.color: "#404249"

                implicitHeight: updateRepositoryColumn.implicitHeight + 32

                Column
                {
                    id: updateRepositoryColumn

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    Label
                    {
                        text: "Update repository"

                        color: "white"

                        font.pixelSize: 18
                        font.bold: true
                    }

                    Label
                    {
                        width: parent.width

                        text: "repository will be used to download and check for updates from that specific repository/server."

                        color: "#B5BAC1"

                        wrapMode: Text.WordWrap
                    }

                    TextField
                    {
                        id: updateRepositoryField

                        width: parent.width

                        placeholderText: "For example: https://beanchat.ir/bc/api/latest.php"
                        text: user.updateRepositoryAddress


                        color: "white"

                        selectByMouse: true

                        background: Rectangle
                        {
                            radius: 6

                            color: "#1E1F22"
                            border.color: updateRepositoryField.activeFocus
                                          ? "#5865F2"
                                          : "#404249"
                        }
                    }

                    RowLayout
                    {
                        width: parent.width

                        Item
                        {
                            Layout.fillWidth: true
                        }

                        Button
                        {
                            text: "Cancel"

                            onClicked:
                            {
                                updateRepositoryField.text = user.updateRepositoryAddress
                            }
                        }

                        Button
                        {
                            text: "Save"

                            onClicked:
                            {
                                user.updateRepositoryAddress=updateRepositoryField.text
                            }
                        }
                    }
                }
            }


            ///////////////////////////////////////////////////////
            // CHECK FOR UPDATE NOW
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10
                color: "#2B2D31"
                border.color: "#404249"

                implicitHeight: checkForUpdateNow.implicitHeight + 32

                Column
                {
                    id: checkForUpdateNow

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    RowLayout
                    {
                        width: parent.width

                        Label
                        {
                            text: "Check for update now"

                            color: "white"

                            font.pixelSize: 18
                            font.bold: true

                            Layout.fillWidth: true
                        }

                        Button
                        {
                            text: "check now"

                            onClicked:
                            {
                                user.checkForUpdate()
                            }
                        }
                    }

                    Label
                    {
                        width: parent.width

                        text: "click to check for available updates right now manually."

                        color: "#B5BAC1"

                        wrapMode: Text.WordWrap
                    }
                }
            }


            ///////////////////////////////////////////////////////
            // Footer
            ///////////////////////////////////////////////////////

            Label
            {
                width: parent.width

                horizontalAlignment: Text.AlignHCenter

                color: "#80848E"

                font.pixelSize: 12

                text: "beanchat update configs."
            }

            Item
            {
                width: 1
                height: 24
            }
        }
    }
}
