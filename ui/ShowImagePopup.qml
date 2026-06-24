import QtQuick
import QtQuick.Controls

Popup
{
    id: imagePopup

    parent: Overlay.overlay

    x: 0
    y: 0

    width: Overlay.overlay.width
    height: Overlay.overlay.height

    modal: true

    padding: 0

    background: Rectangle
    {
        color: "#E0000000"
    }

    property string imageSource: ""

    Image
    {
        anchors.centerIn: parent

        width: parent.width * 0.95
        height: parent.height * 0.95

        source: imagePopup.imageSource

        fillMode: Image.PreserveAspectFit
    }

    MouseArea
    {
        anchors.fill: parent

        onClicked: imagePopup.close()
    }
}
