import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import BeanChatClient 1.0

import "../constants/"

Popup
{
    id: root

    property ClientUser clientUser

    //when user is disocnnected and no longer access to clinetUser.data... to prevent empty profiep page
    property string cachedName: ""
    property string cachedAvatar: ""
    property int cachedId: 0
    property string cachedIdentity: ""

    onClientUserChanged:
    {
        if (!clientUser)
            return;

        cachedName = clientUser.username
        cachedAvatar = clientUser.avatarPath
        cachedId = clientUser.id
        cachedIdentity= clientUser.identity
    }

    onClosed:
    {
        cachedAvatar=""
        cachedName=""
        cachedId=-1
        cachedIdentity=""
    }

    width: 380
    height: 600

    modal: true
    focus: true
    padding: 0

    anchors.centerIn: Overlay.overlay
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside


    background: Rectangle
    {
        radius: 14
        color: "#2B2D31"
    }

    Rectangle
    {
        id: banner

        anchors.top: parent.top
        width: parent.width
        height: 80

        radius: 14
        color: "#5865F2"

        Rectangle
        {
            anchors.bottom: parent.bottom
            width: parent.width
            height: radius
            color: "#5865F2"
        }
    }

    Rectangle
    {
        id: avatarFrame
        width: 96
        height: 96
        radius: width/2

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: banner.bottom
        anchors.topMargin: -48

        color: "#5865F2"

        border.width: 4
        border.color: "#2B2D31"

        clip: true

        Image
        {
            id: avatarImage

            anchors.fill: parent
            visible: source!==""

            source: clientUser
                    ? clientUser.avatarPath
                    : cachedAvatar

            fillMode: Image.PreserveAspectCrop
        }

        Text
        {
            anchors.centerIn: parent

            visible: !avatarImage.visible

            text: clientUser
                    ? clientUser.username.charAt(0).toUpperCase()
                    : cachedName.charAt(0).toUpperCase()

            color: "white"
            font.pixelSize: 34
            font.bold: true
        }

        Rectangle
        {
            width: 18
            height: 18
            radius: 9

            anchors.right: parent.right
            anchors.bottom: parent.bottom

            border.width: 3
            border.color: "#2B2D31"

            color: UiHelpers.statusColor(
                       clientUser
                       ? clientUser.status
                       : ClientUser.Offline)
        }
    }

    ScrollView
    {
        anchors
        {
            top: avatarFrame.bottom
            topMargin: 18

            left: parent.left
            right: parent.right
            bottom: parent.bottom

            margins: 18
        }
        clip:true
        ScrollBar.vertical.policy: ScrollBar.AsNeeded
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        Column
        {
            width: parent.width
            spacing: 14

            Text
            {
                id:userName
                width: parent.width

                horizontalAlignment: Text.AlignHCenter

                text: clientUser
                      ? clientUser.username
                      : cachedId == -1
                        ? "Offline User"
                        : cachedName

                color:  clientUser ? UiHelpers.relationColor(clientUser.relationship) : "white"

                font.pixelSize: 24
                font.bold: true
            }

            Text
            {
                id:userDescription
                width: parent.width
                visible: clientUser ? true : false
                horizontalAlignment: Text.AlignHCenter

                text: clientUser.description.length>0? clientUser.description : "Default description"

                color: "#B5BAC1"

                font.pixelSize: 13
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            }

            Label
            {
                text: "NOTE"
                color: "#B5BAC1"
                visible: clientUser ? (clientUser.self?false:true): false //show when user is available or not self
                font.bold: true
                width: parent.width
            }

            TextField
            {
                id: userNoteField
                placeholderText: "enter note for this user"
                placeholderTextColor: "white"
                width: parent.width
                visible: clientUser ? (clientUser.self?false:true): false
                text: clientUser ? (clientUser.note.length>0? clientUser.note : "") : ""
                color: "white"
                function updateNote()
                {
                    if(clientUser)
                    {
                        relationshipManager.setNote(clientUser.identity, userNoteField.text)
                        clientUser.note=userNoteField.text
                    }
                    else
                        console.log("cannot set note for null user")
                }

                onAccepted: userNoteField.updateNote()
                onTextChanged: userNoteField.updateNote()

                background: Rectangle
                {
                    radius: 6
                    color: "#1E1F22"
                }
            }

            Rectangle
            {
                width: parent.width
                height: 1
                color: "#404249"
            }

            Label
            {
                text: "INFO"
                color: "#B5BAC1"
                font.bold: true
                width: parent.width
            }

            Rectangle
            {
                width: parent.width
                radius: 8
                color: "#1E1F22"

                implicitHeight: infoColumn.implicitHeight + 20

                Column
                {
                    id: infoColumn

                    anchors.fill: parent
                    anchors.margins: 10

                    spacing: 8

                    Text
                    {
                        text: "ID (" + (clientUser ? clientUser.id : cachedId) +")"
                        color: "#949BA4"
                        font.pixelSize: 11
                        font.bold: true
                    }

                    Rectangle
                    {
                        id:userIdentityBase
                        width: parent.width
                        height: 38
                        radius: 10
                        color: "#2B2D31"
                        clip: true
                        Flickable
                        {
                            anchors
                            {
                                left: parent.left
                                right: parent.right
                                verticalCenter: parent.verticalCenter
                                leftMargin: 5
                            }

                            height: 24

                            contentWidth: identityText.contentWidth
                            contentHeight: identityText.contentHeight

                            clip: true
                            flickableDirection: Flickable.HorizontalFlick

                            TextEdit
                            {
                                id: identityText

                                text: clientUser ? clientUser.identity : cachedIdentity

                                readOnly: true

                                color: "white"
                                font.pixelSize: 10

                                wrapMode: TextEdit.NoWrap

                                selectByMouse: true
                            }
                        }
                    }


                   Item
                   {
                       id:userStatusAndRelationBase
                       width: parent.width
                       height: 60
                       visible: clientUser ? true : false
                       Column
                       {
                           id:userStatusBase
                           Text
                           {
                               text: "STATUS"
                               color: "#949BA4"
                               font.pixelSize: 11
                               font.bold: true
                           }

                           Text
                           {
                               text: UiHelpers.statusText(
                                         clientUser
                                         ? clientUser.status
                                         : ClientUser.Offline)

                               color: UiHelpers.statusColor(
                                          clientUser
                                          ? clientUser.status
                                          : ClientUser.Offline)

                               font.pixelSize: 15
                               font.bold: true
                           }
                       }

                       Rectangle
                       {
                           id:baseFriendOrBlock
                           anchors.right: parent.right
                           anchors.rightMargin: 10
                           anchors.verticalCenter: parent.verticalCenter
                           visible: clientUser ? (clientUser.self? false : true) : false//only show when user is avaiable and not self
                           color: "transparent"
                           width: 60
                           height: 50
                           Image
                           {
                               id:friendUserButton
                               anchors.right: parent.right
                               width: visible ? 25 : 0
                               height: width
                               visible: clientUser ?
                                            (clientUser.relationship===Relationship.None
                                            ||clientUser.relationship===Relationship.Friend) : false

                               source: clientUser ?
                                           clientUser.relationship===Relationship.Friend ?
                                               "../icons/user-is-friend.png" : "../icons/user-add-friend.png" : ""
                               MouseArea
                               {
                                   anchors.fill: parent
                                   cursorShape: Qt.PointingHandCursor
                                   onClicked:
                                   {
                                       if(clientUser)
                                       {
                                           if(clientUser.relationship===Relationship.None)
                                           {
                                               relationshipManager.addFriend(clientUser.identity)
                                               clientUser.relationship=Relationship.Friend
                                           }
                                           else if(clientUser.relationship===Relationship.Friend)
                                           {
                                               relationshipManager.removeFriend(clientUser.identity)
                                               clientUser.relationship=Relationship.None
                                           }
                                       }
                                       else
                                           console.log("user is null, cant add/remove friend him.")
                                   }
                               }
                           }
                           Image
                           {
                               id:blockUserButton
                               anchors.right: friendUserButton.visible
                                                  ? friendUserButton.left
                                                  : parent.right
                               anchors.rightMargin: friendUserButton.visible ? 10 : 0
                                   width: visible ? 25 : 0
                                   height: width

                               visible: clientUser ?
                                            (clientUser.relationship===Relationship.None
                                            ||clientUser.relationship===Relationship.Blocked) : false

                               source: clientUser ?
                                           (clientUser.relationship==Relationship.Blocked
                                            ? "../icons/user-is-blocked" : "../icons/user-add-block.png") : ""
                               MouseArea
                               {
                                   anchors.fill: parent
                                   cursorShape: Qt.PointingHandCursor
                                   onClicked:
                                   {
                                       if(clientUser)
                                       {
                                           if(clientUser.relationship===Relationship.None)
                                           {
                                               relationshipManager.blockUser(clientUser.identity)
                                               clientUser.relationship=Relationship.Blocked
                                               relationshipManager.setMuted(clientUser.identity,true)
                                               clientUser.localMuted=true
                                           }
                                           else if(clientUser.relationship===Relationship.Blocked)
                                           {
                                               relationshipManager.unblockUser(clientUser.identity)
                                               clientUser.relationship=Relationship.None
                                               relationshipManager.setMuted(clientUser.identity,false)
                                               clientUser.localMuted=false
                                           }
                                       }
                                       else
                                           console.log("user is null, cant block/unblock him.")
                                   }
                               }
                           }

                       }


                   }

                    Text
                    {
                        text: "CURRENT CHANNEL"
                        color: "#949BA4"
                        visible: clientUser ? true : false
                        font.pixelSize: 11
                        font.bold: true
                    }
                    Rectangle
                    {
                        id:currentChannelBox
                        width: parent.width
                        height: 38
                        radius: 10
                        color: "#2B2D31"
                        visible: clientUser ? true : false
                        Text
                        {
                            id:channelName
                            text: clientUser
                                  ? (clientUser.channelId===0? "None" : (user.getChannelName(clientUser.channelId)))
                                  : "Offline"
                            anchors
                            {
                                left:parent.left
                                leftMargin:10
                                verticalCenter: parent.verticalCenter
                            }

                            color: "white"
                            font.pixelSize: 15
                            width: parent.width-leftMargin
                            elide: Text.ElideRight
                        }
                        Rectangle
                        {
                            id:buttonJoinHisChannel
                            width: (parent.width-10)/3
                            height: 38
                            visible: clientUser ?
                                         clientUser.channelId>0 //don't show when user isn't in any channel.
                                         && clientUser.self!==true //don't show join when profile is self.
                                         && clientUser.channelId!==user.myChannelId //don't show when we're on the same channel
                                         : false
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            radius: 8

                            color: "#5865F2"

                            Text
                            {
                                anchors.centerIn: parent
                                text: "Join"
                                color: "white"
                                font.bold: true
                            }

                            MouseArea
                            {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor

                                onClicked:
                                {
                                    if(clientUser)
                                    {
                                        var channelId = clientUser.channelId;
                                        console.log("try to join channel id:" , channelId, " via join button from profile.")
                                        if(user.isChannelLocked(channelId))
                                        {
                                            //show popup enter password
                                            channelPasswordPopup.channelId=channelId;
                                            channelPasswordPopup.channelName=channelName.text;
                                            channelPasswordPopup.open()
                                        }
                                        else
                                            user.joinChannel(channelId,"") //non locked passwords default password is empty ""

                                        //close this popup
                                        close()
                                    }

                                }
                            }
                        }

                    }



                    Text
                    {
                        text: "APP VERSION"
                        color: "#949BA4"
                        visible: clientUser ? true : false
                        font.pixelSize: 11
                        font.bold: true
                    }
                    Rectangle
                    {
                        id:appVersionBox
                        width: parent.width
                        height: 38
                        radius: 10
                        color: "#2B2D31"
                        visible: clientUser ? true : false
                        Text
                        {
                            text: clientUser
                                  ? (clientUser.appVersion + " on " + clientUser.osName)
                                  : "version"
                            anchors
                            {
                                left:parent.left
                                leftMargin:10
                                verticalCenter: parent.verticalCenter
                            }

                            color: "white"
                            font.pixelSize: 15
                            width: parent.width-leftMargin
                            elide: Text.ElideRight
                        }
                    }

                }
            }

            Row
            {
                id:messageOrCallBox
                width: parent.width
                spacing: 10
                visible: clientUser ? (clientUser.self?false:true) : false //show only when user is available or not self

                Rectangle
                {
                    width: (parent.width-10)/2
                    height: 38

                    radius: 8

                    color: "#5865F2"

                    Text
                    {
                        anchors.centerIn: parent
                        text: "Send Message"
                        color: "white"
                        font.bold: true
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked:
                        {
                            // TODO
                        }
                    }
                }

                Rectangle
                {
                    width: (parent.width-10)/2
                    height: 38

                    radius: 8

                    color: "#3A3C42"

                    Text
                    {
                        anchors.centerIn: parent
                        text: "Call"
                        color: "white"
                        font.bold: true
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        onClicked:
                        {
                            // TODO
                        }
                    }
                }
            }

        }

    }
}
