cmake_minimum_required(VERSION 3.14)

project(MailView)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
#include_directories(C:/OpenSSL-Win32/include/)
#link_directories(C:/OpenSSL-Win32/lib/VC/)

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

    set(OPENSSL_ROOT_DIR "E:/projects/_c_cpp/mail_client/vcpkg/installed/x64-windows")
    set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
    set(OPENSSL_CRYPTO_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/crt/x64_amd64/ssleay32.lib")
    set(OPENSSL_SSL_LIBRARY "${OPENSSL_ROOT_DIR}/lib/VC/crt/x64_amd64/libeay32.lib")

  find_package(OpenSSL REQUIRED)
  #target_link_libraries(MailView PRIVATE OpenSSL::SSL)
  #target_link_libraries(MailView PRIVATE OpenSSL::Crypto)


#####################################################################################################################

target_link_libraries(MailView PRIVATE
    Qt6::Widgets
    Qt6::Core
    Qt6::Gui
    OpenSSL::SSL
    OpenSSL::Crypto
)


add_definitions(-DQT_DEPRECATED_WARNINGS)
install(TARGETS MailView DESTINATION build)
