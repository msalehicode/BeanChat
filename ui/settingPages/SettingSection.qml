import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item
{
    id: root

    property string title: ""
    property string description: ""

    // Put any controls here
    default property alias content: contentColumn.data

    implicitWidth: parent ? parent.width : 700
    implicitHeight: layout.implicitHeight + 32

    ColumnLayout
    {
        id: layout

        anchors.fill: parent
        anchors.margins: 16

        spacing: 12

        ////////////////////////////////////////////////////////
        // Title
        ////////////////////////////////////////////////////////

        Label
        {
            Layout.fillWidth: true

            text: root.title

            color: "white"

            font.pixelSize: 18
            font.bold: true

            wrapMode: Text.WordWrap
        }

        ////////////////////////////////////////////////////////
        // Description
        ////////////////////////////////////////////////////////

        Label
        {
            Layout.fillWidth: true

            visible: description.length > 0

            text: root.description

            color: "#B5BAC1"

            font.pixelSize: 13

            wrapMode: Text.WordWrap
        }

        ////////////////////////////////////////////////////////
        // Controls
        ////////////////////////////////////////////////////////

        Column
        {
            id: contentColumn

            width: parent.width

            spacing: 12
        }

        ////////////////////////////////////////////////////////
        // Divider
        ////////////////////////////////////////////////////////

        Rectangle
        {
            Layout.fillWidth: true

            height: 1

            color: "#3F4147"

            opacity: 0.85
        }
    }
}
