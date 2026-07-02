import QtQuick
import QtQuick.Controls

import QtQuick.Controls.Material

import QtQuick.Layouts

Item {
    anchors.fill: parent
    property int widthBase: 250 //uses for userlist and chanenlist


    property color bg1 : "#414247"
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
            id:importantNotifierBar
            width: parent.width
            visible: false
            height:25
            Rectangle
            {
                anchors.fill: parent
                color: "red"
                Text
                {
                    // text:"You are offline"
                    text: "Audio Input not found."
                    anchors.centerIn: parent
                    color:"white"
                    font.bold: true
                }
                Rectangle
                {
                    id:closeButtonNotifierBar
                    width: 20
                    height: 20
                    color: "black"
                    anchors
                    {
                        right:parent.right
                        rightMargin:20
                        verticalCenter:parent.verticalCenter
                    }

                    Text {
                        text: "X"
                        anchors.centerIn: parent
                        color:"white"
                    }
                    MouseArea
                    {
                        anchors.fill: parent
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
                visible: user.isConnectedToServer
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
                            id:indicatorStackView
                            Layout.fillWidth: true
                            visible: user.myChannelName.length>0
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
                                height: 40
                                color: rightPanel.currentTab === 1 ? "#2a82da" : "#222"

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

        deleteButton.onClicked:
        {
            user.deleteSavedServer(id,dbIndex)
            close()
        }
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

    Connections
    {
        target:user
        onIsConnectedToServerChanged:
        {
            //when disconnected/connected, reset right panel index to connected users.
            rightPanel.currentTab=0
        }
        onMyChannelNameChanged:
        {
            if(user.myChannelName==="") //channel not exists or deleted.
                rightPanel.currentTab=0
        }
    }
}
