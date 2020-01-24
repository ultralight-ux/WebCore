/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2015 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "InspectorFrontendClient.h"
#include <wtf/Forward.h>
#include <wtf/Noncopyable.h>
#include <wtf/text/WTFString.h>

namespace WebCore {

class FloatRect;
class Frame;
class InspectorController;
class InspectorBackendDispatchTask;
class InspectorFrontendHost;
class Page;

class WEBCORE_EXPORT InspectorFrontendClientLocal : public InspectorFrontendClient {
    WTF_MAKE_NONCOPYABLE(InspectorFrontendClientLocal);
    WTF_MAKE_FAST_ALLOCATED;
public:
    class WEBCORE_EXPORT Settings {
    public:
        Settings() = default;
        virtual ~Settings() = default;
        virtual String getProperty(const String& name);
        virtual void setProperty(const String& name, const String& value);
        virtual void deleteProperty(const String& name);
    };

    InspectorFrontendClientLocal(InspectorController* inspectedPageController, Page* frontendPage, std::unique_ptr<Settings>);
    virtual ~InspectorFrontendClientLocal();

    void resetState() override;

    void windowObjectCleared() final;
    void frontendLoaded() override;

    void startWindowDrag() override { }
    void moveWindowBy(float x, float y) final;

    UserInterfaceLayoutDirection userInterfaceLayoutDirection() const final;

    void requestSetDockSide(DockSide) final;
    void changeAttachedWindowHeight(unsigned) final;
    void changeAttachedWindowWidth(unsigned) final;
    void changeSheetRect(const FloatRect&) final;
    void openInNewTab(const String& url) final;
    bool canSave()  override { return false; }
    void save(const String&, const String&, bool, bool) override { }
    void append(const String&, const String&) override { }

    virtual void attachWindow(DockSide) = 0;
    virtual void detachWindow() = 0;

    void sendMessageToBackend(const String& message) final;

    bool isUnderTest() final;
    bool isRemote() const final { return false; }
    unsigned inspectionLevel() const final;

    bool canAttachWindow();
    void setDockingUnavailable(bool);

    static unsigned constrainedAttachedWindowHeight(unsigned preferredHeight, unsigned totalWindowHeight);
    static unsigned constrainedAttachedWindowWidth(unsigned preferredWidth, unsigned totalWindowWidth);

    // Direct Frontend API
    bool isDebuggingEnabled();
    void setDebuggingEnabled(bool);

    bool isTimelineProfilingEnabled();
    void setTimelineProfilingEnabled(bool);

    bool isProfilingJavaScript();
    void startProfilingJavaScript();
    void stopProfilingJavaScript();

    void showConsole();

    void showMainResourceForFrame(Frame*);

    void showResources();

    void setAttachedWindow(DockSide);

    Page* inspectedPage() const;
    Page* frontendPage() const { return m_frontendPage; }
protected:
    virtual void setAttachedWindowHeight(unsigned) = 0;
    virtual void setAttachedWindowWidth(unsigned) = 0;
    void restoreAttachedWindowHeight();

    virtual void setSheetRect(const WebCore::FloatRect&) = 0;

private:
    bool evaluateAsBoolean(const String& expression);
    void evaluateOnLoad(const String& expression);

    friend class FrontendMenuProvider;
    InspectorController* m_inspectedPageController { nullptr };
    Page* m_frontendPage { nullptr };
    // TODO(yurys): this ref shouldn't be needed.
    RefPtr<InspectorFrontendHost> m_frontendHost;
    std::unique_ptr<InspectorFrontendClientLocal::Settings> m_settings;
    bool m_frontendLoaded { false };
    DockSide m_dockSide;
    Vector<String> m_evaluateOnLoad;
    Ref<InspectorBackendDispatchTask> m_dispatchTask;
};

} // namespace WebCore
