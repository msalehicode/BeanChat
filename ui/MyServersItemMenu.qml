import QtQuick
import QtQuick.Controls

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
    property alias deleteButton: deleteItem

    width: 250
    height: modifyItem.visible && disconnectItem.visible ? 252 :
            (disconnectItem.visible||modifyItem.visible? 214 : 176 )
    padding: 8

    modal: false
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    background: Rectangle
    {
        radius: 8
        color: "#111214"
        border.color: "#2b2d31"
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
                    color: "white"
                    font.pixelSize: 15
                    font.bold: true
                    width: implicitWidth>root.width/1.50 ? root.width/1.50 : implicitWidth
                    elide: Text.ElideRight
                }

                Text
                {
                    text: root.ip + ":" + root.port
                    color: "#949ba4"
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

            text: "Connect"

            leftPadding: 12

            background: Rectangle
            {
                radius: 5
                color: connectItem.hovered ? "#4752C4" : "transparent"
            }

            contentItem: Text
            {
                text: "🔌  Connect"
                color: "white"
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

            text: "Disconnect"

            leftPadding: 12

            background: Rectangle
            {
                radius: 5
                color: disconnectItem.hovered ? "#4752C4" : "transparent"
            }

            contentItem: Text
            {
                text: "🔌  Disconnect"
                color: "white"
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
                color: modifyItem.hovered ? "#4752C4" : "transparent"
            }

            contentItem: Text
            {
                text: "✏️  Modify"
                color: "white"
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
                color: deleteItem.hovered ? "#DA373C" : "transparent"
            }

            contentItem: Text
            {
                text: "🗑  Delete"
                color: deleteItem.hovered ? "white" : "#ED4245"
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
