import QtQuick
import QtQuick.Controls

import CustomVideo 1.0

Item {
    anchors.fill: parent
    Rectangle
    {
        color:"Red"
        anchors.fill: parent
    }


    VideoItem {
        id: video
        objectName: "videoItem"
        anchors.fill: parent
    }
    // Column
    // {
        // width: parent.width
        // height:parent.height
        // Item {
        //     width: parent.width
        //     height: parent.height/2

        //     VideoItem {
        //         id: monitor
        //         objectName: "monitorItem"
        //         anchors.fill: parent
        //     }
        // }
    // }
}
