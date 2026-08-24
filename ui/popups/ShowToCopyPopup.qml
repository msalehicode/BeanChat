import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import "../constants/"

Popup
{
    id: root

    modal: true
    focus: true

    width: 420
    height: 270

    anchors.centerIn: Overlay.overlay

    // Parameters
    property string titleText: "TITLE"
    property string descriptionText: "DESCRIPTION"
    property string value;

    onClosed:
    {
        //reset values
        root.titleText= "TITLE"
        root.descriptionText= "DESCRIPTION"
    }

    background: Rectangle
    {
        color: Theme.current.background
        radius: 8
        border.color: Theme.current.surface3
    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        Text
        {
            text: root.titleText
            color: Theme.current.text
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: parent.width
            elide: Text.ElideRight
        }

        Text
        {
            text: root.descriptionText
            color: Theme.current.textSecondary
            Layout.alignment: Qt.AlignHCenter
            Layout.maximumWidth: parent.width
            elide: Text.ElideRight
        }


        Rectangle
        {
            Layout.fillWidth: true
            Layout.preferredHeight: 52

            radius: 8
            color: Theme.current.surface
            border.color: Theme.current.surface3

            RowLayout
            {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8

                Text
                {
                    text: root.value
                    color: Theme.current.text

                    font.family: "Consolas"    // or "monospace"
                    font.pixelSize: 18
                    font.bold: true

                    Layout.fillWidth: true

                    elide: Text.ElideMiddle
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }

                Rectangle
                {
                    id: copyButton

                    Layout.preferredWidth: 80
                    Layout.fillHeight: true

                    radius: 6
                    color: copyLabel.text === "Copied!" ? Theme.current.success : Theme.current.accent

                    Behavior on color
                    {
                        ColorAnimation
                        {
                            duration: 180
                        }
                    }

                    scale: copyMouseArea.pressed ? 0.96 : 1.0

                    Behavior on scale
                    {
                        NumberAnimation
                        {
                            duration: 100
                        }
                    }

                    Text
                    {
                        id: copyLabel
                        anchors.centerIn: parent
                        text: "Copy"
                        color: Theme.current.text
                        font.bold: true

                        Behavior on opacity
                        {
                            NumberAnimation
                            {
                                duration: 120
                            }
                        }
                    }

                    MouseArea
                    {
                        id: copyMouseArea

                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor

                        hoverEnabled: true

                        onClicked:
                        {
                            logger.action("clicked on copy to clipboard")
                            clipboardHelper.copy(root.value)

                            copyLabel.text = "Copied!"
                            copiedTimer.restart()
                        }
                    }
                }


                Timer
                {
                    id: copiedTimer
                    interval: 1500
                    repeat: false

                    onTriggered: copyLabel.text = "Copy"
                }
            }
        }



        Rectangle
        {
            width: (parent.width)/1.5
            height: 38

            radius: 8

            color: Theme.current.accent
            Layout.alignment: Qt.AlignHCenter

            Text
            {
                anchors.centerIn: parent
                text: "CLOSE"
                color: Theme.current.text
                font.bold: true
            }

            MouseArea
            {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor

                onClicked: root.close()
            }
        }


    }
}
