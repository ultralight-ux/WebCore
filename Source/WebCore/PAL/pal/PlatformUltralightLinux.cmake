list(APPEND PAL_PUBLIC_HEADERS
    #crypto/gcrypt/Handle.h
    #crypto/gcrypt/Initialization.h
    #crypto/gcrypt/Utilities.h

   # system/glib/SleepDisablerGLib.h
)

list(APPEND PAL_SOURCES
    #crypto/gcrypt/CryptoDigestGCrypt.cpp
    crypto/openssl/CryptoDigestOpenSSL.cpp

    system/ClockGeneric.cpp

   # system/glib/SleepDisablerGLib.cpp

   # system/gtk/SoundGtk.cpp

    text/KillRing.cpp

    unix/LoggingUnix.cpp
    
    system/Sound.cpp
)

list(APPEND PAL_SYSTEM_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})

list(APPEND PAL_LIBRARIES ${OPENSSL_CRYPTO_LIBRARY})
