import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import BeanChat

import "../components"
import "../constants"
Page
{
    id: root

    Material.theme: Material.Dark
    Material.accent: Theme.current.accent

    property string selectedTheme: user.currentTheme

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
                color: Theme.current.text
                font.pixelSize: 28
                font.bold: true
            }

            Label
            {
                text: "Customize the look and feel of BeanChat"
                color: Theme.current.textSecondary
                font.pixelSize: 14
            }


            ///////////////////////////////////////////////////////
            // THEMES
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width
                radius: 10
                color: Theme.current.surface
                border.color: Theme.current.border
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
                            color: Theme.current.text
                            font.pixelSize: 18
                            font.bold: true
                        }

                        Label
                        {
                            width: parent.width
                            text: "Choose a theme for BeanChat. You can change the appearance whenever you want."
                            color: Theme.current.textSecondary
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
                        // DARK THEME
                        ///////////////////////////////////////////////////

                        Rectangle
                        {
                            id: darkTheme
                            Layout.fillWidth: true
                            Layout.preferredHeight: 225
                            radius: 10
                            color: "#232428"
                            border.width:
                                selectedTheme === "dark"
                                ? 5
                                : 1

                            border.color:
                                selectedTheme === "dark"
                                ? "#5865F2"
                                : darkMouse.containsMouse
                                  ? "#5865F2"
                                  : "#404249"

                            scale: darkMouse.containsMouse ? 1.015 : 1

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
                                        text: "Dark"
                                        color: "white"
                                        font.pixelSize: 15
                                        font.bold: true
                                        Layout.fillWidth: true
                                    }

                                    Label
                                    {
                                        visible: selectedTheme === "dark"
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
                                id: darkMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked:
                                {
                                    Theme.setTheme("dark")
                                }
                            }
                        }


                        ///////////////////////////////////////////////////
                        // LIGHT THEME
                        ///////////////////////////////////////////////////

                        Rectangle
                        {
                            id: lightTheme
                            Layout.fillWidth: true
                            Layout.preferredHeight: 225
                            radius: 10
                            color: "#E9ECEF"
                            border.width:
                                selectedTheme === "light"
                                ? 5
                                : 1

                            border.color:
                                selectedTheme === "light"
                                ? "#2580C2"
                                : lightMouse.containsMouse
                                  ? "#2580C2"
                                  : "#404249"

                            scale: lightMouse.containsMouse ? 1.015 : 1

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
                                        text: "Light"
                                        color: "#202428"
                                        font.pixelSize: 15
                                        font.bold: true
                                        Layout.fillWidth: true
                                    }

                                    Label
                                    {
                                        visible: selectedTheme === "light"
                                        text: "✓"
                                        color: "#2580C2"
                                        font.pixelSize: 18
                                        font.bold: true
                                    }
                                }

                                Label
                                {
                                    width: parent.width
                                    text: "Classic light appearance"
                                    color: "#626970"
                                    font.pixelSize: 12
                                }
                            }

                            MouseArea
                            {
                                id: lightMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked:
                                {
                                    Theme.setTheme("light")
                                }
                            }
                        }


                        ///////////////////////////////////////////////////
                        // GREEN
                        ///////////////////////////////////////////////////

                        Rectangle
                        {
                            id: greenTheme
                            Layout.fillWidth: true
                            Layout.preferredHeight: 225
                            radius: 10
                            color: "#080A0A"

                            border.width:
                                selectedTheme === "green"
                                ? 5
                                : 1

                            border.color:
                                selectedTheme === "green"
                                ? "#44FF00"
                                : greenMouse.containsMouse
                                  ? "#44FF00"
                                  : "#303530"

                            scale: greenMouse.containsMouse ? 1.015 : 1

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
                                        text: "Green"
                                        color: "white"
                                        font.pixelSize: 15
                                        font.bold: true
                                        Layout.fillWidth: true
                                    }

                                    Label
                                    {
                                        visible: selectedTheme === "green"
                                        text: "✓"
                                        color: "#44FF00"
                                        font.pixelSize: 18
                                        font.bold: true
                                    }
                                }

                                Label
                                {
                                    width: parent.width
                                    text: "Razer-inspired green gaming interface"
                                    color: "#7E897E"
                                    font.pixelSize: 12
                                }
                            }

                            MouseArea
                            {
                                id: greenMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked:
                                {
                                    Theme.setTheme("green")
                                }
                            }
                        }


                        ///////////////////////////////////////////////////
                        // DARKBLUE
                        ///////////////////////////////////////////////////

                        Rectangle
                        {
                            id: darkblueTheme
                            Layout.fillWidth: true
                            Layout.preferredHeight: 225
                            radius: 10
                            color: "#171D25"

                            border.width:
                                selectedTheme === "darkblue"
                                ? 5
                                : 1

                            border.color:
                                selectedTheme === "darkblue"
                                ? "#1A9FFF"
                                : darkblueMouse.containsMouse
                                  ? "#1A9FFF"
                                  : "#303943"

                            scale: darkblueMouse.containsMouse ? 1.015 : 1

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
                                        text: "DarkBlue"
                                        color: "white"
                                        font.pixelSize: 15
                                        font.bold: true
                                        Layout.fillWidth: true
                                    }

                                    Label
                                    {
                                        visible: selectedTheme === "darkblue"
                                        text: "✓"
                                        color: "#1A9FFF"
                                        font.pixelSize: 18
                                        font.bold: true
                                    }
                                }

                                Label
                                {
                                    width: parent.width
                                    text: "Steam-inspired darkblue gaming interface"
                                    color: "#8F98A4"
                                    font.pixelSize: 12
                                }
                            }

                            MouseArea
                            {
                                id: darkblueMouse
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked:
                                {
                                    Theme.setTheme("darkblue")
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
                color: Theme.current.textMuted
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
