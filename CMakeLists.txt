cmake_minimum_required(VERSION 3.16)

project(BeanChat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Multimedia Network)

# hotkey
find_package(QHotkey REQUIRED)

# opus
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPUS REQUIRED opus)

# rnnoise
pkg_check_modules(RNNOISE REQUIRED IMPORTED_TARGET rnnoise)


# ffmpeg
pkg_check_modules(FFMPEG REQUIRED
    libavcodec
    libavformat
    libavutil
    libswscale
)




qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appBeanChat
    main.cpp
)

qt_add_resources(appBeanChat resources
    PREFIX "/"
    FILES
        soundeffects/message.wav
        soundeffects/user-join.wav
        soundeffects/user-left.wav
)

qt_add_qml_module(appBeanChat
    URI BeanChat
    VERSION 1.0
    QML_FILES
        Main.qml
        MainPage.qml
        SettingsPage.qml
        VideoChat.qml
        TextChat.qml
        PrivateChatPage.qml

    SOURCES
        # just in case
        setup.h

        # general
        videosink.h
        participant.h
        user.h user.cpp
        soundmanager.h soundmanager.cpp

        # capture
        audiocapture.h audiocapture.cpp
        cameracapture.h cameracapture.cpp
        audiospeaker.h audiospeaker.cpp
        monitorcapture.h monitorcapture.cpp

        # C++ models for qml
        channelmodel.h channelmodel.cpp
        chatmodel.h chatmodel.cpp
        participantmodel.h

        # C++ qml components
        myvideoitem.h myvideoitem.cpp

        # encode decode
        # opusencoder.h opusencoder.cpp
        # videoencoder.h videoencoder.cpp
        # ffmpegencoder.h ffmpegencoder.cpp

    # icons
    RESOURCES icons/headphone.png icons/headphone-red.png icons/microphone.png icons/microphone-red.png icons/videocall.png icons/videocall-red.png icons/voice.png icons/folder.png icons/chat.png icons/beanChatIcon.png


    # from Server. later move these into a library
    SOURCES
        ../BeanChatServer/src/network/packet.h ../BeanChatServer/src/network/packet.cpp
        ../BeanChatServer/src/network/packets.h
        ../BeanChatServer/src/network/packethelpers.h
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appBeanChat PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appBeanChat
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)



# Opus
target_include_directories(appBeanChat PRIVATE ${OPUS_INCLUDE_DIRS})
target_link_libraries(appBeanChat PRIVATE ${OPUS_LIBRARIES})

# ffmpeg
target_include_directories(appBeanChat PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(appBeanChat PRIVATE ${FFMPEG_LIBRARIES})

# rnnoise
target_link_libraries(appBeanChat PRIVATE PkgConfig::RNNOISE)

# hotkey
target_link_libraries(appBeanChat PRIVATE
    qhotkey
)

# debug tool to get sigfault infos.
target_compile_options(appBeanChat PRIVATE
    -fsanitize=address
    -g)
target_link_options(appBeanChat PRIVATE
    -fsanitize=address)


target_link_libraries(appBeanChat
    PRIVATE Qt6::Quick
    Qt6::Multimedia
    Qt6::Core
    Qt6::Network
)

include(GNUInstallDirs)
install(TARGETS appBeanChat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
