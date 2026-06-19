import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Page {

    background: Rectangle
    {
        color:"black"
    }

    header: Label
    {
        text:"settings"
        color:"white"
    }

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
        anchors.right: parent.right
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
            Row
            {
                Label
                {
                    text:"effects volume:"
                    color:"white"
                }
                Slider
                {
                    width: meter.width
                    from: 0.0
                    to: 1.0
                    value: soundManager.volume
                    onMoved: soundManager.volume = value
                }
            }

            Row
            {
                // Label
                // {
                //     text:"note: when a button is set as push to talk hotkey, \nOS may don't send that key to other apps/windows. \nalso due to coding stuff some keys are forbidden for now cant be set."
                //     color:"white"
                // }

                Label
                {
                    text:"push to talk:"
                    color:"white"
                }
                CheckBox
                {
                    checked: microphone.pushToTalkStatus
                    onCheckedChanged:
                    {
                        microphone.pushToTalkStatus = checked
                        microphone.volumeGateStatus = !checked //when its push to talk doesnt make sense to do volume gate.
                    }
                }
            }

            Row
            {
                Label
                {
                    text:"push to talk key = " + microphone.pushToTalkKeyString
                    color:"white"
                }
                Button{
                    text: "change key"
                    onClicked: keyCapture.waitingForKey=true
                }
                Button
                {
                    text: "cancel"
                    onClicked: keyCapture.waitingForKey=false
                }

                Rectangle
                {
                    id: keyCapture

                    width: 200
                    height: 40

                    property bool waitingForKey: false

                    focus: waitingForKey
                    onFocusChanged:
                    {
                        if(!focus)
                            keyCapture.waitingForKey=false //cancel process
                    }

                    visible: waitingForKey


                    Text
                    {
                        anchors.centerIn: parent

                        text: "Press a key..."
                    }

                    MouseArea
                    {
                        anchors.fill: parent

                        onClicked:
                        {
                            keyCapture.waitingForKey = true
                            keyCapture.forceActiveFocus()
                        }
                    }

                    Keys.onPressed: function(event)
                    {
                        if (!keyCapture.waitingForKey)
                            return

                        let k = event.key

                        let allowed =
                            // A-Z
                            (k >= Qt.Key_A && k <= Qt.Key_Z)

                            // F1-F12
                            || (k >= Qt.Key_F1 && k <= Qt.Key_F12)

                            // 0-9
                            || (k >= Qt.Key_0 && k <= Qt.Key_9)

                            // Symbols you want
                            || k === Qt.Key_BracketLeft      // [
                            || k === Qt.Key_BracketRight     // ]
                            || k === Qt.Key_Backslash        // \
                            || k === Qt.Key_Semicolon        // ;
                            || k === Qt.Key_Comma            // ,
                            || k === Qt.Key_Period           // .
                            || k === Qt.Key_Slash            // /
                            || k === Qt.Key_QuoteLeft        // `
                            || k === Qt.Key_Equal            // =
                            || k === Qt.Key_Minus            // -

                            // Special keys
                            || k === Qt.Key_Tab
                            || k === Qt.Key_Return
                            || k === Qt.Key_Space

                            // Arrow keys
                            || k === Qt.Key_Left
                            || k === Qt.Key_Right
                            || k === Qt.Key_Up
                            || k === Qt.Key_Down

                            // Context menu key
                            || k === Qt.Key_Menu


                        if (!allowed)
                        {
                            console.log("Key not allowed:", k)
                            return
                        }

                        microphone.pushToTalkKey = k
                        microphone.pushToTalkModifiers = 0

                        microphone.initialPushToTalkHotkey()

                        keyCapture.waitingForKey = false
                        event.accepted = true
                    }
                }
            }

            Row
            {
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
            }


            Row
            {
                Label
                {
                    text:"noise:"
                    color:"white"
                }
                Column
                {
                    CheckBox
                    {
                        checked: microphone.rnnoiseStatus
                        onCheckedChanged: microphone.rnnoiseStatus = checked
                    }
                    Slider
                    {
                        width: meter.width
                        visible: microphone.rnnoiseStatus

                        from: 0.0
                        to: 1.0

                        value: microphone.rnnoiseValue

                        onMoved:
                            microphone.rnnoiseValue = value
                    }
                }


            }




            Row
            {
                Label
                {
                    text:"volume gate:"
                    color:"white"
                }
                Column
                {
                    CheckBox
                    {
                        checked: microphone.volumeGateStatus
                        onCheckedChanged:
                        {
                            microphone.volumeGateStatus = checked
                            microphone.pushToTalkStatus = !checked //when its volumeGate doesnt make sense to do pushToTalk
                        }
                    }
                    Rectangle
                    {
                        id: meter
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
                            visible: microphone.volumeGateStatus
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
                            visible: microphone.volumeGateStatus

                            from: 0.0
                            to: 0.5

                            value: microphone.volumeGateThreshold

                            onMoved:
                                microphone.volumeGateThreshold = value
                        }

                    }


                }
            }




            Row
            {
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

            }



            TextInput
            {
                id:usernameText
                text: "usr"
                color: "white"
                font.pixelSize: 20
                width: 60
                height: implicitHeight
                // onAccepted: user.s
            }

            Button
            {
                text:"create channel"
                onClicked: user.createChannel("dota2","123456")
            }
        }
    }
}
