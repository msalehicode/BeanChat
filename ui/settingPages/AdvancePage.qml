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

                color: "white"

                font.pixelSize: 28
                font.bold: true
            }

            Label
            {
                text: "Advanced stuff."

                color: "#B5BAC1"

                font.pixelSize: 14
            }

            ///////////////////////////////////////////////////////
            // DEVELOPER
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: "#2B2D31"

                border.color: "#404249"

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

                        color: "white"

                        font.pixelSize: 18
                        font.bold: true
                    }

                    Label
                    {
                        text: "Temporary developer actions."

                        color: "#B5BAC1"
                    }

                    Button
                    {
                        width: 180
                        height: 38

                        text: "Create Channel"

                        Material.background: "#5865F2"

                        onClicked:
                            user.createChannel("dota2", "123456")
                    }
                }
            }


        }
    }
}
