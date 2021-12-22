WEBKIT_OPTION_BEGIN()
include(UltralightOptions)
WEBKIT_OPTION_END()

include(UltralightDefinitions)

set(ICU_LIBRARIES ${WEBKIT_LIBRARIES_DIR}/lib/libicuuc.a
                  ${WEBKIT_LIBRARIES_DIR}/lib/libicuin.a
                  ${WEBKIT_LIBRARIES_DIR}/lib/libicudt.a)
set(CMAKE_MACOSX_RPATH 1)
set(USE_CF 1)

set(CMAKE_CC_COMPILE_OPTIONS_IPO "-flto=full")
set(CMAKE_CXX_COMPILE_OPTIONS_IPO "-flto=full")

if (${BUILD_DBG})
  # Debug Config
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -DNDEBUG -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch x86_64 -g -DNDEBUG -fPIC -std=gnu++14 -stdlib=libc++ -Wno-trigraphs -fno-exceptions -Wno-missing-field-initializers -Wnon-virtual-dtor -Wno-overloaded-virtual -Wno-exit-time-destructors -Wno-missing-braces -Wparentheses -Wswitch -Wunused-function -Wno-unused-label -Wno-unused-parameter -Wunused-variable -Wunused-value -Wempty-body -Wuninitialized -Wno-unknown-pragmas -Wno-shadow -Wno-four-char-constants -Wno-conversion -Wconstant-conversion -Wint-conversion -Wbool-conversion -Wenum-conversion -Wno-float-conversion -Wnon-literal-null-conversion -Wobjc-literal-conversion -Wsign-compare -Wno-shorten-64-to-32 -Wno-c++11-extensions -isysroot ${CMAKE_SYSROOT} -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET} -Wdeprecated-declarations -Winvalid-offsetof -fvisibility=hidden -fno-threadsafe-statics -Wno-sign-conversion -Winfinite-recursion -Wmove -Wno-comma -Wblock-capture-autoreleasing -Wno-strict-prototypes -Wno-nonportable-include-path -frtti")
else ()
  # Release Configs
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Oz -DNDEBUG -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -arch x86_64 -march=core2 -mtune=haswell -Oz -DNDEBUG -fPIC -std=gnu++14 -stdlib=libc++ -Wno-trigraphs -fno-exceptions -Wno-missing-field-initializers -Wnon-virtual-dtor -Wno-overloaded-virtual -Wno-exit-time-destructors -Wno-missing-braces -Wparentheses -Wswitch -Wunused-function -Wno-unused-label -Wno-unused-parameter -Wunused-variable -Wunused-value -Wempty-body -Wuninitialized -Wno-unknown-pragmas -Wno-shadow -Wno-four-char-constants -Wno-conversion -Wconstant-conversion -Wint-conversion -Wbool-conversion -Wenum-conversion -Wno-float-conversion -Wnon-literal-null-conversion -Wobjc-literal-conversion -Wsign-compare -Wno-shorten-64-to-32 -Wno-c++11-extensions -isysroot ${CMAKE_SYSROOT} -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET} -Wdeprecated-declarations -Winvalid-offsetof -fvisibility=hidden -fno-threadsafe-statics -Wno-sign-conversion -Winfinite-recursion -Wmove -Wno-comma -Wblock-capture-autoreleasing -Wno-strict-prototypes -Wno-nonportable-include-path -frtti")
endif ()
