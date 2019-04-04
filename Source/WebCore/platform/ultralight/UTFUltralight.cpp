#include "config.h"
#include "UTFUltralight.h"
#include <unicode/ustring.h>

int32_t ConvertUTF16ToUTF32(char32_t* dest, int32_t destLen, const char16_t* src, int32_t srcLen) {
  int32_t resultLen = 0;
  UErrorCode error = U_ZERO_ERROR;
  
  // Substitute invalid sequences with Unicode Replacement Character (U+FFFD)
  u_strToUTF32WithSub((UChar32*)dest, destLen, &resultLen, (const UChar*)src, srcLen, 0xFFFD, 0, &error);
  
  // Ignore Buffer Overflow Error if we are determining size (destLen == 0)
  return (U_SUCCESS(error) || (error == U_BUFFER_OVERFLOW_ERROR && !destLen)) ? resultLen : 0;
}

int32_t ConvertUTF32ToUTF16(char16_t* dest, int32_t destLen, const char32_t* src, int32_t srcLen) {
  int32_t resultLen = 0;
  UErrorCode error = U_ZERO_ERROR;

  // Substitute invalid sequences with Unicode Replacement Character (U+FFFD)
  u_strFromUTF32WithSub((UChar*)dest, destLen, &resultLen, (const UChar32*)src, srcLen, 0xFFFD, 0, &error);
  
  // Ignore Buffer Overflow Error if we are determining size (destLen == 0)
  return (U_SUCCESS(error) || (error == U_BUFFER_OVERFLOW_ERROR && !destLen)) ? resultLen : 0;
}

int32_t ConvertUTF16ToUTF8(char* dest, int32_t destLen, const char16_t* src, int32_t srcLen) {
  int32_t resultLen = 0;
  UErrorCode error = U_ZERO_ERROR;
  
  // Substitute invalid sequences with Unicode Replacement Character (U+FFFD)
  u_strToUTF8WithSub(dest, destLen, &resultLen, (const UChar*)src, srcLen, 0xFFFD, 0, &error);

  // Ignore Buffer Overflow Error if we are determining size (destLen == 0)
  return (U_SUCCESS(error) || (error == U_BUFFER_OVERFLOW_ERROR && !destLen)) ? resultLen : 0;
}

int32_t ConvertUTF8ToUTF16(char16_t* dest, int32_t destLen, const char* src, int32_t srcLen) {
  int32_t resultLen = 0;
  UErrorCode error = U_ZERO_ERROR;
  
  // Substitute invalid sequences with Unicode Replacement Character (U+FFFD)
  u_strFromUTF8WithSub((UChar*)dest, destLen, &resultLen, src, srcLen, 0xFFFD, 0, &error);

  // Ignore Buffer Overflow Error if we are determining size (destLen == 0)
  return (U_SUCCESS(error) || (error == U_BUFFER_OVERFLOW_ERROR && !destLen)) ? resultLen : 0;
}
