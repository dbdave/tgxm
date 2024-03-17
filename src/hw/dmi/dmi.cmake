if (WITH_DMI AND (TGXM_OS_WIN OR TGXM_OS_LINUX OR TGXM_OS_FREEBSD OR (TGXM_OS_MACOS AND NOT TGXM_ARM)))
    set(WITH_DMI ON)
else()
    set(WITH_DMI OFF)
endif()

if (WITH_DMI)
    add_definitions(/DTGXM_FEATURE_DMI)

    list(APPEND HEADERS
        src/hw/dmi/DmiBoard.h
        src/hw/dmi/DmiMemory.h
        src/hw/dmi/DmiReader.h
        src/hw/dmi/DmiTools.h
        )

    list(APPEND SOURCES
        src/hw/dmi/DmiBoard.cpp
        src/hw/dmi/DmiMemory.cpp
        src/hw/dmi/DmiReader.cpp
        src/hw/dmi/DmiTools.cpp
        )

    if (TGXM_OS_WIN)
        list(APPEND SOURCES src/hw/dmi/DmiReader_win.cpp)
    elseif(TGXM_OS_LINUX OR TGXM_OS_FREEBSD)
        list(APPEND SOURCES src/hw/dmi/DmiReader_unix.cpp)
    elseif(TGXM_OS_MACOS)
        list(APPEND SOURCES src/hw/dmi/DmiReader_mac.cpp)
    endif()
else()
    remove_definitions(/DTGXM_FEATURE_DMI)
endif()
