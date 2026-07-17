import QtQuick
import QtQuick.Controls
import QtMultimedia

Item
{
    id: root

    property int attachmentId

    property bool downloaded: user.hasAttachmentImage(attachmentId)

    property real progress: 0
    property bool downloading: false

    width: 340
    height: downloaded ? 70 : 60

    Loader
    {
        anchors.fill: parent
        sourceComponent: downloaded
                         ? playerComponent
                         : downloadComponent
    }

    Component
    {
        id: downloadComponent

        Rectangle
        {
            radius: 8
            color: "#2B2D31"

            Text
            {
                id: txt

                anchors.centerIn: parent

                color: "white"

                text: downloading
                      ? Math.round(progress*100)+"%"
                      : "Download Audio"
            }

            ProgressBar
            {
                anchors.top: txt.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                visible: downloading

                from:0
                to:1

                value: progress
            }

            MouseArea
            {
                anchors.fill: parent

                cursorShape: Qt.PointingHandCursor

                onClicked:
                    user.downloadAttachment(root.attachmentId)
            }
        }
    }

    Component
    {
        id: playerComponent

        Rectangle
        {
            radius:8
            color:"#2B2D31"

            MediaPlayer
            {
                id: player

                source: user.attachmentUrl(root.attachmentId)

                audioOutput: output
            }

            AudioOutput
            {
                id: output
            }

            Row
            {
                anchors.fill: parent
                anchors.margins: 8

                spacing:10

                Button
                {
                    width:40
                    height:40

                    text:
                        player.playbackState === MediaPlayer.PlayingState
                        ? "⏸"
                        : "▶"

                    onClicked:
                    {
                        if(player.playbackState===MediaPlayer.PlayingState)
                            player.pause()
                        else
                            player.play()
                    }
                }

                Column
                {
                    width: parent.width-60

                    spacing:4

                    Slider
                    {
                        id: slider

                        width: parent.width

                        from:0
                        to:Math.max(player.duration,1)

                        value: player.position

                        onMoved:
                            player.setPosition(value)
                    }

                    Row
                    {
                        width: parent.width

                        Text
                        {
                            text: formatTime(player.position)
                            color:"white"
                        }

                        Item
                        {
                            width: parent.width
                        }

                        Text
                        {
                            text: formatTime(player.duration)
                            color:"white"
                        }
                    }
                }
            }
        }
    }

    function formatTime(ms)
    {
        var sec=Math.floor(ms/1000)

        var m=Math.floor(sec/60)
        var s=sec%60

        return m + ":" + (s<10 ? "0"+s : s)
    }

    Connections
    {
        target:user

        function onAttachmentDownloadProgress(id,p)
        {
            if(id!==root.attachmentId)
                return

            downloading=true
            progress=p
        }

        function onAttachmentDownloaded(id)
        {
            if(id!==root.attachmentId)
                return

            downloading=false
            downloaded=true
        }

        function onAttachmentDownloadFailed(id,reason)
        {
            if(id!==root.attachmentId)
                return

            downloading=false
            console.log(reason)
        }
    }
}
