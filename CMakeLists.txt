cmake_minimum_required(VERSION 3.16)

project(BeanChat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APP_CURRENT_VERSION "0.12.0")
set(APP_TitleName "Bean Chat")
set(APP_OrganizationName "orgBeanChat")
set(APP_ApplicationName "appBeanChat")

find_package(Qt6 REQUIRED COMPONENTS Core Quick Multimedia Network Sql)

# hotkey
if(UNIX AND NOT APPLE)
    find_package(QHotkey REQUIRED)
elseif(WIN32)
    add_subdirectory(QHotkey)
endif()

# opus
if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OPUS REQUIRED opus)
elseif(WIN32)
    list(APPEND CMAKE_PREFIX_PATH "C:/Libraries/opus")
    find_package(Opus CONFIG REQUIRED)
endif()


# rnnoise
if(UNIX AND NOT APPLE)
    pkg_check_modules(RNNOISE REQUIRED IMPORTED_TARGET rnnoise)
endif()


# ffmpeg
if(UNIX AND NOT APPLE)
    pkg_check_modules(FFMPEG REQUIRED
       libavcodec
       libavformat
       libavutil
       libswscale
    )
endif()



qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appBeanChat
    main.cpp
)

qt_add_resources(appBeanChat resources
    PREFIX "/"
    FILES
        ui/soundeffects/message.wav
        ui/soundeffects/user-join.wav
        ui/soundeffects/user-left.wav
        ui/soundeffects/message-back.wav
)

qt_add_qml_module(appBeanChat
    URI BeanChat
    VERSION 1.0
    QML_FILES
        ui/Main.qml
        ui/MainPage.qml
        ui/SettingsPage.qml
        ui/VideoChat.qml
        ui/TextChat.qml
        ui/PrivateChatPage.qml
        ui/ConnectedUsers.qml
        ui/ChannelList.qml
        ui/CurrentChannelParticipants.qml
        ui/MyServers.qml


        #pop up dialogs
        ui/ConnectPopup.qml
        ui/EnterChannelPasswordPopup.qml
        ui/ShowImagePopup.qml
        ui/ModifySavedServerPopup.qml
        ui/MyServersItemMenu.qml
        ui/CreateChannelPopup.qml
        ui/ServerMenu.qml
        ui/ModifyChannelPopup.qml

        # settingsPage pages/components
        ui/settingPages/SettingSection.qml
        ui/settingPages/SettingsSidebar.qml
        ui/settingPages/SettingSwitch.qml
        ui/settingPages/SettingCombo.qml
        ui/settingPages/SettingSlider.qml
        ui/settingPages/MicLevelMeter.qml
        ui/settingPages/HotkeyCapture.qml
        ui/settingPages/AudioInputPage.qml
        ui/settingPages/AdvancePage.qml
        ui/settingPages/CameraInputPage.qml
        ui/settingPages/AudioOutputPage.qml

    SOURCES
        # just in case
        setup.h

        # general
        videosink.h
        participant.h
        user.h user.cpp
        soundmanager.h soundmanager.cpp
        database.h database.cpp
        settingsmanager.h settingsmanager.cpp

        # capture
        audiocapture.h audiocapture.cpp
        cameracapture.h cameracapture.cpp
        audiospeaker.h audiospeaker.cpp
        monitorcapture.h monitorcapture.cpp

        # C++ models for qml
        channelmodel.h channelmodel.cpp
        chatmodel.h chatmodel.cpp
        participantmodel.h
        connectedusersmodel.h connectedusersmodel.cpp
        myserversmodel.h myserversmodel.cpp

        # C++ qml components
        myvideoitem.h myvideoitem.cpp

        # encode decode
        opuscodec.h opuscodec.cpp
        ffmpegencoder.h ffmpegencoder.cpp
        ffmpegdecoder.h ffmpegdecoder.cpp

    # icons
    RESOURCES
    ui/icons/beanChatIcon.png
    ui/icons/chat.png
    ui/icons/users.png
    ui/icons/folder.png
    ui/icons/lock.png
    ui/icons/send.png
    ui/icons/down.png
    ui/icons/voice.png
    ui/icons/settings.png
    ui/icons/arrow-right.png
    ui/icons/save.png
    ui/icons/endcall.png ui/icons/endcall-red.png
    ui/icons/camera.png ui/icons/camera-closed.png ui/icons/camera-red.png
    ui/icons/disconnect.png ui/icons/disconnect-red.png
    ui/icons/headphone.png ui/icons/headphone-closed.png ui/icons/headphone-red.png
    ui/icons/microphone.png ui/icons/microphone-closed.png ui/icons/microphone-red.png
    ui/icons/screen.png ui/icons/screen-closed.png ui/icons/screen-red.png
    ui/icons/signal.png ui/icons/signal-green.png ui/icons/signal-orange.png ui/icons/signal-red.png ui/icons/signal-yellow.png
    ui/icons/os/android.png ui/icons/os/ios.png ui/icons/os/linux.png ui/icons/os/macos.png ui/icons/os/windows.png

    # from Server. later move these into a library
    SOURCES
        ../BeanChatServer/src/network/packet.h
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



# switch line down if your BeanChatServer is situated by this CMake file
target_include_directories(appBeanChat PRIVATE ${CMAKE_SOURCE_DIR}/../BeanChatServer/src )
# target_include_directories(appBeanChat PRIVATE ${CMAKE_SOURCE_DIR}/BeanChatServer/src )



# Opus
if(UNIX AND NOT APPLE)
    target_include_directories(appBeanChat PRIVATE ${OPUS_INCLUDE_DIRS})
    target_link_libraries(appBeanChat PRIVATE ${OPUS_LIBRARIES})
elseif(WIN32)
    target_link_libraries(appBeanChat PRIVATE Opus::opus)
endif()


# ffmpeg
if(UNIX AND NOT APPLE)
    target_include_directories(appBeanChat PRIVATE ${FFMPEG_INCLUDE_DIRS})
    target_link_libraries(appBeanChat PRIVATE ${FFMPEG_LIBRARIES})
elseif(WIN32)
    target_include_directories(appBeanChat PRIVATE C:/Libraries/ffmpeg/include)
    target_link_directories(appBeanChat PRIVATE C:/Libraries/ffmpeg/lib)
    target_link_libraries(appBeanChat PRIVATE
        avcodec
        avformat
        avutil
        swresample
        swscale
    )
endif()

# rnnoise
if(UNIX AND NOT APPLE)
    target_link_libraries(appBeanChat PRIVATE PkgConfig::RNNOISE)
elseif(WIN32)
    target_include_directories(appBeanChat PRIVATE C:/Libraries/rnnoise/include)
    target_link_libraries(appBeanChat PRIVATE C:/Libraries/rnnoise/lib/librnnoise.a)
endif()

# hotkey
target_link_libraries(appBeanChat PRIVATE
    qhotkey
)

# announce cmake variables to Cpp side
target_compile_definitions(appBeanChat PRIVATE
    APP_VERSION="${APP_CURRENT_VERSION}"
    APP_TITLE="${APP_TitleName}"
    APP_ORGANIZATION="${APP_OrganizationName}"
    APP_NAME="${APP_ApplicationName}"
)


target_compile_definitions(appBeanChat PRIVATE

    #user configs
    USER_DEFAULT_SOUNDEFFECTS_VOLUME=1.0
    USER_DEFAULT_USERNAME="beanChatUser"
    USER_DEFAULT_IDENTITY="beanChatIdentity"
    USER_DEFAULT_SERVER_IP="localhost"
    USER_DEFAULT_SERVER_PORT=9987
    USER_DEFAULT_SERVER_PORT_STR="9987" #this also hardcoded in QML's add server popup
    USER_BUILD_TYPE_DEBUG="Beta"
    USER_BUILD_TYPE_RELEASE="Alpha"

    #timers
    CHANNEL_MODEL_TALKING_TIMER_INTERAVL=100 #check last time of user talk time then update their isTalking status
    CHANNEL_MODEL_TALKING_TIMEOUT=100 #If no voice packet has been received for more than 100 milliseconds, consider the user no longer talking
    UDP_CONNECTION_LOST_TIMER_INTERVAL=10000 #10s, when udp conneted would start this and on each udp request-ping received from server, reset connection timeout to later know is udp connection still alive or not

    #files
    DATABASE_FILENAME="/BeanChat.db"

    #debug prints
    D_PRINT_VIDEOSINK_INFO=false
    D_PRINT_CAMERA_INFO=false
    D_PRINT_MYVIDEOITEM_INFO=false
    D_PRINT_VOICE_INFO=false
    D_PRINT_VIDEO_INFO=false

    # opus
    OPUS_DEFAULT_SAMPLE_RATE=48000
    OPUS_DEFAULT_CHANNELS=1
    OPUS_DEFAULT_BITRATE=32000
    OPUS_DEFAULT_FRAMESIZE=960

    #speaker
    SPEAKER_DEFAULT_SAMPLE_RATE=48000
    SPEAKER_DEFAULT_CHANNEL=1

    # ffmpeg
    FFMPEG_DECODER_DEFAULT_FPS=30
    FFMPEG_DECODER_DEFAULT_BITRATE=500000
    FFMPEG_DECODER_DEFAULT_GOPSIZE=30



    # camera
    CAMERA_DEFAuLT_WIDTH=640
    CAMERA_DEFAULT_HEIGHT=480
    CAMERA_DEFAULT_FPS=15
    CAMERA_DEFAULT_BITRATE=50000 #bitrate -> 100000 / 8 = 12500 bytes/sec ≈ 12.5 KB/sec
    CAMERA_DEFAULT_KEYFRAME=15 #keyframe every 15 frames (1 second)

)



# app default configs for devices such as mic,camera,speaker
target_compile_definitions(appBeanChat PRIVATE

    #audio input default config
    MIC_DEFAULT_PTT_STATUS=false
    MIC_DEFAULT_PTT_HOTKEY=47 # slash key as default
    MIC_DEFAULT_VOLUMEGATE_STATUS=true
    MIC_DEFAULT_VOLUMEGATE_THRESHOLD=0.0
    MIC_DEFAULT_RNNOISE_STATUS=false
    MIC_DEFAULT_RNNOISE_VALUE=0.0
    MIC_DEFUALT_DEVICE_INDEX=0

    # qsettings group and keys , also these values are inside ui/settingPages/AudioInputPage.qml too
    MIC_SETTING_DEVICE="AudioInput/Device"
    MIC_SETTING_RNN_STATUS="AudioInput/NoiseSuppression"
    MIC_SETTING_RNN_VALUE="AudioInput/NoiseSuppressionValue"
    MIC_SETTING_VOLUMEGATE="AudioInput/VolumeGate"
    MIC_SETTING_VOLUMEGATE_THRESHOLD="AudioInput/VolumeGateThreshold"
    MIC_SETTING_PTT_STATUS="AudioInput/PushToTalk"
    MIC_SETTING_PTT_HOTKEY="AudioInput/PushToTalkHotkey"

    #audio output default config
    SPEAKER_DEFAULT_DEVICE_INDEX=0
    SPEAKER_DEFAULT_EFFECTS_VOLUME=1.0

    # qsettings group and keys , also these values are inside ui/settingPages/AudioOutputPage.qml too
    SPEAKER_SETTING_DEVICE="AudioOutput/Device"
    SPEAKER_SETTING_EFFECTS_VOLUME="AudioOutput/EffectsVolume"


    #camera input default config
    CAMERA_DEFAULT_DEVICE_INDEX=0

    # qsettings group and keys , also these values are inside ui/settingPages/CameraInputPage.qml too
    CAMERA_SETTING_DEVICE="CameraInput/Device"
)




# debug tool to get sigfault infos.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
target_compile_options(appBeanChat PRIVATE
   -fsanitize=address
   -g)
target_link_options(appBeanChat PRIVATE
   -fsanitize=address)
endif()


target_link_libraries(appBeanChat
    PRIVATE Qt6::Quick
    Qt6::Multimedia
    Qt6::Core
    Qt6::Network
    Qt6::Sql
)

include(GNUInstallDirs)
install(TARGETS appBeanChat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)



#copy thridparty libraries for release windows
if(WIN32)
    add_custom_command(TARGET appBeanChat POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/Libraries/ffmpeg/bin/avcodec-62.dll"
        "$<TARGET_FILE_DIR:appBeanChat>"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/Libraries/ffmpeg/bin/avformat-62.dll"
        "$<TARGET_FILE_DIR:appBeanChat>"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/Libraries/ffmpeg/bin/avutil-60.dll"
        "$<TARGET_FILE_DIR:appBeanChat>"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/Libraries/ffmpeg/bin/swresample-6.dll"
        "$<TARGET_FILE_DIR:appBeanChat>"


        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/Libraries/ffmpeg/bin/swscale-9.dll"
        "$<TARGET_FILE_DIR:appBeanChat>"


        #noise
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "C:/Libraries/rnnoise/bin/librnnoise-0.dll"
        "$<TARGET_FILE_DIR:appBeanChat>"
    )
endif()



# to deploy
qt_generate_deploy_app_script(
    TARGET appBeanChat
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
