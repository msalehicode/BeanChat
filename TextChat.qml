import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Item {
    id:root
    anchors.fill: parent

    Rectangle
    {
        color:"black"
        anchors.fill: parent
    }

    Column
    {
        width: parent.width
        height: parent.height

        Rectangle
        {
            id:title
            width: parent.width
            // anchors.top: parent.top
            height: 60
            color:"grey"
            Text
            {
                text: user.myChannelName
                anchors.centerIn: parent
                color:"white"
            }
        }
        ListView
        {
            id: chatView
            width: parent.width
            height: parent.height-(title.height+enterTextBase.height)
            model: chatModel
            spacing: 8
            clip: true
            anchors.left: parent.left
            anchors.leftMargin: handle.width


            delegate: Item
            {
                width: chatView.width
                height: bubble.height + 8

                Rectangle {
                    id: bubble

                    width: Math.min(chatView.width * 0.2,
                                    textItem.paintedWidth + textItem.implicitWidth)

                    height: textItem.paintedHeight + 20

                    radius: 12
                    color: model.senderId===user.myId  ? "#3b82f6" : "#404040"

                    anchors {
                        right: model.senderId===user.myId ? parent.right : undefined
                        left: model.senderId===user.myId  ? undefined : parent.left
                        leftMargin: 20
                        rightMargin: 20
                    }

                    Text {
                        id: textItem
                        anchors.fill: parent
                        anchors.margins: 10

                        text: model.senderId + " : " +model.textMessage
                        wrapMode: Text.Wrap
                        color: "white"
                    }

                    Text
                    {
                        id:textTime
                        text: model.timestamp
                        color:"red"
                        anchors.bottom: parent.bottom
                        anchors.right: parent.right
                    }
                }
            }


        }


    }
    Rectangle
    {
        id:enterTextBase
        width: parent.width
        height: 60
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: handle.width/2
        color:"transparent"
        Rectangle
        {
            id:bgEnterMessage
            width: parent.width-buttonSendMessage.width
            height: parent.height
            color: "grey"
            TextInput
            {
                id:enteredMessage
                anchors.fill: parent
                text: ""
                color: "black"
                font.pixelSize: 20
                onAccepted: buttonSendMessage.clicked()
            }
        }
        Button
        {
            id:buttonSendMessage
            height: parent.height
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: "Send"
            onClicked:
            {
                user.sendMessage(enteredMessage.text)
                enteredMessage.clear()
            }
        }
    }


    Rectangle
    {
        id: handle

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
                startWidth = root.parent.width
            }

            onPositionChanged: function(mouse)
            {
                if (pressed)
                {
                    let dx = mouse.x - startX
                    let pos = startWidth - dx

                    if (pos < 250)
                        pos = 250
                    else if (pos > 800)
                        pos = 800

                    root.parent.width = pos
                }
            }
        }
    }
}
