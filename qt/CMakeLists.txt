cmake_minimum_required(VERSION 3.16)
set(APP_NAME $ENV{PROJECT_NAME})
project(${APP_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# activer les helpers Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

# générer compile_commands.json pour IntelliSense/clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Network)

file(GLOB_RECURSE PROJECT_SOURCES
	CONFIGURE_DEPENDS
	"src/*.cpp"
	"src/*/*.cpp"
)

set(PROJECT_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc")

add_executable(${APP_NAME} ${PROJECT_SOURCES} ${PROJECT_RESOURCES})
target_include_directories(${APP_NAME} PRIVATE src src/services src/widgets src/utils)
target_link_libraries(${APP_NAME} PRIVATE Qt6::Widgets Qt6::Sql Qt6::Network)