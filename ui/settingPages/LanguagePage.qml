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

                color: Theme.current.text

                font.pixelSize: 28
                font.bold: true
            }

            Label
            {
                text: "Choose the language used throughout BeanChat."

                color: Theme.current.textSecondary

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

                color: Theme.current.surface
                border.color: Theme.current.border

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

                        color: Theme.current.text

                        font.pixelSize: 18
                        font.bold: true
                    }


                    Label
                    {
                        text: "Select the language you want BeanChat to use."

                        color: Theme.current.textSecondary

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
