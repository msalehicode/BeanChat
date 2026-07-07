cmake_minimum_required(VERSION 3.16)

project(BeanChat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(APP_CURRENT_VERSION "0.20.8")
set(APP_TitleName "BeanChat")
set(APP_OrganizationName "orgBeanChat")
set(APP_ApplicationName "appBeanChat")

find_package(Qt6 REQUIRED COMPONENTS Core Quick Multimedia Network Sql QuickDialogs2)

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

#introduce notification singltones to qml
set_source_files_properties(
    ui/managers/NotificationManager.qml
    ui/constants/NotificationTypes.qml
    ui/constants/UiHelpers.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
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


        # notification system (overlays, components, managers)
        ui/constants/NotificationTypes.qml
        ui/constants/UiHelpers.qml
        ui/managers/NotificationManager.qml
        ui/overlays/NotificationOverlay.qml
        ui/components/NotificationCard.qml


        #menu
        ui/MyServersItemMenu.qml
        ui/ServerMenu.qml

        #popups
        ui/popups/ConnectPopup.qml
        ui/popups/EnterChannelPasswordPopup.qml
        ui/popups/ShowImagePopup.qml
        ui/popups/ModifySavedServerPopup.qml
        ui/popups/CreateChannelPopup.qml
        ui/popups/ModifyChannelPopup.qml
        ui/popups/ModifyProfilePopup.qml
        ui/popups/UserProfilePopup.qml
        ui/popups/UserContextPopup.qml

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
        user.h user.cpp

        #managers
        managers/soundmanager.h managers/soundmanager.cpp
        managers/settingsmanager.h managers/settingsmanager.cpp
        managers/avatarmanager.h managers/avatarmanager.cpp
        managers/clientusermanager.h managers/clientusermanager.cpp
        managers/identitymanager.h managers/identitymanager.cpp

        #video
        video/videosink.h
        video/myvideoitem.h video/myvideoitem.cpp
        video/cameracapture.h video/cameracapture.cpp
        video/monitorcapture.h video/monitorcapture.cpp
        video/ffmpegencoder.h video/ffmpegencoder.cpp
        video/ffmpegdecoder.h video/ffmpegdecoder.cpp

        #audio
        audio/audiocapture.h audio/audiocapture.cpp
        audio/audiospeaker.h audio/audiospeaker.cpp
        audio/opuscodec.h audio/opuscodec.cpp
        audio/audiomixer.h audio/audiomixer.cpp

        #database
        database/database.h database/database.cpp

        # C++ models for qml
        models/channelmodel.h models/channelmodel.cpp
        models/chatmodel.h models/chatmodel.cpp
        models/participantmodel.h models/participantmodel.cpp
        models/connectedusersmodel.h models/connectedusersmodel.cpp
        models/myserversmodel.h models/myserversmodel.cpp
        models/clientuser.h models/clientuser.cpp
        models/identity.h models/identity.cpp


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
    ui/icons/web.png
    ui/icons/arrow-right.png
    ui/icons/save.png
    ui/icons/user-add-block.png ui/icons/user-is-blocked.png
    ui/icons/user-add-friend.png ui/icons/user-is-friend.png
    ui/icons/endcall.png ui/icons/endcall-red.png
    ui/icons/camera.png ui/icons/camera-closed.png ui/icons/camera-red.png
    ui/icons/disconnect.png ui/icons/disconnect-red.png
    ui/icons/headphone.png ui/icons/headphone-closed.png ui/icons/headphone-red.png
    ui/icons/microphone.png ui/icons/microphone-closed.png ui/icons/microphone-red.png
    ui/icons/screen.png ui/icons/screen-closed.png ui/icons/screen-red.png
    ui/icons/signal.png ui/icons/signal-green.png ui/icons/signal-orange.png ui/icons/signal-red.png ui/icons/signal-yellow.png
    ui/icons/os/android.png ui/icons/os/ios.png ui/icons/os/linux.png ui/icons/os/macos.png ui/icons/os/windows.png
    QML_FILES ui/popups/GetValuePopup.qml

)

add_subdirectory(
    ../BeanChatCommon
    BeanChatCommon
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

target_include_directories(
    BeanChatCommon
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)



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
    USER_DEFAULT_SOUNDEFFECTS_VOLUME=0.5
    USER_DEFAULT_SERVER_IP=""
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
    D_PRINT_AUDIO_INFO=false #audio in and out + mixer

    # opus
    OPUS_DEFAULT_SAMPLE_RATE=48000
    OPUS_DEFAULT_CHANNELS=1
    OPUS_DEFAULT_BITRATE=32000
    OPUS_DEFAULT_FRAMESIZE=960

    #speaker
    SPEAKER_DEFAULT_SAMPLE_RATE=48000
    SPEAKER_DEFAULT_CHANNEL=1

    SPEAKER_DEFAULT_CHANNEL_USERS_VOLUME=100 #default volume of each user in channel

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
    MIC_DEFAULT_PTT_HOTKEY=96 #GraveAccent (tilde) key as default
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
    SPEAKER_DEFAULT_EFFECTS_VOLUME=0.5

    # qsettings group and keys , also these values are inside ui/settingPages/AudioOutputPage.qml too
    SPEAKER_SETTING_DEVICE="AudioOutput/Device"
    SPEAKER_SETTING_EFFECTS_VOLUME="AudioOutput/EffectsVolume"


    #user info
    USER_SETTING_USERNAME="User/Username"
    USER_SETTING_LAST_IDENTITY_NAME="User/LastIdentity"

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
    Qt6::QuickDialogs2

    BeanChatCommon
)

include(GNUInstallDirs)
install(TARGETS appBeanChat
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)





# ==========================================================
# Windows deployment
# ==========================================================
if(WIN32)
    get_target_property(WINDEPLOYQT_EXECUTABLE Qt6::windeployqt IMPORTED_LOCATION)

    set(DEPLOY_DIR "${CMAKE_BINARY_DIR}/deploy/${APP_TitleName}-${APP_CURRENT_VERSION}-Windows")

    install(FILES
        $<TARGET_FILE:OpenSSL::Crypto>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    add_custom_command(TARGET appBeanChat POST_BUILD

        # recreate deployment directory
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${DEPLOY_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}"

        # copy executable
        COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:appBeanChat>"
            "${DEPLOY_DIR}"

        # deploy Qt
        COMMAND
            "${WINDEPLOYQT_EXECUTABLE}"
            --release
            --qmldir "${CMAKE_SOURCE_DIR}"
            "${DEPLOY_DIR}/$<TARGET_FILE_NAME:appBeanChat>"


        # ffmpeg
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Libraries/ffmpeg/bin/avcodec-62.dll"
            "${DEPLOY_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Libraries/ffmpeg/bin/avformat-62.dll"
            "${DEPLOY_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Libraries/ffmpeg/bin/avutil-60.dll"
            "${DEPLOY_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Libraries/ffmpeg/bin/swresample-6.dll"
            "${DEPLOY_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Libraries/ffmpeg/bin/swscale-9.dll"
            "${DEPLOY_DIR}"

        # rnnoise
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "C:/Libraries/rnnoise/bin/librnnoise-0.dll"
            "${DEPLOY_DIR}"

        COMMENT "Deploying BeanChat..."


        #crypto
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:OpenSSL::Crypto>
                "${DEPLOY_DIR}"
    )

endif()


# to deploy
qt_generate_deploy_app_script(
    TARGET appBeanChat
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
