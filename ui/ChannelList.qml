import QtQuick
import QtQuick.Controls.Material

import QtQuick.Layouts


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
            visible: user.isConnectedToServer
            height: 60
            Text
            {
                text: user.myServerName
                anchors.centerIn: parent
                color:"white"
            }
        }

        ListView
        {
            id:channelView
            width: parent.width
            height: user.isConnectedToServer ?
                        parent.height-(serverTitle.height+userStuff.height+userStuffOnServer.height)
                      :  parent.height-(userStuff.height)
            clip: true
            model: channelModel
            spacing: 10
            ScrollBar.vertical: ScrollBar
            {
                policy: ScrollBar.AsNeeded
                opacity:0.8
                width: 12
                visible: channelView.contentHeight > channelView.height

                contentItem: Rectangle
                {
                    implicitWidth: 8
                    radius: width / 2
                    color: "#72767d"
                }

                background: Rectangle
                {
                    color: "#202225"
                }
            }
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

                        Image
                        {
                            id:theChannelLockStatus
                            width:23
                            height: 20
                            visible: model.isLocked
                            source: "icons/lock.png"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onDoubleClicked:
                        {
                            console.log("try to join channel id:" , channelId, " name=", channelName)
                            if(model.isLocked)
                            {
                                //show popup enter password
                                channelPasswordPopup.channelId=model.channelId;
                                channelPasswordPopup.channelName=model.channelName;
                                channelPasswordPopup.open()
                            }
                            else
                                user.joinChannel(channelId,"") //non locked passwords default password is empty ""
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

                                    Layout.preferredWidth: 27
                                    Layout.preferredHeight: 27

                                    radius: width / 2
                                    color: "black"

                                    Rectangle
                                    {
                                        anchors.centerIn: parent

                                        width: parent.width + 4
                                        height: width

                                        radius: width / 2

                                        color: "transparent"

                                        border.width: 4
                                        border.color: "lime"

                                        visible: modelData.isTalking

                                        z: -1
                                    }
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
                                        visible: modelData.hasVideo
                                        Layout.preferredWidth: 20
                                        Layout.preferredHeight: 20
                                        fillMode: Image.PreserveAspectFit
                                        source: "icons/camera.png"
                                    }

                                    Image
                                    {
                                        visible: modelData.muted
                                        Layout.preferredWidth: 20
                                        Layout.preferredHeight: 20
                                        fillMode: Image.PreserveAspectFit
                                        source: "icons/microphone-closed.png"
                                    }


                                    Image
                                    {
                                        visible: modelData.deafened
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

        Rectangle
        {
            id: userStuffOnServer
            color: "#1e1f23"
            width: parent.width
            visible: user.isConnectedToServer
            height: 50

            // Left side
            Row
            {
                id: serverInfoRow

                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                spacing: 5


                Rectangle
                {
                    id:signalBase
                    width: iconW
                    height: iconW
                    color: "transparent"

                    property bool infoVisible: false

                    Image
                    {
                        anchors.fill: parent

                        source: signalIcon(user.myPing)

                        function signalIcon(ping)
                        {
                            if(ping < 0)
                                return "icons/signal.png"; //undefined

                            if(ping <= 60)
                                return "icons/signal-green.png";

                            if(ping <= 120)
                                return "icons/signal-yellow.png";

                            if(ping <= 200)
                                return "icons/signal-orange.png";

                            return "icons/signal-red.png";
                        }
                    }

                    MouseArea
                    {
                        anchors.fill: parent

                        onClicked:
                        {
                            parent.infoVisible = !parent.infoVisible
                        }
                    }

                    Rectangle
                    {
                        visible: parent.infoVisible

                        width: 170
                        height: 100

                        radius: 8

                        color: "#111214"
                        border.color: "#333333"

                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10

                        anchors.left: parent.left

                        z: 999

                        Rectangle
                        {
                            id:closeUserConnectionInfo
                            width:10
                            height:10
                            color:"red"
                            anchors
                            {
                                top:parent.top
                                topMargin:10
                                right:parent.right
                                rightMargin:10
                            }

                            MouseArea
                            {
                                anchors.fill: parent
                                onClicked: signalBase.infoVisible=false
                            }

                        }

                        Column
                        {
                            anchors.fill: parent
                            anchors.margins: 10

                            spacing: 4

                            Text
                            {
                                id:connnectionInfoLabel
                                text: "Connection Info"
                                color: "white"
                                font.bold: true
                            }

                            Text
                            {
                                text: "Ping: " + user.myPing + " ms"
                                color: "#b5bac1"
                            }

                            Text
                            {
                                text: "Voice Packet Loss: " + Number(user.myVoicePacketLoss).toFixed(2) + "%"
                                color: "#b5bac1"
                            }

                            Text
                            {
                                text: "Video Packet Loss: " + Number(user.myVideoPacketLoss).toFixed(2) + "%"
                                color: "#b5bac1"
                            }
                        }
                    }
                }


                Column
                {
                    spacing:0

                    Text
                    {
                        text: user.myServerName
                        color: "white"
                        font.bold: true
                        font.pixelSize: 14
                    }

                    Item
                    {
                        width: parent.width
                        height: 15
                        visible: user.myChannelName.length>0
                        Row
                        {
                            spacing: 5
                            Item
                            {
                                //spacer
                                width: 5
                                height: parent.height
                            }
                            Image {
                                width: 15
                                height: width
                                source: "icons/voice.png"
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Text
                            {
                                text: user.myChannelName
                                color: "#dadce0"
                                font.pixelSize: 12
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                    }


                }
            }

            // Right side buttons
            Row
            {
                id: serverActionButtonsRow

                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10


                Rectangle
                {
                    width: iconW
                    height: iconW
                    color: "transparent"

                    Image
                    {
                        anchors.fill: parent
                        source: user.isCameraOpen ? "icons/camera.png" : "icons/camera-closed.png"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: user.isCameraOpen = !user.isCameraOpen
                    }
                }

                Rectangle
                {
                    width: iconW
                    height: iconW
                    color: "transparent"

                    Image
                    {
                        anchors.fill: parent
                        source: "icons/screen-closed.png"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: console.log("share screen")
                    }
                }

                Rectangle
                {
                    width: iconW
                    height: iconW
                    color: "transparent"

                    Image
                    {
                        anchors.fill: parent
                        source: "icons/disconnect.png"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: user.disconnect();
                    }
                }

            }
        }

        Rectangle
        {
            id: userStuff
            color: bg2
            width: parent.width
            height: 65

            // Left side
            Row
            {
                id: userInfoRow
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter

                spacing: 5

                Rectangle
                {
                    id: avatar

                    width: 35
                    height: width
                    radius: width / 2

                    color: "grey"

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: console.log("prof")
                    }

                    // Status indicator
                    Rectangle
                    {
                        id: userStatus

                        width: 14
                        height: 14
                        radius: width / 2

                        anchors.right: parent.right
                        anchors.bottom: parent.bottom

                        anchors.rightMargin: -1
                        anchors.bottomMargin: -1

                        border.width: 2
                        border.color: bg2   // same as panel background

                        color: "lime"      // Online
                    }
                }

                Text
                {
                    id: usernameOnUserStuff
                    text: user.myUsername
                    color: "white"
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            // Right side buttons
            Row
            {
                id: actionButtonsRow

                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter

                spacing: 10

                Rectangle
                {
                    width: iconW
                    height: iconW
                    color: "transparent"

                    Image
                    {
                        anchors.fill: parent
                        source: !user.muteMicrophone
                                ? "icons/microphone.png"
                                : "icons/microphone-closed.png"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: user.muteMicrophone = !user.muteMicrophone
                    }
                }

                Rectangle
                {
                    width: iconW
                    height: iconW
                    color: "transparent"

                    Image
                    {
                        anchors.fill: parent
                        source: !user.muteHeadphone
                                ? "icons/headphone.png"
                                : "icons/headphone-closed.png"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: user.muteHeadphone = !user.muteHeadphone
                    }
                }


                Rectangle
                {
                    width: iconW
                    height: iconW
                    color: "transparent"

                    Image
                    {
                        anchors.fill: parent
                        source: "icons/settings.png"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: changePage("settings")
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

