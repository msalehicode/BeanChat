import QtQuick
import QtQuick.Controls

import CustomVideo 1.0

Item {
    anchors.fill: parent
    property int widthBase: 200 //uses for userlist and chanenlist


    property color bg1 : "#4a4c51"
    property color bg2: "#121315"

    property int iconH: 30
    property int iconW: 30


    ListModel
    {
        id:listmodelChannelClients

        ListElement {
            name: "Alex"
            camera: 0
            talking: 0
        }

        ListElement {
            name: "P.A.thfinder"
            camera: 1
            talking: 1
        }
    }

    ListModel {
        id: channelsModel

        ListElement {
            type: "file"
            name: "GENERAL"
        }

        ListElement {
            type: "text"
            name: "general"
        }

        ListElement {
            type: "text"
            name: "memes"
        }

        ListElement {
            type: "voice"
            name: "Voice Chat"
        }

        ListElement {
            type: "user"
            name: "Alice"
            videoCall:false
            talking:false
            indent: 1
        }

        ListElement {
            type: "user"
            name: "Bob"
            videoCall:false
            talking:false
            indent: 1
        }


        ListElement {
            type: "voice"
            name: "Voice Chat 2"
        }

        ListElement {
            type: "user"
            name: "Alex"
            talking:false
            videoCall:false
            indent: 1
        }

        ListElement {
            type: "user"
            name: "P.A.thfinder"
            talking:true
            videoCall:true
            indent: 1
        }
    }

    Column
    {
        anchors.fill: parent
        Item
        {
            id:connectionStatusBase
            width: parent.width
            height:25
            // visible: false
            Rectangle
            {
                anchors.fill: parent
                color: "red"
                Text
                {
                    text:"You are offline"
                    anchors.centerIn: parent
                    color:"white"
                    font.bold: true
                }
            }
        }


        Row
        {
            width: parent.width
            height: parent.height-connectionStatusBase.height
            Item
            {
                id:leftPanel
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
                        id:accountButton
                        color:"black"
                        width:60
                        height: 60
                        anchors.horizontalCenter: parent.horizontalCenter
                        Image
                        {
                            width: 60
                            height: 60
                            source:"beanChatIcon.png"
                            anchors.centerIn: parent
                        }
                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: changePage("pv")
                        }
                        Rectangle
                        {
                            id:privateChatNotificationIndicator
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
                        clip: true
                        height: parent.height-(accountButton.height+addServer.height+10)
                        model: 20
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
            Item
            {
                id:channelList
                width: widthBase
                height: parent.height

                Rectangle
                {
                    anchors.fill: parent
                    color: bg1
                }
                Column
                {
                    anchors.fill: parent
                    Rectangle
                    {
                        id:serverTitle
                        color: bg2
                        width:parent.width
                        height: 60
                        Text
                        {
                            text:"SERVER TITLE"
                            anchors.centerIn: parent
                            color:"white"
                        }
                    }

                    ListView {
                        width: parent.width
                        height: parent.height-(serverTitle.height+userStuff.height)
                        model: channelsModel
                        clip: true
                        spacing: 5

                        delegate: Rectangle {
                            width: parent.width/1.20
                            height: 40
                            radius: height
                            // anchors.horizontalCenter: parent.horizontalCenter
                            color:type!=="user"? "#2f3136" :"#808185"
                            anchors.left: parent.left
                            anchors.leftMargin: indent ? indent * parent.width/10 : 5


                            Row {
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 5

                                Rectangle
                                {
                                    visible: type==="user" ? true : false
                                    width: 30
                                    height: width
                                    radius: width
                                    color: talking ? "#e5c35b" : "#b29747"
                                }

                                Image
                                {
                                    visible: type!=="user"?true:false
                                    width: 30
                                    height: width
                                    source: (function(status) {
                                        switch(status) {
                                            case "voice": return "icons/voice.png";
                                            case "text":  return "icons/chat.png";
                                            case "file": return "icons/folder.png";
                                            default:  return "";
                                        }
                                    })(type)
                                }

                                Text {
                                    text: name
                                    color:"white"
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                            Image
                            {
                                visible: type==="user"? (videoCall?true:false) : false
                                width: 25
                                height: width
                                anchors.right: parent.right
                                anchors.rightMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
                                source: "icons/videocall.png"
                            }
                        }
                    }

                    Rectangle
                    {
                        id:userStuff
                        color:bg2
                        width:parent.width
                        height: 60
                        Row
                        {
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 5
                            Rectangle{
                                width:40
                                height:40
                                radius: 40
                                color:"white"
                                Text
                                {
                                    text:"prof"
                                    anchors.centerIn: parent
                                    color:"white"
                                }
                                MouseArea
                                {
                                    anchors.fill: parent
                                    onClicked: console.log("prof")
                                }
                            }
                            Rectangle{
                                width:40
                                height:40
                                color:"transparent"
                                Image {
                                    width: iconW
                                    height: iconH
                                    source: "icons/microphone.png"
                                    anchors.centerIn: parent
                                }
                                MouseArea
                                {
                                    anchors.fill: parent
                                    onClicked: console.log("mic")
                                }
                            }
                            Rectangle{
                                width:40
                                height:40
                                color:"transparent"
                                Image {
                                    width: iconW
                                    height: iconH
                                    source: "icons/headphone.png"
                                    anchors.centerIn: parent
                                }
                                MouseArea
                                {
                                    anchors.fill: parent
                                    onClicked: console.log("spkr")
                                }
                            }

                            Rectangle{
                                width:40
                                height:40
                                color:"transparent"
                                Image {
                                    width: iconW
                                    height: iconH
                                    source: "icons/videocall.png"
                                    anchors.centerIn: parent
                                }
                                MouseArea
                                {
                                    anchors.fill: parent
                                    onClicked: camera.stop();
                                }
                            }
                        }


                    }
                }

                // Right-edge handle
                Rectangle
                {
                    id: handle
                    width: 15
                    height: parent.height
                    x: parent.width - width+10
                    y: 0
                    color: "grey"

                    MouseArea
                    {
                        anchors.fill: parent
                        cursorShape: Qt.SizeHorCursor
                        property real startX: 0
                        property real startWidth: 0

                        onPressed: function(mouse)
                        {
                            startX = mouse.x
                            startWidth = channelList.width
                        }

                        onPositionChanged: function(mouse)
                        {
                            if (pressed)
                            {
                                let dx = mouse.x - startX
                                let pos = Math.max(50, startWidth + dx);
                                if(pos <250) //minimum with is 250
                                    pos=250
                                else if(pos>800) //max width is 800
                                    pos=800

                                channelList.width = pos
                            }
                        }
                    }

                }

            }

            Item
            {
                id:centerContentBase
                width: parent.width-(chatBase.width+channelList.width+leftPanel.width)
                height: parent.height
                clip: true

                Rectangle {
                    id: centerContent
                    anchors.fill: parent
                    color: "transparent"

                    Item {
                        id: gridContainer

                        width: clientsGrid.width
                        height: clientsGrid.height

                        anchors.centerIn: parent

                        Grid {
                            id: clientsGrid

                            property int cellWidth: 300
                            property int cellHeight: 150

                            spacing: 10

                            columns: Math.max(
                                         1,
                                         Math.floor(centerContent.width /
                                                    (cellWidth + spacing))
                                     )

                            Repeater {
                                model: listmodelChannelClients

                                delegate: Rectangle {
                                            width: clientsGrid.cellWidth
                                            height: clientsGrid.cellHeight
                                            radius: 15
                                            color: "grey"

                                            border.width: model.talking ? 2.5 : 0
                                            border.color: "yellow"

                                            Rectangle {
                                                id: userPicture
                                                width: 50
                                                height: width
                                                radius: width / 2
                                                anchors.centerIn: parent
                                                color: "white"
                                            }

                                            Text {
                                                text: name
                                                anchors.top: userPicture.bottom
                                                anchors.horizontalCenter: parent.horizontalCenter
                                                color: "white"
                                                font.pixelSize: 15
                                            }
                                        }
                            }

                            width: childrenRect.width
                            height: childrenRect.height
                        }
                    }
                }
            }

            Item
            {
                id:chatBase
                width: 400
                height: parent.height
                // anchors.fill: parent
                clip: true
                TextChat {
                    // anchors.fill: parent

                }

            }


        }

    }


}
