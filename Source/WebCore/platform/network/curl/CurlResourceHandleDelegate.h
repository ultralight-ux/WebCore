/*
 * Copyright (C) 2004, 2006 Apple Inc.  All rights reserved.
 * Copyright (C) 2021 Ultralight, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#pragma once

#include "CurlRequestClient.h"
#include "ResourceResponse.h"
#include "SharedBuffer.h"

#include <wtf/FastMalloc.h>
#include <wtf/Noncopyable.h>
#include <wtf/Ref.h>
#include <wtf/Lock.h>

namespace WebCore {

class ResourceHandle;
class ResourceHandleClient;
class ResourceHandleInternal;

class CurlResourceHandleDelegate final : public CurlRequestClient {
    WTF_MAKE_NONCOPYABLE(CurlResourceHandleDelegate); WTF_MAKE_FAST_ALLOCATED;
public:
    CurlResourceHandleDelegate(ResourceHandle&);

    const ResourceResponse& response() const { return m_response; }

    // CurlRequestClient methods

    void ref() override final;
    void deref() override final;

    void curlDidSendData(CurlRequest&, unsigned long long bytesSent, unsigned long long totalBytesToBeSent) override final;
    void curlDidReceiveResponse(CurlRequest&, CurlResponse&&) override final;
    void curlDidComplete(CurlRequest&, NetworkLoadMetrics&&) override final;
    void curlDidFailWithError(CurlRequest&, ResourceError&&, CertificateInfo&&) override final;

    void curlConsumeReceiveQueue(CurlRequest&, WTF::ReaderWriterQueue<RefPtr<SharedBuffer>>& queue) override final;

private:
    ResourceHandle& m_handle;
    ResourceResponse m_response;

    bool cancelledOrClientless();
    ResourceHandleClient* client() const;
    ResourceHandleInternal* d();
};

} // namespace WebCore
