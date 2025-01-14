if (WIN32)
    set(TGXM_OS_WIN ON)
elseif (APPLE)
    set(TGXM_OS_APPLE ON)

    if (IOS OR CMAKE_SYSTEM_NAME STREQUAL iOS)
        set(TGXM_OS_IOS ON)
    else()
        set(TGXM_OS_MACOS ON)
    endif()
else()
    set(TGXM_OS_UNIX ON)

    if (ANDROID OR CMAKE_SYSTEM_NAME MATCHES "Android")
        set(TGXM_OS_ANDROID ON)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(TGXM_OS_LINUX ON)
    elseif(CMAKE_SYSTEM_NAME STREQUAL FreeBSD OR CMAKE_SYSTEM_NAME STREQUAL DragonFly)
        set(TGXM_OS_FREEBSD ON)
    endif()
endif()


if (TGXM_OS_WIN)
    add_definitions(-DWIN32 -DTGXM_OS_WIN)
elseif(TGXM_OS_APPLE)
    add_definitions(-DTGXM_OS_APPLE)

    if (TGXM_OS_IOS)
        add_definitions(-DTGXM_OS_IOS)
    else()
        add_definitions(-DTGXM_OS_MACOS)
    endif()

    if (TGXM_ARM)
        set(WITH_SECURE_JIT ON)
    endif()
elseif(TGXM_OS_UNIX)
    add_definitions(-DTGXM_OS_UNIX)

    if (TGXM_OS_ANDROID)
        add_definitions(-DTGXM_OS_ANDROID)
    elseif (TGXM_OS_LINUX)
        add_definitions(-DTGXM_OS_LINUX)
    elseif (TGXM_OS_FREEBSD)
        add_definitions(-DTGXM_OS_FREEBSD)
    endif()
endif()

if (WITH_SECURE_JIT)
    add_definitions(-DTGXM_SECURE_JIT)
endif()
