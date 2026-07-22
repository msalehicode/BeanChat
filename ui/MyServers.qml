import QtQuick
import QtQuick.Controls

// import QtQuick.Controls.Material
// import QtQuick.Layouts

Item
{
    width:65
    height:parent.height
    Rectangle
    {
        color: "transparent"
        anchors.fill: parent
    }
    Column
    {
        anchors.fill: parent
        readonly property int spacerHeight:5
        Rectangle
        {
            //spacer
            width: parent.width
            height: parent.spacerHeight
            color: "transparent"
        }

        Rectangle
        {
            id:beanChatButton
            color:"transparent"
            width:65
            height: 65
            anchors.horizontalCenter: parent.horizontalCenter
            Image {
                width: 65
                height: 60
                source: "icons/beanChatIcon.png"
                fillMode: Image.PreserveAspectFit
                smooth: true
                mipmap: true
                antialiasing: true
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
                    text: "64"
                    anchors.centerIn: parent
                    color:"white"
                    font.bold: true
                    font.pixelSize: 12
                }
            }
            MouseArea
            {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: logger.action("clicked on beanchat.")
            }
        }

        Rectangle
        {
            //spacer
            width: parent.width
            height: parent.spacerHeight
            color: "transparent"
        }

        Rectangle
        {
            width: parent.width
            height: 1
            color: "#404249"
        }

        Rectangle
        {
            //spacer
            width: parent.width
            height: parent.spacerHeight
            color: "transparent"
        }




        ListView
        {
            id:myServers
            width: parent.width
            height: parent.height-(beanChatButton.height+addServer.height+parent.spacerHeight*4)
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

                color: "#0B1018"


                Image
                {
                    id:serverAvatar
                    source: model.avatarPath
                    visible: model.avatarPath.length>0
                    anchors.fill: parent
                }

                Text
                {
                    anchors.centerIn: parent
                    visible: !serverAvatar.visible
                    text: model.name.length > 0
                          ? model.name[0].toUpperCase()
                          : "?"

                    color: "white"

                    font.bold: true
                    font.pixelSize: 22
                }

                Rectangle
                {
                    anchors.fill: parent
                    radius: parent.radius
                    color: "transparent"
                    border.width: 3
                    border.color: model.isActive ? "#57F287" : "#25314B"
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
                    cursorShape: Qt.PointingHandCursor
                    onEntered:
                    {
                        delegateRoot.color = "#182033"
                        delegateRoot.scale = 1.12
                    }
                    onExited:
                    {
                        delegateRoot.color = "#101624"
                        delegateRoot.scale = 1.0
                    }

                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onDoubleClicked:
                    {
                        logger.action("double clicked on server on myServers. db-index="+model.dbIndex+" model-id="+model.id)
                        //update server id, to know which server is connecting to (for avatars path)
                        user.connectedServerId=model.dbIndex

                        //connect
                        user.switchOrConnectToServer(model.ip, model.port, model.id)
                    }
                    onClicked: function(mouse)
                    {
                        if (mouse.button === Qt.RightButton)
                        {
                            logger.action("right clicked on server,  db-index="+model.dbIndex+" model-id="+model.id)
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
                cursorShape: Qt.PointingHandCursor
                onClicked:
                {
                    logger.action("clicked on add server")
                    connectPopup.open()
                }
            }
        }
    }


}
