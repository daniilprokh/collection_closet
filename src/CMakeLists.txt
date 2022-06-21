set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(MAIN_SOURCES
    main.cpp
)

set(HEADERS
    collection_closet.h
    format.h
    format_widget.h
    login_widget.h
    object.h
    object_widget.h
    operation_widget.h
    types.h
)

set(SOURCES
    collection_closet.cpp
    format.cpp
    format_widget.cpp
    login_widget.cpp
    object.cpp
    object_widget.cpp
    operation_widget.cpp
)

set(Qt5_DIR "D:/Soft/Programming/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake/Qt5")

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS Sql REQUIRED)
find_package(Qt5 COMPONENTS LinguistTools REQUIRED)

qt5_wrap_ui(UI_FILES
    collection_closet.ui
    login_widget.ui
)

qt5_add_translation(QM_TRANSLATION "resources/collection_closet.ts")

qt5_add_resources(QRC_WRAPPERS resource.qrc OPTIONS -no-compress)

add_executable (${PROJECT_NAME}
    ${MAIN_SOURCES} ${SOURCES} ${HEADERS}
    ${UI_FILES} ${QM_TRANSLATION} ${QRC_WRAPPERS}
)

target_link_libraries (${PROJECT_NAME} PUBLIC Qt5::Widgets Qt5::Sql)

install(FILES $<TARGET_FILE:Qt5::Widgets> DESTINATION $<TARGET_FILE_DIR:${PROJECT_NAME}>)
install(FILES $<TARGET_FILE:Qt5::Core> DESTINATION $<TARGET_FILE_DIR:${PROJECT_NAME}>)
install(FILES $<TARGET_FILE:Qt5::Gui> DESTINATION $<TARGET_FILE_DIR:${PROJECT_NAME}>)
install(FILES $<TARGET_FILE:Qt5::Sql> DESTINATION $<TARGET_FILE_DIR:${PROJECT_NAME}>)

if(WIN32)
    install(DIRECTORY "${Qt5_DIR}/../../../plugins/platforms" DESTINATION $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    install(DIRECTORY "${Qt5_DIR}/../../../plugins/sqldrivers" DESTINATION $<TARGET_FILE_DIR:${PROJECT_NAME}>)
endif()