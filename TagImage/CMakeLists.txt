# CMakeList.txt: CMake-Projekt für "TagImage". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required (VERSION 3.8)
#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_EXTENSIONS OFF)

find_package(cpprestsdk REQUIRED)
find_package(exiv2 REQUIRED CONFIG NAMES exiv2)
find_path(BFGROUP_LYRA_INCLUDE_DIRS "lyra/arg.hpp")
find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")
include_directories(${LIBCONFIG++_INCLUDE_DIRS})
# Fügen Sie der ausführbaren Datei dieses Projekts eine Quelle hinzu.
add_executable (TagImage "TagImage.cpp" "TagImage.h" "XMPTagger.h" "XMPTagger.cpp" "ImaggaClient.h" "ImaggaClient.cpp" "multipart_parser.cpp" "multipart_parser.h")
target_compile_features(TagImage PRIVATE cxx_std_11) #exiv2 won't compile with c++17'
target_include_directories(TagImage PRIVATE ${BFGROUP_LYRA_INCLUDE_DIRS})
target_link_libraries(TagImage PRIVATE cpprestsdk::cpprest)
target_link_libraries(TagImage PRIVATE exiv2lib exiv2-xmp)
target_include_directories(TagImage PRIVATE ${SIMPLEINI_INCLUDE_DIRS})

# TODO: Fügen Sie bei Bedarf Tests hinzu, und installieren Sie Ziele.
file(COPY config.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR})