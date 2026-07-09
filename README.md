# BeanChat

A modern voice, video and text chat application built with **Qt 6.9 (QML/C++)**.

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

| Platform | Status | Stage | Version |
|-----------|--------|--------|--------|
| 🌐 Server | ✅ | Debug | 0.17.13 |
| 🪟 Windows Client | ✅ | Debug | v0.21.20 |
| 🐧 Linux Client | ? | ? | ? |
| 🤖 Android Client | 🚧 | 🚧 | ? |
| 🍎 macOS Client | 🚧 | 🚧 | ? |
| 📱 iOS Client | 🚧 | 🚧 | ? |

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
