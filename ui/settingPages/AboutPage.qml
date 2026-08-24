import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material

import "../constants/"

Page
{
    id: root

    Material.theme: Material.Dark
    Material.accent: Theme.current.accent

    background: Rectangle
    {
        color: Theme.current.background
    }


    Flickable
    {
        anchors.fill: parent

        clip: true

        contentWidth: width
        contentHeight: aboutColumn.implicitHeight + 40

        ScrollBar.vertical: ScrollBar {}


        Column
        {
            id: aboutColumn

            width: Math.min(parent.width - 80, 820)

            anchors.horizontalCenter: parent.horizontalCenter

            spacing: 24

            topPadding: 32
            bottomPadding: 40


            ///////////////////////////////////////////////////////
            // Header
            ///////////////////////////////////////////////////////

            Label
            {
                text: "About"

                color: Theme.current.text

                font.pixelSize: 28
                font.bold: true
            }


            Label
            {
                text: "Information about this application"

                color: Theme.current.textSecondary

                font.pixelSize: 14
            }



            ///////////////////////////////////////////////////////
            // App Card
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width
                height: 230

                radius: 12

                color: Theme.current.surface
                border.color: Theme.current.border


                Column
                {
                    anchors.centerIn: parent

                    width: parent.width

                    spacing: 8


                    Image
                    {
                        width: 80
                        height: 80

                        source: "../icons/beanChatIcon.png"

                        fillMode: Image.PreserveAspectFit

                        anchors.horizontalCenter: parent.horizontalCenter
                    }


                    Label
                    {
                        width: parent.width

                        text: "BeanChat"

                        color: Theme.current.text

                        font.pixelSize: 26

                        font.bold: true

                        horizontalAlignment: Text.AlignHCenter
                    }


                    Label
                    {
                        width: parent.width

                        text: "Version "+ user.myAppVersion + " - " + user.buildType

                        color: Theme.current.textSecondary

                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }


            ///////////////////////////////////////////////////////
            // Source Code
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: Theme.current.surface

                border.color: Theme.current.border


                implicitHeight: githubColumn.implicitHeight + 32


                Column
                {
                    id: githubColumn

                    anchors.fill: parent

                    anchors.margins: 16

                    spacing: 14


                    RowLayout
                    {
                        width: parent.width


                        Label
                        {
                            text: "Source Code"

                            color: Theme.current.text

                            font.pixelSize: 18

                            font.bold: true

                            Layout.fillWidth: true
                        }


                        Button
                        {
                            text: "Open Github"


                            onClicked:
                            {
                                Qt.openUrlExternally(
                                    "https://github.com/msalehicode/BeanChat/"
                                )
                            }
                        }
                    }


                    Label
                    {
                        width: parent.width

                        text:
                        "BeanChat is completely open source. " +
                        "You can inspect the code, suggest improvements or contribute."

                        color: Theme.current.textSecondary

                        wrapMode: Text.WordWrap
                    }
                }
            }


            ///////////////////////////////////////////////////////
            // Support BeanChat
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: Theme.current.surface

                border.color: Theme.current.border


                implicitHeight: supportColumn.implicitHeight + 32


                Column
                {
                    id: supportColumn

                    anchors.fill: parent

                    anchors.margins: 16

                    spacing: 14

                    RowLayout
                    {
                        width: parent.width


                        Label
                        {
                            text: "Support BeanChat"

                            color: Theme.current.text

                            font.pixelSize: 18

                            font.bold: true

                            Layout.fillWidth: true
                        }


                        Button
                        {
                            text: "🐞 Report Bug"

                            onClicked:
                            {
                                Qt.openUrlExternally("https://github.com/msalehicode/BeanChat/issues")
                            }
                        }
                    }

                    Label
                    {
                        width: parent.width

                        text:
                        "BeanChat is free and open source. " +
                        "Your support helps us continue improving the project, adding new features and keeping development active.\n\n" +
                        "We appreciate everyone who helps BeanChat grow — whether through donations, bug reports, testing, feedback or contributions. " +
                        "To recognize our amazing community members, we created a badge system that highlights their valuable support and contributions."

                        color: Theme.current.textSecondary

                        wrapMode: Text.WordWrap
                    }
                }
            }

            ///////////////////////////////////////////////////////
            // About
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: Theme.current.surface

                border.color: Theme.current.border


                implicitHeight: aboutTextColumn.implicitHeight + 32


                Column
                {
                    id: aboutTextColumn

                    anchors.fill: parent

                    anchors.margins: 16

                    spacing: 12


                    Label
                    {
                        text: "About"

                        color: Theme.current.text

                        font.pixelSize: 18

                        font.bold: true
                    }


                    Label
                    {
                        width: parent.width

                        textFormat: Text.RichText

                        text:
                        "<style>
                            b {
                                color: " + Theme.current.text + ";
                            }

                            .highlight {
                                color: " + Theme.current.accent + ";
                                font-weight: bold;
                            }

                            .green {
                                color: " + Theme.current.success + ";
                                font-weight: bold;
                            }
                        </style>" +

                        "Open source is not only about transparency — it is about <b>privacy</b>. " +
                        "The ability to <b>learn</b>, <b>customize</b> and build upon software is what makes technology truly powerful.<br><br>" +

                        "<span class='green'>This project is also personal to us.</span><br>" +
                        "As Iranian gamers, we have experienced firsthand how difficult it can be to find reliable and accessible software " +
                        "when services are limited by <b>sanctions, restrictions and regional limitations</b>. " +
                        "Sometimes, we simply want a great application that we can use freely, without constantly worrying about whether it will be available to us tomorrow.<br><br>" +

                        "That is one of the reasons we wanted to create BeanChat — " +
                        "a <span class='highlight'>free, open source and accessible</span> communication platform that people can use, understand and improve.<br><br>" +

                        "<b>Our hope is that BeanChat can make gaming and communication a little easier for everyone</b>, " +
                        "especially for gamers who need <b>low latency, good performance and a reliable way to stay connected</b>.<br><br>" +

                        "<span class='highlight'>We cannot wait to hear from you.</span><br>" +
                        "Your feedback, ideas and support help BeanChat grow and become better with every update. " +
                        "Whether you use BeanChat, report an issue, test a new feature, share an idea or contribute code, " +
                        "you are becoming part of this journey.<br><br>" +

                        "Contact us: <span class='highlight'>support@BeanChat.ir</span><br><br>"+

                        "<b>Thank you for being part of the BeanChat community.</b> ❤️"
                        color: Theme.current.textSecondary

                        font.pixelSize: 14

                        lineHeight: 1.4

                        wrapMode: Text.WordWrap
                    }
                }
            }


            ///////////////////////////////////////////////////////
            // Resources
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: Theme.current.surface

                border.color: Theme.current.border


                implicitHeight: resourcesColumn.implicitHeight + 32


                Column
                {
                    id: resourcesColumn

                    anchors.fill: parent

                    anchors.margins: 16

                    spacing: 14


                    Label
                    {
                        text: "Resources"

                        color: Theme.current.text

                        font.pixelSize: 18

                        font.bold: true
                    }


                    Label
                    {
                        width: parent.width

                        text: "Need help? Visit the documentation to learn how to install, configure and use BeanChat."

                        color: Theme.current.textSecondary

                        wrapMode: Text.WordWrap
                    }

                    RowLayout
                    {
                        width: parent.width


                        Button
                        {
                            text: "🌐 Website"

                            Layout.fillWidth: true


                            onClicked:
                            {
                                Qt.openUrlExternally("https://beanchat.ir")
                            }
                        }


                        Button
                        {
                            text: "📚 Documentation"

                            Layout.fillWidth: true


                            onClicked:
                            {
                                Qt.openUrlExternally("https://beanchat.ir/docs")
                            }
                        }
                    }


                    RowLayout
                    {
                        width: parent.width


                        Button
                        {
                            text: "🐞 Report Bug"

                            Layout.fillWidth: true


                            onClicked:
                            {
                                Qt.openUrlExternally("https://github.com/msalehicode/BeanChat/issues")
                            }
                        }


                        Button
                        {
                            text: "⭐ Github"

                            Layout.fillWidth: true


                            onClicked:
                            {
                                Qt.openUrlExternally(
                                    "https://github.com/msalehicode/BeanChat"
                                )
                            }
                        }
                    }


                }
            }


            ///////////////////////////////////////////////////////
            // Technology
            ///////////////////////////////////////////////////////

            Rectangle
            {
                width: parent.width

                radius: 10

                color: Theme.current.surface

                border.color: Theme.current.border


                implicitHeight: techColumn.implicitHeight + 32


                Column
                {
                    id: techColumn

                    anchors.fill: parent

                    anchors.margins: 16

                    spacing: 12


                    Label
                    {
                        text: "Built With"

                        color: Theme.current.text

                        font.pixelSize: 18

                        font.bold: true
                    }


                    Label
                    {
                        width: parent.width

                        text:
                        "• Qt 6.9\n" +
                        "• QML\n" +
                        "• C++ 17\n" +
                        "• QHotkey\n" +
                        "• Ffmpeg\n" +
                        "• Opus \n" +
                        "• Nayuki QR code\n" +
                        "• innosetup 6.7.3"

                        color: Theme.current.textSecondary
                    }
                }
            }

            ///////////////////////////////////////////////////////
            // Footer
            ///////////////////////////////////////////////////////

            Label
            {
                width: parent.width

                horizontalAlignment: Text.AlignHCenter

                color: Theme.current.textMuted

                font.pixelSize: 12

                text: "BeanChat • Open Source Chat Application"
            }


            Item
            {
                width: 1
                height: 24
            }
        }
    }
}
