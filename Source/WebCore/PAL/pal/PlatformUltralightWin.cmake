list(APPEND PAL_SOURCES
    system/ClockGeneric.cpp

    system/win/SoundWin.cpp

    text/KillRing.cpp

    win/LoggingWin.cpp

    crypto/openssl/CryptoDigestOpenSSL.cpp
)

list(APPEND PAL_INCLUDE_DIRECTORIES
    "${CMAKE_BINARY_DIR}/../include/private"
)

set(PAL_OUTPUT_NAME PAL${DEBUG_SUFFIX})
