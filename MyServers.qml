import QtQuick
import QtQuick.Controls

// import QtQuick.Controls.Material

// import QtQuick.Layouts

Item
{
    width:60
    height:parent.height
    Rectangle
    {
        color: "transparent"
        anchors.fill: parent
    }
    Column
    {
        anchors.fill: parent
        Rectangle
        {
            id:beanChatButton
            color:"black"
            width:60
            height: 60
            anchors.horizontalCenter: parent.horizontalCenter
            Image
            {
                width: 60
                height: 60
                source:"icons/beanChatIcon.png"
                anchors.centerIn: parent
            }
            MouseArea
            {
                anchors.fill: parent
                onClicked: console.log("clicked on beanchat.")
            }
            Rectangle
            {
                id:beanChatNotificationIndicator
                color: "red"
                width: 20
                height: width
                radius: width
                anchors
                {
                    bottom: parent.bottom
                    left: parent.right
                    leftMargin: -20
                }
                Text {
                    text: "5"
                    anchors.centerIn: parent
                    color:"white"
                    font.bold: true
                    font.pixelSize: 12
                }
            }
        }

        ListView
        {
            id:myServers
            width: parent.width
            height: parent.height-(beanChatButton.height+addServer.height+10)
            clip: true
            model: 35
            spacing: 5
            delegate: Rectangle
            {
                anchors.horizontalCenter:parent.horizontalCenter
                width:parent.width/1.5
                height: width
                radius: width
                color:"pink"
                Text
                {
                    text:"SER " + index
                    anchors.centerIn: parent
                    color:"black"
                }
                MouseArea
                {
                    anchors.fill: parent
                    onClicked: model.index===0 ? user.login() : console.log(" load server index="+model.index)
                }
            }
        }

        Rectangle
        {
            id:addServer
            color:"black"
            anchors.horizontalCenter:parent.horizontalCenter
            width:parent.width/1.5
            height: width
            radius: width
            border.width: 4
            border.color: "purple"
            Text
            {
                text: "+"
                font.pixelSize: 30
                color: "purple"
                anchors.centerIn: parent
            }
            MouseArea
            {
                anchors.fill: parent
                onClicked:console.log("add new server")
            }
        }
    }


}
