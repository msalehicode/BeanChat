#pragma once
#include <QLoggingCategory>

//audio
Q_DECLARE_LOGGING_CATEGORY(_microphone) //audioCapture
Q_DECLARE_LOGGING_CATEGORY(_speaker) //audioSpeaker
Q_DECLARE_LOGGING_CATEGORY(_opus) //encode and decode opus
Q_DECLARE_LOGGING_CATEGORY(_voice) //send receive voice
Q_DECLARE_LOGGING_CATEGORY(_soundEffect) //soundManager

//save, load
Q_DECLARE_LOGGING_CATEGORY(_database) //for database and databaseMigrator
Q_DECLARE_LOGGING_CATEGORY(_settings) //settingsManager
Q_DECLARE_LOGGING_CATEGORY(_identity) //identityManager
Q_DECLARE_LOGGING_CATEGORY(_avatar) //save load avatars

//video
Q_DECLARE_LOGGING_CATEGORY(_camera) //cameraCapture
Q_DECLARE_LOGGING_CATEGORY(_video) //send receive video
Q_DECLARE_LOGGING_CATEGORY(_ffmpeg)  //encoder and decoder
Q_DECLARE_LOGGING_CATEGORY(_videoitem)//myVideoItem

//network
Q_DECLARE_LOGGING_CATEGORY(_tcp)
Q_DECLARE_LOGGING_CATEGORY(_udp)


Q_DECLARE_LOGGING_CATEGORY(_ui) //dedicated to QML for track user actions

Q_DECLARE_LOGGING_CATEGORY(_main) //for main.cpp
Q_DECLARE_LOGGING_CATEGORY(_app) //user.cpp
Q_DECLARE_LOGGING_CATEGORY(_updater) //check for update and latest response

Q_DECLARE_LOGGING_CATEGORY(_clientUser)
Q_DECLARE_LOGGING_CATEGORY(_models) //channel, ...
