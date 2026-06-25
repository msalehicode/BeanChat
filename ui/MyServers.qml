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
            model: myServersModel
            spacing: 5
            delegate: Rectangle
            {
                id: delegateRoot

                anchors.horizontalCenter: parent.horizontalCenter

                width: parent.width / 1.5
                height: width
                radius: width / 2

                // Dark base instead of full green/orange
                color: "#2B2D31"

                border.width: model.isActive ? 3 : 2
                border.color: model.isActive
                              ? "#57F287"
                              : "#404249"

                Text
                {
                    anchors.centerIn: parent

                    text: model.name.length > 0
                          ? model.name[0].toUpperCase()
                          : "?"

                    color: "white"

                    font.bold: true
                    font.pixelSize: 22
                }

                Behavior on scale
                {
                    NumberAnimation
                    {
                        duration: 120
                    }
                }

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true

                    onEntered: delegateRoot.scale = 1.08
                    onExited: delegateRoot.scale = 1.0

                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onDoubleClicked: user.switchOrConnectToServer(model.ip, model.port, model.id)
                    onClicked: function(mouse)
                    {
                        if (mouse.button === Qt.RightButton)
                        {
                            var p = mapToItem(null, width, height/2)

                            myServersItemMenu.x = p.x + 12
                            myServersItemMenu.y = p.y - myServersItemMenu.height/2

                            myServersItemMenu.serverName = model.name
                            myServersItemMenu.ip = model.ip
                            myServersItemMenu.port = model.port
                            myServersItemMenu.id = model.id
                            myServersItemMenu.dbIndex = model.dbIndex
                            myServersItemMenu.isConnected = model.isActive

                            myServersItemMenu.open()
                        }
                    }

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
                onClicked: connectPopup.open()
            }
        }
    }


}
