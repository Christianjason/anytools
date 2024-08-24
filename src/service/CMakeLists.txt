set(TARGET_NAME atk_service)
set(LIBS_DIR ../../libs)
add_executable(${TARGET_NAME} main.c
        service.h
        service.c
        cJSON.c
        api_infos.c
        api_networks.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(EVENT REQUIRED libevent)

# 添加libevent的头文件目录
target_include_directories(${TARGET_NAME} PRIVATE ${EVENT_INCLUDE_DIRS})

# 链接libevent库
target_link_libraries(${TARGET_NAME} PRIVATE ${EVENT_LIBRARIES})