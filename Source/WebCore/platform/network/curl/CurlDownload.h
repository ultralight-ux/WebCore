/*
 * Copyright (C) 2013 Apple Inc.  All rights reserved.
 * Copyright (C) 2017 Sony Interactive Entertainment Inc.
 * Copyright (C) 2017 NAVER Corp.
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

#if USE(CURL)

#include "CurlRequest.h"
#include "CurlRequestClient.h"
#include "ResourceRequest.h"
#include "ResourceResponse.h"

namespace WebCore {

class CurlRequest;
class ResourceHandle;
class SharedBuffer;

class WEBCORE_EXPORT CurlDownloadListener {
public:
    virtual void didReceiveResponse(uint32_t id, const ResourceResponse&) { }
    virtual void didReceiveData(uint32_t id, RefPtr<SharedBuffer>&& buffer) {}
    virtual void didFinish(uint32_t id) { }
    virtual void didFail(uint32_t id) { }
};

class WEBCORE_EXPORT CurlDownload final : public ThreadSafeRefCounted<CurlDownload>, public CurlRequestClient {
public:
    CurlDownload() = default;
    ~CurlDownload();

    void ref() override { ThreadSafeRefCounted<CurlDownload>::ref(); }
    void deref() override { ThreadSafeRefCounted<CurlDownload>::deref(); }

    void init(CurlDownloadListener&, uint32_t id, const URL&);
    void init(CurlDownloadListener&, uint32_t id, ResourceHandle*, const ResourceRequest&, const ResourceResponse&);

    void setListener(CurlDownloadListener* listener) { m_listener = listener; }

    void start();
    bool cancel();

    bool downloadsToFile() const { return m_downloadsToFile; }
    void setDownloadsToFile(bool downloadsToFile) { m_downloadsToFile = downloadsToFile; }

    bool deletesFileUponFailure() const { return m_deletesFileUponFailure; }
    void setDeletesFileUponFailure(bool deletesFileUponFailure) { m_deletesFileUponFailure = deletesFileUponFailure; }

    void setDestination(const String& destination) { m_destination = destination; }

private:
    Ref<CurlRequest> createCurlRequest(ResourceRequest&);
    void curlDidSendData(CurlRequest&, unsigned long long, unsigned long long) override { }
    void curlDidReceiveResponse(CurlRequest&, CurlResponse&&) override;
    void curlDidComplete(CurlRequest&, NetworkLoadMetrics&&) override;
    void curlDidFailWithError(CurlRequest&, ResourceError&&, CertificateInfo&&) override;
    void curlConsumeReceiveQueue(CurlRequest&, WTF::ReaderWriterQueue<RefPtr<SharedBuffer>>& queue) override;

    bool shouldRedirectAsGET(const ResourceRequest&, bool crossOrigin);
    void willSendRequest();

    CurlDownloadListener* m_listener { nullptr };
    bool m_isCancelled { false };

    uint32_t m_id { 0 };
    ResourceRequest m_request;
    ResourceResponse m_response;
    bool m_deletesFileUponFailure { false };
    bool m_downloadsToFile { false };
    String m_destination;
    unsigned m_redirectCount { 0 };
    RefPtr<CurlRequest> m_curlRequest;
};

} // namespace WebCore

#endif
