cmake_minimum_required(VERSION 2.8)

project(zero-problem CXX)

set(Boost_USE_DEBUG_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS regex filesystem system)
find_package(wxWidgets REQUIRED COMPONENTS core base)

add_executable(zproblem
  analysis.cpp
  board.cpp
  coordinate.cpp
  game.cpp
  GenerateHtml.cpp
  gtp_lz.cpp
  gtp_lz_stub.cpp
  html.cpp
  illustration.cpp
  job.cpp
  job_list.cpp
  leela_zero.cpp
  log_file.cpp
  Main.cpp
  MainWindow.cpp
  MergeHtmlDialog.cpp
  move.cpp
  mutex.cpp
  range.cpp
  search_speed.cpp
  setting.cpp
  SettingDialog.cpp
  StringUtil.cpp
  TimeString.cpp
  variation.cpp
  variation_list.cpp
)

include(${wxWidgets_USE_FILE})
target_include_directories(zproblem
  PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${wxWidgets_INCLUDE_DIRS}
)
target_link_libraries(zproblem
  PRIVATE
    ${Boost_LIBRARIES}
    ${wxWidgets_LIBRARIES}
)
target_compile_options(zproblem PUBLIC -O2 -Wall)

install(TARGETS zproblem DESTINATION /usr/bin)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/Resources/Zero Problem.desktop" DESTINATION /usr/share/applications)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Resources/48x48/zproblem.png DESTINATION /usr/share/pixmaps)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Resources/32x32/zproblem.png DESTINATION /usr/share/icons/hicolor/32x32/apps)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Resources/48x48/zproblem.png DESTINATION /usr/share/icons/hicolor/48x48/apps)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Resources/64x64/zproblem.png DESTINATION /usr/share/icons/hicolor/64x64/apps)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Resources/128x128/zproblem.png DESTINATION /usr/share/icons/hicolor/128x128/apps)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Resources/zproblem.svg DESTINATION /usr/share/icons/hicolor/scalable/apps)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Licenses DESTINATION /usr/share/doc/zproblem)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Htmls/how_to_build.html DESTINATION /usr/share/doc/zproblem/Htmls)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Htmls/user_guide.html DESTINATION /usr/share/doc/zproblem/Htmls)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Htmls/user_guide_ja.html DESTINATION /usr/share/doc/zproblem/Htmls)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Locale/ja/zproblem.mo DESTINATION /usr/share/locale/ja/LC_MESSAGES)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_DESCRIPTION "Go problem generator")
set(CPACK_PACKAGE_CONTACT "Myself")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "9")
set(CPACK_PACKAGE_VERSION_PATCH "0")
include(CPack)
