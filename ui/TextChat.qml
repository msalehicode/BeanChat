import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import QtQuick.Dialogs
import BeanChatClient 1.0
import "constants/"

Item
{
    id: root

    width: parent.width
    height: parent.height

    Rectangle
    {
        anchors.fill: parent
        color: "transparent"
    }
    property string selectedFilePath: ""
    property string selectedFileName: ""
    property int uploadedFileAttachedId:0
    property bool uploadingFile:false
    function resetAttachFile()
    {
        if(root.uploadingFile)
        {
            logger.info("textChat","a file is being uploaded, wait to fail or success, cant cancel it")
            return;
        }

        textSelectedFileName.color="white"
        root.uploadedFileAttachedId=0
        root.selectedFilePath=""
        root.selectedFileName=""
        root.uploadingFile=false
    }


    //show error when upload file failed.
    property bool errorDialogVisible: false
    property string errorDialogTitle: ""
    property string errorDialogMessage: ""
    function showError(title, message)
    {
        errorDialogTitle = title
        errorDialogMessage = message
        errorDialogVisible = true
    }

    function uploadSelectedFile()
    {
        if(!root.uploadingFile) //upload once, or wait for upload complete
        {
            //show file name to user
            root.selectedFileName =
                    root.selectedFilePath.substring(
                        root.selectedFilePath.lastIndexOf("/") + 1)

            user.sendFile(root.selectedFilePath)
            root.uploadingFile=true
            textSelectedFileName.color="grey"
        }
    }


    FileDialog
    {
        id: fileDialog

        title: "Select file"

        fileMode: FileDialog.OpenFile

        onAccepted:
        {
            logger.action("a file accepted by file dialog on textChat")
            //to know name and show as selected file to user
            root.selectedFilePath = selectedFile.toString()
            uploadSelectedFile()
        }

        onRejected:
        {
            logger.info("textChat", "file dialog rejected")
        }
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
        color: "#080B10"
        clip: true

        Row
        {
            width: implicitWidth
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter
            spacing:7
            Text
            {
                text: "Chat in " + user.myChannelName
                color: "white"
                font.pixelSize: 18
                font.bold: true
                anchors.verticalCenter: parent.verticalCenter
                width: implicitWidth>root.width/1.50 ? root.width/1.50 : implicitWidth
                elide: Text.ElideRight
            }
            Image
            {
                width:30
                height: width
                source: "icons/save.png"
                visible: user.myChannelSavesChat
                anchors.verticalCenter: parent.verticalCenter
            }
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
            opacity:0.8
            width: 8
            visible: chatView.contentHeight > chatView.height

            contentItem: Rectangle
            {
                width: 4
                radius: width / 2
                color: "#839ac7"
            }

            background: Rectangle
            {
                color: "#3c5484"
            }
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
                       ? "#080B10"
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
                    id:userInfo
                    width: 40
                    height: 40
                    radius: 20

                    color: noAvatarLetter.visible ? avatarBg : "transparent"
                    border.width: 1

                    border.color: "#2C3B57"

                    Text
                    {
                        id:noAvatarLetter
                        anchors.centerIn: parent
                        text:  model.senderName.charAt(0).toUpperCase()
                        visible: !userAvatar.visible
                        color: "white"
                        font.bold: true
                    }
                    Image
                    {
                        id:userAvatar
                        anchors.fill: parent
                        visible: model.senderAvatarPath.length>0
                        source: model.senderRelation===Relationship.Blocked? "" //dont show blocked user's avatar
                                        : model.senderAvatarPath
                    }
                    MouseArea
                    {
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        anchors.fill: parent

                        onClicked:
                        {
                            profilePopup.clientUser = user.clientUser(model.senderId)
                            profilePopup.open()
                        }
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
                        border.color: bg2

                        color: UiHelpers.statusColor(model.senderStatus)
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
                            text: model.senderName
                            color: UiHelpers.relationColor(model.senderRelation)
                            font.bold: true
                            font.pixelSize: 15
                            MouseArea
                            {
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                anchors.fill: parent

                                onClicked:
                                {
                                    logger.action("clicked senderName to open profilePopup by textChat")
                                    profilePopup.clientUser = user.clientUser(model.senderId)
                                    profilePopup.open()
                                }
                            }
                        }

                        Text
                        {
                            text: model.timestamp
                            color: "#949BA4"
                            font.pixelSize: 11
                        }
                    }

                    Column
                    {
                        id:messageContentBase
                        width: parent.width
                        spacing: 6
                        height: visible? implicitHeight : 0
                        visible: model.senderRelation === Relationship.Blocked && !blockedOverlay.revealed ? false : true

                        //make sure show end of long messages
                        onHeightChanged:
                        {
                            if (chatView.atYEnd)
                                Qt.callLater(chatView.positionViewAtEnd)
                        }

                        Loader
                        {
                            id: messageLoader

                            width: parent.width

                            source:
                            {
                                    logger.info("textChat","loader loading message, messageType ="+model.messageType)

                                    switch (model.messageType)
                                    {
                                    case MessageType.Image:
                                        return "messageDelegates/MessageImage.qml"
                                    case MessageType.AnimatedImage:
                                        return "messageDelegates/MessageImageAnimated.qml"
                                    case MessageType.Video:
                                        return "messageDelegates/MessageVideo.qml"
                                    case MessageType.Audio:
                                        return "messageDelegates/MessageAudio.qml"
                                    default:
                                        logger.warning("textChat","undefined messageType")
                                        return ""
                                    }
                            }

                            onLoaded:
                            {
                                item.attachmentId = model.attachId;

                                if(model.messageType===MessageType.AnimatedImage)
                                {
                                    item.imageClicked.connect(function(attachmentId)
                                           {
                                               showImageAnimatedPopup.imageSource = user.attachmentUrl(attachmentId)
                                               showImageAnimatedPopup.open()
                                           })
                                }
                                else if(model.messageType===MessageType.Image)
                                {
                                    item.imageClicked.connect(function(attachmentId)
                                           {
                                               showImagePopup.imageSource = "image://attachments/" + attachmentId
                                               showImagePopup.open()
                                           })
                                }

                                //Video and Audio are ok, they don't need click for now.

                            }
                        }


                        // Shared caption
                        TextEdit
                        {
                            id: caption

                            visible: model.textMessage.length > 0

                            width: parent.width

                            readOnly: true
                            selectByMouse: true

                            wrapMode: TextEdit.Wrap
                            textFormat: TextEdit.RichText

                            color: "#DBDEE1"

                            text: delegatedItem.makeLinksClickable(model.textMessage)

                            //make sure show end of long messages
                            onContentHeightChanged:
                            {
                                if (chatView.atYEnd)
                                    Qt.callLater(chatView.positionViewAtEnd)
                            }

                            onLinkActivated: function(link)
                            {
                                Qt.openUrlExternally(link)
                            }

                            cursorDelegate: null


                            MouseArea
                            {
                                anchors.fill: parent
                                acceptedButtons: Qt.NoButton

                                cursorShape: caption.hoveredLink.length > 0
                                             ? Qt.PointingHandCursor
                                             : Qt.IBeamCursor

                                hoverEnabled: true
                            }
                        }
                    }


                    // Overlay
                    Rectangle
                    {
                        id: blockedOverlay

                        width: parent.width
                        height: messageContentBase.visible ? 0
                                  : (messageContentBase.implicitHeight>50
                                        ?messageContentBase.implicitHeight : 50)

                        color: "black"   // dark transparent
                        opacity: 0.5
                        radius: 15

                        property bool revealed: false

                        Column
                        {
                            anchors.centerIn: parent
                            spacing: 8

                            Text
                            {
                                text: "Blocked content"
                                color: "white"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                            }

                            Text
                            {
                                text: "Click to reveal"
                                color: "#cccccc"
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }

                        MouseArea
                        {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked:
                            {
                                logger.action("clicked on reveal for blocked content (from blocked user) on textChat")
                                blockedOverlay.revealed = true
                            }
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
        color: "#141C2B"
        border.color: "#2C3B57"
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
            cursorShape: Qt.PointingHandCursor
            onClicked:
            {
                logger.action("clicked on scroll to bottom in textChat")
                chatView.positionViewAtEnd()
            }
        }

    }

    Rectangle
    {
        id: enterTextBase

        function sendMessage()
        {
            if(root.uploadingFile)
            {
                logger.info("textChat"," send message ignored because a file is being uploaded...")
                return; //should wait to upload.. or fail upload
            }

            var msg;

            //a file is uploaded and ready to send with caption to it.
            if(root.uploadedFileAttachedId>0)
            {
                logger.info("textChat", "send message: a file is uploaded we have attachid="+root.uploadedFileAttachedId)
                msg = messageInput.text.trim()

                user.sendMessage(msg, root.uploadedFileAttachedId, root.selectedFilePath)

                messageInput.clear()
                resetAttachFile()
                return;
            }

            msg = messageInput.text.trim()

            if (msg.length === 0)
                return

            logger.info("textChat","sendMessage: sending simple meesage without any attachment")
            user.sendMessage(msg)

            messageInput.clear()

            // chatView.positionViewAtEnd()

        }

        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        color: "#05070b"

        height: inputBackground.height + 16

        Rectangle
        {
            id:controlSelectedFiles
            visible: selectedFilePath.length > 0

            anchors.left: inputBackground.left
            anchors.right: inputBackground.right
            anchors.bottom: inputBackground.top

            anchors.bottomMargin: 8

            height: 46

            radius: 8

            color: "#080B10"

            Row
            {
                anchors.fill: parent

                anchors.leftMargin: 12
                anchors.rightMargin: 12

                spacing: 10

                Item
                {
                    width: 30
                    height: 30

                    anchors.verticalCenter: parent.verticalCenter

                    Image
                    {
                        anchors.fill: parent
                        source: UiHelpers.uploadedFileIcon(root.selectedFileName)
                        opacity: root.uploadingFile ? 0.3 : 1
                    }

                    BusyIndicator
                    {
                        anchors.centerIn: parent

                        width: 30
                        height: 30

                        running: root.uploadingFile
                        visible: running
                    }
                }

                Text
                {
                    id:textSelectedFileName
                    anchors.verticalCenter: parent.verticalCenter

                    width: parent.width - removeButton.width - 40

                    text: selectedFileName

                    color: "white"

                    elide: Text.ElideRight
                }

                Rectangle
                {
                    id: removeButton

                    width: 26
                    height: 26

                    radius: 13

                    color: "#3B3E45"

                    anchors.verticalCenter: parent.verticalCenter

                    Text
                    {
                        anchors.centerIn: parent
                        text: "✕"
                        color: "white"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        enabled: !root.uploadingFile
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            logger.action("clicked on remove attached file in textChat")
                            root.resetAttachFile()
                        }
                    }
                }
            }
        }

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

            enabled: !root.uploadingFile //only enable when upload is not in process
            opacity: root.uploadingFile ? 0.5 : 1
            radius: 8
            color: "#080B10"

            height: Math.min(
                        Math.max(messageInput.contentHeight + 24, 48),
                        180)

            Row
            {
                anchors.fill: parent

                anchors.leftMargin: 12
                anchors.rightMargin: 12

                spacing: 10

                Rectangle
                {
                    id: attachButton

                    width: 40
                    height: 40
                    radius: 20

                    anchors.verticalCenter: parent.verticalCenter

                    color: "#2B2D31"

                    Image
                    {
                        anchors.centerIn: parent
                        width: 18
                        height: 18
                        source: "icons/attach.png"
                    }

                    MouseArea
                    {
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor

                        onClicked:
                        {
                            logger.action("clicked on attach button on textChat")
                            fileDialog.open()
                        }
                    }
                }
                ScrollView
                {
                    id: messageScroll

                    width: parent.width - attachButton.width - sendButton.width - 30
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
                            color:"transparent"
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
                                logger.action("send message by keypressed on textinput of textChat")
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
                        cursorShape: Qt.PointingHandCursor
                        onClicked:
                        {
                            logger.action("send message by send button of textChat")
                            enterTextBase.sendMessage()
                        }
                    }

                }
            }
        }
    }

    //show error upload file
    Rectangle
    {
        anchors.fill: parent

        visible: root.errorDialogVisible

        color: "#000000"
        opacity: 0.65

        z: 1000

        MouseArea
        {
            anchors.fill: parent
        }

        Rectangle
        {
            width: 360
            height: 165
            radius: 10

            anchors.centerIn: parent

            color: "#2B2D31"

            border.width: 1
            border.color: "#1E1F22"

            Column
            {
                anchors.fill: parent

                anchors.margins: 18

                spacing: 18

                Text
                {
                    text: root.errorDialogTitle

                    font.pixelSize: 20
                    font.bold: true

                    color: "white"
                }

                Text
                {
                    width: parent.width

                    text: root.errorDialogMessage

                    color: "#DBDEE1"

                    wrapMode: Text.WordWrap

                    font.pixelSize: 14
                }

                Item
                {
                    width: parent.width
                    height: 40

                    Rectangle
                    {
                        width: 90
                        height: 36

                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter

                        radius: 6

                        color: okMouse.containsMouse ? "#6D8CF5" : "#5865F2"

                        Text
                        {
                            anchors.centerIn: parent

                            text: "OK"

                            color: "white"

                            font.bold: true
                        }

                        MouseArea
                        {
                            id: okMouse

                            anchors.fill: parent

                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor

                            onClicked:
                            {
                                logger.action("clicked on ok for errorDialogVisible (upload) in textChat")
                                root.errorDialogVisible = false
                            }
                        }
                    }
                }
            }

            implicitHeight: contentHeight + 36

            property int contentHeight: childrenRect.height
        }
    }


    //handle drop to attach file
    DropArea
    {
        anchors.fill: parent

        onEntered: function(drag)
        {
            drag.accept(Qt.CopyAction)
        }

        onDropped: function(drop)
        {
            if (!drop.hasUrls || uploadingFile)
                return

            // for (let i = 0; i < drop.urls.length; ++i)
            // {
            logger.action("a file dropped into textChat")
                    root.selectedFilePath = drop.urls[0] //for now we only support one file at the time.
                    uploadSelectedFile()
            // }
        }
    }

    Connections
    {
        target: user

        function scrollToBottom()
        {
            Qt.callLater(function() {
                chatView.positionViewAtEnd()
            })
        }

        function onNewMessage()
        {
            scrollToBottom()
        }

        function onMessageSent()
        {
            scrollToBottom()
        }

        function onSendFileResult(status,error,attachId)
        {
            root.uploadingFile=false  //allow user remove attached
            if(status===true)
            {
                root.uploadedFileAttachedId=attachId
                textSelectedFileName.color="white"
            }
            else
            {
                textSelectedFileName.color="red"
                root.showError("Upload Failed", error)
                root.resetAttachFile()
            }
        }
    }

}
