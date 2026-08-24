import QtQuick
import QtQuick.Controls
import QtMultimedia

import "../constants/"

Item
{
    id: root

    property int attachmentId

    property bool showingVideo: user.hasAttachmentImage(attachmentId)

    property real progress: 0
    property bool downloading: false

    width: 320
    height: showingVideo ? 220 : 60

    Loader
    {
        anchors.fill: parent
        sourceComponent: showingVideo ? videoComponent
                                      : downloadComponent
    }

    Component
    {
        id: downloadComponent

        Rectangle
        {
            radius: 8
            color: Theme.current.surface

            Text
            {
                id: downloadText
                anchors.centerIn: parent
                text: downloading
                      ? Math.round(progress * 100) + "%"
                      : "Download Video"

                color: Theme.current.text
            }

            ProgressBar
            {
                anchors.top: downloadText.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                visible: downloading

                from: 0
                to: 1
                value: progress
            }

            MouseArea
            {
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
        id: videoComponent

        Rectangle
        {
            color: Theme.current.background

            MediaPlayer
            {
                id: player

                source: user.attachmentUrl(root.attachmentId)

                videoOutput: videoOutput
                audioOutput: audioOutput
            }

            AudioOutput
            {
                id: audioOutput
            }

            VideoOutput
            {
                id: videoOutput

                anchors.fill: parent
                fillMode: VideoOutput.PreserveAspectFit
            }

            BusyIndicator
            {
                anchors.centerIn: parent

                running: player.mediaStatus === MediaPlayer.LoadingMedia
                      || player.mediaStatus === MediaPlayer.BufferingMedia

                visible: running
            }

            Rectangle
            {
                id: playOverlay

                width: 70
                height: 70
                radius: 35

                anchors.centerIn: parent

                color: "#80000000"

                visible: player.playbackState !== MediaPlayer.PlayingState

                Text
                {
                    anchors.centerIn: parent

                    text: "▶"

                    color: Theme.current.text

                    font.pixelSize: 34
                }

                MouseArea
                {
                    anchors.fill: parent

                    cursorShape: Qt.PointingHandCursor

                    onClicked:
                    {
                        if (player.playbackState === MediaPlayer.PlayingState)
                            player.pause()
                        else
                            player.play()
                    }
                }
            }

            Slider
            {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                from: 0
                to: player.duration

                value: player.position

                onMoved:
                    player.setPosition(value)
            }

            Text
            {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 6

                color: Theme.current.text

                text:
                    Math.floor(player.position / 1000)
                    + " / "
                    + Math.floor(player.duration / 1000)
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
            progress = 1
            showingVideo = true
        }

        function onAttachmentDownloadFailed(id, reason)
        {
            if (id !== root.attachmentId)
                return

            downloading = false
            console.log(reason)
        }
    }
}
