import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id:root
    anchors.fill: parent

    Rectangle
    {
        color:"black"
        anchors.fill: parent
    }

    ListModel {
        id: chatModel

        ListElement {
            text: "Hello!"
            mine: false
        }

        ListElement {
            text: "Hi, how are you?"
            mine: true
        }

        ListElement {
            text: "This is a scrollable Qt Quick chat example."
            mine: false
        }


        ListElement {
            text: "This is a scrollable Qt Quick chat example."
            mine: false
        }


        ListElement {
            text: "This is a scrollable Qt Quick chat example."
            mine: true
        }


        ListElement {
            text: "This is a scrollable Qt Quick chat example."
            mine: false
        }


        ListElement {
            text: "This is a scrollable Qt Quick chat example. This is a scrollable Qt Quick chat example.This is a scrollable Qt Quick chat example.This is a scrollable Qt Quick chat example.This is a scrollable Qt Quick chat example."
            mine: false
        }
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
                text:"text Chat title"
                anchors.centerIn: parent
                color:"white"
            }
        }

        ListView {
            id: chatView
            width: parent.width
            height: parent.height-(title.height+enterText.height)
            model: chatModel
            spacing: 8
            clip: true

            delegate: Item {
                width: chatView.width
                height: bubble.height + 8

                Rectangle {
                    id: bubble

                    width: Math.min(chatView.width * 0.2,
                                    textItem.paintedWidth + textItem.implicitWidth)

                    height: textItem.paintedHeight + 20

                    radius: 12
                    color: mine ? "#3b82f6" : "#404040"

                    anchors {
                        right: mine ? parent.right : undefined
                        left: mine ? undefined : parent.left
                        leftMargin: 20
                        rightMargin: 20
                    }

                    Text {
                        id: textItem
                        anchors.fill: parent
                        anchors.margins: 10

                        text: model.text
                        wrapMode: Text.Wrap
                        color: "white"
                    }
                }
            }
        }


        Rectangle
        {
            id:enterText
            width: parent.width
            height: 60
            // anchors.bottom: parent.bottom
            color:"grey"
            Text
            {
                text:"enter text to send"
                anchors.centerIn: parent
                color:"white"
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
