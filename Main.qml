import QtQuick
import QtQuick.Controls

Window {
    id:rootWindow
    width: 980
    height: 700
    minimumWidth: 310
    minimumHeight: 439
    visible: true
    title: qsTr("Bean Chat")

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
