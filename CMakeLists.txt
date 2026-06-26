cmake_minimum_required(VERSION 3.16)

project(BeanChat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APP_CURRENT_VERSION "0.12.0")
set(APP_TitleName "Bean Chat")
set(APP_OrganizationName "orgBeanChat")
set(APP_ApplicationName "appBeanChat")

find_package(Qt6 REQUIRED COMPONENTS Core Quick Multimedia Network Sql)

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



# switch line down if your BeanChatServer is situated by this CMake file
target_include_directories(appBeanChat PRIVATE ${CMAKE_SOURCE_DIR}/../BeanChatServer/src )
# target_include_directories(appBeanChat PRIVATE ${CMAKE_SOURCE_DIR}/BeanChatServer/src )



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

# announce cmake variables to Cpp side
target_compile_definitions(appBeanChat PRIVATE
    APP_VERSION="${APP_CURRENT_VERSION}"
    APP_TITLE="${APP_TitleName}"
    APP_ORGANIZATION="${APP_OrganizationName}"
    APP_NAME="${APP_ApplicationName}"
)

# app default configs
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
    Qt6::Sql
)

include(GNUInstallDirs)
install(TARGETS appBeanChat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)



# to deploy
qt_generate_deploy_app_script(
    TARGET appBeanChat
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
