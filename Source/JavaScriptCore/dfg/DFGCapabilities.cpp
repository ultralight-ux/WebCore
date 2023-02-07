/*
 * Copyright (C) 2011-2020 Apple Inc. All rights reserved.
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

#include "config.h"
#include "DFGCapabilities.h"

#if ENABLE(DFG_JIT)

#include "CodeBlock.h"
#include "DFGCommon.h"
#include "JSCellInlines.h"
#include "Options.h"

namespace JSC { namespace DFG {

bool isSupported()
{
    return Options::useDFGJIT() && MacroAssembler::supportsFloatingPoint();
}

bool isSupportedForInlining(CodeBlock* codeBlock)
{
    return codeBlock->ownerExecutable()->isInliningCandidate();
}

bool mightCompileEval(CodeBlock* codeBlock)
{
    return isSupported()
        && codeBlock->bytecodeCost() <= Options::maximumOptimizationCandidateBytecodeCost()
        && codeBlock->ownerExecutable()->isOkToOptimize();
}
bool mightCompileProgram(CodeBlock* codeBlock)
{
    return isSupported()
        && codeBlock->bytecodeCost() <= Options::maximumOptimizationCandidateBytecodeCost()
        && codeBlock->ownerExecutable()->isOkToOptimize();
}
bool mightCompileFunctionForCall(CodeBlock* codeBlock)
{
    return isSupported()
        && codeBlock->bytecodeCost() <= Options::maximumOptimizationCandidateBytecodeCost()
        && codeBlock->ownerExecutable()->isOkToOptimize();
}
bool mightCompileFunctionForConstruct(CodeBlock* codeBlock)
{
    return isSupported()
        && codeBlock->bytecodeCost() <= Options::maximumOptimizationCandidateBytecodeCost()
        && codeBlock->ownerExecutable()->isOkToOptimize();
}

bool mightInlineFunctionForCall(CodeBlock* codeBlock)
{
    return codeBlock->bytecodeCost() <= Options::maximumFunctionForCallInlineCandidateBytecodeCost()
        && isSupportedForInlining(codeBlock);
}
bool mightInlineFunctionForClosureCall(CodeBlock* codeBlock)
{
    return codeBlock->bytecodeCost() <= Options::maximumFunctionForClosureCallInlineCandidateBytecodeCost()
        && isSupportedForInlining(codeBlock);
}
bool mightInlineFunctionForConstruct(CodeBlock* codeBlock)
{
    return codeBlock->bytecodeCost() <= Options::maximumFunctionForConstructInlineCandidateBytecoodeCost()
        && isSupportedForInlining(codeBlock);
}
bool canUseOSRExitFuzzing(CodeBlock* codeBlock)
{
    return codeBlock->ownerExecutable()->canUseOSRExitFuzzing();
}

static bool verboseCapabilities()
{
    return verboseCompilationEnabled() || Options::verboseDFGFailure();
}

inline void debugFail(CodeBlock* codeBlock, OpcodeID opcodeID, CapabilityLevel result)
{
    if (verboseCapabilities() && !canCompile(result))
        dataLog("DFG rejecting opcode in ", *codeBlock, " because of opcode ", opcodeNames[opcodeID], "\n");
}

} } // namespace JSC::DFG

#endif
