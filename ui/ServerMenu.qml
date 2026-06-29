import QtQuick
import QtQuick.Controls

Rectangle
{
    id: root

    property bool opened: false

    signal createChannelClicked()
    signal settingsClicked()

    width: parent.width
    height: opened ? content.implicitHeight + 16 : 0

    clip: true
    color: "#111214"
    border.color: "#2b2d31"
    border.width: 1
    radius: 8

    Behavior on height
    {
        NumberAnimation
        {
            duration: 150
            easing.type: Easing.OutCubic
        }
    }

    Column
    {
        id: content
        anchors.fill: parent
        anchors.margins: 8

        ItemDelegate
        {
            id: createChannelItem

            width: parent.width
            height: 36

            background: Rectangle
            {
                radius: 5
                color: createChannelItem.hovered ? "#4752C4" : "transparent"
            }

            contentItem: Text
            {
                text: "➕  Create Channel..."
                color: "white"
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: root.createChannelClicked()
        }

        ItemDelegate
        {
            id: settingsItem

            width: parent.width
            height: 36

            background: Rectangle
            {
                radius: 5
                color: settingsItem.hovered ? "#4752C4" : "transparent"
            }

            contentItem: Text
            {
                text: "⚙  Server Settings"
                color: "white"
                font.pixelSize: 14
                verticalAlignment: Text.AlignVCenter
            }

            onClicked: root.settingsClicked()
        }
    }
}
