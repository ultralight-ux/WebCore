/*
 * Copyright (C) 2018 Apple Inc. All rights reserved.
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

#import "config.h"
#import "GPUShaderModule.h"

#if ENABLE(WEBGPU)

#import "GPUDevice.h"
#import "GPUErrorScopes.h"
#import "GPUShaderModuleDescriptor.h"
#import "Logging.h"

#import <Metal/Metal.h>
#import <wtf/BlockObjCExceptions.h>

namespace WebCore {

RefPtr<GPUShaderModule> GPUShaderModule::tryCreate(const GPUDevice& device, const GPUShaderModuleDescriptor& descriptor)
{
    if (!device.platformDevice()) {
        LOG(WebGPU, "GPUShaderModule::create(): Invalid GPUDevice!");
        return nullptr;
    }
    
    if (GPUDevice::useWHLSL)
        return adoptRef(new GPUShaderModule(WHLSL::createShaderModule(descriptor.code)));

    PlatformShaderModuleSmartPtr module;

    BEGIN_BLOCK_OBJC_EXCEPTIONS

    NSError *error = [NSError errorWithDomain:@"com.apple.WebKit.GPU" code:1 userInfo:nil];
    module = adoptNS([device.platformDevice() newLibraryWithSource:descriptor.code options:nil error:&error]);
    if (!module)
        LOG(WebGPU, "Shader compilation error: %s", [[error localizedDescription] UTF8String]);

    END_BLOCK_OBJC_EXCEPTIONS

    return module ? adoptRef(new GPUShaderModule(WTFMove(module))) : nullptr;
}

GPUShaderModule::GPUShaderModule(PlatformShaderModuleSmartPtr&& module)
    : m_platformShaderModule(WTFMove(module))
{
}

GPUShaderModule::GPUShaderModule(UniqueRef<WHLSL::ShaderModule>&& whlslModule)
    : m_whlslModule(whlslModule.moveToUniquePtr())
{
}

}

#endif // ENABLE(WEBGPU)
