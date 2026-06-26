import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Item
{
    id: root

    property alias currentIndex: menu.currentIndex

    Rectangle
    {
        anchors.fill: parent
        color: "#2B2D31"
    }

    ColumnLayout
    {
        anchors.fill: parent

        anchors.margins: 16

        spacing: 10

        ////////////////////////////////////////////////////////////////
        // Header
        ////////////////////////////////////////////////////////////////

        Label
        {
            text: "USER SETTINGS"

            color: "#949BA4"

            font.pixelSize: 12
            font.bold: true

            Layout.leftMargin: 12
        }

        ////////////////////////////////////////////////////////////////
        // Menu
        ////////////////////////////////////////////////////////////////

        ListView
        {
            id: menu

            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: 0

            interactive: true

            clip: true

            spacing: 2

            model:
            [
                {
                    title: "Audio Input",
                    icon: "🎤"
                },
                {
                    title: "Audio Output",
                    icon: "🔊"
                },
                {
                    title: "Video",
                    icon: "🎥"
                },
                {
                    title: "Appearance",
                    icon: "🎨"
                },
                {
                    title: "Notifications",
                    icon: "🔔"
                },
                {
                    title: "Privacy & Safety",
                    icon: "🛡"
                },
                {
                    title: "Advanced",
                    icon: "⚙"
                }
            ]

            delegate: Rectangle
            {
                width: menu.width
                height: 40

                radius: 6

                property bool hovered: false
                property bool selected: index === root.currentIndex

                color:
                    selected ? "#404249"
                             : hovered ? "#35373C"
                                       : "transparent"

                Behavior on color
                {
                    ColorAnimation
                    {
                        duration: 120
                    }
                }

                MouseArea
                {
                    anchors.fill: parent

                    hoverEnabled: true

                    cursorShape: Qt.PointingHandCursor

                    onEntered:
                        parent.hovered = true

                    onExited:
                        parent.hovered = false

                    onClicked:
                        menu.currentIndex = index
                }

                RowLayout
                {
                    anchors.fill: parent

                    anchors.leftMargin: 12
                    anchors.rightMargin: 12

                    spacing: 10

                    Label
                    {
                        text: modelData.icon

                        font.pixelSize: 16
                    }

                    Label
                    {
                        Layout.fillWidth: true

                        text: modelData.title

                        color:
                            selected ? "white"
                                     : "#DBDEE1"

                        font.pixelSize: 14
                        font.bold: selected
                    }
                }
            }
        }

        ////////////////////////////////////////////////////////////////
        // Bottom
        ////////////////////////////////////////////////////////////////

        Rectangle
        {
            Layout.fillWidth: true

            height: 1

            color: "#3F4147"
        }

        Label
        {
            Layout.leftMargin: 12

            text: "BeanChat"

            color: "#B5BAC1"

            font.bold: true
        }

        Label
        {
            Layout.leftMargin: 12

            text: user.myAppVersion

            color: "#777C85"

            font.pixelSize: 11
        }
    }
}
