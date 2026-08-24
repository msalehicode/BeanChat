import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import "../constants/"

Page
{
    id: root

    Material.theme: Material.Dark
    Material.accent: Theme.current.accent

    background: Rectangle
    {
        color: Theme.current.background
    }

    Flickable
    {
        anchors.fill: parent

        clip: true

        contentWidth: width
        contentHeight: notificationsColumn.implicitHeight + 40

        ScrollBar.vertical: ScrollBar {}

        Column
        {
            id: notificationsColumn

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
                text: "Notifications"

                color: Theme.current.text

                font.pixelSize: 28
                font.bold: true
            }

            Label
            {
                text: "Configure notifications"

                color: Theme.current.textSecondary

                font.pixelSize: 14
            }

            ///////////////////////////////////////////////////////
            // NOTIFICATIONS
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10
                color: Theme.current.surface
                border.color: Theme.current.border

                implicitHeight: notificationsStatusColumn.implicitHeight + 32

                Column
                {
                    id: notificationsStatusColumn

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    RowLayout
                    {
                        width: parent.width

                        Label
                        {
                            text: "Show notifications"

                            color: Theme.current.text

                            font.pixelSize: 18
                            font.bold: true

                            Layout.fillWidth: true
                        }

                        SettingSwitch
                        {
                            checked: true

                            onToggled:
                            {
                                // user.checkUpdate=checked
                            }
                        }
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

                color: Theme.current.textMuted

                font.pixelSize: 12

                text: "beanchat notifications configs."
            }

            Item
            {
                width: 1
                height: 24
            }
        }
    }
}
