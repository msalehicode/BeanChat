import QtQuick
import QtQuick.Controls

import QtQuick.Controls.Material

import QtQuick.Layouts

import "popups/"
import BeanChatClient 1.0
import "constants/"

Item {
    anchors.fill: parent
    readonly property int widthBase: 250 //uses for userlist and chanenlist

    readonly property int iconH: 22
    readonly property int iconW: 22

    Rectangle
    {
        anchors.fill: parent
        color: Theme.current.background
    }



    Column
    {
        anchors.fill: parent
        Item
        {
            id:importantNotifierBar
            property string nColor:Theme.current.danger
            property string ntextColor:Theme.current.text
            property string nText: "Example important notification text"
            property string nActionButtonText: "Button"
            property var nActionButtonTask:null
            width: parent.width
            visible: false
            height:25
            Rectangle
            {
                anchors.fill: parent
                color: importantNotifierBar.nColor
                Text
                {
                    id:importantNotifierBarText
                    text:  importantNotifierBar.nText
                    anchors.centerIn: parent
                    color:importantNotifierBar.ntextColor
                    font.bold: true
                }

                Rectangle
                {
                    id: actionButtonImportantNotificationBar
                    width: 100
                    height: parent.height-6
                    radius: 6
                    color:  Theme.current.inputHover
                    visible: importantNotifierBar.nActionButtonTask!==null
                    anchors
                    {
                        verticalCenter:parent.verticalCenter
                        left:importantNotifierBarText.right
                        leftMargin:15
                    }

                    Text
                    {
                        anchors.centerIn: parent
                        text: importantNotifierBar.nActionButtonText
                        color: Theme.current.text
                        font.bold: true
                    }

                    MouseArea
                    {
                        id: actionImportantMouseArea
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked:
                        {
                            if(importantNotifierBar.nActionButtonTask)
                            {
                                importantNotifierBar.visible=false
                                importantNotifierBar.nActionButtonTask()
                                logger.action("clicked on action Important MouseArea")
                            }
                            else
                                logger.action("clicked on action Important MouseArea but action not defined")
                        }
                    }
                }



                Rectangle
                {
                    id: closeButtonNotifierBar
                    width: 22
                    height: 22
                    radius: 6
                    color:  Theme.current.inputHover
                    anchors
                    {
                        right:parent.right
                        rightMargin:20
                        verticalCenter:parent.verticalCenter
                    }

                    Text
                    {
                        anchors.centerIn: parent
                        text: "X"
                        color: Theme.current.text
                        font.bold: true
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked:
                        {
                            logger.action("clicked on closeButtonNotifierBar")
                            importantNotifierBar.visible=false
                        }
                    }
                }


            }
        }


        Row
        {
            width: parent.width
            height: importantNotifierBar.visible ? parent.height-importantNotifierBar.height : parent.height

            MyServers
            {
                id:leftPanel
                //beanChat button, List of Saved Servers, Add Server Button
            }

            ChannelList
            {
                id:channelList
                //current Connected server name, list of channels and users inside them, user control buttons
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



            //VOICE????:
            CurrentChannelParticipants
            {
                id:centerContentBase
                visible: user.currentTextChannelId===0 //TODO: later make participants if they have camera or .. on mini picture (float) over text channel content
                //when joint a channel would display current channel users. with their status, camera feed, and ...
            }

            //OR
            //TEXT???????
            CurrentTextChannel
            {
                id:textChannelContent
                visible: user.currentTextChannelId>0
            }
            //-----------------------------------


            Rectangle
            {
                id: handleRight
                width: handle.width
                height: parent.height
                x: -width / 2
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
                        startX = mouse.x
                        startWidth = rightPanel.width
                        logger.action("pressed on righthandle")
                    }

                    onPositionChanged: function(mouse)
                    {
                        if (pressed)
                        {
                            let dx = mouse.x - startX
                            let pos = Math.max(50, startWidth - dx);
                            if (pos < 300)
                                pos = 300
                            else if (pos > 800)
                                pos = 800

                            rightPanel.width = pos
                            logger.action("position of the right handle changed")
                        }
                    }
                }
            }

            Item
            {
                id:rightPanel
                width: 300-handleRight.width
                height: parent.height
                clip: true
                property int currentTab: 0

                Rectangle
                {
                    anchors.fill: parent
                    color: Theme.current.background
                    visible: user.isConnectedToServer
                    ConnectedUsers {}

/*
                    ColumnLayout
                    {
                        anchors.fill: parent
                        visible: user.isConnectedToServer
                        RowLayout
                        {
                            id:indicatorStackView
                            Layout.fillWidth: true
                            Layout.preferredHeight: 40
                            visible: user.myChannelName.length>0
                            // spacing: 2

                            Rectangle
                            {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 40

                                color: rightPanel.currentTab === 0 ? Theme.current.accentPressed : Theme.current.surface3

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
                                        color: Theme.current.text
                                        anchors.verticalCenter: parent.verticalCenter
                                    }
                                }
                                MouseArea
                                {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked:
                                    {
                                        rightPanel.currentTab = 0
                                        user.isChatOpen=false
                                        rightPanel.width=300
                                        logger.action("clicked on open connected users tab")
                                    }
                                }
                            }

                            Rectangle
                            {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 40
                                color: rightPanel.currentTab === 1 ? Theme.current.accentPressed : Theme.current.surface3

                                Row
                                {
                                    anchors.centerIn: parent
                                    spacing: 5
                                    Image
                                    {
                                        width: 30
                                        height: 30
                                        source: "icons/chat.png"
                                    }

                                    Text
                                    {
                                        text: "Chat"
                                        color: Theme.current.text
                                        anchors.verticalCenter: parent.verticalCenter
                                    }

                                    // Unread badge
                                    Rectangle
                                    {
                                        visible: !user.isChatOpen && user.chatUnreadMessages>0

                                        width: user.chatUnreadMessages > 99 ? 24 : 18
                                        height: width
                                        radius: width / 2

                                        color: Theme.current.danger
                                        anchors.verticalCenter: parent.verticalCenter


                                        Text
                                        {
                                            anchors.centerIn: parent
                                            text: user.chatUnreadMessages > 99 ? "99+" : user.chatUnreadMessages
                                            color: Theme.current.text
                                            font.pixelSize: 12
                                            font.bold: true
                                        }
                                    }
                                }

                                MouseArea
                                {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked:
                                    {
                                        rightPanel.currentTab = 1
                                        user.chatUnreadMessages=0; //reset unread messags count
                                        user.isChatOpen=true
                                        rightPanel.width=400
                                        logger.action("clicked on open tab for channel chat ")
                                    }
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

*/
                }

            }


        }

    }


    // ================================= MENU =================================
    MyServersItemMenu
    {
        id:myServersItemMenu
        connectButton.onClicked:
        {
            //update server id, to know which server is connecting to (for avatars path)
            logger.action("clicked on connect button by myServersItemMenu")
            user.connectedServerId=dbIndex

            //connect
            user.switchOrConnectToServer(ip,
                                         port,
                                         id)
            close()
        }

        disconnectButton.onClicked:
        {
            logger.action("clicked on disconnect button by myServersItemMenu")
            user.disconnect()
            close()
        }

        modifyButton.onClicked:
        {
            logger.action("clicked on modify button by myServersItemMenu")
            modifySavedServerPopup.dbIndex = dbIndex;
            modifySavedServerPopup.currentIp = ip;
            modifySavedServerPopup.currentPort = port;
            modifySavedServerPopup.currentName = serverName
            modifySavedServerPopup.serverId = id;

            close()
            modifySavedServerPopup.open()
        }

        shareQRcodeButton.onClicked:
        {
            logger.action("clicked on share QRcode button by myServersItemMenu")
            close()
            showQRcodePopup.qrData=ip+":"+port
            showQRcodePopup.titleText="Join Me (" + serverName + ")"
            showQRcodePopup.descriptionText="Scan this QR Code to join " + serverName
            showQRcodePopup.open()
        }


        shareServerCodeButton.onClicked:
        {
            logger.action("clicked on share serverCode button by myServersItemMenu")
            close()
            showToCopyPopup.value=serverCode.encode(ip,Number(port))
            showToCopyPopup.titleText="Share via ServerCode"
            showToCopyPopup.descriptionText="Share this code with others to join you\nwithout entering Ip and Port"
            showToCopyPopup.open()
        }

        deleteButton.onClicked:
        {
            logger.action("clicked on delete button by myServersItemMenu")
            user.deleteSavedServer(id,dbIndex)
            close()
        }
    }


    UserContextPopup
    {
        id:userContextPopup
    }


    // ================================= POP UP =================================
    ModifySavedServerPopup
    {
        id:modifySavedServerPopup
        onSaveClicked:
        {
            logger.action("clicked on save button on modifySavedServerPopup")
            user.updateSavedServer(serverId,
                        dbIndex,
                        serverName,
                        ip,
                        port)
        }
    }


    ConnectPopup
    {
        id: connectPopup
    }

    EnterChannelPasswordPopup
    {
        id: channelPasswordPopup

        onPasswordEntered:
        {
            logger.action("clicked on enter button on channelPasswordPopup")
            user.joinChannel(channelId, password, isTextChannel)
        }
    }

    EnterChannelPasswordPopup
    {
        id: channelPasswordForMoveUserPopup
    }

    ShowImagePopup
    {
        id:showImagePopup
    }

    ShowImageAnimatedPopup
    {
        id:showImageAnimatedPopup
    }

    CreateChannelPopup
    {
        id:createChannelPopup
        onCreateClicked:
        {
            logger.action("clicked on create button on createChannelPopup")
            user.createChannel(channelName,channelPassword,saveChats,isVoiceChannel)
        }
    }

    ModifyChannelPopup
    {
        id:modifyChannelPopup
        onSaveClicked:
        {
            logger.action("clicked on save button on modifyChannelPopup")
            user.updateChannel(targetChannelId, channelName, channelPassword, saveChats)
        }
        onDeleteClicked:
        {
            logger.action("clicked on delete button on modifyChannelPopup")
            user.deleteChannel(targetChannelId)
        }
    }

    ModifyProfilePopup
    {
        id:modifyProfilePopup
    }

    UserProfilePopup
    {
        id: profilePopup
    }

    GetValuePopup
    {
        id:getValuePopup
    }

    ShowQRcode
    {
        id:showQRcodePopup
    }

    ShowToCopyPopup
    {
        id:showToCopyPopup
    }


    Rectangle
    {
        id:connectionWaitOverlay
        anchors.fill: parent
        visible: user.connectionStatus===ConnectionStatus.Connecting
                 || user.connectionStatus===ConnectionStatus.Disconnecting ? true : false
        color:Theme.current.inputHover
        opacity: 0.8
        z:999999
        Text
        {
            color:Theme.current.text
            anchors.centerIn: parent
            font.pixelSize: 30
            text: user.connectionStatus===ConnectionStatus.Connecting ? "Connecting ...."
                                                                      : user.connectionStatus===ConnectionStatus.Disconnecting ? "Disconnecting ..."
                                                                                                                               : "Unkown = " + user.connectionStatus
        }
        MouseArea
        {
            anchors.fill: parent
        }
    }

    Connections
    {
        target: user

        function onIsConnectedToServerChanged()
        {
            //when disconnected/connected, reset right panel index to connected users.
            rightPanel.currentTab = 0
        }

        function onMyChannelNameChanged()
        {
            if(user.myChannelName==="") //channel not exists or deleted.
                rightPanel.currentTab=0
        }

        function onShowImportantNotifierBar(nText,nColor)
        {
            if(nColor===3) //bluc update,
            {
                importantNotifierBar.nColor=Theme.current.accent
                importantNotifierBar.ntextColor=Theme.current.text
                importantNotifierBar.nText=nText
                importantNotifierBar.nActionButtonText="UPDATE NOW"
                importantNotifierBar.nActionButtonTask= function()
                {
                    user.updateApp()
                }
                importantNotifierBar.visible=true
            }
            else
            {
                importantNotifierBar.nColor=Theme.current.danger
                importantNotifierBar.ntextColor=Theme.current.text
                importantNotifierBar.nText=nText
                importantNotifierBar.nActionButtonText=""
                importantNotifierBar.nActionButtonTask=null
                importantNotifierBar.visible=true
            }
        }
    }
}
