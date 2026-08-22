import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import "../components"

Page
{
    id: root

    Material.theme: Material.Dark
    Material.accent: "#5865F2"

    property string selectedTheme: "discord"

    background: Rectangle
    {
        color: "#313338"
    }

    function selectTheme(theme)
    {
        selectedTheme = theme

        // Connect this to your C++/User theme system.
        //
        // Example:
        // user.theme = theme
        //
        // Or:
        // user.setTheme(theme)
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
            // HEADER
            ///////////////////////////////////////////////////////

            Label
            {
                text: "Appearance"

                color: "white"

                font.pixelSize: 28
                font.bold: true
            }

            Label
            {
                text: "Customize the look and feel of BeanChat"

                color: "#B5BAC1"

                font.pixelSize: 14
            }


            ///////////////////////////////////////////////////////
            // THEMES
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: "#2B2D31"
                border.color: "#404249"

                implicitHeight: themesColumn.implicitHeight + 32

                Column
                {
                    id: themesColumn

                    anchors.fill: parent
                    anchors.margins: 16

                    spacing: 18


                    ///////////////////////////////////////////////////
                    // HEADER
                    ///////////////////////////////////////////////////

                    Column
                    {
                        width: parent.width

                        spacing: 4

                        Label
                        {
                            text: "Themes"

                            color: "white"

                            font.pixelSize: 18
                            font.bold: true
                        }

                        Label
                        {
                            width: parent.width

                            text: "Choose a theme for BeanChat. You can change the appearance whenever you want."

                            color: "#B5BAC1"

                            font.pixelSize: 13

                            wrapMode: Text.WordWrap
                        }
                    }


                    ///////////////////////////////////////////////////
                    // THEME GRID
                    ///////////////////////////////////////////////////

                    GridLayout
                    {
                        width: parent.width

                        columns: width >= 700 ? 2 : 1

                        columnSpacing: 14
                        rowSpacing: 14


                        ///////////////////////////////////////////////////
                        // DISCORD
                        ///////////////////////////////////////////////////

                        Rectangle
                        {
                            id: discordTheme

                            Layout.fillWidth: true
                            Layout.preferredHeight: 225

                            radius: 10

                            color: "#232428"

                            border.width:
                                selectedTheme === "discord"
                                ? 5
                                : 1

                            border.color:
                                selectedTheme === "discord"
                                ? "#5865F2"
                                : discordMouse.containsMouse
                                  ? "#5865F2"
                                  : "#404249"

                            scale: discordMouse.containsMouse ? 1.015 : 1

                            Behavior on scale
                            {
                                NumberAnimation
                                {
                                    duration: 120
                                }
                            }

                            Column
                            {
                                anchors.fill: parent
                                anchors.margins: 12

                                spacing: 9


                                ///////////////////////////////////////////////////
                                // PREVIEW
                                ///////////////////////////////////////////////////

                                ThemePreview
                                {
                                    width: parent.width
                                    height: 135

                                    backgroundColor: "#313338"
                                    serverBarColor: "#1E1F22"
                                    channelColor: "#2B2D31"
                                    headerColor: "#232428"

                                    itemColor: "#404249"
                                    itemHoverColor: "#5865F2"
                                    accentColor: "#5865F2"

                                    textColor: "#FFFFFF"
                                    secondaryTextColor: "#B5BAC1"
                                    mutedColor: "#4E5058"
                                }


                                ///////////////////////////////////////////////////
                                // INFO
                                ///////////////////////////////////////////////////

                                RowLayout
                                {
                                    width: parent.width

                                    Label
                                    {
                                        text: "Discord"

                                        color: "white"

                                        font.pixelSize: 15
                                        font.bold: true

                                        Layout.fillWidth: true
                                    }

                                    Label
                                    {
                                        visible: selectedTheme === "discord"

                                        text: "✓"

                                        color: "#5865F2"

                                        font.pixelSize: 18
                                        font.bold: true
                                    }
                                }

                                Label
                                {
                                    width: parent.width

                                    text: "Discord-inspired dark interface"

                                    color: "#B5BAC1"

                                    font.pixelSize: 12
                                }
                            }

                            MouseArea
                            {
                                id: discordMouse

                                anchors.fill: parent

                                hoverEnabled: true

                                cursorShape: Qt.PointingHandCursor

                                onClicked:
                                {
                                    selectTheme("discord")
                                }
                            }
                        }


                        ///////////////////////////////////////////////////
                        // TEAMSPEAK 3
                        ///////////////////////////////////////////////////

                        Rectangle
                        {
                            id: teamspeakTheme

                            Layout.fillWidth: true
                            Layout.preferredHeight: 225

                            radius: 10

                            color: "#E9ECEF"

                            border.width:
                                selectedTheme === "teamspeak"
                                ? 5
                                : 1

                            border.color:
                                selectedTheme === "teamspeak"
                                ? "#2580C2"
                                : teamspeakMouse.containsMouse
                                  ? "#2580C2"
                                  : "#404249"

                            scale: teamspeakMouse.containsMouse ? 1.015 : 1

                            Behavior on scale
                            {
                                NumberAnimation
                                {
                                    duration: 120
                                }
                            }

                            Column
                            {
                                anchors.fill: parent
                                anchors.margins: 12

                                spacing: 9


                                ///////////////////////////////////////////////////
                                // PREVIEW
                                ///////////////////////////////////////////////////

                                ThemePreview
                                {
                                    width: parent.width
                                    height: 135

                                    backgroundColor: "#F4F5F6"
                                    serverBarColor: "#D5D9DD"
                                    channelColor: "#E9ECEF"
                                    headerColor: "#2580C2"

                                    itemColor: "#FFFFFF"
                                    itemHoverColor: "#DCE5EC"
                                    accentColor: "#2580C2"

                                    textColor: "#202428"
                                    secondaryTextColor: "#626970"
                                    mutedColor: "#A9B0B6"
                                }


                                ///////////////////////////////////////////////////
                                // INFO
                                ///////////////////////////////////////////////////

                                RowLayout
                                {
                                    width: parent.width

                                    Label
                                    {
                                        text: "TeamSpeak 3"

                                        color: "#202428"

                                        font.pixelSize: 15
                                        font.bold: true

                                        Layout.fillWidth: true
                                    }

                                    Label
                                    {
                                        visible: selectedTheme === "teamspeak"

                                        text: "✓"

                                        color: "#2580C2"

                                        font.pixelSize: 18
                                        font.bold: true
                                    }
                                }

                                Label
                                {
                                    width: parent.width

                                    text: "Classic TeamSpeak 3 light appearance"

                                    color: "#626970"

                                    font.pixelSize: 12
                                }
                            }

                            MouseArea
                            {
                                id: teamspeakMouse

                                anchors.fill: parent

                                hoverEnabled: true

                                cursorShape: Qt.PointingHandCursor

                                onClicked:
                                {
                                    selectTheme("teamspeak")
                                }
                            }
                        }


                        ///////////////////////////////////////////////////
                        // GAMER
                        ///////////////////////////////////////////////////

                        Rectangle
                        {
                            id: gamerTheme

                            Layout.fillWidth: true
                            Layout.preferredHeight: 225

                            radius: 10

                            color: "#080A0A"

                            border.width:
                                selectedTheme === "gamer"
                                ? 5
                                : 1

                            border.color:
                                selectedTheme === "gamer"
                                ? "#44FF00"
                                : gamerMouse.containsMouse
                                  ? "#44FF00"
                                  : "#303530"

                            scale: gamerMouse.containsMouse ? 1.015 : 1

                            Behavior on scale
                            {
                                NumberAnimation
                                {
                                    duration: 120
                                }
                            }

                            Column
                            {
                                anchors.fill: parent
                                anchors.margins: 12

                                spacing: 9


                                ///////////////////////////////////////////////////
                                // PREVIEW
                                ///////////////////////////////////////////////////

                                ThemePreview
                                {
                                    width: parent.width
                                    height: 135

                                    backgroundColor: "#050606"
                                    serverBarColor: "#080A0A"
                                    channelColor: "#101510"
                                    headerColor: "#0A0D0A"

                                    itemColor: "#222922"
                                    itemHoverColor: "#263326"
                                    accentColor: "#44FF00"

                                    textColor: "#E8FFE8"
                                    secondaryTextColor: "#7E897E"
                                    mutedColor: "#394439"
                                }


                                ///////////////////////////////////////////////////
                                // INFO
                                ///////////////////////////////////////////////////

                                RowLayout
                                {
                                    width: parent.width

                                    Label
                                    {
                                        text: "Razer"

                                        color: "white"

                                        font.pixelSize: 15
                                        font.bold: true

                                        Layout.fillWidth: true
                                    }

                                    Label
                                    {
                                        visible: selectedTheme === "gamer"

                                        text: "✓"

                                        color: "#44FF00"

                                        font.pixelSize: 18
                                        font.bold: true
                                    }
                                }

                                Label
                                {
                                    width: parent.width

                                    text: "RGB gaming aesthetic inspired by gaming hardware"

                                    color: "#7E897E"

                                    font.pixelSize: 12
                                }
                            }

                            MouseArea
                            {
                                id: gamerMouse

                                anchors.fill: parent

                                hoverEnabled: true

                                cursorShape: Qt.PointingHandCursor

                                onClicked:
                                {
                                    selectTheme("gamer")
                                }
                            }
                        }


                        ///////////////////////////////////////////////////
                        // STEAM
                        ///////////////////////////////////////////////////

                        Rectangle
                        {
                            id: steamTheme

                            Layout.fillWidth: true
                            Layout.preferredHeight: 225

                            radius: 10

                            color: "#171D25"

                            border.width:
                                selectedTheme === "steam"
                                ? 5
                                : 1

                            border.color:
                                selectedTheme === "steam"
                                ? "#1A9FFF"
                                : steamMouse.containsMouse
                                  ? "#1A9FFF"
                                  : "#303943"

                            scale: steamMouse.containsMouse ? 1.015 : 1

                            Behavior on scale
                            {
                                NumberAnimation
                                {
                                    duration: 120
                                }
                            }

                            Column
                            {
                                anchors.fill: parent
                                anchors.margins: 12

                                spacing: 9


                                ///////////////////////////////////////////////////
                                // PREVIEW
                                ///////////////////////////////////////////////////

                                ThemePreview
                                {
                                    width: parent.width
                                    height: 135

                                    backgroundColor: "#1B2838"
                                    serverBarColor: "#171D25"
                                    channelColor: "#16202D"
                                    headerColor: "#171D25"

                                    itemColor: "#52677A"
                                    itemHoverColor: "#1A9FFF"
                                    accentColor: "#1A9FFF"

                                    textColor: "#D6D7D8"
                                    secondaryTextColor: "#8F98A4"
                                    mutedColor: "#52677A"
                                }


                                ///////////////////////////////////////////////////
                                // INFO
                                ///////////////////////////////////////////////////

                                RowLayout
                                {
                                    width: parent.width

                                    Label
                                    {
                                        text: "Steam"

                                        color: "white"

                                        font.pixelSize: 15
                                        font.bold: true

                                        Layout.fillWidth: true
                                    }

                                    Label
                                    {
                                        visible: selectedTheme === "steam"

                                        text: "✓"

                                        color: "#1A9FFF"

                                        font.pixelSize: 18
                                        font.bold: true
                                    }
                                }

                                Label
                                {
                                    width: parent.width

                                    text: "Steam-inspired dark gaming interface"

                                    color: "#8F98A4"

                                    font.pixelSize: 12
                                }
                            }

                            MouseArea
                            {
                                id: steamMouse

                                anchors.fill: parent

                                hoverEnabled: true

                                cursorShape: Qt.PointingHandCursor

                                onClicked:
                                {
                                    selectTheme("steam")
                                }
                            }
                        }
                    }
                }
            }


            ///////////////////////////////////////////////////////
            // FOOTER
            ///////////////////////////////////////////////////////

            Label
            {
                width: parent.width

                horizontalAlignment: Text.AlignHCenter

                color: "#80848E"

                font.pixelSize: 12

                text: "BeanChat appearance settings."
            }

            Item
            {
                width: 1
                height: 24
            }
        }
    }
}
