import QtQuick
import QtQuick.Controls.Material

import QtQuick.Layouts


Item
{
    id:channelList

    readonly property int handleMinimumWidth: 450
    readonly property int handleMaximumWidth: 550
    readonly property int handleWidth: 8 //to tell others i've handle with this width


    width: handleMinimumWidth
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
            id: serverTitle

            width: parent.width
            height: 60
            visible: user.isConnectedToServer
            color: bg2

            property bool opened: false

            Column {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 16
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    text: "(" + user.myServerName + ")"
                    width: parent.width-34
                    font.bold: true
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: "white"
                }

                Text {
                    text: user.serverName + " [" + user.serverVersion + "] Uptime: " + user.serverUptime
                    width: parent.width-34
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: "white"
                }
            }

            Image
            {
                id: arrow

                width: 18
                height: 18

                anchors.right: parent.right
                anchors.rightMargin: 16
                anchors.verticalCenter: parent.verticalCenter

                source: "icons/arrow-right.png"

                rotation: serverTitle.opened ? 90 : 0

                transformOrigin: Item.Center

                Behavior on rotation
                {
                    NumberAnimation
                    {
                        duration: 150
                        easing.type: Easing.OutCubic
                    }
                }
            }

            MouseArea
            {
                anchors.fill: parent
                onClicked: serverTitle.opened = !serverTitle.opened
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
            flickableDirection: Flickable.VerticalFlick
            acceptedButtons: Qt.NoButton // Disable drag/flick with the mouse
            boundsMovement: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar
            {
                policy: ScrollBar.AsNeeded
                opacity:0.8
                width: 8
                visible: channelView.contentHeight > channelView.height

                contentItem: Rectangle
                {
                    width: 4
                    radius: width / 2
                    color: "#66676a"
                }

                background: Rectangle
                {
                    color: bg2
                }
            }
            delegate: Column
            {
                width: ListView.view.width
                spacing:10


                Rectangle
                {
                    id:theChannel
                    width: parent.width/1.02
                    height: 43
                    radius: height
                    anchors.horizontalCenter: parent.horizontalCenter
                    Row
                    {
                        anchors.fill: parent
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        spacing: 7
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
                            width: implicitWidth>250 ? 250 : implicitWidth
                            elide: Text.ElideRight
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
                        id:channelMouseArea
                        anchors.fill: parent
                        hoverEnabled: true

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


                    property bool dragHover: false

                    color: dragHover
                           ? "#4f545c"
                           : (channelMouseArea.containsMouse ? "#222427" : "#2f3136")

                    DropArea
                    {
                        anchors.fill: parent

                        onEntered:
                        {
                            theChannel.dragHover = true
                        }

                        onExited:
                        {
                            theChannel.dragHover = false
                        }

                        onDropped: function(drop)
                        {
                            theChannel.dragHover = false

                            console.log("Move user",
                                        drop.source.draggedUserId,
                                        "to channel",
                                        channelId)

                            if(user.isChannelLocked(channelId))
                            {
                                channelPasswordForMoveUserPopup.channelId = channelId
                                channelPasswordForMoveUserPopup.channelName = channelName

                                let draggedId = drop.source.draggedUserId
                                let targetChannel = channelId

                                channelPasswordForMoveUserPopup.onOk = function(password)
                                {
                                    user.moveUser(draggedId, targetChannel, password)
                                }

                                channelPasswordForMoveUserPopup.open()
                            }
                            else
                                user.moveUser(drop.source.draggedUserId
                                              ,channelId, "")
                        }
                    }
                    Image
                    {
                        source: "icons/settings.png"
                        width: 20
                        visible: channelMouseArea.containsMouse ? 1 : 0
                        height: width
                        anchors
                        {
                            right:parent.right
                            rightMargin: 10
                            verticalCenter:parent.verticalCenter
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked:
                            {
                                modifyChannelPopup.initialChannelName=model.channelName
                                modifyChannelPopup.initialChannelPassword= model.isLocked ? "***" : ""
                                modifyChannelPopup.initialSaveChats=model.saveChats
                                modifyChannelPopup.targetChannelId=model.channelId
                                modifyChannelPopup.open()
                            }
                        }
                    }
                }

                Repeater
                {
                    model: users

                    delegate: Rectangle
                    {
                        id:userBase
                        property int draggedUserId: modelData.userid

                        Drag.active: dragHandler.active
                        Drag.dragType: Drag.Automatic
                        Drag.hotSpot.x: width / 2
                        Drag.hotSpot.y: height / 2

                        width: parent.width
                        anchors.left:parent.left
                        anchors.leftMargin: 25
                        height: 40
                        color:"transparent"

                        DragHandler
                        {
                            id: dragHandler
                        }
                        Rectangle
                        {
                            width: parent.width-35
                            height: parent.height
                            radius: height
                            color: userMouseArea.containsMouse ? "#525356":"#66676a"
                            RowLayout
                            {
                                anchors.fill: parent
                                anchors.leftMargin: 15
                                anchors.rightMargin: 10
                                spacing: 10

                                Rectangle
                                {
                                    id: userAvatar

                                    Layout.preferredWidth: 27
                                    Layout.preferredHeight: 27

                                    radius: width / 2
                                    color: "#1E1F22"

                                    Text
                                    {
                                        anchors.centerIn: parent
                                        visible: modelData.avatarPath === ""
                                        text: "?"
                                        color: "white"
                                        font.pixelSize: 16
                                    }

                                    Image
                                    {
                                        id: avatar
                                        anchors.fill: parent
                                        source: modelData.avatarPath
                                    }

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
                                    font.pixelSize: 13
                                    color: "white"
                                    font.bold:modelData.userid === user.myId
                                    Layout.alignment: Qt.AlignVCenter
                                    Layout.preferredWidth: implicitWidth>200 ? 200 : implicitWidth
                                    elide: Text.ElideRight
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

                            MouseArea
                            {
                                id:userMouseArea
                                anchors.fill: parent
                                hoverEnabled: true
                                onClicked: console.log("clicked on user.")
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
                        height: 111

                        radius: 8

                        color: "#111214"
                        border.color: "#333333"

                        anchors.bottom: parent.top
                        anchors.bottomMargin: 10

                        anchors.left: parent.left

                        z: 999

                        Rectangle
                        {
                            id: closeButton
                            width: 18
                            height: 18
                            radius: 14
                            anchors
                            {
                                right:parent.right
                                rightMargin:10
                                top:parent.top
                                topMargin:10
                            }

                            color: mAreaCloseConnectionInfo.containsMouse ? "red" : "#3b3d43"

                            Text
                            {
                                anchors.centerIn: parent

                                text: "✕"

                                color: mAreaCloseConnectionInfo.containsMouse ? "white" : "#b9bbbe"

                                font.pixelSize: 15
                            }

                            MouseArea
                            {
                                id: mAreaCloseConnectionInfo
                                anchors.fill: parent

                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor

                                onClicked:  signalBase.infoVisible=false
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
                        text: user.serverName
                        color: "white"
                        font.bold: true
                        font.pixelSize: 14
                        width: implicitWidth>250 ? 250 : implicitWidth
                        elide: Text.ElideRight
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
                                width: implicitWidth>250 ? 250 : implicitWidth
                                elide: Text.ElideRight
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

                Rectangle
                {
                    width: iconW
                    height: iconW
                    color: "transparent"
                    visible: user.serverWebsite.length>0
                    Image
                    {
                        anchors.fill: parent
                        source: "icons/web.png"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: Qt.openUrlExternally(user.serverWebsite)
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
                    id:myAvatar
                    width: 35
                    height: width
                    radius: width / 2

                    color: "#1E1F22"
                    Text
                    {
                        anchors.centerIn: parent
                        visible: user.myAvatarPath === ""
                        text: "?"
                        color: "white"
                        font.pixelSize: 16
                    }

                    Image
                    {
                        source: user.myAvatarPath
                        anchors.fill: parent
                    }
                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            modifyProfilePopup.avatarSource=user.myAvatarPath
                            modifyProfilePopup.open()
                        }
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
                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked: console.log("clicked on my status indicator")
                        }
                    }
                }

                Text
                {
                    id: usernameOnUserStuff
                    text: user.myUsername
                    color: "white"
                    anchors.verticalCenter: parent.verticalCenter
                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked:
                        {
                            modifyProfilePopup.avatarSource=user.myAvatarPath
                            modifyProfilePopup.open()
                        }
                    }
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
                    visible: user.isConnectedToServer

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
                    visible: user.isConnectedToServer

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
        width: channelList.handleWidth
        height: parent.height
        x: parent.width - width+channelList.handleWidth
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
                    if(pos <channelList.handleMinimumWidth) //minimum with
                        pos=channelList.handleMinimumWidth
                    else if(pos>channelList.handleMaximumWidth) //max width
                        pos=channelList.handleMaximumWidth

                    channelList.width = pos
                }
            }
        }

    }


    Rectangle
    {
        id:bgServerMenuClickToClose
        anchors.fill: parent
        z: 998
        visible: serverTitle.opened
        color:"black"
        opacity: 0.3
        MouseArea
        {
            anchors.fill: parent
            onClicked: serverTitle.opened = false
        }
    }

    // ================================= MENU =================================
    ServerMenu
    {
        id: serverMenu

        width: parent.width
        y: serverTitle.height
        z: 999

        opened: serverTitle.opened

        onCreateChannelClicked:
        {
            serverTitle.opened = false
            createChannelPopup.open()
        }

        onSettingsClicked:
        {
            serverTitle.opened = false
            console.log("Settings")
        }
    }

}

