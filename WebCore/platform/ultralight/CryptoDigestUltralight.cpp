/*
* Copyright (C) 2016 Apple Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"
#include "CryptoDigest.h"

#if OS(WINDOWS)
#include <wincrypt.h>

namespace WebCore {

  struct CryptoDigestContext {
    CryptoDigest::Algorithm algorithm;
    HCRYPTPROV hContext{ 0 };
    HCRYPTHASH hHash{ 0 };
  };

  CryptoDigest::CryptoDigest()
    : m_context(new CryptoDigestContext)
  {
  }

  CryptoDigest::~CryptoDigest()
  {
    if (HCRYPTHASH hHash = m_context->hHash)
      CryptDestroyHash(hHash);
    if (HCRYPTPROV hContext = m_context->hContext)
      CryptReleaseContext(hContext, 0);
  }

  std::unique_ptr<CryptoDigest> CryptoDigest::create(Algorithm algorithm)
  {
    std::unique_ptr<CryptoDigest> digest(new CryptoDigest);
    digest->m_context->algorithm = algorithm;
    if (!CryptAcquireContext(&digest->m_context->hContext, nullptr, nullptr , PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
      return nullptr;
    bool succeeded = false;
    switch (algorithm) {
    case CryptoDigest::Algorithm::SHA_1: {
      succeeded = CryptCreateHash(digest->m_context->hContext, CALG_SHA1, 0, 0, &digest->m_context->hHash);
      break;
    }
    case CryptoDigest::Algorithm::SHA_256: {
      succeeded = CryptCreateHash(digest->m_context->hContext, CALG_SHA_256, 0, 0, &digest->m_context->hHash);
      break;
    }
    case CryptoDigest::Algorithm::SHA_384: {
      succeeded = CryptCreateHash(digest->m_context->hContext, CALG_SHA_384, 0, 0, &digest->m_context->hHash);
      break;
    }
    case CryptoDigest::Algorithm::SHA_512: {
      succeeded = CryptCreateHash(digest->m_context->hContext, CALG_SHA_512, 0, 0, &digest->m_context->hHash);
      break;
    }
    }
    if (succeeded)
      return digest;
    return nullptr;
  }

  void CryptoDigest::addBytes(const void* input, size_t length)
  {
    if (!input || !length)
      return;
    RELEASE_ASSERT(CryptHashData(m_context->hHash, reinterpret_cast<const BYTE*>(input), length, 0));
  }

  Vector<uint8_t> CryptoDigest::computeHash()
  {
    Vector<uint8_t> result;
    DWORD digestLengthBuffer;
    DWORD digestLengthBufferSize = sizeof(digestLengthBuffer);

    RELEASE_ASSERT(CryptGetHashParam(m_context->hHash, HP_HASHSIZE, reinterpret_cast<BYTE*>(&digestLengthBuffer), &digestLengthBufferSize, 0));
    result.resize(digestLengthBuffer);

    RELEASE_ASSERT(CryptGetHashParam(m_context->hHash, HP_HASHVAL, result.data(), &digestLengthBuffer, 0));
    RELEASE_ASSERT(result.size() == digestLengthBuffer);
    return result;
  }

} // namespace WebCore
#elif OS(DARWIN)
#include <CommonCrypto/CommonCrypto.h>

namespace WebCore {
    
    struct CryptoDigestContext {
        CryptoDigest::Algorithm algorithm;
        void* ccContext;
    };
    
    inline CC_SHA1_CTX* toSHA1Context(CryptoDigestContext* context)
    {
        ASSERT(context->algorithm == CryptoDigest::Algorithm::SHA_1);
        return static_cast<CC_SHA1_CTX*>(context->ccContext);
    }
    inline CC_SHA256_CTX* toSHA224Context(CryptoDigestContext* context)
    {
        ASSERT(context->algorithm == CryptoDigest::Algorithm::SHA_224);
        return static_cast<CC_SHA256_CTX*>(context->ccContext);
    }
    inline CC_SHA256_CTX* toSHA256Context(CryptoDigestContext* context)
    {
        ASSERT(context->algorithm == CryptoDigest::Algorithm::SHA_256);
        return static_cast<CC_SHA256_CTX*>(context->ccContext);
    }
    inline CC_SHA512_CTX* toSHA384Context(CryptoDigestContext* context)
    {
        ASSERT(context->algorithm == CryptoDigest::Algorithm::SHA_384);
        return static_cast<CC_SHA512_CTX*>(context->ccContext);
    }
    inline CC_SHA512_CTX* toSHA512Context(CryptoDigestContext* context)
    {
        ASSERT(context->algorithm == CryptoDigest::Algorithm::SHA_512);
        return static_cast<CC_SHA512_CTX*>(context->ccContext);
    }
    
    CryptoDigest::CryptoDigest()
    : m_context(new CryptoDigestContext)
    {
    }
    
    CryptoDigest::~CryptoDigest()
    {
        switch (m_context->algorithm) {
            case CryptoDigest::Algorithm::SHA_1:
                delete toSHA1Context(m_context.get());
                return;
            case CryptoDigest::Algorithm::SHA_224:
                delete toSHA224Context(m_context.get());
                return;
            case CryptoDigest::Algorithm::SHA_256:
                delete toSHA256Context(m_context.get());
                return;
            case CryptoDigest::Algorithm::SHA_384:
                delete toSHA384Context(m_context.get());
                return;
            case CryptoDigest::Algorithm::SHA_512:
                delete toSHA512Context(m_context.get());
                return;
        }
    }
    
    
    std::unique_ptr<CryptoDigest> CryptoDigest::create(CryptoDigest::Algorithm algorithm)
    {
        std::unique_ptr<CryptoDigest> digest(new CryptoDigest);
        digest->m_context->algorithm = algorithm;
        
        switch (algorithm) {
            case CryptoDigest::Algorithm::SHA_1: {
                CC_SHA1_CTX* context = new CC_SHA1_CTX;
                digest->m_context->ccContext = context;
                CC_SHA1_Init(context);
                return digest;
            }
            case CryptoDigest::Algorithm::SHA_224: {
                CC_SHA256_CTX* context = new CC_SHA256_CTX;
                digest->m_context->ccContext = context;
                CC_SHA224_Init(context);
                return digest;
            }
            case CryptoDigest::Algorithm::SHA_256: {
                CC_SHA256_CTX* context = new CC_SHA256_CTX;
                digest->m_context->ccContext = context;
                CC_SHA256_Init(context);
                return digest;
            }
            case CryptoDigest::Algorithm::SHA_384: {
                CC_SHA512_CTX* context = new CC_SHA512_CTX;
                digest->m_context->ccContext = context;
                CC_SHA384_Init(context);
                return digest;
            }
            case CryptoDigest::Algorithm::SHA_512: {
                CC_SHA512_CTX* context = new CC_SHA512_CTX;
                digest->m_context->ccContext = context;
                CC_SHA512_Init(context);
                return digest;
            }
        }
    }
    
    void CryptoDigest::addBytes(const void* input, size_t length)
    {
        switch (m_context->algorithm) {
            case CryptoDigest::Algorithm::SHA_1:
                CC_SHA1_Update(toSHA1Context(m_context.get()), input, length);
                return;
            case CryptoDigest::Algorithm::SHA_224:
                CC_SHA224_Update(toSHA224Context(m_context.get()), input, length);
                return;
            case CryptoDigest::Algorithm::SHA_256:
                CC_SHA256_Update(toSHA256Context(m_context.get()), input, length);
                return;
            case CryptoDigest::Algorithm::SHA_384:
                CC_SHA384_Update(toSHA384Context(m_context.get()), input, length);
                return;
            case CryptoDigest::Algorithm::SHA_512:
                CC_SHA512_Update(toSHA512Context(m_context.get()), input, length);
                return;
        }
    }
    
    Vector<uint8_t> CryptoDigest::computeHash()
    {
        Vector<uint8_t> result;
        switch (m_context->algorithm) {
            case CryptoDigest::Algorithm::SHA_1:
                result.resize(CC_SHA1_DIGEST_LENGTH);
                CC_SHA1_Final(result.data(), toSHA1Context(m_context.get()));
                break;
            case CryptoDigest::Algorithm::SHA_224:
                result.resize(CC_SHA224_DIGEST_LENGTH);
                CC_SHA224_Final(result.data(), toSHA224Context(m_context.get()));
                break;
            case CryptoDigest::Algorithm::SHA_256:
                result.resize(CC_SHA256_DIGEST_LENGTH);
                CC_SHA256_Final(result.data(), toSHA256Context(m_context.get()));
                break;
            case CryptoDigest::Algorithm::SHA_384:
                result.resize(CC_SHA384_DIGEST_LENGTH);
                CC_SHA384_Final(result.data(), toSHA384Context(m_context.get()));
                break;
            case CryptoDigest::Algorithm::SHA_512:
                result.resize(CC_SHA512_DIGEST_LENGTH);
                CC_SHA512_Final(result.data(), toSHA512Context(m_context.get()));
                break;
        }
        return result;
    }
    
} // namespace WebCore
#else
#error "Unhandled platform."
#endif
