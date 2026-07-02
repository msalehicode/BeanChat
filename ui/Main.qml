import QtQuick
import QtQuick.Controls

import "overlays/"
import "managers/"


Window {
    id:rootWindow
    width: 980
    height: 700
    minimumWidth: 1143
    minimumHeight: 646
    visible: true
    title: user.appTitle

    Rectangle
    {
        id:windowBackground
        anchors.fill: parent
        color:"#1e2124"
    }

    function changePage(page)
    {
        if (page === "settings")
            mainStack.push(settingsPageComponent)
        else if (page === "pv")
            mainStack.push(privatePageComponent)
        else
            mainStack.pop()
    }


    StackView {
        id: mainStack
        anchors.fill: parent
        initialItem: mainPageComponent
    }

    Component {
        id: mainPageComponent
        MainPage {}
    }

    Component {
        id: settingsPageComponent
        SettingsPage {}
    }

    Component {
        id: privatePageComponent
        PrivateChatPage {}
    }


    //defined here to show notifications even if user is in different stack pages
    NotificationOverlay
    {
        anchors.fill: parent
        z: 99999
        /*
            QML usage:
                NotificationManager.error("you are not boy?!!",2000) -> this will show anyway
                NotificationManager.error("you are not boy?!!",2000,1) -> this would shown
                NotificationManager.error("you are not boy?!!",2000,1) -> this will be ignored due to at the same time we have this id (1)
        */
    }


    Connections
    {
        target:user
        function onNotificationRequested(type, text, id, duration)
        {
            if(id===-1)
            {
                NotificationManager.show({
                                             type: type,
                                             text: text,
                                             duration: duration
                                         })
            }
            else
            {
                NotificationManager.show({
                                             type: type,
                                             text: text,
                                             duration: duration,
                                             id
                                         })
            }


        }
    }


}
