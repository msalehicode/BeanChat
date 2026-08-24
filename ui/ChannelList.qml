import QtQuick
import QtQuick.Controls.Material

import QtQuick.Layouts
import BeanChatClient 1.0

import "constants/"
import "popups/"
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
        color: Theme.current.background
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
            color: Theme.current.surface3

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
                    color: Theme.current.text
                }

                Text {
                    text: user.serverName + " [" + user.serverVersion + "] Uptime: " + user.serverUptime
                    width: parent.width-34
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    color: Theme.current.text
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
                cursorShape: Qt.PointingHandCursor
                onClicked:
                {
                    logger.action("clicked on server title to open/close server's setting... popup")
                    serverTitle.opened = !serverTitle.opened
                }

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
                    color: Theme.current.icon
                }

                background: Rectangle
                {
                    color: Theme.current.surface2
                }
            }
            delegate: Column
            {
                width: ListView.view.width
                spacing:10


                Rectangle
                {
                    id:theChannel
                    width: parent.width/1.04
                    height: 43
                    radius: height
                    anchors.horizontalCenter: parent.horizontalCenter
                    border.color: model.saveChats ? Theme.current.success : Theme.current.border
                    border.width: 2
                    color: dragHover
                           ? Theme.current.selected
                           : (channelMouseArea.containsMouse ? Theme.current.hover : Theme.current.surface2)

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
                            visible:  model.channelType===ChannelType.Voice
                            source: "icons/voice.png"
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Text
                        {
                            text:"#"
                            color: Theme.current.channelText
                            font.pixelSize: 15
                            visible:  model.channelType===ChannelType.Text
                            anchors.verticalCenter: parent.verticalCenter
                        }

                        Text
                        {
                            id:theChannelName
                            anchors.verticalCenter: parent.verticalCenter
                            text: channelName
                            font.pixelSize: 15
                            font.bold:  model.channelType===ChannelType.Text && user.currentTextChannelId===model.channelId
                            color: Theme.current.channelTextActive
                            width: implicitWidth>250 ? 250 : implicitWidth
                            elide: Text.ElideRight
                        }

                        Rectangle
                        {
                            id:unreadMessagesCountIndicator
                            color: Theme.current.danger
                            width: 20
                            height: width
                            radius: width
                            visible: model.channelType===ChannelType.Text && model.channelUnreadMessagesCount>0
                            anchors.verticalCenter: parent.verticalCenter
                            Text {
                                text: model.channelUnreadMessagesCount
                                anchors.centerIn: parent
                                color: Theme.current.text
                                font.bold: true
                                font.pixelSize: 12
                            }
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

                        onClicked:
                        {
                            if(model.channelType===ChannelType.Voice)
                            {
                                logger.action("clicked on channel type = voice")
                                if(user.myChannelId===model.channelId)
                                {
                                    logger.info("channelList","we are already in this channel cannot join request wont send.")
                                    if(user.currentTextChannelId>0) //user was seeing  text channel
                                    {
                                        user.currentTextChannelId=0//hide text channel therefore would show current channel pareticipant
                                        logger.info("channelList","hiding current text channel content")
                                    }
                                    else
                                        logger.warning("channelList","currentTextChannelId is 0 or less")
                                    return;
                                }

                                logger.info("channelList", "try to join channel id:"  + channelId + " name="+ channelName)
                                if(model.isLocked)
                                {
                                    //show popup enter password
                                    logger.info("channelList","selected channel is locked, showing enter password popup");
                                    channelPasswordPopup.channelId=model.channelId;
                                    channelPasswordPopup.channelName=model.channelName;
                                    channelPasswordPopup.isTextChannel=false
                                    channelPasswordPopup.open()
                                }
                                else
                                {
                                    logger.info("channelList","hiding currentTextChannel and sending join request for not locked channel");
                                    user.currentTextChannelId=0//hide text channel therefore would show current channel pareticipant
                                    user.joinChannel(channelId,"") //non locked passwords default password is empty ""
                                }
                            }
                            else if(model.channelType===ChannelType.Text)
                            {
                                logger.action("clicked on channel type = text")

                                user.currentTextChannelName=model.channelName;
                                user.currentTextChannelSaveMessages=model.saveChats;

                                //prevent rejoin current channel
                                if(user.currentTextChannelId===model.channelId)
                                {
                                    logger.warning("channelList","user is seeing same text channel so request join wont send")
                                    return;
                                }

                                if(model.isLocked)
                                {
                                    //show popup enter password
                                    logger.info("channelList","selected text channel is locked showing enter password popup")
                                    channelPasswordPopup.channelId=model.channelId;
                                    channelPasswordPopup.channelName=model.channelName;
                                    channelPasswordPopup.isTextChannel=true
                                    channelPasswordPopup.open()
                                    //if channel join accepted and message chunk received would on C++ side do  currentTextChannelId to channel
                                }
                                else
                                {

                                    if(!model.saveChats) //if message are temporary
                                    {
                                        logger.info("channelList","selected text channel is temporary lets see text channel.. id=" + model.channelId)
                                        user.currentTextChannelId=model.channelId; //therefore would hide participants and show text channel
                                    }
                                    //else when channel join accepted and message chunk received would on C++ side do  currentTextChannelId to channel

                                    logger.info("channelList", "send join request to server for text channel (not locked)");
                                    user.joinChannel(model.channelId,"",true)
                                }
                            }
                        }
                    }


                    property bool dragHover: false


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

                            logger.info("channelList","Move user with id "+  drop.source.draggedUserId + " to channel id " + channelId)

                            if(user.isChannelLocked(channelId))
                            {
                                logger.info("channelList", "selected channel (to move user) is locked, showing enter password popup");
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
                            cursorShape: Qt.PointingHandCursor
                            onClicked:
                            {
                                logger.action("clicked on channel settings (would open modifyChannelPopup)")
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
                        height: 43
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

                            border.color: Theme.current.border
                            border.width: 1
                            color: userMouseArea.containsMouse
                                   ? Theme.current.hover
                                   : Theme.current.surface2
                            RowLayout
                            {
                                anchors.fill: parent
                                anchors.leftMargin: 17
                                anchors.rightMargin: 10
                                spacing: 10

                                Rectangle
                                {
                                    id: userAvatar

                                    Layout.preferredWidth: 27
                                    Layout.preferredHeight: 27

                                    radius: width / 2
                                    color: noAvatarLetter.visible ? Theme.current.surface3 : "transparent"

                                    Text
                                    {
                                        id:noAvatarLetter
                                        anchors.centerIn: parent
                                        visible: modelData.avatarPath === ""
                                        text:  modelData.username.charAt(0).toUpperCase()
                                        color: Theme.current.text
                                        font.pixelSize: 16
                                    }

                                    Image
                                    {
                                        id: avatar
                                        anchors.fill: parent
                                        source: modelData.relationship===Relationship.Blocked ? "" //dont show blocked user's avatar
                                                        : modelData.avatarPath
                                        fillMode: Image.PreserveAspectFit
                                        mipmap: true
                                    }

                                    Rectangle
                                    {
                                        id:isTalkingIndicator
                                        anchors.centerIn: parent

                                        width: parent.width + 4
                                        height: width

                                        radius: width / 2

                                        color: "transparent"

                                        border.width:  modelData.isTalking ? 3 : 1

                                        border.color: modelData.isTalking ? UiHelpers.isTalkingColor(modelData.isLocalMuted,modelData.relationship)
                                                      : Theme.current.border

                                        z: -1
                                    }
                                }
                                Text
                                {
                                    id: userName
                                    text: modelData.username
                                    font.pixelSize: 13
                                    color: UiHelpers.relationColor(modelData.relationship)
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
                                acceptedButtons: Qt.LeftButton | Qt.RightButton

                                onClicked: function(mouse)
                                {
                                    if (mouse.button === Qt.RightButton)
                                    {
                                        logger.action("right clicked on user on channelList")
                                        var selectedUser = user.clientUser(modelData.userid)
                                        if (selectedUser.self)
                                        {
                                            logger.action("its self, action abort.")
                                            return
                                        }

                                        userContextPopup.userId = modelData.userid
                                        userContextPopup.username = modelData.username
                                        userContextPopup.clientUser = selectedUser

                                        var p = userMouseArea.mapToItem(userContextPopup.parent, mouse.x, mouse.y)

                                        var margin = 8

                                        // Default: open below cursor
                                        var x = p.x
                                        var y = p.y

                                        // Not enough room below? Open above.
                                        if (y + userContextPopup.height > userContextPopup.parent.height - margin)
                                            y = p.y - userContextPopup.height

                                        // Not enough room on the right? Shift left.
                                        if (x + userContextPopup.width > userContextPopup.parent.width - margin)
                                            x = userContextPopup.parent.width - userContextPopup.width - margin

                                        // Prevent negative coordinates.
                                        x = Math.max(margin, x)
                                        y = Math.max(margin, y)

                                        userContextPopup.x = x
                                        userContextPopup.y = y

                                        userContextPopup.open()
                                    }

                                    if (mouse.button === Qt.LeftButton)
                                    {
                                        logger.action("open user's profile by channelList")
                                        profilePopup.clientUser = user.clientUser(modelData.userid)
                                        profilePopup.open()
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
            color: Theme.current.surface3
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
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            logger.action("clicked on signal base (open/close it)")
                            parent.infoVisible = !parent.infoVisible
                        }
                    }

                    Rectangle
                    {
                        visible: parent.infoVisible

                        width: 270
                        height: 200

                        radius: 8

                        color: Theme.current.surface3
                        border.color: Theme.current.border

                        anchors.bottom: parent.top
                        anchors.bottomMargin: 16

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

                            color: mAreaCloseConnectionInfo.containsMouse
                                   ? Theme.current.danger
                                   : Theme.current.hover

                            Text
                            {
                                anchors.centerIn: parent

                                text: "✕"

                                color: mAreaCloseConnectionInfo.containsMouse
                                       ? Theme.current.text
                                       : Theme.current.textSecondary

                                font.pixelSize: 15
                            }

                            MouseArea
                            {
                                id: mAreaCloseConnectionInfo
                                anchors.fill: parent

                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor

                                onClicked:
                                {
                                    logger.action("clicked on close button connectio info")
                                    signalBase.infoVisible=false
                                }
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
                                color: Theme.current.text
                                font.bold: true
                            }

                            Text
                            {
                                text: "Ping: " + user.myPing + " ms"
                                color: Theme.current.textSecondary
                            }



                            GridLayout
                            {
                                   width: parent.width
                                   columns: 3

                                   columnSpacing: 15
                                   rowSpacing: 4

                                   clip: true

                                   Rectangle
                                   {
                                       Layout.columnSpan: 3
                                       Layout.fillWidth: true
                                       height: 1
                                       color: Theme.current.border
                                   }

                                   // Column widths
                                   Text
                                   {
                                       text: ""
                                       Layout.preferredWidth: 80
                                   }

                                   Text
                                    {
                                        text: "Voice"
                                        color: Theme.current.textSecondary
                                        font.bold: true
                                        Layout.preferredWidth: 70
                                        elide: Text.ElideRight
                                    }

                                    Text
                                    {
                                        text: "Video"
                                        color: Theme.current.textSecondary
                                        font.bold: true
                                        Layout.preferredWidth: 70
                                        elide: Text.ElideRight
                                    }
                                    Text
                                    {
                                        text: "Packet Loss:"
                                        color: Theme.current.textSecondary
                                        elide: Text.ElideRight
                                    }

                                    Text
                                    {
                                        text: Number(user.myVoicePacketLoss).toFixed(2) + "%"
                                        color: Theme.current.textSecondary
                                        Layout.preferredWidth: 70
                                        elide: Text.ElideRight
                                    }
                                    Text
                                    {
                                        text: Number(user.myVideoPacketLoss).toFixed(2) + "%"
                                        color: Theme.current.textSecondary
                                        Layout.preferredWidth: 70
                                        elide: Text.ElideRight
                                    }


                                    Rectangle
                                    {
                                        Layout.columnSpan: 3
                                        Layout.fillWidth: true
                                        height: 1
                                        color: Theme.current.border
                                    }

                                    Text
                                    {
                                        text: ""
                                        Layout.preferredWidth: 80
                                    }

                                    Text
                                     {
                                         text: "Sent"
                                         color: Theme.current.textSecondary
                                         font.bold: true
                                         Layout.preferredWidth: 70
                                         elide: Text.ElideRight
                                     }

                                     Text
                                     {
                                         text: "Received"
                                         color: Theme.current.textSecondary
                                         font.bold: true
                                         Layout.preferredWidth: 70
                                         elide: Text.ElideRight
                                     }
                                    Text
                                    {
                                        text: "Packets:"
                                        color: Theme.current.textSecondary
                                        elide: Text.ElideRight
                                    }

                                    Text
                                    {
                                        text: user.totalPacketsSent
                                        color: Theme.current.textSecondary
                                        Layout.preferredWidth: 70
                                        elide: Text.ElideRight
                                    }

                                    Text
                                    {
                                        text: user.totalPacketsReceived
                                        color: Theme.current.textSecondary
                                        Layout.preferredWidth: 70
                                        elide: Text.ElideRight
                                    }


                                    Text
                                    {
                                        text: "Data:"
                                        color: Theme.current.textSecondary
                                        elide: Text.ElideRight
                                    }

                                    Text
                                    {
                                        text: user.totalBytesSent
                                        color: Theme.current.textSecondary
                                        Layout.preferredWidth: 70
                                        elide: Text.ElideRight
                                    }

                                    Text
                                    {
                                        text: user.totalBytesReceived
                                        color: Theme.current.textSecondary
                                        Layout.preferredWidth: 70
                                        elide: Text.ElideRight
                                    }

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
                        color: Theme.current.text
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
                            Image
                            {
                                width: 15
                                height: 10
                                source: "icons/curved-arrow-down.png"
                                anchors.verticalCenter: parent.verticalCenter
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
                                color: Theme.current.textSecondary
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
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            logger.action("clicked on open/close camera")
                            user.isCameraOpen = !user.isCameraOpen
                        }
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
                        cursorShape: Qt.PointingHandCursor
                        onClicked: logger.action("clicked on sharescreen")
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
                        source: "icons/leave-channel.png"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            logger.action("clicked on leave channel")
                            user.joinChannel(0,"") //go to default channel (channel less id is 0)
                        }
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
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            logger.action("clicked on disconnect")
                            user.disconnect();
                        }
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
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            logger.action("clicked on open website of server")
                            Qt.openUrlExternally(user.serverWebsite)
                        }
                    }
                }

            }
        }

        Rectangle
        {
            id: userStuff
            color: Theme.current.surface3
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

                    color: Theme.current.surface3
                    Text
                    {
                        anchors.centerIn: parent
                        visible: user.myAvatarPath === ""
                        text:  user.myUsername.charAt(0).toUpperCase()
                        color: Theme.current.text
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
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            logger.action("clicked on myAvatar (would open statusPopup)")
                            var p = userStatus.mapToItem(statusPopup.parent, 0, 0)

                            statusPopup.x = p.x + userStatus.width //- statusPopup.width + 100
                            statusPopup.y = p.y - statusPopup.height - 8

                            statusPopup.open()
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
                        border.color: Theme.current.surface3

                        color: UiHelpers.statusColor(user.myStatus)
                        MouseArea
                        {
                            id:userStatusMouseArea
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked:
                            {
                                logger.action("clicked on statusIndicator (would open statusPopup)")
                                var p = userStatus.mapToItem(statusPopup.parent, 0, 0)

                                statusPopup.x = p.x + userStatus.width //- statusPopup.width + 100
                                statusPopup.y = p.y - statusPopup.height - 8

                                statusPopup.open()
                            }
                        }
                    }

                }

                Text
                {
                    id: usernameOnUserStuff
                    text: user.myUsername
                    color: Theme.current.text
                    anchors.verticalCenter: parent.verticalCenter
                    MouseArea
                    {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            logger.action("clicked on username (would open modifyProfilePopup)")
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
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            logger.action("clicked on mute/unmute microphone")
                            user.muteMicrophone = !user.muteMicrophone
                        }
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
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            logger.action("clicked on deaf/undeaf headphone")
                            user.muteHeadphone = !user.muteHeadphone
                        }
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
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            logger.action("clicked on user settings")
                            changePage("settings")
                        }
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
        color: Theme.current.surface3

        MouseArea
        {
            anchors.fill: parent
            cursorShape: Qt.SizeHorCursor
            property real startX: 0
            property real startWidth: 0

            onPressed: function(mouse)
            {
                logger.action("pressed on handle of channelList")
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
                    logger.action("chaning position of channelList by handle")
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
        color: Theme.current.surface3
        opacity: 0.3
        MouseArea
        {
            anchors.fill: parent
            onClicked:
            {
                logger.action("closing servermenu by bgServerMenuClickToClose")
                serverTitle.opened = false
            }
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
            logger.action("clicked on create channel by serverMenu")
            serverTitle.opened = false
            createChannelPopup.open()
        }

        onSettingsClicked:
        {
            logger.action("clicked on settings by serverMenu")
            serverTitle.opened = false
            console.log("Settings")
        }
    }




    StatusPopup
    {
        id: statusPopup

        onStatusSelected:
        {
            logger.info("channelList", "activity status selected="+status)
            user.updateMyActivityStatus(status)
        }
    }



}

