import QtQuick
import QtQuick.Controls

import QtQuick.Controls.Material

import QtQuick.Layouts

import "popups/"

Item {
    anchors.fill: parent
    readonly property int widthBase: 250 //uses for userlist and chanenlist


    readonly property color bg1 : "#414247"
    readonly property color bg2: "#121315"
    readonly property color avatarBg: "#040833"
    readonly property int iconH: 22
    readonly property int iconW: 22

    Rectangle
    {
        anchors.fill: parent
        color: "#05070b"
    }



    Column
    {
        anchors.fill: parent
        Item
        {
            id:importantNotifierBar
            property string nColor:"red"
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
                    color:"white"
                    font.bold: true
                }

                Rectangle
                {
                    id: actionButtonImportantNotificationBar
                    width: 100
                    height: parent.height-6
                    radius: 6
                    color:  "black"
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
                        color: "white"
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
                            }
                            else
                                console.log("action not defined.")
                        }
                    }
                }



                Rectangle
                {
                    id: closeButtonNotifierBar
                    width: 22
                    height: 22
                    radius: 6
                    color:  "black"
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
                        color: "white"
                        font.bold: true
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        hoverEnabled: true
                        onClicked: importantNotifierBar.visible=false
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

            CurrentChannelParticipants
            {
                id:centerContentBase
                //when joint a channel would display current channel users. with their status, camera feed, and ...
            }

            Item
            {
                id:rightPanel
                width: 250
                height: parent.height
                clip: true
                property int currentTab: 0

                Rectangle
                {
                    anchors.fill: parent
                    color: "#141C2B"
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

                                color: rightPanel.currentTab === 0 ? "#839ac7" : "#05070b"

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
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked:
                                    {
                                        rightPanel.currentTab = 0
                                        user.isChatOpen=false
                                        rightPanel.width=300
                                    }
                                }
                            }

                            Rectangle
                            {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 40
                                color: rightPanel.currentTab === 1 ? "#839ac7" : "#05070b"

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
                                        color: "white"
                                        anchors.verticalCenter: parent.verticalCenter
                                    }

                                    // Unread badge
                                    Rectangle
                                    {
                                        visible: !user.isChatOpen && user.chatUnreadMessages>0

                                        width: user.chatUnreadMessages > 99 ? 24 : 18
                                        height: width
                                        radius: width / 2

                                        color: "red"
                                        anchors.verticalCenter: parent.verticalCenter


                                        Text
                                        {
                                            anchors.centerIn: parent
                                            text: user.chatUnreadMessages > 99 ? "99+" : user.chatUnreadMessages
                                            color: "white"
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


                    Rectangle
                    {
                        id: handleRight
                        width: 15
                        height: parent.height
                        visible: user.isConnectedToServer
                        x: -width / 2
                        y: 0
                        color: "#05070b"

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
                                    let pos = Math.max(50, startWidth - dx);
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


    // ================================= MENU =================================
    MyServersItemMenu
    {
        id:myServersItemMenu
        connectButton.onClicked:
        {
            //update server id, to know which server is connecting to (for avatars path)
            user.connectedServerId=dbIndex

            //connect
            user.switchOrConnectToServer(ip,
                                         port,
                                         id)
            close()
        }

        disconnectButton.onClicked:
        {
            user.disconnect()
            close()
        }

        modifyButton.onClicked:
        {
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
            close()
            showQRcodePopup.qrData=ip+":"+port
            showQRcodePopup.titleText="Join Me (" + serverName + ")"
            showQRcodePopup.descriptionText="Scan this QR Code to join " + serverName
            showQRcodePopup.open()
        }


        shareServerCodeButton.onClicked:
        {
            close()
            showToCopyPopup.value=serverCode.encode(ip,Number(port))
            showToCopyPopup.titleText="Share via ServerCode"
            showToCopyPopup.descriptionText="Share this code with others to join you\nwithout entering Ip and Port"
            showToCopyPopup.open()
        }

        deleteButton.onClicked:
        {
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
            user.joinChannel(channelId, password)
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

    CreateChannelPopup
    {
        id:createChannelPopup
        onCreateClicked: user.createChannel(channelName,channelPassword,saveChats)
    }

    ModifyChannelPopup
    {
        id:modifyChannelPopup
        onSaveClicked: user.updateChannel(targetChannelId, channelName, channelPassword, saveChats)
        onDeleteClicked: user.deleteChannel(targetChannelId)
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
                importantNotifierBar.nColor="#5865F2"
                importantNotifierBar.nText=nText
                importantNotifierBar.nActionButtonText="UPDATE NOW"
                importantNotifierBar.nActionButtonTask= function()
                {
                    user.updateApp()
                }
                importantNotifierBar.visible=true
            }
        }
    }
}
