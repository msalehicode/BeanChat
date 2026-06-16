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

        if(page==="settings")
            mainStack.push(settingsPage)
        else if(page==="pv")
            mainStack.push(settingsPage)
        else
            mainStack.pop()
    }

    MainPage
    {
        id:mainPage
    }

    SettingsPage
    {
        id: settingsPage
    }

    StackView
    {
        id:mainStack
        anchors.fill: parent
        initialItem: mainPage
    }


}
