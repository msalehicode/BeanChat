### build on windows guide step by step.

1.install qt6.9.3+ and Development tool QtCreator 20.0

2.run qtcreator
if cant find qtcreator by shortcuts, to find qtCreator go to C:/Qt/Tools/QtCreator/bin/qtcreator.exe


3.then download git from git site https://git-scm.com/install/windows and install it
3.1 run git-bash console


4.Install third-party libraries has two way:

4.1 install libraries from vcpkg:
first install vcpkg:
```
git clone https://github.com/microsoft/vcpkg
cd vcpkg
bootstrap-vcpkg.bat
```

to install libraries:
```
vcpkg install opus:x64-windows
vcpkg install rnnoise:x64-windows
vcpkg install ffmpeg:x64-windows
//also qhotkey needed

cmake -B build -D CMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

after that can use in cmake like:
```
find_package(Opus CONFIG REQUIRED)
find_package(FFMPEG REQUIRED)
```



4.2 install and compile them manually

4.2.1 instlalation of QHotkey
cd to directory of project
> git clone https://github.com/Skycoder42/QHotkey.git

then change CMAKE of QHotkey defines to
```
add_subdirectory(QHotkey)

target_link_libraries(MyApp PRIVATE qhotkey)
```

it may dont recognize QHotkey despite we introduced it so inside file audioCapture.h we simple do replace
#include <QHotkey>
with
#include "QHotkey/QHotkey/qhotkey.h" //which is pathing from local directory QHotkey and on compile stage it would be static library baked in our app.exe


4.2.2: instllation of Opus (voice encoder/decoder)
> git clone https://github.com/xiph/opus.git
then cd to that opus and run these
```
mkdir build
cd build

cmake .. ^
-G "MinGW Makefiles" ^
-DCMAKE_BUILD_TYPE=Release ^
-DCMAKE_INSTALL_PREFIX=C:/Libraries/opus
```

then
```
mingw32-make
mingw32-make install
```
then in cmake do

```
list(APPEND CMAKE_PREFIX_PATH "C:/Libraries/opus")
find_package(Opus CONFIG REQUIRED)


target_link_libraries(appBeanChat PRIVATE Opus::opus)
``

4.2.3 installation of Rnnoise
> git clone https://github.com/xiph/rnnoise.git

needs run autogen.sh so its linux thing so need install an app to help us
donwload **msys2** from https://www.msys2.org/ for windows x64 and install it in default location which is C:/....
Open Start Menu and launch: MSYS2 MinGW 64-bit
(because we are using MinGW as compiler on Qt so depend on your compiler run that MSYS2 compiler name)
(or MSYS2 MinGW MINGW64 if that's what it's called).
please note Do not open the plain MSYS2 MSYS terminal.

insdie msys2 temrinal type these:
first update MSYS2:
> pacman -Syu

If it tells you to close the terminal, close it.
Then reopen MSYS2 MinGW 64-bit and run:
> pacman -Su

Install the build tools on msys2

> pacman -S git autoconf automake libtool make pkgconf cmake

now install g++ compiler here if don't would get error on running ./configure of rnnoise

> pacman -S mingw-w64-x86_64-gcc

also run this and it will show 13 items and asks install all of them? say install all
> pacman -S mingw-w64-x86_64-toolchain

then close it and make sure open msys2 mingw which can run gcc --version  or g++ command should be found.
to make sure it's installed inside msys2 run
> g++ --version
or
> gcc --version

time to get rnnoise from github and build it run blow commands one by one
```
git clone https://github.com/xiph/rnnoise.git
cd rnnoise
7. Build it
./autogen.sh
./configure
make
```

then where u are just do:
> ls .libs
should see:
```
librnnoise.a
librnnoise.dll.a
librnnoise.dll
```
ok we did compile it but wanna install it for Windows Libraries so don't run [MAKE INSTALL] here just follow blow commands
then run these:
```
make clean
./configure --prefix=/c/Libraries/rnnoise
make
make install
```

then in CMAKE project just add/replace:
```
target_include_directories(appBeanChat PRIVATE C:/Libraries/rnnoise/include)
target_link_libraries(appBeanChat PRIVATE C:/Libraries/rnnoise/lib/librnnoise.a)
```



4.2.4 installation of ffmpeg (video encoder/decoder)

first go to https://www.gyan.dev/ffmpeg/builds/ and download file: **ffmpeg-release-full-shared.zip** OR .7z

then unarchive it into this path: C:/Libraries/ffmpeg

therefore in cmake write:
```
target_include_directories(appBeanChat PRIVATE C:/Libraries/ffmpeg/include)

target_link_directories(appBeanChat PRIVATE C:/Libraries/ffmpeg/lib)

target_link_libraries(appBeanChat PRIVATE
    avcodec
    avformat
    avutil
    swresample
    swscale
)
```

5. get BeanChatServer from github
then we need to get BeanChatServer because some headers are using Server's headers in BeanChat Client:
go to project directory then run these:
```
cd ..
git clone git@github.com:msalehicode/BeanChatServer.git
```

6. build release for project
now hit Run CMake, should don't complaint about any dependency or..
now on configure select Release and hit Build.
you should get appBeanChat.exe and runs fine for yourself.

7. use windeployqt.exe to tell qt copies all those files are included or qt/qml framework needed for this app
after you have got Release from project do run this command (depend on your compiler choice, we are using MinGW64 so we are using mingw_64\bin\'s deployer)
> C:\Qt\6.9.3\mingw_64\bin\windeployqt.exe --release "C:\path\to\your\project\Desktop....-release\appBeanChat.exe"

then windeployqt will copy all of the required Qt DLLs and plugins.


8. copy third-party librarys manually (CAN SKIP THIS ONE IF YOU ARE USING that new CMAKE which has if(WIN32) and .. jump to step 9)
you still need to manually copy the non-Qt DLLs your app
e.g we use **rnnoise** and **ffmpeg** and **opus** and **QHotkey**
except Qhotkey (this one we did static link so dont need .dll copy)


8.1 copy RNNoise DLLs:
we need **librnnoise-0.dll** from **C:\Libraries\rnnoise\bin**

8.2 copy FFMPEG DDLs:
From: **C:\Libraries\ffmpeg\bin**
copy at least:
```
avcodec-*.dll
avformat-*.dll
avutil-*.dll
swresample-*.dll
swscale-*.dll
```

8.3 Opus (only if you built/shared it as a DLL)
**libopus-0.dll** (or whatever your build produced)

to verify it's really static or not
Run this command:
> dir C:\Libraries\opus\bin

If there is no bin directory or no **opus.dll / libopus-0.dll**, then **it's a static build**.
no need to copy, otherwise copy those .dll files



8.4 then run you appBeanChat.exe see what is missing,
> [!NOTE]
> if you're running on llvm-ming..
> if got **libc++.dll** and **libunwind.dll** not found
> copy them from your compiler e.g E:\Qt\Tools\llvm-mingw1706_64\bin
> Copy those DLLs into the same folder as appBeanChat.exe
>
> this may happen when u're compiling via llvm and your libraries are compiled by something like mingw.
> to avoid this make sure you are using one compiler for all of stages.
> if your app immediately closes after launch this can because of multiple compiler OR missing .dll OR you code crashes. need to check and test all of them
```

9. COPY whole directory Desktop....-release AND TEST ON ANOTHER WINDOWS which is empty/clean IT SHOULD RUN FINE.
