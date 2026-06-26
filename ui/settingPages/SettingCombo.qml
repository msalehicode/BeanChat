import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ComboBox
{
    id: root

    Layout.fillWidth: true

    implicitHeight: 40

    font.pixelSize: 14

    leftPadding: 14
    rightPadding: 40

    hoverEnabled: true

    delegate: ItemDelegate
    {
        required property var modelData
        required property int index

        width: ListView.view.width
        height: 38

        highlighted: root.highlightedIndex === index

        contentItem: Text
        {
            text: modelData

            color: "white"

            font.pixelSize: 14

            verticalAlignment: Text.AlignVCenter

            elide: Text.ElideRight
        }

        background: Rectangle
        {
            color: parent.highlighted
                   ? "#5865F2"
                   : "#2B2D31"
        }
    }

    indicator: Text
    {
        anchors.right: parent.right
        anchors.rightMargin: 14

        anchors.verticalCenter: parent.verticalCenter

        text: "▼"

        color: "#B5BAC1"

        font.pixelSize: 10
    }

    contentItem: Text
    {
        text: root.displayText

        color: "white"

        font.pixelSize: 14

        verticalAlignment: Text.AlignVCenter

        elide: Text.ElideRight
    }

    background: Rectangle
    {
        radius: 8

        border.width: 1

        border.color:
            root.visualFocus
                ? "#5865F2"
                : (root.hovered
                      ? "#6D6F78"
                      : "#1E1F22")

        color: "#1E1F22"

        Behavior on border.color
        {
            ColorAnimation
            {
                duration: 120
            }
        }
    }

    popup: Popup
    {
        y: root.height + 4

        width: root.width

        padding: 4

        background: Rectangle
        {
            radius: 8

            color: "#111214"

            border.color: "#404249"
        }

        contentItem: ListView
        {
            clip: true

            implicitHeight: contentHeight

            model: root.popup.visible
                   ? root.delegateModel
                   : null

            currentIndex: root.highlightedIndex

            ScrollIndicator.vertical: ScrollIndicator {}
        }
    }
}
