# BeanChat

<p align="center">
  <img width="250" src="https://github.com/user-attachments/assets/5be196ab-995b-496f-a434-e900b4e187fb">
</p>

A modern voice, video and text chat application built with **Qt 6.9 (QML/C++)**.

---

## 📑 Table of Contents

* [Preview](#-preview)
* [Features](#-features)
* [Building](#-building)

  * [Linux](#linux)
  * [Windows](BUILD_WINDOWS_GUIDE.md)
* [Dependencies](#-dependencies)
* [License](#license)

---

## 🎥 Preview

https://github.com/user-attachments/assets/e15b8c5a-4412-4311-97cc-68b4200d40c1

---

## ✨ Features

* Voice chat using **Opus** encoding/decoding.
* Video chat using JPEG frame transmission (currently no video codec compression). Includes a local preview for the sender.
* Text chat with clickable URLs. Image URLs (`.png`, `.jpg`, etc.) automatically display an inline preview while preserving the original link.
* Create and join channels.
* Password-protected channels.
* Real-time user status indicators (microphone, speaker, and camera), including a speaking indicator when a user is talking.
* Audio features including **RNNoise** noise suppression, **Push-to-Talk**, and **Voice Activity Detection (Volume Gate)**.
* Packet loss handling for voice and video, plus a colored signal icon displaying connection quality and ping.
* Connected users list showing each user's operating system icon.
* Save servers to a **My Servers** list, with support for editing and removing saved servers.
* Configurable audio and video devices (microphone, speaker, and camera), plus adjustable sound effect volume.
* Sound effects for message sent/received and user join/leave events.

---

# 🛠 Building

> [!TIP]
> Building on **Windows**?
>
> See the complete **[Windows Build Guide](BUILD_WINDOWS_GUIDE.md)**.

## Linux

> [!IMPORTANT]
> Install the required dependencies before building.

### 1. RNNoise

```bash
git clone https://github.com/xiph/rnnoise.git
cd rnnoise
./autogen.sh
./configure
make
sudo make install
```

### 2. QHotkey

```bash
git clone https://github.com/Skycoder42/QHotkey.git
cd QHotkey
mkdir build
cd build
cmake ..
make -j$(nproc)
sudo make install
```

### 3. Opus

```bash
sudo apt install libopus-dev
```

### 4. FFmpeg

```bash
sudo apt install \
    ffmpeg \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libswscale-dev
```

### 5. Build BeanChat

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

---

## 📦 Dependencies

* Qt 6.9+
* RNNoise
* QHotkey
* Opus
* FFmpeg

---

## 📄 License
## License
This project uses Qt under the LGPL v3 license.
