import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../constants/"

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

            color: Theme.current.text

            font.pixelSize: 14

            verticalAlignment: Text.AlignVCenter

            elide: Text.ElideRight
        }

        background: Rectangle
        {
            color: parent.highlighted
                   ? Theme.current.selection
                   : Theme.current.surface
        }
    }

    indicator: Text
    {
        anchors.right: parent.right
        anchors.rightMargin: 14

        anchors.verticalCenter: parent.verticalCenter

        text: "▼"

        color: Theme.current.icon

        font.pixelSize: 10
    }

    contentItem: Text
    {
        text: root.displayText

        color: Theme.current.text

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
                ? Theme.current.accent
                : (root.hovered
                      ? Theme.current.borderLight
                      : Theme.current.inputBorder)

        color: Theme.current.input

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
            color: Theme.current.surface3
            border.color: Theme.current.border
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
