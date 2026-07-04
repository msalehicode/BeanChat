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

    onClientUserChanged:
    {
        if (!clientUser)
            return;

        cachedName = clientUser.username
        cachedAvatar = clientUser.avatarPath
        cachedId = clientUser.id
    }

    onClosed:
    {
        cachedAvatar=""
        cachedName=""
        cachedId=-1
    }

    width: 380
    height: 490

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
        height: 110

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
            visible: clientUser ? clientUser.avatarPath.length>0 : false

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

    Column
    {
        anchors
        {
            top: avatarFrame.bottom
            topMargin: 18

            left: parent.left
            right: parent.right

            margins: 18
        }

        spacing: 14

        Text
        {
            width: parent.width

            horizontalAlignment: Text.AlignHCenter

            text: clientUser
                  ? clientUser.username
                  : cachedId == -1
                    ? "Offline User"
                    : cachedName

            color: "white"

            font.pixelSize: 24
            font.bold: true
        }

        Text
        {
            width: parent.width

            horizontalAlignment: Text.AlignHCenter

            text: "ID: " + (clientUser ? clientUser.id : cachedId)

            color: "#B5BAC1"

            font.pixelSize: 13
        }

        Rectangle
        {
            width: parent.width
            height: 1
            color: "#404249"
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
                        width: (parent.width-10)/3
                        height: 38
                        visible: clientUser ? clientUser.channelId>0 && clientUser.self!==true: false //don't show join button for ourself OR channelid=0 means not in anychannel yet.
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



            }
        }

        Row
        {
            width: parent.width
            spacing: 10

            Rectangle
            {
                width: (parent.width-10)/2
                height: 38

                radius: 8

                color: "#5865F2"

                Text
                {
                    anchors.centerIn: parent
                    text: "Message"
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
                    text: "Mention"
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
