WEBKIT_OPTION_BEGIN()
include(UltralightOptions)
WEBKIT_OPTION_DEFAULT_PORT_VALUE(USE_SYSTEM_MALLOC PRIVATE OFF)
WEBKIT_OPTION_END()

include(UltralightDefinitions)

set(ICU_LIBRARIES ${WEBKIT_LIBRARIES_DIR}/lib/libicuin.a
                  ${WEBKIT_LIBRARIES_DIR}/lib/libicuuc.a
                  ${WEBKIT_LIBRARIES_DIR}/lib/libicudt.a)

set(USE_GLIB 1)

if (${BUILD_DBG})
  # Debug Config
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -DNDEBUG -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DNDEBUG -ffunction-sections -fdata-sections -fPIC -std=c++14 -Wno-trigraphs -fno-exceptions -Wno-missing-field-initializers -Wnon-virtual-dtor -Wno-overloaded-virtual -Wno-exit-time-destructors -Wno-missing-braces -Wparentheses  -Wswitch -Wunused-function -Wno-unused-label -Wno-unused-parameter -Wunused-variable -Wunused-value -Wempty-body -Wuninitialized -Wno-unknown-pragmas -Wno-shadow -Wno-four-char-constants -Wno-conversion -Wconstant-conversion -Wint-conversion -Wbool-conversion -Wenum-conversion -Wno-float-conversion -Wnon-literal-null-conversion -Wobjc-literal-conversion -Wsign-compare -Wno-shorten-64-to-32 -Wno-c++11-extensions -Wdeprecated-declarations -Winvalid-offsetof -fvisibility=hidden -fno-threadsafe-statics -Wno-sign-conversion -Winfinite-recursion -Wno-strict-prototypes -DU_SHOW_CPLUSPLUS_API=0")
else ()
  # Release Config
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Oz -DNDEBUG -fPIC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=core2 -mtune=haswell -Oz -DNDEBUG -ffunction-sections -fdata-sections -fPIC -std=c++14 -Wno-trigraphs -fno-exceptions -Wno-missing-field-initializers -Wnon-virtual-dtor -Wno-overloaded-virtual -Wno-exit-time-destructors -Wno-missing-braces -Wparentheses -Wswitch -Wunused-function -Wno-unused-label -Wno-unused-parameter -Wunused-variable -Wunused-value -Wempty-body -Wuninitialized -Wno-unknown-pragmas -Wno-shadow -Wno-four-char-constants -Wno-conversion -Wconstant-conversion -Wint-conversion -Wbool-conversion -Wenum-conversion -Wno-float-conversion -Wnon-literal-null-conversion -Wobjc-literal-conversion -Wsign-compare -Wno-shorten-64-to-32 -Wno-c++11-extensions -Wdeprecated-declarations -Winvalid-offsetof -fvisibility=hidden -fno-threadsafe-statics -Wno-sign-conversion -Winfinite-recursion -Wno-strict-prototypes -DU_SHOW_CPLUSPLUS_API=0")
endif ()

# Allow relocatable binaries on Linux, load from executable path
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath='$ORIGIN'")