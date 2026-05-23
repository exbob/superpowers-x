include(FetchContent)

# Suppress deprecation warnings for cleaner third-party fetch output.
cmake_policy(SET CMP0135 NEW)
set(_OLD_CMAKE_WARN_DEPRECATED "${CMAKE_WARN_DEPRECATED}")
set(CMAKE_WARN_DEPRECATED OFF)

# ===== tomlc17 module =====
set(TOMLC17_VERSION "R260517")
set(TOMLC17_TARBALL_URL "https://github.com/cktan/tomlc17/archive/refs/tags/${TOMLC17_VERSION}.tar.gz")
set(TOMLC17_TARBALL_SHA256 "b8489dc181e606983307523f8bc3a188e7d59fe6b8eb7e85a406cfebe7f2ec92")
set(TOMLC17_FETCH_ROOT "${CMAKE_SOURCE_DIR}/thirdpart/tomlc17")

FetchContent_Declare(tomlc17_upstream
	URL "${TOMLC17_TARBALL_URL}"
	URL_HASH SHA256=${TOMLC17_TARBALL_SHA256}
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
	SOURCE_DIR "${TOMLC17_FETCH_ROOT}"
)

FetchContent_GetProperties(tomlc17_upstream)
if(NOT tomlc17_upstream_POPULATED)
	FetchContent_Populate(tomlc17_upstream)
endif()

set(TOMLC17_SOURCE_SUBDIR "${TOMLC17_FETCH_ROOT}/tomlc17-${TOMLC17_VERSION}")
if(NOT EXISTS "${TOMLC17_SOURCE_SUBDIR}/src/tomlc17.c" AND EXISTS "${TOMLC17_FETCH_ROOT}/src/tomlc17.c")
	set(TOMLC17_SOURCE_SUBDIR "${TOMLC17_FETCH_ROOT}")
endif()

if(NOT EXISTS "${TOMLC17_SOURCE_SUBDIR}/src/tomlc17.c")
	message(FATAL_ERROR "tomlc17 source not found at ${TOMLC17_SOURCE_SUBDIR} (clear thirdpart/tomlc17 and reconfigure)")
endif()

add_library(tomlc17 STATIC "${TOMLC17_SOURCE_SUBDIR}/src/tomlc17.c")
target_include_directories(tomlc17 PUBLIC "${TOMLC17_SOURCE_SUBDIR}/src")

# ===== zlog module =====
# set upstream version and integrity metadata.
set(ZLOG_VERSION "1.2.18")
set(ZLOG_TARBALL_URL "https://github.com/HardySimpson/zlog/archive/refs/tags/${ZLOG_VERSION}.tar.gz")
set(ZLOG_TARBALL_SHA256 "3977dc8ea0069139816ec4025b320d9a7fc2035398775ea91429e83cb0d1ce4e")
set(ZLOG_FETCH_ROOT "${CMAKE_SOURCE_DIR}/thirdpart/zlog")

# Declare source fetch without automatic add_subdirectory.
FetchContent_Declare(zlog_upstream
	URL "${ZLOG_TARBALL_URL}"
	URL_HASH SHA256=${ZLOG_TARBALL_SHA256}
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
	SOURCE_DIR "${ZLOG_FETCH_ROOT}"
)

# Download/extract only when not already populated.
FetchContent_GetProperties(zlog_upstream)
if(NOT zlog_upstream_POPULATED)
	FetchContent_Populate(zlog_upstream)
endif()

# Prefer GitHub archive top directory; fallback to SOURCE_DIR root if needed.
set(ZLOG_SOURCE_SUBDIR "${ZLOG_FETCH_ROOT}/zlog-${ZLOG_VERSION}")
if(NOT EXISTS "${ZLOG_SOURCE_SUBDIR}/CMakeLists.txt" AND EXISTS "${ZLOG_FETCH_ROOT}/CMakeLists.txt")
	set(ZLOG_SOURCE_SUBDIR "${ZLOG_FETCH_ROOT}")
endif()

# Fail fast when source layout is unexpected.
if(NOT EXISTS "${ZLOG_SOURCE_SUBDIR}/CMakeLists.txt")
	message(FATAL_ERROR "zlog source not found at ${ZLOG_SOURCE_SUBDIR} (clear thirdpart/zlog and reconfigure)")
endif()

# Build zlog as static and keep third-party target out of default all target.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory("${ZLOG_SOURCE_SUBDIR}" "${CMAKE_BINARY_DIR}/thirdpart/zlog-build" EXCLUDE_FROM_ALL)
