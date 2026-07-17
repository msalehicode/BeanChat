import QtQuick
import QtQuick.Controls

Item
{
    id: root

    property int attachmentId

    property bool showingImage: user.hasAttachmentImage(attachmentId)


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
            color: "#2B2D31"
            radius: 8

            Text
            {
                id:downloadText
                anchors.centerIn: parent
                text: downloading ? Math.round(progress * 100) + "%" : "Download Image"
                color: "white"
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
                to: 1
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

        AnimatedImage
        {
            id:downloadedImage
            cache: false
            fillMode: Image.PreserveAspectFit
            width: 300
            height: status === AnimatedImage.Ready
                        ? width * implicitHeight / implicitWidth
                        : 180

            source:  user.attachmentUrl(root.attachmentId)
            playing: root.showingImage && root.visible


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

                   text: "Failed to load animated image"
                   color: "red"
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

            console.log("ainmated image downloaded fine.. att id=", attachId)
            showingImage = true;
        }

        function onAttachmentDownloadFailed(id, reason)
        {
            if (id !== root.attachmentId)
                return

            downloading = false
            console.log("load ainmated image failed.. att id=",attachId)
            root.downloadError=true
            downloadText.text= "download failed error: " +errorMessage
            downloadText.color="red"
        }

    }
}
