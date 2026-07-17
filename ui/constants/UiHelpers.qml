pragma Singleton

import QtQuick
import BeanChatClient 1.0

QtObject
{
    function statusColor(status)
    {
        switch (status)
        {
        case Presence.Online:
            return "lime"

        case Presence.Idle:
            return "orange"

        case Presence.Away:
            return "yellow"

        case Presence.Busy:
            return "red"

        case Presence.DoNotDisturb:
            return "#ff0066"

        case Presence.Offline:
        default:
            return "gray"
        }
    }

    function relationColor(relation)
    {
        switch(relation)
        {
            case Relationship.None: return "white"
            case Relationship.Blocked: return "#FF7070"
            case Relationship.Friend: return "lime"
        }
    }

    function isTalkingColor(isMuted=false,relation=Relationship.None)
    {
        if(isMuted)
            return "#FF0000"

        switch(relation)
        {
            case Relationship.Blocked: return "#FF7070"
            case Relationship.None: return "#acbbd9"
            case Relationship.Friend: return "lime"
        }
    }

    function statusText(status)
    {
        switch (status)
        {
        case Presence.Online:
            return "Online"

        case Presence.Idle:
            return "Idle"

        case Presence.Away:
            return "Away"

        case Presence.Busy:
            return "Busy"

        case Presence.DoNotDisturb:
            return "Do Not Disturb"

        case Presence.Offline:
        default:
            return "Offline"
        }
    }


    function uploadedFileIcon(fileName)
    {
        var ext = fileName.split(".").pop().toLowerCase()

        switch (ext)
        {
        // Images
        case "png":
        case "jpg":
        case "jpeg":
        case "bmp":
        case "webp":
        case "heic":
        case "heif":
            return "icons/file-image.png"

        // Animated images
        case "gif":
        case "apng":
            return "icons/file-gif.png"

        // Video
        case "mp4":
        case "mkv":
        case "avi":
        case "mov":
        case "webm":
        case "flv":
        case "wmv":
        case "m4v":
            return "icons/file-video.png"

        // Audio
        case "mp3":
        case "wav":
        case "ogg":
        case "flac":
        case "aac":
        case "m4a":
        case "opus":
            return "icons/file-audio.png"

            // Code
        case "cpp":
        case "c":
        case "h":
        case "hpp":
        case "qml":
        case "js":
        case "ts":
        case "py":
        case "java":
        case "cs":
        case "php":
        case "html":
        case "css":
        case "json":
        case "xml":
        case "sql":
        case "sh":
        case "cmake":
            return "icons/file-code.png"

        // Archives
        case "zip":
        case "rar":
        case "7z":
        case "tar":
        case "gz":
        case "xz":
            return "icons/file-archive.png"


        // Text
        case "txt":
        case "md":
        case "log":
        case "ini":
        case "conf":
            return "icons/file-text.png"

            // Documents
        case "pdf":
            // return "icons/file-pdf.png"

        case "doc":
        case "docx":
        case "odt":
            // return "icons/file-word.png"

        case "xls":
        case "xlsx":
        case "ods":
            // return "icons/file-excel.png"

        case "ppt":
        case "pptx":
        case "odp":
            // return "icons/file-powerpoint.png"

        default:
            return "icons/file-file.png"
        }
    }
}
