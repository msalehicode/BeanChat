import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

Item
{
    id: root

    width: parent.width
    height: parent.height

    Rectangle
    {
        anchors.fill: parent
        color: "#313338"
    }

    Rectangle
    {
        id: title

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        height: 60
        color: "#222427"

        Text
        {
            anchors.centerIn: parent

            text: "Chat in " + user.myChannelName

            color: "white"
            font.pixelSize: 18
            font.bold: true
        }
    }

    ListView
    {
        id: chatView

        anchors {
            top: title.bottom
            left: parent.left
            right: parent.right
            bottom: enterTextBase.top
        }

        clip: true
        spacing: 2

        model: chatModel

        ScrollBar.vertical: ScrollBar
        {
            policy: ScrollBar.AsNeeded
        }

        boundsBehavior: Flickable.StopAtBounds

        flickableDirection: Flickable.VerticalFlick

        pressDelay: 150

        // onCountChanged:
        //     {
        //         positionViewAtEnd()
        //     }

        delegate: Item
        {
            id:delegatedItem
            width: chatView.width
            implicitHeight: contentColumn.implicitHeight + 12

            //detect those links which are image to show them automatically.
            property bool isImage: model.textMessage.match(/^https?:\/\/.*\.(png|jpg|jpeg|gif|webp)$/i)

            function makeLinksClickable(text)
            {
                text = text.replace(/&/g, "&amp;")
                text = text.replace(/</g, "&lt;")
                text = text.replace(/>/g, "&gt;")

                text = text.replace(
                    /(https?:\/\/[^\s]+)/g,
                    '<a href="$1">$1</a>')

                text = text.replace(/\n/g, "<br>")

                return text
            }

            Rectangle
            {
                anchors.fill: parent
                color: hoverHandler.hovered
                       ? "#222427"
                       : "transparent"
                z: -1
            }

            HoverHandler
            {
                id: hoverHandler
            }

            Row
            {
                anchors {
                    left: parent.left
                    right: parent.right
                    leftMargin: 12
                    rightMargin: 12
                    top: parent.top
                    topMargin: 6
                }

                spacing: 12

                Rectangle
                {
                    width: 40
                    height: 40
                    radius: 20

                    color: "#5865F2"

                    Text
                    {
                        anchors.centerIn: parent

                        text: model.senderId.toString().charAt(0)

                        color: "white"
                        font.bold: true
                    }
                }

                Column
                {
                    id: contentColumn

                    width: parent.width - 60

                    spacing: 2

                    Row
                    {
                        spacing: 8

                        Text
                        {
                            text: model.senderId
                            color: "white"
                            font.bold: true
                            font.pixelSize: 15
                        }

                        Text
                        {
                            text: model.timestamp
                            color: "#949BA4"
                            font.pixelSize: 11
                        }
                    }


                    Image
                    {
                        visible: delegatedItem.isImage

                        source: delegatedItem.isImage
                                    ? model.textMessage
                                    : ""

                        width: 300
                        fillMode: Image.PreserveAspectFit

                        cache: true
                        MouseArea
                        {
                            anchors.fill: parent

                            cursorShape: Qt.PointingHandCursor

                            onClicked:
                            {
                                showImagePopup.imageSource = model.textMessage
                                showImagePopup.open()
                            }
                        }
                    }

                    TextEdit
                    {
                        id: messageText

                        textFormat: TextEdit.RichText
                        readOnly: true
                        selectByMouse: true

                        font.pixelSize: !delegatedItem.isImage? 14 : 7
                        width: parent.width
                        wrapMode: TextArea.Wrap
                        color: "#DBDEE1"
                        text: delegatedItem.makeLinksClickable(model.textMessage)

                        cursorDelegate: null

                        MouseArea
                        {
                            anchors.fill: parent
                            acceptedButtons: Qt.NoButton

                            cursorShape: messageText.hoveredLink.length > 0
                                         ? Qt.PointingHandCursor
                                         : Qt.IBeamCursor

                            hoverEnabled: true
                        }

                        onLinkActivated: function(link)
                        {
                            Qt.openUrlExternally(link)
                        }
                    }

                }
            }
        }
    }


    Rectangle
    {
        id: scrollToBottomButton

        width: 48
        height: 48

        visible: !chatView.atYEnd

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: enterTextBase.top

        anchors.bottomMargin: 12
        radius: width / 2
        color: "#2B2D31"
        border.color: "#4A4D52"
        z:100
        Image
        {
            width: parent.width/2
            height: width
            anchors.centerIn: parent
            source: "icons/down.png"
        }

        MouseArea
        {
            anchors.fill: parent
            onClicked: chatView.positionViewAtEnd()
        }

    }

    Rectangle
    {
        id: enterTextBase

        function sendMessage()
        {
            var msg = messageInput.text.trim()

            if (msg.length === 0)
                return

            user.sendMessage(msg)

            messageInput.clear()

            // chatView.positionViewAtEnd()
        }

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        color: "#313338"

        height: inputBackground.height + 16

        Rectangle
        {
            id: inputBackground

            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom

                leftMargin: 16
                rightMargin: 16
                bottomMargin: 8
            }

            radius: 8
            color: "#383A40"

            height: Math.min(
                        Math.max(messageInput.contentHeight + 24, 48),
                        180)

            Row
            {
                anchors.fill: parent

                anchors.leftMargin: 12
                anchors.rightMargin: 12

                spacing: 10


                ScrollView
                {
                    id: messageScroll

                    width: parent.width - sendButton.width - 20
                    height: parent.height

                    ScrollBar.vertical.policy: ScrollBar.AsNeeded

                    TextArea
                    {
                        id: messageInput

                        width: messageScroll.availableWidth

                        wrapMode: TextEdit.Wrap

                        color: "#DBDEE1"

                        background: Rectangle
                        {
                            color:"#4a4c51"
                        }

                        topPadding: 10
                        bottomPadding: 10

                        placeholderText: "Message"

                        Keys.onPressed: function(event)
                        {
                            if ((event.key === Qt.Key_Return
                                 || event.key === Qt.Key_Enter)
                                && !(event.modifiers & Qt.ShiftModifier))
                            {
                                event.accepted = true
                                enterTextBase.sendMessage()
                            }
                        }
                    }
                }

                Rectangle
                {
                    id: sendButton

                    width: 40
                    height: 40
                    radius: 20
                    color: "#5865F2"
                    anchors.verticalCenter: parent.verticalCenter

                    Image
                    {
                        width: parent.width/2
                        height: width
                        anchors.centerIn: parent
                        source: "icons/send.png"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: enterTextBase.sendMessage()
                    }

                }
            }
        }
    }

}
