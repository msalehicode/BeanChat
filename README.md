# BeanChat

<p align="center">
  <img src="icons/BeanChat.png" width="128" alt="BeanChat Logo">
</p>

<p align="center">
  <b>Plant a server. Grow a community.</b><br>
  Open-source voice, video and text chat for self-hosted communities.
</p>

---

<p align="center">
  <a href="https://beanchat.ir">
    <img src="https://img.shields.io/badge/Website-beanchat.ir-3b82f6?style=for-the-badge&logo=googlechrome&logoColor=white">
  </a>

  <a href="https://github.com/msalehicode/BeanChat/releases/latest">
    <img src="https://img.shields.io/github/v/tag/msalehicode/BeanChat?include_prereleases&style=for-the-badge&color=10B981">
  </a>

  <a href="https://github.com/msalehicode/BeanChat/blob/master/LICENSE">
    <img src="https://img.shields.io/github/license/msalehicode/BeanChat?style=for-the-badge">
  </a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-00599C?style=flat&logo=cplusplus&logoColor=white">
  <img src="https://img.shields.io/badge/Qt-6.9-41CD52?style=flat&logo=qt&logoColor=white">
  <img src="https://img.shields.io/badge/QML-Qt%20Quick-41CD52?style=flat">
 <br/><br/>
  <img src="https://img.shields.io/badge/Platform-Windows-0078D6?style=flat&logo=windows">
  <img src="https://img.shields.io/badge/Platform-Linux-FCC624?style=flat&logo=linux&logoColor=black">
  <img src="https://img.shields.io/badge/Platform-Android-3DDC84?style=flat&logo=android">
</p>

<p align="center">
  <img src="https://img.shields.io/github/stars/msalehicode/BeanChat?style=flat">
  <img src="https://img.shields.io/github/last-commit/msalehicode/BeanChat?style=flat">
  <img src="https://img.shields.io/github/issues/msalehicode/BeanChat?style=flat">
</p>

---


## 📥 Releases

<table>
<tr>
<td width="50%" valign="top">

### 📦 Latest Release

Download the latest version from GitHub.

**[⬇️ Download Latest](https://github.com/msalehicode/BeanChat/releases/latest)**

or browse

**[📚 All Releases](https://github.com/msalehicode/BeanChat/releases)**

</td>

<td width="50%" valign="top">

### 🖥️ Platforms

| Platform | Status | Stage | Version | Download |
|-----------|--------|--------|--------|--------|
| 🗄️ Server | ✅ | Beta | 0.23.28 | [⬇️](https://beanchat.ir/bc/#server) |
| 🪟 Windows Client | ✅ | Beta | v0.33.90 | [⬇️ beta](https://beanchat.ir/bc/#downloads) |
| 🤖 Android Client | - | - | - | - |
| 🐧 Linux Client | 🛠️ | Beta | v0.33.90 | Working on |
| 🍎 macOS Client | - | - | - | - |
| 📱 iOS Client | - | - | - | - |

</td>
</tr>
</table>


## 📑 Table of Contents

* [Preview](#-preview)
* [Features](#-features)
* [Building](#-building)

  * [Linux](#linux)
  * [Windows](BUILD_WINDOWS_GUIDE.md)
* [Dependencies](#-dependencies)
* [License](#-license)

---

## 🎥 Preview v0.17.4
https://github.com/user-attachments/assets/9dd1aafb-20a4-4aa3-b21d-816efbbe39e1





---

## ✨ Features

1. Voice chat using (**Opus** encoding/decoding).
2. Video chat using (**ffmpeg** encode/decode), Includes a local preview for the sender.
3. Text chat with clickable URLs. Image URLs (`.png`, `.jpg`, etc.) automatically display an inline preview while preserving the original link.
4. Create and join, modify and delete channels
5. **Password-protected** channels.
6. **Real-time user status indicators** (microphone, speaker, and camera), including a speaking indicator when a user is talking.
7. Audio features including **RNNoise** noise suppression, **Push-to-Talk**, and **Voice Activity Detection (Volume Gate)**.
8. **Packet loss handling for voice and video** and user's **ping**, plus a colored signal icon displaying connection quality.
9. Connected users are listed with showing each user's operating system icon.
10. Save servers to a **My Servers** list, with support for editing and removing saved servers.
11. **Configurable audio and video devices** (microphone, speaker, and camera), plus **adjustable sound effect volume.**
12. **Sound effects** for message sent/received and user join/leave events.
13. Participant model in center of screen for current channel with their information and video feed.
14. Users can **set avatar picture**.
15. **Move user** in/out of channel.
16. **Floating notification** system for erros/messages/... on top of screen.
17. **Servers can have their information** such as Name, Version, Website, Avatar and Uptime).
18. **User Profile**, can open User's profile by clicking on their name/avatar to see their data (statis,name,id,current channel with button join his channel).
19. **Mute/Unmute** users' voice locally, also can **adjust voice volume** of users by right click on their name in user's setting popup.
20. **identity** system for login into accounts. also can switch/rename/create/delete identities when user is disconnected from any server.
21. **Add Friend, Block User, Save Note for other users**
    - blocked user -> change username to red, hide their avatar, mute locally, blur their text messages (only can be shown by clock reveal per message.
    - added friend users -> change their username and isTalking to green.
    - enter note/details for eavh user, to recognize them later. it would shown on any server because it saved for that user's identity.
22. **share server via serverCode** (servers with ip:port), enter serverCode to connect to server.
23. **share server via QR code image** (ip:port or domain address) [THIS IS FOR FUTURE TO SHARE SERVER WITH PHONE]
24. **copy to clipboard** (for serverCode to copy code, and UserProfile to copy identity)
25. **check for new updates on start** if its available show on importantNotificationBar with UPDATE NOW button, when clicked on update now, try open updater then close self to let update. (**BeanChatUpdater** repository to checks for missing/different files then download changes and install them).
26. **logger** save important events / errors of application and only report those crashes ones to our sever to know bugs and fix them.
27. **show offline users of server** and list in connectedUsers
28. **upload/download file** in chat
    - show icon, name of uploaded/attached file in chat before sending and can remove that attached file
    - drag and drop file to chat works (but only one file by the time)
    - a simple player for video and audio files
    - show image and animated-image (gif) in chat with small size and when clicked open popup with larger size
    - show error if exsits before upload file (server may say no, e.g to large files)
29. **text channel type** with save and load messages
    - send file is possible
    - can keep them private with password
    - also can be temporary (won't save and load chats)
30. **global badges** for users (donators, testers, developer, ..).
31. **auto reconnect when connection lost to server** with like 200 attempts and whether user was on a voice channel would rejoin him.
32. user can **manually change update repository**, even can **disable check for updates** or **manually check for updates**.
33. **Real time switch theme** available themes are: Dark, Light, DarkBlue, Green

---

# 🛠 Building


> [!IMPORTANT]
> Install the required dependencies before building.

## Windows

> [!TIP]
> Building on **Windows**?
>
> See the complete **[Windows Build Guide](BUILD_WINDOWS_GUIDE.md)**.

## Linux

> [!TIP]
> Building on **Ubuntu**?
>
> See gist [How Deploy Qt/QML For Ubuntu](https://gist.github.com/msalehicode/c285afa2ce87536db6492b301e9f4f18) to understand how deploy for Ubuntu

---
### Instructions to install requirements on Linux:

#### 1. RNNoise

```bash
git clone https://github.com/xiph/rnnoise.git
cd rnnoise
./autogen.sh
./configure
make
sudo make install
```

#### 2. QHotkey

```bash
git clone https://github.com/Skycoder42/QHotkey.git
cd QHotkey
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
```

#### 3. Opus

```bash
sudo apt install libopus-dev
```

#### 4. FFmpeg

```bash
sudo apt install \
    ffmpeg \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libswscale-dev
```
#### 5. OpenSSL
```bash
sudo apt install libssl-dev
```

#### 6. Build BeanChat

build Release then run
```bash
cmake --install "path/to/build/Desktop_Qt_6_9_3-Release" --prefix "path/to/somewhere/deployed"
```

---

## 📦 Dependencies

* Qt 6.9+
* RNNoise
* QHotkey
* Opus
* FFmpeg
* OpenSSL (Crypto)

---

## 📄 License
This project uses Qt under the LGPL v3 license.


---
>[!TIP]
> Windows location:
>
>database,identities is located at:
> C:\Users\vboxuser\AppData\Roaming\orgBeanChat\appBeanChat
>
>avatars cache path is located at:
>C:\Users\vboxuser\AppData\Local\orgBeanChat\appBeanChat\cache\servers
>
>config(QSettings) is located at: WIN+R -> type: regedit. route to:
>HKEY_CURRENT_USER\Software\orgBeanChat\appBeanChat
