import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import "./settingPages/"
Page
{
    id: root

    Material.theme: Material.Dark
    Material.accent: "#5865F2"

    property int currentPage: 0

    background: Rectangle
    {
        color: "#313338"
    }



    ColumnLayout
    {
        anchors.fill: parent
        spacing: 0

        // Top bar
        Item
        {
            Layout.fillWidth: true
            Layout.preferredHeight: 70
            Rectangle
            {
                width: rectangleSideBar.width
                height: parent.height

                color: "#2B2D31"
            }
            RoundButton
            {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 24

                width: 46
                height: 46

                text: "←"

                onClicked: rootWindow.changePage()
            }
        }


        //Main content

        RowLayout
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0


            ////////////////////////////////////////////////////////////
            // Sidebar
            ////////////////////////////////////////////////////////////

            Rectangle
            {
                id:rectangleSideBar
                Layout.preferredWidth: 240
                Layout.fillHeight: true

                color: "#2B2D31"

                SettingsSidebar
                {
                    id: sidebar

                    anchors.fill: parent

                    currentIndex: root.currentPage

                    onCurrentIndexChanged:
                        root.currentPage = currentIndex
                }
            }

            ////////////////////////////////////////////////////////////
            // Content
            ////////////////////////////////////////////////////////////

            Rectangle
            {
                Layout.fillWidth: true
                Layout.fillHeight: true

                color: "#313338"

                StackLayout
                {
                    anchors.fill: parent

                    currentIndex: root.currentPage

                    ////////////////////////////////////////////////////
                    // Audio input
                    ////////////////////////////////////////////////////

                    AudioInputPage
                    {
                    }



                    ////////////////////////////////////////////////////
                    // Audio output
                    ////////////////////////////////////////////////////

                    AudioOutputPage
                    {

                    }


                    ////////////////////////////////////////////////////
                    // Video
                    ////////////////////////////////////////////////////

                    CameraInputPage
                    {

                    }

                    ////////////////////////////////////////////////////
                    // Appearance
                    ////////////////////////////////////////////////////


                    Item
                    {
                        Label
                        {
                            anchors.centerIn: parent

                            text: "Appearance\nComing Soon"

                            horizontalAlignment: Text.AlignHCenter

                            color: "white"

                            font.pixelSize: 28

                            font.bold: true
                        }
                    }

                    ////////////////////////////////////////////////////
                    // Notifications
                    ////////////////////////////////////////////////////

                    Item
                    {
                        Label
                        {
                            anchors.centerIn: parent

                            text: "Notifications\nComing Soon"

                            horizontalAlignment: Text.AlignHCenter

                            color: "white"

                            font.pixelSize: 28

                            font.bold: true
                        }
                    }

                    ////////////////////////////////////////////////////
                    // Privacy
                    ////////////////////////////////////////////////////

                    Item
                    {
                        Label
                        {
                            anchors.centerIn: parent

                            text: "Privacy\nComing Soon"

                            horizontalAlignment: Text.AlignHCenter

                            color: "white"

                            font.pixelSize: 28

                            font.bold: true
                        }
                    }

                    ////////////////////////////////////////////////////
                    // Advanced
                    ////////////////////////////////////////////////////

                    AdvancePage
                    {

                    }
                }
            }
        }
    }

}
