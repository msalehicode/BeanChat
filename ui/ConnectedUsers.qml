import QtQuick
import QtQuick.Controls.Material

import QtQuick.Layouts


Item
{
    Rectangle
    {
        color: "#313338"
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

            color: "#222427"

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
                    color: "#72767d"
                }

                background: Rectangle
                {
                    color: "#202225"
                }
            }

            delegate: Rectangle
            {
                width: connectedUsersView.width/1.11
                anchors.horizontalCenter:parent.horizontalCenter
                height: 60
                radius: height
                color: "#202225"

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
                        color: "white"
                        text: userId + " " + userName
                        anchors.verticalCenter: parent.verticalCenter
                    }


                    Image
                    {
                        id:iconUserOs
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

                        source: getOsIcon(model.userOsName)
                        width: 20
                        height: 20
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    // Text
                    // {
                    //     text: userOsVersion + " " +userAppVersion + " " +userAppBuildType
                    //     color: "white"
                    //     anchors.verticalCenter: parent.verticalCenter
                    // }
                }



            }
        }
    }
}
