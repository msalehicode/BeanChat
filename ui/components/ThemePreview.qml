import QtQuick

Item
{
    id: root

    // ============================================================
    // THEME COLORS
    // ============================================================

    property color backgroundColor: "#05070b"
    property color serverBarColor: "#05070b"
    property color channelColor: "#141C2B"
    property color headerColor: "#080B10"
    property color itemColor: "#0B1018"
    property color itemHoverColor: "#1C273A"
    property color accentColor: "#839ac7"

    property color textColor: "#FFFFFF"
    property color secondaryTextColor: "#80848E"
    property color mutedColor: "#4E5058"


    // ============================================================
    // BACKGROUND
    // ============================================================

    Rectangle
    {
        anchors.fill: parent

        radius: 7
        color: root.backgroundColor

        clip: true


        // ========================================================
        // SERVER BAR
        // ========================================================

        Rectangle
        {
            id: serverBar

            width: parent.width * 0.12
            height: parent.height

            color: root.serverBarColor

            Column
            {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                anchors.topMargin: 7

                spacing: 5

                Rectangle
                {
                    width: 17
                    height: 17

                    radius: width / 2

                    color: root.accentColor
                }

                Rectangle
                {
                    width: 17
                    height: 17

                    radius: width / 2

                    color: root.itemColor
                }

                Rectangle
                {
                    width: 17
                    height: 17

                    radius: width / 2

                    color: root.itemColor
                }
            }
        }


        // ========================================================
        // CHANNEL LIST
        // ========================================================

        Rectangle
        {
            id: channels

            anchors.left: serverBar.right

            width: parent.width * 0.29
            height: parent.height

            color: root.channelColor

            Column
            {
                anchors.fill: parent
                anchors.margins: 6

                spacing: 5

                // Server header
                Rectangle
                {
                    width: parent.width
                    height: 17

                    radius: 3

                    color: root.headerColor

                    Text
                    {
                        anchors.centerIn: parent

                        text: "BeanChat"

                        color: root.textColor

                        font.pixelSize: 6
                        font.bold: true
                    }
                }


                // Channel
                Rectangle
                {
                    width: parent.width
                    height: 13

                    radius: 3

                    color: root.accentColor

                    Text
                    {
                        anchors.left: parent.left
                        anchors.leftMargin: 4

                        anchors.verticalCenter: parent.verticalCenter

                        text: "# general"

                        color: root.textColor

                        font.pixelSize: 6
                    }
                }


                Rectangle
                {
                    width: parent.width * 0.75
                    height: 6

                    radius: 3

                    color: root.mutedColor
                }

                Rectangle
                {
                    width: parent.width * 0.85
                    height: 6

                    radius: 3

                    color: root.mutedColor
                }

                Rectangle
                {
                    width: parent.width * 0.65
                    height: 6

                    radius: 3

                    color: root.mutedColor
                }

                Item
                {
                    width: 1
                    height: 3
                }

                Rectangle
                {
                    width: parent.width * 0.8
                    height: 6

                    radius: 3

                    color: root.mutedColor
                }
            }
        }


        // ========================================================
        // MAIN CONTENT
        // ========================================================

        Rectangle
        {
            id: mainContent

            anchors.left: channels.right
            anchors.right: rightPanel.left

            height: parent.height

            color: root.backgroundColor


            // Top header
            Rectangle
            {
                width: parent.width
                height: 19

                color: root.headerColor

                Text
                {
                    anchors.left: parent.left
                    anchors.leftMargin: 7

                    anchors.verticalCenter: parent.verticalCenter

                    text: "# general"

                    color: root.textColor

                    font.pixelSize: 7
                    font.bold: true
                }
            }


            Column
            {
                anchors.left: parent.left
                anchors.right: parent.right

                anchors.top: parent.top
                anchors.topMargin: 30

                anchors.leftMargin: 8
                anchors.rightMargin: 8

                spacing: 7


                // Message 1
                Row
                {
                    spacing: 5

                    Rectangle
                    {
                        width: 16
                        height: 16

                        radius: 8

                        color: root.accentColor
                    }

                    Column
                    {
                        spacing: 3

                        Rectangle
                        {
                            width: 35
                            height: 5

                            radius: 2

                            color: root.textColor
                        }

                        Rectangle
                        {
                            width: 60
                            height: 4

                            radius: 2

                            color: root.mutedColor
                        }
                    }
                }


                // Message 2
                Row
                {
                    spacing: 5

                    Rectangle
                    {
                        width: 16
                        height: 16

                        radius: 8

                        color: root.mutedColor
                    }

                    Column
                    {
                        spacing: 3

                        Rectangle
                        {
                            width: 42
                            height: 5

                            radius: 2

                            color: root.textColor
                        }

                        Rectangle
                        {
                            width: 75
                            height: 4

                            radius: 2

                            color: root.mutedColor
                        }
                    }
                }
            }
        }


        // ========================================================
        // RIGHT PANEL
        // ========================================================

        Rectangle
        {
            id: rightPanel

            anchors.right: parent.right

            width: parent.width * 0.20
            height: parent.height

            color: root.channelColor


            Column
            {
                anchors.fill: parent
                anchors.margins: 5

                spacing: 5


                // ====================================================
                // USERS HEADER
                // ====================================================

                Rectangle
                {
                    width: parent.width
                    height: 17

                    color: root.headerColor

                    Text
                    {
                        anchors.centerIn: parent

                        text: "Users"

                        color: root.textColor

                        font.pixelSize: 6
                        font.bold: true
                    }
                }


                // ====================================================
                // USER 1
                // ====================================================

                Row
                {
                    width: parent.width
                    height: 12

                    spacing: 4

                    Rectangle
                    {
                        width: 12
                        height: 12

                        radius: 6

                        color: root.accentColor
                    }

                    Rectangle
                    {
                        width: parent.width - 16
                        height: 5

                        anchors.verticalCenter: parent.verticalCenter

                        radius: 2

                        color: root.secondaryTextColor
                    }
                }


                // ====================================================
                // USER 2
                // ====================================================

                Row
                {
                    width: parent.width
                    height: 12

                    spacing: 4

                    Rectangle
                    {
                        width: 12
                        height: 12

                        radius: 6

                        color: root.mutedColor
                    }

                    Rectangle
                    {
                        width: parent.width - 16
                        height: 5

                        anchors.verticalCenter: parent.verticalCenter

                        radius: 2

                        color: root.mutedColor
                    }
                }
            }
        }
    }
}
