import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Item {
    anchors.fill: parent
    Button
    {
        text:"back"
        onClicked: rootWindow.changePage()
    }
    Text
    {
        text:user.messages
        color:"white"
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Rectangle
    {
        width: implicitWidth
        height: implicitHeight
        anchors.centerIn: parent
        color:"black"

        Column
        {
            TextInput
            {
                id:usernameText
                text: "usr"
                color: "white"
                width: 60
                height: implicitHeight
            }

            Button
            {
                text:"create channel"
                onClicked: user.createChannel("dota2","123456")
            }
            Button
            {
                text:"join channel"
                onClicked: user.joinChannel(2)
            }

            Button
            {
                text:"send message"
                onClicked: user.sendMessage("hellooooo")
            }
            Button
            {
                text:"login"
                onClicked: user.login(usernameText.text,"ABC123")
            }
        }
    }
}
