import QtQuick
import QtQuick.Layouts
import CustomVideo 1.0
import QtQuick.Controls


Item
{
    width: parent.width-(rightPanel.width+(channelList.width)+leftPanel.width)
    height: parent.height
    clip: true

    Rectangle {
        id: centerContent
        anchors.fill: parent
        anchors.left: parent.left
        anchors.leftMargin: channelList.handleWidth
        color: "transparent"


        GridView
        {
            id:channelClientsGridView

            cellWidth: 250
            cellHeight: 150
            anchors.fill: parent

            // property int minTileWidth: 220
            // property int maxTileWidth: 320

            // property int columns:
            //     Math.max(1,
            //              Math.ceil(Math.sqrt(count)))

            // property int rows:
            //     Math.ceil(count / columns)

            // cellWidth: Math.max(
            //                minTileWidth,
            //                Math.min(
            //                    maxTileWidth,
            //                    parent.width / columns))

            // cellHeight: cellWidth * 9 / 16

            // property int contentRowsHeight:
            //     rows * cellHeight

            // width: Math.min(
            //            parent.width,
            //            Math.min(count, columns) * cellWidth)

            // height: Math.min(
            //             parent.height,
            //             contentRowsHeight)

            // x: (parent.width - width) / 2

            // y: contentRowsHeight < parent.height
            //    ? (parent.height - contentRowsHeight) / 2
            //    : 0

            model: participantModel

            delegate:Rectangle
            {
                // width: channelClientsGridView.cellWidth - 20
                // height:channelClientsGridView.cellHeight - 20
                width: channelClientsGridView.cellWidth - 10
                height: channelClientsGridView.cellHeight - 10
                radius: 30
                color: "grey"

                Rectangle
                {
                    id:userWithVideo
                    anchors.fill: parent
                    radius:parent.radius
                    visible: model.isCameraOpen
                    color:"transparent"
                    clip: true
                    VideoItem
                    {
                        anchors.fill: parent
                        sink: model.videoSink
                        radius: 45 //due to its C++ Property radius value may be varied to normal qml radiuses. e.g 30 has to be 45 too looks simular
                        Component.onCompleted:
                        {
                            console.log("building VideItem, username=",username, "sink=",videoSink, "isCameraOpen=",isCameraOpen)
                        }
                    }
                }


                Rectangle
                {
                    id:baseUserParticipantInfo
                    anchors.fill: parent
                    radius: parent.radius
                    color: model.isCameraOpen ? "transparent" : "purple"
                    border.width: model.isTalking ? 4 : 0
                    border.color: "lime"

                    Rectangle
                    {
                        id:userAvatarCenterParticipant
                        width: 50
                        height: 50
                        color: "white"
                        radius: 30
                        visible: !model.isCameraOpen
                        anchors.centerIn: parent
                    }
                    Rectangle
                    {
                        id:userStatusIconsOverParticipant
                        width: parent.width
                        height: 40
                        radius: height
                        color: "transparent"
                        anchors.bottom: parent.bottom

                        RowLayout
                        {
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            spacing: 5

                            Text
                            {
                                text: "(" + model.userId + ") " + model.username
                                font.pixelSize: 15
                                color: "white"
                                font.bold:model.userId === user.myId
                                Layout.alignment: Qt.AlignVCenter
                            }

                            Item
                            {
                                Layout.fillWidth: true
                            }

                            RowLayout
                            {
                                spacing: 2
                                Layout.alignment: Qt.AlignVCenter

                                // Image
                                // {
                                //     visible: model.isScreenShare
                                //     Layout.preferredWidth: 20
                                //     Layout.preferredHeight: 20
                                //     fillMode: Image.PreserveAspectFit
                                //     source: "icons/screen.png"
                                // }

                                Image
                                {
                                    visible: model.isMuted
                                    Layout.preferredWidth: 20
                                    Layout.preferredHeight: 20
                                    fillMode: Image.PreserveAspectFit
                                    source: "icons/microphone-closed.png"
                                }


                                Image
                                {
                                    visible: model.isDeafened
                                    Layout.preferredWidth: 20
                                    Layout.preferredHeight: 20
                                    fillMode: Image.PreserveAspectFit
                                    source: "icons/headphone-closed.png"
                                }
                            }
                        }
                    }

                }


            }
        }

    }

}
