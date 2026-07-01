import QtQuick
import QtQuick.Controls

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


}
