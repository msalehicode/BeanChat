import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Item {
    anchors.fill: parent
    Button
    {
        text:"back"
        onClicked: rootWindow.changePage()
    }
    Text
    {
        text:user.messages
        color:"white"
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Rectangle
    {
        width: parent.width/1.5
        height: implicitHeight
        // anchors.centerIn: parent
        anchors.horizontalCenter: parent.horizontalCenter
        color:"black"

        Column
        {
            spacing: 5
            Label
            {
                text:"microphones:"
                color:"white"
            }

            ComboBox {
                id: micPicker
                width: 200
                height: 100
                // This tells the combo box to use your list of names
                model: microphone.audioInputNames

                // Update the C++ backend when the selection changes
                onCurrentIndexChanged: {
                    microphone.currentAudioInput = currentIndex
                }

                // Optional: Keeps the UI in sync if the list changes (e.g., unplugged)
                Component.onCompleted: {
                    currentIndex = microphone.currentAudioInput
                }
            }

            Label
            {
                text:"noise:"
                color:"white"
            }
            CheckBox
            {
                checked: microphone.rnnoiseStatus
                onCheckedChanged: microphone.rnnoiseStatus = checked
            }

            Label
            {
                text:"volume gate:"
                color:"white"
            }
            CheckBox
            {
                checked: microphone.volumeGateStatus
                onCheckedChanged: microphone.volumeGateStatus = checked
            }

            Rectangle
            {
                id: meter
                visible: microphone.volumeGateStatus
                anchors.horizontalCenter: parent.horizontalCenter
                width: 300
                height: 20
                radius: 4
                color: "#303030"

                // Current mic level
                Rectangle
                {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter

                    width: parent.width * (microphone.currentVolume / 0.5)
                    height: parent.height

                    color: "lime"
                    radius: parent.radius
                }

                // Gate threshold marker
                Rectangle
                {
                    width: 4
                    height: parent.height + 8

                    x: parent.width *
                       (microphone.volumeGateThreshold / 0.5)
                       - width/2

                    y: -4

                    color: "red"
                    radius: 2
                }
                Slider
                {
                    width: meter.width

                    from: 0.0
                    to: 0.5

                    value: microphone.volumeGateThreshold

                    onMoved:
                        microphone.volumeGateThreshold = value
                }

            }


            Label
            {
                text:"speakers:"
                color:"white"
            }

            ComboBox {
                id: speakerPicker
                width: 200
                height: 100
                // This tells the combo box to use your list of names
                model: speaker.audioOutputNames

                // Update the C++ backend when the selection changes
                onCurrentIndexChanged: {
                    speaker.currentAudioOutput = currentIndex
                }

                // Optional: Keeps the UI in sync if the list changes (e.g., unplugged)
                Component.onCompleted: {
                    currentIndex = speaker.currentAudioOutput
                }
            }
            TextInput
            {
                id:usernameText
                text: "usr"
                color: "white"
                width: 60
                height: implicitHeight
            }

            Button
            {
                text:"create channel"
                onClicked: user.createChannel("dota2","123456")
            }

            Button
            {
                text:"send message"
                onClicked: user.sendMessage("hellooooo")
            }
            Button
            {
                text:"login"
                onClicked: user.login(usernameText.text,"ABC123")
            }
        }
    }
}
