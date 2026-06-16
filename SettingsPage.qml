import QtQuick
import QtQuick.Controls

Item {
    anchors.fill: parent
    Button
    {
        text:"back"
        onClicked: rootWindow.changePage()
    }
}
