import QtQuick
import QtQuick.Controls

import "../constants/"

Item
{
    id: root

    property int attachmentId

    property bool showingImage: user.hasAttachmentImage(attachmentId)

    property string statusText: "Download Image"
    property color statusColor: Theme.current.text

    property real progress: 0
    property bool downloading: false

    signal imageClicked(int attachmentId)



    width: 300
    height: showingImage ? 180 : 60

    Loader
    {
        anchors.fill: parent

        sourceComponent: showingImage ? imageComponent
                                    : downloadComponent
    }

    Component
    {
        id: downloadComponent

        Rectangle
        {
            color: Theme.current.surface2
            border.color: Theme.current.border
            border.width: 1
            radius: 8

            Text
            {
                id:downloadText
                anchors.centerIn: parent
                text: downloading
                         ? Math.round(progress * 100) + "%"
                         : root.statusText

                color: root.statusColor
            }

            ProgressBar
            {
                anchors
                {
                    top:downloadText.bottom
                    horizontalCenter: downloadText.horizontalCenter
                }

                visible: downloading
                from: 0
                to: 100
                value: progress
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked:
                {
                    user.downloadAttachment(root.attachmentId)
                }
            }
        }
    }

    Component
    {
        id: imageComponent

        Image
        {
            id:downloadedImage
            anchors.fill: parent
            cache: false
            fillMode: Image.PreserveAspectFit
            source: "image://attachments/" + root.attachmentId

            //try to make image smaller as we needed, if image has Large reso it would fail due to memory alloc, later wold fix this.
            //by using thumb nail and when clicked would request and show orginal image
            sourceSize.width: width
            sourceSize.height: height

            BusyIndicator
               {
                   anchors.centerIn: parent
                   running: downloadedImage.status === Image.Loading
                   visible: running
               }

               Text
               {
                   anchors.centerIn: parent

                   visible: downloadedImage.status === Image.Error

                   text: "Failed to load image"
                   color: Theme.current.error
               }

            MouseArea
            {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor

                onClicked:
                {
                    //if image is downloaded and loaded open show image popup..
                    if (downloadedImage.status === Image.Ready && showingImage)
                        imageClicked(root.attachmentId)
                }
            }
        }
    }


    Connections
    {
        target: user

        function onAttachmentDownloadProgress(id, p)
        {
            if (id !== root.attachmentId)
                return

            downloading = true
            progress = p
        }


        function onAttachmentDownloaded(id)
        {
            if (id !== root.attachmentId)
                return

            downloading = false
            progress = 1.0

            console.log("image downloaded fine.. att id=",id)
            showingImage = true;
        }

        function onAttachmentDownloadFailed(id, reason)
        {
            if (id !== root.attachmentId)
                return

            downloading = false
            console.log("load image failed.. att id=",id)
            root.statusText= "download failed error: " +reason
            root.statusColor =Theme.current.error
        }

    }
}
