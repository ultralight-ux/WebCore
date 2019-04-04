#ifndef __UTF_ULTRALIGHT_H__
#define __UTF_ULTRALIGHT_H__

#ifdef __cplusplus
extern "C" {
#endif

// Convert UTF-16 to UTF-32. Call with NULL dest/destLen to get length to pre-allocate. Returns
// number of characters written. Will return 0 upon invalid conversion.
WEBCORE_EXPORT int32_t ConvertUTF16ToUTF32(char32_t* dest, int32_t destLen, const char16_t* src, int32_t srcLen);

// Convert UTF-32 to UTF-16. Call with NULL dest/destLen to get length to pre-allocate. Returns
// number of characters written. Will return 0 upon invalid conversion.
WEBCORE_EXPORT int32_t ConvertUTF32ToUTF16(char16_t* dest, int32_t destLen, const char32_t* src, int32_t srcLen);

// Convert UTF-16 to UTF-8. Call with NULL dest/destLen to get length to pre-allocate. Returns
// number of characters written. Will return 0 upon invalid conversion.
WEBCORE_EXPORT int32_t ConvertUTF16ToUTF8(char* dest, int32_t destLen, const char16_t* src, int32_t srcLen);

// Convert UTF-8 to UTF-16. Call with NULL dest/destLen to get length to pre-allocate. Returns
// number of characters written. Will return 0 upon invalid conversion.
WEBCORE_EXPORT int32_t ConvertUTF8ToUTF16(char16_t* dest, int32_t destLen, const char* src, int32_t srcLen);

#ifdef __cplusplus
}
#endif

#endif // __UTF_ULTRALIGHT_H__