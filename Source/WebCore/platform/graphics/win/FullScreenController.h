/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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

#if ENABLE(FULLSCREEN_API)

#include <memory>

namespace WebCore {

class FullScreenControllerClient;

class FullScreenController {
public:
    WEBCORE_EXPORT FullScreenController(FullScreenControllerClient*);
    WEBCORE_EXPORT ~FullScreenController();

public:
    WEBCORE_EXPORT void enterFullScreen();
    WEBCORE_EXPORT void exitFullScreen();
    WEBCORE_EXPORT void repaintCompleted();
    
    WEBCORE_EXPORT bool isFullScreen() const;

    WEBCORE_EXPORT void close();

protected:
    void enterFullScreenRepaintCompleted();
    void exitFullScreenRepaintCompleted();

    class Private;
    friend class Private;
    std::unique_ptr<FullScreenController::Private> m_private;
};

} // namespace WebCore

#endif // ENABLE(FULLSCREEN_API)
