# Build on Windows Guide (Step by Step)

> [!IMPORTANT]
> This guide assumes you are using **Qt 6.9.3+** with the **MinGW** compiler.

---

# 📑 Table of Contents

1. Install Qt
2. Run Qt Creator
3. Install Git
4. Install Third-Party Libraries

   * 4.1 Install and compile them manually

     * 4.1.1 QHotkey
     * 4.1.2 Opus
     * 4.1.3 RNNoise
     * 4.1.4 FFmpeg
   * 4.2 Install libraries from vcpkg
5. Get BeanChatServer
6. Build Release
7. Run `windeployqt`
8. Copy Third-Party DLLs
9. Test on another Windows

---

# 1. Install Qt 6.9.3+ and Development tool QtCreator 20.0

---

# 2. Run QtCreator

If can't find QtCreator by shortcuts, to find QtCreator go to:

```text
C:/Qt/Tools/QtCreator/bin/qtcreator.exe
```

---

# 3. Download Git

Download Git from:

https://git-scm.com/install/windows

and install it.

### 3.1 Run Git Bash console

---

# 4. Install third-party libraries

There are two ways:

* **4.1** Install and compile them manually.
* **4.2** Install them using **vcpkg**.

---

## 4.1 Install and compile them manually

---

### 4.1.1 Installation of QHotkey

Go to the project directory.

Clone QHotkey:

```bash
git clone https://github.com/Skycoder42/QHotkey.git
```

Then change **CMake** of QHotkey defines to:

```cmake
add_subdirectory(QHotkey)

target_link_libraries(MyApp PRIVATE qhotkey)
```

> [!NOTE]
> It may don't recognize QHotkey despite we introduced it, so inside file `audioCapture.h` we simple do replace:

Replace:

```cpp
#include <QHotkey>
```

with

```cpp
#include "QHotkey/QHotkey/qhotkey.h"
```

which is pathing from local directory `QHotkey` and on compile stage it would be static library baked in our `app.exe`.

---

### 4.1.2 Installation of Opus (voice encoder/decoder)

Clone Opus:

```bash
git clone https://github.com/xiph/opus.git
```

Then `cd` to that `opus` directory and run these:

```bash
mkdir build
cd build

cmake .. ^
-G "MinGW Makefiles" ^
-DCMAKE_BUILD_TYPE=Release ^
-DCMAKE_INSTALL_PREFIX=C:/Libraries/opus
```

Then run:

```bash
mingw32-make
mingw32-make install
```

Then in CMake do:

```cmake
list(APPEND CMAKE_PREFIX_PATH "C:/Libraries/opus")

find_package(Opus CONFIG REQUIRED)

target_link_libraries(appBeanChat PRIVATE Opus::opus)
```

---

### 4.1.3 Installation of RNNoise

Clone RNNoise:

```bash
git clone https://github.com/xiph/rnnoise.git
```

> [!IMPORTANT]
> Needs run `autogen.sh` so its linux thing, so need install an app to help us.

Download **MSYS2** from:

https://www.msys2.org/

for Windows x64 and install it in default location which is `C:/...`

Open Start Menu and launch:

```text
MSYS2 MinGW 64-bit
```

because we are using MinGW as compiler on Qt so depend on your compiler run that MSYS2 compiler name.

(or **MSYS2 MinGW MINGW64** if that's what it's called).

> [!WARNING]
> Do **not** open the plain **MSYS2 MSYS** terminal.

Inside MSYS2 terminal type these.

First update MSYS2:

```bash
pacman -Syu
```

> [!NOTE]
> If it tells you to close the terminal, close it.
>
> Then reopen **MSYS2 MinGW 64-bit** and run:

```bash
pacman -Su
```

Install the build tools on MSYS2:

```bash
pacman -S git autoconf automake libtool make pkgconf cmake
```

Now install `g++` compiler here, if don't would get error on running `./configure` of RNNoise.

```bash
pacman -S mingw-w64-x86_64-gcc
```

Also run this and it will show 13 items and asks install all of them, say install all:

```bash
pacman -S mingw-w64-x86_64-toolchain
```

Then close it and make sure open MSYS2 MinGW which can run `gcc --version` or `g++` command should be found.

To make sure it's installed inside MSYS2 run:

```bash
g++ --version
```

or

```bash
gcc --version
```

Time to get RNNoise from GitHub and build it.

Run below commands one by one:

```bash
git clone https://github.com/xiph/rnnoise.git
cd rnnoise
./autogen.sh
./configure
make
```

Then where u are just do:

```bash
ls .libs
```

Should see:

```text
librnnoise.a
librnnoise.dll.a
librnnoise.dll
```

Ok we did compile it but wanna install it for Windows Libraries, so don't run `make install` here.

Just follow below commands.

Run these:

```bash
make clean
./configure --prefix=/c/Libraries/rnnoise
make
make install
```

Then in CMake project just add/replace:

```cmake
target_include_directories(appBeanChat PRIVATE C:/Libraries/rnnoise/include)

target_link_libraries(appBeanChat PRIVATE
    C:/Libraries/rnnoise/lib/librnnoise.a
)
```

---

### 4.1.4 Installation of FFmpeg

First go to:

https://www.gyan.dev/ffmpeg/builds/

Download file:

**ffmpeg-release-full-shared.zip**

or

**ffmpeg-release-full-shared.7z**

Then unarchive it into this path:

```text
C:/Libraries/ffmpeg
```

Therefore in CMake write:

```cmake
target_include_directories(appBeanChat PRIVATE
    C:/Libraries/ffmpeg/include
)

target_link_directories(appBeanChat PRIVATE
    C:/Libraries/ffmpeg/lib
)

target_link_libraries(appBeanChat PRIVATE
    avcodec
    avformat
    avutil
    swresample
    swscale
)
```

---

## 4.2 Install libraries from vcpkg

First install **vcpkg**:

```bash
git clone https://github.com/microsoft/vcpkg
cd vcpkg
bootstrap-vcpkg.bat
```

To install libraries:

```bash
vcpkg install opus:x64-windows
vcpkg install rnnoise:x64-windows
vcpkg install ffmpeg:x64-windows

# also qhotkey needed

cmake -B build ^
-D CMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

After that can use in CMake like:

```cmake
find_package(Opus CONFIG REQUIRED)
find_package(FFMPEG REQUIRED)
```

---

# 5. Get BeanChatServer from GitHub

Then we need to get **BeanChatServer** because some headers are using Server's headers in BeanChat Client.

Go to project directory then run these:

```bash
cd ..
git clone git@github.com:msalehicode/BeanChatServer.git
```

> [!TIP]
> If you don't have Git SSH configured, you can clone using HTTPS instead:
>
> ```bash
> git clone https://github.com/msalehicode/BeanChatServer.git
> ```

---

# 6. Build Release for project

Now hit **Run CMake**, should don't complaint about any dependency or...

Then on configure select **Release** and hit **Build**.

You should get:

```text
appBeanChat.exe
```

and runs fine for yourself.

---

# 7. Use `windeployqt.exe`

Use `windeployqt.exe` to tell Qt copies all those files are included or Qt/QML framework needed for this app.

After you have got **Release** from project do run this command.

> [!NOTE]
> Depend on your compiler choice.
>
> We are using **MinGW64**, so we are using `mingw_64\bin`'s deployer.

```bash
C:\Qt\6.9.3\mingw_64\bin\windeployqt.exe --release "C:\path\to\your\project\Desktop....-release\appBeanChat.exe"
```

or

```bash
C:\Qt\6.9.3\mingw_64\bin\windeployqt6.exe --release --qmldir "C:\Users\vboxuser\BeanChat"  "C:\path\to\your\project\Desktop....-release\appBeanChat.exe"
```

Then `windeployqt` will copy all of the required Qt DLLs and plugins.

---

# 8. Copy third-party libraries manually

> [!TIP]
> **CAN SKIP THIS ONE IF YOU ARE USING that new CMAKE which has `if(WIN32)` and automatically copies libraries.**
>
> Jump to **Step 9**.

You still need to manually copy the non-Qt DLLs your app needs.

e.g we use:

* **RNNoise**
* **FFmpeg**
* **Opus**
* **QHotkey**

Except **QHotkey** (this one we did static link so dont need `.dll` copy).

---

## 8.1 Copy RNNoise DLLs

We need:

```text
librnnoise-0.dll
```

from:

```text
C:\Libraries\rnnoise\bin
```

---

## 8.2 Copy FFmpeg DLLs

From:

```text
C:\Libraries\ffmpeg\bin
```

Copy at least:

```text
avcodec-*.dll
avformat-*.dll
avutil-*.dll
swresample-*.dll
swscale-*.dll
```

---

## 8.3 Opus (only if you built/shared it as a DLL)

Copy:

```text
libopus-0.dll
```

(or whatever your build produced).

To verify it's really static or not, run this command:

```bat
dir C:\Libraries\opus\bin
```

> [!NOTE]
> If there is no `bin` directory or no `opus.dll` / `libopus-0.dll`, then it's a **static build**.
>
> No need to copy anything.
>
> Otherwise copy those `.dll` files.

---

## 8.4 Then run your `appBeanChat.exe` and see what is missing

> [!NOTE]
> If you're running on **llvm-mingw**...
>
> If got:
>
> * `libc++.dll`
> * `libunwind.dll`
>
> not found,
>
> copy them from your compiler, e.g.
>
> ```text
> E:\Qt\Tools\llvm-mingw1706_64\bin
> ```
>
> Copy those DLLs into the same folder as `appBeanChat.exe`.

> [!WARNING]
> This may happen when u're compiling via **llvm** and your libraries are compiled by something like **MinGW**.
>
> To avoid this make sure you are using **one compiler for all of stages**.

> [!IMPORTANT]
> If your app immediately closes after launch this can because of:
>
> * Multiple compiler
> * Missing `.dll`
> * Your code crashes
>
> Need to check and test all of them.

---

# 9. Copy whole Release directory and test

Copy the whole:

```text
Desktop....-release
```

directory to another Windows which is empty/clean.

> [!IMPORTANT]
> It should run fine.
