import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import "./settingPages/"
import "constants/"

Page
{
    id: root

    Material.theme: Material.Dark
    Material.accent: Theme.current.accent

    property int currentPage: 0
    property bool weOpenedMic:false;

    background: Rectangle
    {
        color: Theme.current.background
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

                color: Theme.current.surface
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

                color: Theme.current.surface

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

                color: Theme.current.background

                StackLayout
                {
                    anchors.fill: parent

                    currentIndex: root.currentPage




                    ////////////////////////////////////////////////////
                    // PROFILE
                    ////////////////////////////////////////////////////

                    ProfilePage
                    {

                    }

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
                    // APPEARANCE
                    ////////////////////////////////////////////////////

                    AppearancePage
                    {

                    }


                    ////////////////////////////////////////////////////
                    // LANGUAGE
                    ////////////////////////////////////////////////////
                    LanguagePage
                    {

                    }


                    ////////////////////////////////////////////////////
                    // NOTIFICATIONS
                    ////////////////////////////////////////////////////

                    NotificationsPage
                    {

                    }


                    ////////////////////////////////////////////////////
                    // PRIVACY
                    ////////////////////////////////////////////////////

                    PrivacyPage
                    {

                    }


                    ////////////////////////////////////////////////////
                    // Update
                    ////////////////////////////////////////////////////

                    UpdatePage
                    {

                    }

                    ////////////////////////////////////////////////////
                    // ABOUT
                    ////////////////////////////////////////////////////

                    AboutPage
                    {

                    }
                }
            }
        }
    }


    Component.onCompleted:
    {
        //check if mic is open (because of microhpone test/capture we need to know if mic is not open open it)
        if(!microphone.started())
        {
            //start mic
            microphone.start() //to capture and show received sound from microphone for audio level visual
            weOpenedMic=true
        }
    }
    Component.onDestruction:
    {
        if(root.weOpenedMic)
            microphone.stop()
    }

}
