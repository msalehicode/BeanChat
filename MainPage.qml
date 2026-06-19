import QtQuick
import QtQuick.Controls

import CustomVideo 1.0
import QtQuick.Controls.Material

import QtQuick.Layouts

Item {
    anchors.fill: parent
    property int widthBase: 250 //uses for userlist and chanenlist


    property color bg1 : "#4a4c51"
    property color bg2: "#121315"

    property int iconH: 30
    property int iconW: 30

    Rectangle
    {
        anchors.fill: parent
        color:"black"
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
                            source:"icons/beanChatIcon.png"
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
                property int handleWidth: 10 //to tell others i've handle with this width

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

                    ListView
                    {
                        width: parent.width
                        height: parent.height-(serverTitle.height+userStuff.height)
                        clip: true
                        model: channelModel
                        spacing: 10
                        delegate: Column
                        {
                            width: ListView.view.width
                            spacing:10


                            Rectangle
                            {
                                id:theChannel
                                width: parent.width/1.05
                                height: 45
                                radius: height
                                color:"#2f3136"
                                anchors.horizontalCenter: parent.horizontalCenter
                                Row
                                {
                                    anchors.fill: parent
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    spacing: 5
                                    Image {
                                        id:theChannelTypeIcon
                                        width: 20
                                        height: width
                                        source: "icons/voice.png"
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                    Text
                                    {
                                        id:theChannelName
                                        anchors.verticalCenter: parent.verticalCenter
                                        text: channelName
                                        font.pixelSize: 15
                                        color:"white"
                                    }
                                }

                                MouseArea
                                {
                                    anchors.fill: parent
                                    onDoubleClicked:
                                    {
                                        console.log("try to join channel id:" , channelId, " name=", channelName)
                                        user.joinChannel(channelId)
                                    }
                                }
                            }

                            Repeater
                            {
                                model: users

                                delegate: Rectangle
                                {
                                    id:userBase
                                    width: parent.width
                                    anchors.left:parent.left
                                    anchors.leftMargin: 25
                                    height: 40
                                    color:"transparent"
                                    Rectangle
                                    {
                                        width: parent.width/1.20
                                        height: 40
                                        radius: height
                                        color: "#808185"
                                        RowLayout
                                        {
                                            anchors.fill: parent
                                            anchors.leftMargin: 10
                                            anchors.rightMargin: 10
                                            spacing: 5

                                            Rectangle
                                            {
                                                id: userAvatar
                                                Layout.preferredWidth: 20
                                                Layout.preferredHeight: 20
                                                Layout.alignment: Qt.AlignVCenter
                                                radius: width / 2
                                                color:  modelData.isTalking ? "yellow" : "white"
                                                border.width: modelData.isTalking ? 3 : 0
                                                border.color: "yellow"
                                                clip: true
                                            }

                                            Text
                                            {
                                                id: userName
                                                text: "(" + modelData.userid + ") " + modelData.username
                                                font.pixelSize: 15
                                                color: "white"
                                                font.bold:modelData.userid === user.myId
                                                Layout.alignment: Qt.AlignVCenter
                                            }

                                            Item
                                            {
                                                Layout.fillWidth: true
                                            }

                                            RowLayout
                                            {
                                                id: baseUserIcons
                                                spacing: 2
                                                Layout.alignment: Qt.AlignVCenter

                                                Image
                                                {
                                                    visible: true // modelData.isCameraOpen
                                                    Layout.preferredWidth: 20
                                                    Layout.preferredHeight: 20
                                                    fillMode: Image.PreserveAspectFit
                                                    source: "icons/videocall.png"
                                                }

                                                Image
                                                {
                                                    visible: modelData.muted
                                                    Layout.preferredWidth: 20
                                                    Layout.preferredHeight: 20
                                                    fillMode: Image.PreserveAspectFit
                                                    source: "icons/microphone-red.png"
                                                }


                                                Image
                                                {
                                                    visible: modelData.deafened
                                                    Layout.preferredWidth: 20
                                                    Layout.preferredHeight: 20
                                                    fillMode: Image.PreserveAspectFit
                                                    source: "icons/headphone-red.png"
                                                }
                                            }
                                        }
                                    }


                                }
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
                                    source: !user.muteMicrophone ? "icons/microphone.png" : "icons/microphone-red.png"
                                    anchors.centerIn: parent
                                }
                                MouseArea
                                {
                                    anchors.fill: parent
                                    onClicked: user.muteMicrophone=!user.muteMicrophone
                                }
                            }
                            Rectangle{
                                width:40
                                height:40
                                color:"transparent"
                                Image {
                                    width: iconW
                                    height: iconH
                                    source: !user.muteHeadphone? "icons/headphone.png" : "icons/headphone-red.png"
                                    anchors.centerIn: parent
                                }
                                MouseArea
                                {
                                    anchors.fill: parent
                                    onClicked: user.muteHeadphone=!user.muteHeadphone
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
                                    onClicked: console.log("camera")
                                }
                            }
                        }


                    }
                }

                // Right-edge handle
                Rectangle
                {
                    id: handle
                    width: parent.handleWidth
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
                width: parent.width-(chatBase.width+(channelList.width)+leftPanel.width)
                height: parent.height
                clip: true

                Rectangle {
                    id: centerContent
                    anchors.fill: parent
                    anchors.left: parent.left
                    anchors.leftMargin: channelList.handleWidth
                    color: "transparent"


                    GridView
                    {
                        id:channelClientsGridView

                        property int minTileWidth: 220
                        property int maxTileWidth: 320

                        property int columns:
                            Math.max(1,
                                     Math.ceil(Math.sqrt(count)))

                        property int rows:
                            Math.ceil(count / columns)

                        cellWidth: Math.max(
                                       minTileWidth,
                                       Math.min(
                                           maxTileWidth,
                                           parent.width / columns))

                        cellHeight: cellWidth * 9 / 16

                        property int contentRowsHeight:
                            rows * cellHeight

                        width: Math.min(
                                   parent.width,
                                   Math.min(count, columns) * cellWidth)

                        height: Math.min(
                                    parent.height,
                                    contentRowsHeight)

                        x: (parent.width - width) / 2

                        y: contentRowsHeight < parent.height
                           ? (parent.height - contentRowsHeight) / 2
                           : 0

                        model: participantModel

                        delegate:Rectangle
                        {
                            width: channelClientsGridView.cellWidth - 20
                            height:channelClientsGridView.cellHeight - 20
                            radius: 30
                            color: "grey"



                            Rectangle
                            {
                                id:userProfile
                                visible: !model.isCameraOpen
                                anchors.fill: parent
                                color:"purple"
                                radius: parent.radius
                                border.width: model.isTalking ? 4 : 0
                                border.color: "red"
                                Rectangle {
                                    id: userPicture
                                    width: 50
                                    height: width
                                    radius: width / 2
                                    anchors.centerIn: parent
                                    color: "white"
                                }

                                Text {
                                    text: model.username
                                    anchors.top: userPicture.bottom
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    color: "white"
                                    font.pixelSize: 15
                                }
                            }

                            Rectangle
                            {
                                id:userWithVideo
                                anchors.fill: parent
                                radius:parent.radius
                                visible: isCameraOpen
                                color:"transparent"
                                clip: true
                                VideoItem
                                {
                                    anchors.fill: parent
                                    sink: videoSink
                                    radius: 45
                                    Component.onCompleted: {
                                        console.log(username, videoSink)
                                    }
                                    Rectangle
                                    {
                                        id:indicatorTalkingWhenCameraIsOpen //cant show talking status over video so use this..
                                        anchors.fill: parent
                                        color:"transparent"
                                        radius: userWithVideo.radius
                                        border.width: model.isTalking ? 4 : 0
                                        border.color: "yellow"
                                    }
                                }
                                Text {
                                    text: model.username
                                    anchors.top: parent.top
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    color: "white"
                                    font.pixelSize: 15
                                }
                            }
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
