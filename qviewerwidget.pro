QT += core
QT += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += opengl

TARGET = qviewerwidget
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++17

DESTDIR = ../../bin

win32 {

    OSG_LIB_DIRECTORY = $$(OSG)/lib
    OSG_INCLUDE_DIRECTORY = $$(OSG)/include
    message($${OSG_LIB_DIRECTORY})
    message($${OSG_INCLUDE_DIRECTORY})

    CONFIG(debug, debug|release) {

        TARGET = $$join(TARGET,,,_d)

        LIBS += -L$$OSG_LIB_DIRECTORY -losgd
        LIBS += -L$$OSG_LIB_DIRECTORY -losgViewerd
        LIBS += -L$$OSG_LIB_DIRECTORY -losgDBd
        LIBS += -L$$OSG_LIB_DIRECTORY -lOpenThreadsd
        LIBS += -L$$OSG_LIB_DIRECTORY -losgGAd
        LIBS += -L$$OSG_LIB_DIRECTORY -losgQt5d

    } else {

        LIBS += -L$$OSG_LIB_DIRECTORY -losg
        LIBS += -L$$OSG_LIB_DIRECTORY -losgViewer
        LIBS += -L$$OSG_LIB_DIRECTORY -losgDB
        LIBS += -L$$OSG_LIB_DIRECTORY -lOpenThreads
        LIBS += -L$$OSG_LIB_DIRECTORY -losgGA
        LIBS += -L$$OSG_LIB_DIRECTORY -losgQt5
    }

    INCLUDEPATH += $$OSG_INCLUDE_DIRECTORY
}

unix {

    CONFIG(debug, debug|release) {

        TARGET = $$join(TARGET,,,_d)

        LIBS += -losgd
        LIBS += -losgViewerd
        LIBS += -losgDBd
        LIBS += -lOpenThreadsd
        LIBS += -losgGAd
        LIBS += -losgQt5d

    } else {

        LIBS +=  -losg
        LIBS +=  -losgViewer
        LIBS +=  -losgDB
        LIBS +=  -lOpenThreads
        LIBS +=  -losgGA
        LIBS +=  -losgQt5
    }
}

# RTDE
INCLUDEPATH += $$(RTDE_ROOT)
LIBS += -L$$(RTDE_ROOT) -lrtde
message($$(RTDE_ROOT))

# BOOST
INCLUDEPATH += $$(BOOST_ROOT)
BOOST_LIB_DIRECTORY = $$(BOOST_ROOT)/lib64-msvc-14.1
message($$(BOOST_ROOT))
message($${BOOST_LIB_DIRECTORY})
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_atomic-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_math_tr1f-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_bzip2-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_chrono-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_container-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_context-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_contract-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_coroutine-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_date_time-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_exception-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_fiber-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_filesystem-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_graph-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_iostreams-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_locale-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_log-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_log_setup-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_math_c99-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_math_c99f-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_math_c99l-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_math_tr1-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_math_tr1f-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_math_tr1l-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_prg_exec_monitor-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_program_options-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_python37-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_random-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_regex-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_serialization-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_stacktrace_noop-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_stacktrace_windbg-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_stacktrace_windbg_cached-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_system-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_test_exec_monitor-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_thread-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_timer-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_type_erasure-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_unit_test_framework-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_wave-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_wserialization-vc141-mt-x64-1_71
LIBS += -L$${BOOST_LIB_DIRECTORY} -llibboost_zlib-vc141-mt-x64-1_71

# 3DCamera System
LIBS += -L$$PWD/lib -lVisionBooster
message($$PWD/lib)

INCLUDEPATH += $$PWD/include
message($${INCLUDEPATH})
SOURCES += $$files(./src/*.cpp)

HEADERS += $$files(./include/*.h)

FORMS += $$files(./forms/*.ui) \
    forms/settingsdialog.ui
