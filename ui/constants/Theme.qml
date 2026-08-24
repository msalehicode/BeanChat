pragma Singleton

import QtQuick
import "../themes"

QtObject
{
    id: root

    property string currentTheme: "dark"

    property QtObject current: dark

    // Store the theme objects
    property QtObject dark: DarkTheme {}
    property QtObject darkblue: DarkblueTheme {}
    property QtObject green: GreenTheme {}
    property QtObject light: LightTheme {}

    function setTheme(name)
    {
        switch (name)
        {
        case "dark":
            current = dark
            currentTheme = "dark"
            break

        case "darkblue":
            current = darkblue
            currentTheme = "darkblue"
            break

        case "green":
            current = green
            currentTheme = "green"
            break

        case "light":
            current = light
            currentTheme = "light"
            break

        default:
            current = dark
            currentTheme = "dark"
            break
        }
        logger.action("switch theme to="+currentTheme)
        user.currentTheme=currentTheme
    }
    Component.onCompleted:
    {
        logger.action("on settings saved theme="+user.currentTheme)
        setTheme(user.currentTheme)
    }
}
