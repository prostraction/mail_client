cmake_minimum_required(VERSION 3.14)

project(MailView)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

include_directories(C:/OpenSSL-Win32/include/)
link_directories(C:/OpenSSL-Win32/lib/VC/)

set(SOURCES
    account.cpp
    decoder.cpp
    directory.cpp
    internetconnection.cpp
    listview.cpp
    main.cpp
    mainwindow.cpp
    messagesend.cpp
    messageview.cpp
    mime_message.cpp
    mimemessage.cpp
    parser.cpp
    #treelistview.cpp
)

set(HEADERS
    account.h
    decoder.h
    directory.h
    internetconnection.h
    listview.h
    mainwindow.h
    messagesend.h
    messageview.h
    mime_message.h
    mimemessage.h
    parser.h
    #treelistview.h
)

set(FORMS
    autorisationrequest.ui
    mainwindow.ui
    messagesend.ui
    messageview.ui
)

qt_add_executable(MailView
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
)

# OpenSSL
#####################################################################################################################


  find_package(OpenSSL REQUIRED)
  target_link_libraries(main PRIVATE OpenSSL::SSL)
  target_link_libraries(main PRIVATE OpenSSL::Crypto)


#####################################################################################################################

add_definitions(-DQT_DEPRECATED_WARNINGS)
install(TARGETS MailView DESTINATION build)
