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

    property int iconH: 20
    property int iconW: 20

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
                            // onClicked:
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
                        model: 10
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


            ChannelList
            {
                id:channelList
            }

            Item
            {
                id:centerContentBase
                width: parent.width-(rightPanel.width+(channelList.width)+leftPanel.width)
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

                        cellWidth: 250
                        cellHeight: 150
                        anchors.fill: parent

                        // property int minTileWidth: 220
                        // property int maxTileWidth: 320

                        // property int columns:
                        //     Math.max(1,
                        //              Math.ceil(Math.sqrt(count)))

                        // property int rows:
                        //     Math.ceil(count / columns)

                        // cellWidth: Math.max(
                        //                minTileWidth,
                        //                Math.min(
                        //                    maxTileWidth,
                        //                    parent.width / columns))

                        // cellHeight: cellWidth * 9 / 16

                        // property int contentRowsHeight:
                        //     rows * cellHeight

                        // width: Math.min(
                        //            parent.width,
                        //            Math.min(count, columns) * cellWidth)

                        // height: Math.min(
                        //             parent.height,
                        //             contentRowsHeight)

                        // x: (parent.width - width) / 2

                        // y: contentRowsHeight < parent.height
                        //    ? (parent.height - contentRowsHeight) / 2
                        //    : 0

                        model: participantModel

                        delegate:Rectangle
                        {
                            // width: channelClientsGridView.cellWidth - 20
                            // height:channelClientsGridView.cellHeight - 20
                            width: channelClientsGridView.cellWidth - 10
                            height: channelClientsGridView.cellHeight - 10
                            radius: 30
                            color: "grey"

                            Rectangle
                            {
                                id:userWithVideo
                                anchors.fill: parent
                                radius:parent.radius
                                visible: model.isCameraOpen
                                color:"transparent"
                                clip: true
                                VideoItem
                                {
                                    anchors.fill: parent
                                    sink: model.videoSink
                                    radius: 45 //due to its C++ Property radius value may be varied to normal qml radiuses. e.g 30 has to be 45 too looks simular
                                    Component.onCompleted:
                                    {
                                        console.log("building VideItem, username=",username, "sink=",videoSink, "isCameraOpen=",isCameraOpen)
                                    }
                                }
                            }


                            Rectangle
                            {
                                id:baseUserParticipantInfo
                                anchors.fill: parent
                                radius: parent.radius
                                color: model.isCameraOpen ? "transparent" : "purple"
                                border.width: model.isTalking ? 4 : 0
                                border.color: "lime"

                                Rectangle
                                {
                                    id:userAvatarCenterParticipant
                                    width: 50
                                    height: 50
                                    color: "white"
                                    radius: 30
                                    visible: !model.isCameraOpen
                                    anchors.centerIn: parent
                                }
                                Rectangle
                                {
                                    id:userStatusIconsOverParticipant
                                    width: parent.width
                                    height: 40
                                    radius: height
                                    color: "transparent"
                                    anchors.bottom: parent.bottom

                                    RowLayout
                                    {
                                        anchors.fill: parent
                                        anchors.leftMargin: 10
                                        anchors.rightMargin: 10
                                        spacing: 5

                                        Text
                                        {
                                            text: "(" + model.userId + ") " + model.username
                                            font.pixelSize: 15
                                            color: "white"
                                            font.bold:model.userId === user.myId
                                            Layout.alignment: Qt.AlignVCenter
                                        }

                                        Item
                                        {
                                            Layout.fillWidth: true
                                        }

                                        RowLayout
                                        {
                                            spacing: 2
                                            Layout.alignment: Qt.AlignVCenter

                                            // Image
                                            // {
                                            //     visible: model.isScreenShare
                                            //     Layout.preferredWidth: 20
                                            //     Layout.preferredHeight: 20
                                            //     fillMode: Image.PreserveAspectFit
                                            //     source: "icons/screen.png"
                                            // }

                                            Image
                                            {
                                                visible: model.isMuted
                                                Layout.preferredWidth: 20
                                                Layout.preferredHeight: 20
                                                fillMode: Image.PreserveAspectFit
                                                source: "icons/microphone-closed.png"
                                            }


                                            Image
                                            {
                                                visible: model.isDeafened
                                                Layout.preferredWidth: 20
                                                Layout.preferredHeight: 20
                                                fillMode: Image.PreserveAspectFit
                                                source: "icons/headphone-closed.png"
                                            }
                                        }
                                    }
                                }

                            }


                        }
                    }

                }
            }

            Item
            {
                id:rightPanel
                width: 400
                height: parent.height
                clip: true
                property int currentTab: 0

                Rectangle
                {
                    anchors.fill: parent
                    color: "black"
                    ColumnLayout
                    {
                        anchors.fill: parent

                        RowLayout
                        {
                            Layout.fillWidth: true
                            spacing: 2

                            Rectangle
                            {
                                Layout.fillWidth: true
                                height: 40

                                color: rightPanel.currentTab === 0 ? "#2a82da" : "#222"

                                Row
                                {
                                    anchors.centerIn: parent
                                    Image
                                    {
                                        width: 30
                                        height: 30
                                        source: "icons/users.png"
                                    }

                                    Text
                                    {
                                        text: "Users"
                                        color: "white"
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }
                                MouseArea
                                {
                                    anchors.fill: parent
                                    onClicked: rightPanel.currentTab = 0
                                }
                            }

                            Rectangle
                            {
                                Layout.fillWidth: true
                                height: 40

                                color: rightPanel.currentTab === 1 ? "#2a82da" : "#222"

                                Row
                                {
                                    anchors.centerIn: parent
                                    Image
                                    {
                                        width: 30
                                        height: 30
                                        source: "icons/chat.png"
                                    }

                                    Text
                                    {
                                        text: "Chat"
                                        color: "white"
                                        anchors.verticalCenter: parent.verticalCenter

                                    }
                                }


                                MouseArea
                                {
                                    anchors.fill: parent
                                    onClicked: rightPanel.currentTab = 1
                                }
                            }

                        }


                        StackLayout
                        {
                            id:rightPanelStackLayout
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            currentIndex: rightPanel.currentTab

                            ConnectedUsers {}

                            TextChat {}

                        }
                    }


                    Rectangle
                    {
                        id: handleRight
                        width: 15
                        height: parent.height
                        x: -width / 2
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
                                startWidth = rightPanel.width
                            }

                            onPositionChanged: function(mouse)
                            {
                                if (pressed)
                                {
                                    let dx = mouse.x - startX
                                    let pos = startWidth - dx
                                    if (pos < 250)
                                        pos = 250
                                    else if (pos > 800)
                                        pos = 800
                                    rightPanel.width = pos
                                }
                            }
                        }
                    }

                }

            }


        }

    }


}
