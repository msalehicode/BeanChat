import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item
{
    id: root

    property int key: 0
    property string keyString: "None"

    signal keySelected(int key)

    implicitWidth: 420
    implicitHeight: 40

    property bool waitingForKey: false

    RowLayout
    {
        anchors.fill: parent

        spacing: 10

        ////////////////////////////////////////////////////////////
        // Current key
        ////////////////////////////////////////////////////////////

        Rectangle
        {
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            radius: 8

            color: "#1E1F22"

            border.width: 1
            border.color:
                root.waitingForKey
                    ? "#5865F2"
                    : "#404249"

            Behavior on border.color
            {
                ColorAnimation
                {
                    duration: 120
                }
            }

            Label
            {
                anchors.centerIn: parent

                color: "white"

                text:
                    root.waitingForKey
                    ? "Press any supported key..."
                    : root.keyString
            }
        }

        ////////////////////////////////////////////////////////////
        // Change
        ////////////////////////////////////////////////////////////

        Button
        {
            text: "Change"

            onClicked:
            {
                root.waitingForKey = true
                keyGrabber.forceActiveFocus()
            }
        }

        ////////////////////////////////////////////////////////////
        // Cancel
        ////////////////////////////////////////////////////////////

        Button
        {
            visible: root.waitingForKey

            text: "Cancel"

            onClicked:
                root.waitingForKey = false
        }
    }

    ////////////////////////////////////////////////////////////////
    // Invisible keyboard handler
    ////////////////////////////////////////////////////////////////

    Item
    {
        id: keyGrabber

        anchors.fill: parent

        visible: false

        focus: root.waitingForKey

        Keys.onPressed: function(event)
        {
            if(!root.waitingForKey)
                return

            let k = event.key

            let allowed =

                (k >= Qt.Key_A && k <= Qt.Key_Z)

                ||

                (k >= Qt.Key_0 && k <= Qt.Key_9)

                ||

                (k >= Qt.Key_F1 && k <= Qt.Key_F12)

                ||

                k === Qt.Key_Tab
                || k === Qt.Key_Return
                || k === Qt.Key_Space

                ||

                k === Qt.Key_Left
                || k === Qt.Key_Right
                || k === Qt.Key_Up
                || k === Qt.Key_Down

                ||

                k === Qt.Key_BracketLeft
                || k === Qt.Key_BracketRight
                || k === Qt.Key_Backslash
                || k === Qt.Key_Semicolon
                || k === Qt.Key_Comma
                || k === Qt.Key_Period
                || k === Qt.Key_Slash
                || k === Qt.Key_QuoteLeft
                || k === Qt.Key_Equal
                || k === Qt.Key_Minus

                ||

                k === Qt.Key_Menu

            if(!allowed)
                return

            root.key = k

            root.keySelected(k)

            root.waitingForKey = false

            event.accepted = true
        }
    }
}
