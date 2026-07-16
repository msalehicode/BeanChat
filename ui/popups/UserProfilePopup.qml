import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import BeanChatClient 1.0

import "../constants/"

Popup
{
    id: root

    property ClientUser clientUser

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

        color: avatarBg

        border.width: 4
        border.color: "#2B2D31"

        clip: true

        Image
        {
            id: avatarImage

            anchors.fill: parent
            visible: source!==""

            source: clientUser.avatarPath

            fillMode: Image.PreserveAspectCrop
        }

        Text
        {
            anchors.centerIn: parent

            visible: clientUser.avatarPath.length<=1

            text:  clientUser.username.charAt(0).toUpperCase()

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

            color: UiHelpers.statusColor(clientUser.status)
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

                text:clientUser.username

                color: UiHelpers.relationColor(clientUser.relationship)

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
                font.bold: true
                width: parent.width
                visible: !clientUser.self //dont show for self
            }

            TextField
            {
                id: userNoteField
                placeholderText: "enter note for this user"
                placeholderTextColor: "white"
                width: parent.width
                text: clientUser.note.length>0? clientUser.note : ""
                color: "white"
                visible: !clientUser.self //dont show for self
                function updateNote()
                {
                        relationshipManager.setNote(clientUser.identity, userNoteField.text)
                        clientUser.note=userNoteField.text
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
                        text: "ID (" + clientUser.id +")"
                        color: "#949BA4"
                        font.pixelSize: 11
                        font.bold: true
                    }

                    Rectangle
                    {
                        id:userIdentityBase
                        width: parent.width
                        height: 38
                        radius: 8
                        color: "#2B2D31"
                        border.color: "#1e1f22"
                        clip:true
                        Text
                        {
                            id:identityText
                            text: clientUser.identity
                            color: "white"
                            width: parent.width-65
                            font.family: "Consolas"    // or "monospace"
                            font.pixelSize: 12
                            font.bold: true

                            elide: Text.ElideMiddle
                            anchors
                            {
                                verticalCenter:parent.verticalCenter
                                left:parent.left
                                leftMargin:5
                            }
                        }

                        Rectangle
                        {
                            id: copyButton
                            width: 55
                            height: parent.height-6
                            radius: 6
                            color: copyLabel.text === "Copied!" ? "#3BA55D" : "#5865F2"
                            anchors
                            {
                                right:parent.right
                                rightMargin:5
                                verticalCenter:parent.verticalCenter
                            }

                            Behavior on color
                            {
                                ColorAnimation
                                {
                                    duration: 180
                                }
                            }

                            scale: copyMouseArea.pressed ? 0.96 : 1.0

                            Behavior on scale
                            {
                                NumberAnimation
                                {
                                    duration: 100
                                }
                            }

                            Text
                            {
                                id: copyLabel
                                anchors.centerIn: parent
                                text: "Copy"
                                color: "white"
                                font.bold: true

                                Behavior on opacity
                                {
                                    NumberAnimation
                                    {
                                        duration: 120
                                    }
                                }
                            }

                            MouseArea
                            {
                                id: copyMouseArea

                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor

                                hoverEnabled: true

                                onClicked:
                                {
                                    clipboardHelper.copy(identityText.text)

                                    copyLabel.text = "Copied!"
                                    copiedTimer.restart()
                                }
                            }
                            Timer
                            {
                                id: copiedTimer
                                interval: 1500
                                repeat: false

                                onTriggered: copyLabel.text = "Copy"
                            }
                        }


                    }




                   Item
                   {
                       id:userStatusAndRelationBase
                       width: parent.width
                       height: 60
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
                               text: UiHelpers.statusText(clientUser.status)
                               color: UiHelpers.statusColor(clientUser.status)
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
                           visible:clientUser.self? false : true//only show when user isn't self
                           color: "transparent"
                           width: 60
                           height: 50
                           Image
                           {
                               id:friendUserButton
                               anchors.right: parent.right
                               width: visible ? 25 : 0
                               height: width
                               visible: clientUser.relationship===Relationship.None ||
                                             clientUser.relationship===Relationship.Friend

                               source: clientUser.relationship===Relationship.Friend ?
                                                "../icons/user-is-friend.png" : "../icons/user-add-friend.png"
                               MouseArea
                               {
                                   anchors.fill: parent
                                   cursorShape: Qt.PointingHandCursor
                                   onClicked:
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

                               visible: clientUser.relationship===Relationship.None
                                             || clientUser.relationship===Relationship.Blocked

                               source: clientUser.relationship==Relationship.Blocked ?
                                           "../icons/user-is-blocked" : "../icons/user-add-block.png"
                               MouseArea
                               {
                                   anchors.fill: parent
                                   cursorShape: Qt.PointingHandCursor
                                   onClicked:
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
                               }
                           }

                       }


                   }

                    Text
                    {
                        text: "CURRENT CHANNEL"
                        color: "#949BA4"
                        visible: clientUser.status!==Presence.Offline
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
                        visible: clientUser.status!==Presence.Offline
                        Text
                        {
                            id:channelName
                            text: clientUser.channelId===0? "None" : (user.getChannelName(clientUser.channelId))
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
                            visible: clientUser.channelId>0 //don't show when user isn't in any channel.
                                         && clientUser.self!==true //don't show join when profile is self.
                                         && clientUser.channelId!==user.myChannelId //don't show when we're on the same channel

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
                        visible: clientUser.status!==Presence.Offline
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
                        visible: clientUser.status!==Presence.Offline
                        Text
                        {
                            text: clientUser.appVersion + " on " + clientUser.osName
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
                visible: clientUser.self ? false: clientUser.status!==Presence.Offline //show only when user isn't invisible/offline or self
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
