#!/usr/bin/env python3
import glob
import re

f = open("CMakeLists.txt", "w", encoding = "utf-8")
s1 = r'''cmake_minimum_required(VERSION 2.8)

project(zero-problem CXX)

set(Boost_USE_DEBUG_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS regex filesystem system)
find_package(wxWidgets REQUIRED COMPONENTS core base)

add_executable(zproblem
'''
f.write(s1)
for cpp in glob.glob("*.cpp"):
	f.write("  " + cpp + "\n")
f.write(")\n")
s2 = r'''
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
'''
f.write(s2)

for html in glob.glob("Htmls/*.html"):
	name = html[6:]
	src = "${CMAKE_CURRENT_SOURCE_DIR}/Htmls/" + name
	dst = "/usr/share/doc/zproblem/Htmls"
	f.write("install(FILES " + src + " DESTINATION " + dst + ")\n")

for mo in glob.glob("Locale/*/zproblem.mo"):
	lang = mo[7:9]
	src = "${CMAKE_CURRENT_SOURCE_DIR}/Locale/" + lang + "/zproblem.mo"
	dst = "/usr/share/locale/" + lang + "/LC_MESSAGES"
	f.write("install(FILES " + src + " DESTINATION " + dst + ")\n")

v = open("version.txt", "r", encoding = "utf-8")
version = v.read()
v.close()
v_major = "0"
v_minor = "0"
v_patch = "0"
m = re.search(r"(\d+)\.(\d+)\.(\d+)", version)
if m is not None:
	v_major = m.group(1)
	v_minor = m.group(2)
	v_patch = m.group(3)

f.write('\nset(CPACK_GENERATOR "DEB")\n')
f.write('set(CPACK_PACKAGE_DESCRIPTION "Go problem generator")\n')
f.write('set(CPACK_PACKAGE_CONTACT "Myself")\n')
f.write('set(CPACK_PACKAGE_VERSION_MAJOR "' + v_major + '")\n')
f.write('set(CPACK_PACKAGE_VERSION_MINOR "' + v_minor + '")\n')
f.write('set(CPACK_PACKAGE_VERSION_PATCH "' + v_patch + '")\n')
f.write('include(CPack)\n')

f.close()
