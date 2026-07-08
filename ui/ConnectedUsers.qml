import QtQuick
import QtQuick.Controls.Material

import QtQuick.Layouts
import BeanChatClient 1.0

import "constants/"
Item
{
    Rectangle
    {
        color: "transparent"
        anchors.fill: parent
    }

    Column
    {
        anchors.fill: parent
        spacing: 0

        Rectangle
        {
            id: title

            width: parent.width
            height: 60

            color: "#080B10"

            Text
            {
                anchors.centerIn: parent

                text: "Server Users (" + connectedUsersModel.count + ")"

                color: "white"
                font.pixelSize: 18
                font.bold: true
            }
        }

        ListView
        {
            id: connectedUsersView

            width: parent.width
            height: parent.height - title.height

            model: connectedUsersModel
            clip: true
            spacing: 8
            ScrollBar.vertical: ScrollBar
            {
                policy: ScrollBar.AsNeeded
                opacity:0.8
                width: 12
                visible: connectedUsersView.contentHeight > connectedUsersView.height

                contentItem: Rectangle
                {
                    implicitWidth: 8
                    radius: width / 2
                    color: "#839ac7"
                }

                background: Rectangle
                {
                    color: "#3c5484"
                }
            }

            delegate: Rectangle
            {
                width: connectedUsersView.width/1.20
                anchors.horizontalCenter: parent.horizontalCenter
                height: 60
                radius: height
                border.color: "#2C3B57"
                border.width: 2
                color: userMouseArea.containsMouse
                       ? "#1C273A"
                       : "#0B1018"

                Row
                {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    spacing: 10
                    Rectangle
                    {
                        id: avatar
                        anchors.verticalCenter: parent.verticalCenter
                        width: 35
                        height: width
                        radius: width / 2

                        color: noAvatarLetter.visible ? "#839ac7" : "transparent"

                        Text
                        {
                            id:noAvatarLetter
                            anchors.centerIn: parent
                            visible: model.userAvatarPath === ""
                            text: "?"
                            color: "white"
                            font.pixelSize: 16
                        }

                        Image
                        {
                            source: model.userRelation===Relationship.Blocked ? "" //dont show blocked user's avatar
                                            : model.userAvatarPath
                            anchors.fill: parent
                            fillMode: Image.PreserveAspectFit
                            mipmap: true
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

                            color: UiHelpers.statusColor(model.userStatus)
                        }
                    }

                    Text
                    {
                        color: UiHelpers.relationColor(model.userRelation)
                        text: userName
                        anchors.verticalCenter: parent.verticalCenter
                    }

                }


                Image
                {
                    id:iconUserOs
                    source: getOsIcon(model.userOsName)
                    width: 20
                    height: 20
                    anchors
                    {
                        right:parent.right
                        rightMargin: 20
                        verticalCenter:parent.verticalCenter
                    }

                    function getOsIcon(name)
                    {
                        switch(name)
                        {
                            case "Android": return "icons/os/android.png";
                            case "Windows": return "icons/os/windows.png";
                            case "iOS": return "icons/os/ios.png";
                            case "macOS": return "icons/os/macos.png";
                            case "Linux": return "icons/os/linux.png";
                            default: return "";
                        }
                    }
                }



                MouseArea
                {
                    id:userMouseArea
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                    onClicked: function(mouse)
                    {
                        if (mouse.button === Qt.RightButton)
                        {
                            var selectedUser = user.clientUser(model.userId)
                            if (selectedUser.self)
                                return

                            userContextPopup.userId = model.userId
                            userContextPopup.username = model.userName
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

                        if(mouse.button === Qt.LeftButton)
                        {
                            profilePopup.clientUser = user.clientUser(model.userId)
                            profilePopup.open()
                        }

                    }


                }

            }
        }
    }
}
