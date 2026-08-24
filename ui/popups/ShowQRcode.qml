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
    height: 420

    anchors.centerIn: Overlay.overlay

    // Parameters
    property string titleText: "QR CODE"
    property string descriptionText: "SCAN WITH YOUR PHONE"
    property string qrData;

    onClosed:
    {
        //reset values
        root.titleText= "QR CODE"
        root.descriptionText= "SCAN WITH YOUR PHONE"
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


        Item { Layout.fillHeight: true }

        Image
        {
            width: 220
            height: 220
            Layout.alignment: Qt.AlignHCenter
            sourceSize.width: width
            sourceSize.height: height

            fillMode: Image.PreserveAspectFit

            source: "image://qrcode/" + root.qrData
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

                onClicked:
                {
                    onClicked: root.close()
                }
            }
        }


    }
}
