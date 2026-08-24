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
        contentHeight: settingsColumn.implicitHeight + 40

        ScrollBar.vertical: ScrollBar {}

        Column
        {
            id: advanceColumn

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
                text: "Advance"

                color: Theme.current.text

                font.pixelSize: 28
                font.bold: true
            }

            Label
            {
                text: "Advanced stuff."

                color: Theme.current.textSecondary

                font.pixelSize: 14
            }

            ///////////////////////////////////////////////////////
            // DEVELOPER
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: Theme.current.surface

                border.color: Theme.current.border

                implicitHeight: devColumn.implicitHeight + 32

                Column
                {
                    id: devColumn

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 14

                    Label
                    {
                        text: "Developer"

                        color: Theme.current.text

                        font.pixelSize: 18
                        font.bold: true
                    }

                    Label
                    {
                        text: "Temporary developer actions."

                        color: Theme.current.textSecondary
                    }

                    Button
                    {
                        width: 180
                        height: 38

                        text: "Create Channel"

                        Material.background: Theme.current.accent

                        onClicked: console.log("seomthing")
                    }
                }
            }


        }
    }
}
