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
            // HEADER
            ///////////////////////////////////////////////////////

            Label
            {
                text: "Language"

                color: "white"

                font.pixelSize: 28
                font.bold: true
            }

            Label
            {
                text: "Choose the language used throughout BeanChat."

                color: "#B5BAC1"

                font.pixelSize: 14

                wrapMode: Text.WordWrap
            }


            ///////////////////////////////////////////////////////
            // LANGUAGE
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: "#2B2D31"

                border.color: "#404249"

                implicitHeight: languageContent.implicitHeight + 32


                Column
                {
                    id: languageContent

                    anchors.fill: parent

                    anchors.margins: 16

                    spacing: 14


                    Label
                    {
                        text: "Interface Language"

                        color: "white"

                        font.pixelSize: 18
                        font.bold: true
                    }


                    Label
                    {
                        text: "Select the language you want BeanChat to use."

                        color: "#B5BAC1"

                        wrapMode: Text.WordWrap
                    }


                    SettingCombo
                    {
                        id: languageCombo

                        width: parent.width

                        model:
                        [
                            "English",
                        ]

                        currentIndex: 0

                        onActivated:
                        {
                            // settings.setValue(
                            //     "Language",
                            //     currentIndex
                            // )
                        }
                    }
                }
            }

        }
    }
}
