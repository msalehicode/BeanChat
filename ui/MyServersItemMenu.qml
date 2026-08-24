import QtQuick
import QtQuick.Controls

import "constants/"
Popup
{
    id: root

    property string serverName: "BeanChat"
    property string ip: ""
    property string port: ""
    property int id: -1;
    property int dbIndex: -1;
    property bool isConnected: false;

    property alias connectButton: connectItem
    property alias disconnectButton : disconnectItem
    property alias modifyButton: modifyItem
    property alias shareQRcodeButton: shareServerQRCode
    property alias shareServerCodeButton: shareServerCode
    property alias deleteButton: deleteItem

    width: 250
    height: implicitHeight
    padding: 8

    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle
    {
        radius: 8
        color: Theme.current.surface3
        border.color: Theme.current.border
        border.width: 1
    }

    enter: Transition
    {
        ParallelAnimation
        {
            NumberAnimation
            {
                property: "opacity"
                from: 0
                to: 1
                duration: 120
            }

            NumberAnimation
            {
                property: "scale"
                from: 0.95
                to: 1.0
                duration: 120
            }
        }
    }

    exit: Transition
    {
        NumberAnimation
        {
            property: "opacity"
            to: 0
            duration: 100
        }
    }

    contentItem: Column
    {
        spacing: 0


        Rectangle
        {
            width: parent.width
            height: 72
            color: "transparent"

            Column
            {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 3

                Text
                {
                    text: root.serverName
                    color: Theme.current.text
                    font.pixelSize: 15
                    font.bold: true
                    width: implicitWidth>root.width/1.50 ? root.width/1.50 : implicitWidth
                    elide: Text.ElideRight
                }

                Text
                {
                    text: root.ip + ":" + root.port
                    color: Theme.current.textMuted
                    font.pixelSize: 12
                    width: implicitWidth>root.width/1.50 ? root.width/1.50 : implicitWidth
                    elide: Text.ElideMiddle
                }
            }
        }


        ItemDelegate
        {
            id: connectItem

            width: parent.width
            height: 36

            leftPadding: 12

            background: Rectangle
            {
                radius: 5
                color: connectItem.hovered ? Theme.current.accentHover : "transparent"
            }

            contentItem: Text
            {
                text: "🔌  Connect"
                color: Theme.current.text
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }
        }

        ItemDelegate
        {
            id: disconnectItem

            width: parent.width
            height: 36
            visible: root.isConnected

            leftPadding: 12

            background: Rectangle
            {
                radius: 5
                color: disconnectItem.hovered ? Theme.current.accentHover : "transparent"
            }

            contentItem: Text
            {
                text: "🔌  Disconnect"
                color: Theme.current.text
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }
        }

        ItemDelegate
        {
            id: modifyItem

            width: parent.width
            height: 36

            visible: root.dbIndex != -1

            leftPadding: 12

            background: Rectangle
            {
                radius: 5
                color: modifyItem.hovered ? Theme.current.accentHover : "transparent"
            }

            contentItem: Text
            {
                text: "✏️  Modify"
                color: Theme.current.text
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }
        }



        ItemDelegate
        {
            id: shareServerQRCode

            width: parent.width
            height: 36

            leftPadding: 12

            background: Rectangle
            {
                radius: 5
                color: shareServerQRCode.hovered ? Theme.current.accentHover : "transparent"
            }

            contentItem: Text
            {
                text: "🔗 Share via QR Code"
                color: Theme.current.text
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }
        }

        ItemDelegate
        {
            id: shareServerCode

            width: parent.width
            height: 36

            visible: serverCode.isIPv4(root.ip)

            leftPadding: 12

            background: Rectangle
            {
                radius: 5
                color: shareServerCode.hovered ? Theme.current.accentHover : "transparent"
            }

            contentItem: Text
            {
                text: "🔗 Share via ServerCode"
                color: Theme.current.text
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }
        }


        ItemDelegate
        {
            id: deleteItem

            width: parent.width
            height: 36

            leftPadding: 12

            background: Rectangle
            {
                radius: 5
                color: deleteItem.hovered ? Theme.current.danger : "transparent"
            }

            contentItem: Text
            {
                text: "🗑  Delete"
                color: deleteItem.hovered ? Theme.current.text : Theme.current.danger
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
