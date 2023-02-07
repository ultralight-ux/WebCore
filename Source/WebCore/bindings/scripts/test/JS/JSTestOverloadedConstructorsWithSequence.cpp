/*
    This file is part of the WebKit open source project.
    This file has been generated by generate-bindings.pl. DO NOT MODIFY!

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "JSTestOverloadedConstructorsWithSequence.h"

#include "ActiveDOMObject.h"
#include "ExtendedDOMClientIsoSubspaces.h"
#include "ExtendedDOMIsoSubspaces.h"
#include "JSDOMBinding.h"
#include "JSDOMConstructor.h"
#include "JSDOMConvertInterface.h"
#include "JSDOMConvertSequences.h"
#include "JSDOMConvertStrings.h"
#include "JSDOMExceptionHandling.h"
#include "JSDOMGlobalObjectInlines.h"
#include "JSDOMWrapperCache.h"
#include "ScriptExecutionContext.h"
#include "WebCoreJSClientData.h"
#include <JavaScriptCore/FunctionPrototype.h>
#include <JavaScriptCore/HeapAnalyzer.h>
#include <JavaScriptCore/IteratorOperations.h>
#include <JavaScriptCore/JSArray.h>
#include <JavaScriptCore/JSCInlines.h>
#include <JavaScriptCore/JSDestructibleObjectHeapCellType.h>
#include <JavaScriptCore/SlotVisitorMacros.h>
#include <JavaScriptCore/SubspaceInlines.h>
#include <wtf/GetPtr.h>
#include <wtf/PointerPreparations.h>
#include <wtf/URL.h>


namespace WebCore {
using namespace JSC;

// Attributes

static JSC_DECLARE_CUSTOM_GETTER(jsTestOverloadedConstructorsWithSequenceConstructor);

class JSTestOverloadedConstructorsWithSequencePrototype final : public JSC::JSNonFinalObject {
public:
    using Base = JSC::JSNonFinalObject;
    static JSTestOverloadedConstructorsWithSequencePrototype* create(JSC::VM& vm, JSDOMGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSTestOverloadedConstructorsWithSequencePrototype* ptr = new (NotNull, JSC::allocateCell<JSTestOverloadedConstructorsWithSequencePrototype>(vm)) JSTestOverloadedConstructorsWithSequencePrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    template<typename CellType, JSC::SubspaceAccess>
    static JSC::GCClient::IsoSubspace* subspaceFor(JSC::VM& vm)
    {
        STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSTestOverloadedConstructorsWithSequencePrototype, Base);
        return &vm.plainObjectSpace();
    }
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSTestOverloadedConstructorsWithSequencePrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};
STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSTestOverloadedConstructorsWithSequencePrototype, JSTestOverloadedConstructorsWithSequencePrototype::Base);

using JSTestOverloadedConstructorsWithSequenceDOMConstructor = JSDOMConstructor<JSTestOverloadedConstructorsWithSequence>;

static inline EncodedJSValue constructJSTestOverloadedConstructorsWithSequence1(JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame)
{
    VM& vm = lexicalGlobalObject->vm();
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* castedThis = jsCast<JSTestOverloadedConstructorsWithSequenceDOMConstructor*>(callFrame->jsCallee());
    ASSERT(castedThis);
    EnsureStillAliveScope argument0 = callFrame->argument(0);
    auto sequenceOfStrings = argument0.value().isUndefined() ? Converter<IDLSequence<IDLDOMString>>::ReturnType{ } : convert<IDLSequence<IDLDOMString>>(*lexicalGlobalObject, argument0.value());
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    auto object = TestOverloadedConstructorsWithSequence::create(WTFMove(sequenceOfStrings));
    if constexpr (IsExceptionOr<decltype(object)>)
        RETURN_IF_EXCEPTION(throwScope, { });
    static_assert(TypeOrExceptionOrUnderlyingType<decltype(object)>::isRef);
    auto jsValue = toJSNewlyCreated<IDLInterface<TestOverloadedConstructorsWithSequence>>(*lexicalGlobalObject, *castedThis->globalObject(), throwScope, WTFMove(object));
    if constexpr (IsExceptionOr<decltype(object)>)
        RETURN_IF_EXCEPTION(throwScope, { });
    setSubclassStructureIfNeeded<TestOverloadedConstructorsWithSequence>(lexicalGlobalObject, callFrame, asObject(jsValue));
    RETURN_IF_EXCEPTION(throwScope, { });
    return JSValue::encode(jsValue);
}

static inline EncodedJSValue constructJSTestOverloadedConstructorsWithSequence2(JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame)
{
    VM& vm = lexicalGlobalObject->vm();
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* castedThis = jsCast<JSTestOverloadedConstructorsWithSequenceDOMConstructor*>(callFrame->jsCallee());
    ASSERT(castedThis);
    EnsureStillAliveScope argument0 = callFrame->uncheckedArgument(0);
    auto string = convert<IDLDOMString>(*lexicalGlobalObject, argument0.value());
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    auto object = TestOverloadedConstructorsWithSequence::create(WTFMove(string));
    if constexpr (IsExceptionOr<decltype(object)>)
        RETURN_IF_EXCEPTION(throwScope, { });
    static_assert(TypeOrExceptionOrUnderlyingType<decltype(object)>::isRef);
    auto jsValue = toJSNewlyCreated<IDLInterface<TestOverloadedConstructorsWithSequence>>(*lexicalGlobalObject, *castedThis->globalObject(), throwScope, WTFMove(object));
    if constexpr (IsExceptionOr<decltype(object)>)
        RETURN_IF_EXCEPTION(throwScope, { });
    setSubclassStructureIfNeeded<TestOverloadedConstructorsWithSequence>(lexicalGlobalObject, callFrame, asObject(jsValue));
    RETURN_IF_EXCEPTION(throwScope, { });
    return JSValue::encode(jsValue);
}

template<> EncodedJSValue JSC_HOST_CALL_ATTRIBUTES JSTestOverloadedConstructorsWithSequenceDOMConstructor::construct(JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame)
{
    VM& vm = lexicalGlobalObject->vm();
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    size_t argsCount = std::min<size_t>(1, callFrame->argumentCount());
    if (argsCount == 0) {
        RELEASE_AND_RETURN(throwScope, (constructJSTestOverloadedConstructorsWithSequence1(lexicalGlobalObject, callFrame)));
    }
    if (argsCount == 1) {
        JSValue distinguishingArg = callFrame->uncheckedArgument(0);
        if (distinguishingArg.isUndefined())
            RELEASE_AND_RETURN(throwScope, (constructJSTestOverloadedConstructorsWithSequence1(lexicalGlobalObject, callFrame)));
        {
            bool success = hasIteratorMethod(lexicalGlobalObject, distinguishingArg);
            RETURN_IF_EXCEPTION(throwScope, { });
            if (success)
                RELEASE_AND_RETURN(throwScope, (constructJSTestOverloadedConstructorsWithSequence1(lexicalGlobalObject, callFrame)));
        }
        RELEASE_AND_RETURN(throwScope, (constructJSTestOverloadedConstructorsWithSequence2(lexicalGlobalObject, callFrame)));
    }
    return throwVMTypeError(lexicalGlobalObject, throwScope);
}
JSC_ANNOTATE_HOST_FUNCTION(JSTestOverloadedConstructorsWithSequenceConstructorConstruct, JSTestOverloadedConstructorsWithSequenceDOMConstructor::construct);

template<> const ClassInfo JSTestOverloadedConstructorsWithSequenceDOMConstructor::s_info = { "TestOverloadedConstructorsWithSequence"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestOverloadedConstructorsWithSequenceDOMConstructor) };

template<> JSValue JSTestOverloadedConstructorsWithSequenceDOMConstructor::prototypeForStructure(JSC::VM& vm, const JSDOMGlobalObject& globalObject)
{
    UNUSED_PARAM(vm);
    return globalObject.functionPrototype();
}

template<> void JSTestOverloadedConstructorsWithSequenceDOMConstructor::initializeProperties(VM& vm, JSDOMGlobalObject& globalObject)
{
    putDirect(vm, vm.propertyNames->length, jsNumber(0), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    JSString* nameString = jsNontrivialString(vm, "TestOverloadedConstructorsWithSequence"_s);
    m_originalName.set(vm, this, nameString);
    putDirect(vm, vm.propertyNames->name, nameString, JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    putDirect(vm, vm.propertyNames->prototype, JSTestOverloadedConstructorsWithSequence::prototype(vm, globalObject), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum | JSC::PropertyAttribute::DontDelete);
}

/* Hash table for prototype */

static const HashTableValue JSTestOverloadedConstructorsWithSequencePrototypeTableValues[] =
{
    { "constructor"_s, static_cast<unsigned>(JSC::PropertyAttribute::DontEnum), NoIntrinsic, { HashTableValue::GetterSetterType, jsTestOverloadedConstructorsWithSequenceConstructor, 0 } },
};

const ClassInfo JSTestOverloadedConstructorsWithSequencePrototype::s_info = { "TestOverloadedConstructorsWithSequence"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestOverloadedConstructorsWithSequencePrototype) };

void JSTestOverloadedConstructorsWithSequencePrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSTestOverloadedConstructorsWithSequence::info(), JSTestOverloadedConstructorsWithSequencePrototypeTableValues, *this);
    JSC_TO_STRING_TAG_WITHOUT_TRANSITION();
}

const ClassInfo JSTestOverloadedConstructorsWithSequence::s_info = { "TestOverloadedConstructorsWithSequence"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestOverloadedConstructorsWithSequence) };

JSTestOverloadedConstructorsWithSequence::JSTestOverloadedConstructorsWithSequence(Structure* structure, JSDOMGlobalObject& globalObject, Ref<TestOverloadedConstructorsWithSequence>&& impl)
    : JSDOMWrapper<TestOverloadedConstructorsWithSequence>(structure, globalObject, WTFMove(impl))
{
}

void JSTestOverloadedConstructorsWithSequence::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));

    static_assert(!std::is_base_of<ActiveDOMObject, TestOverloadedConstructorsWithSequence>::value, "Interface is not marked as [ActiveDOMObject] even though implementation class subclasses ActiveDOMObject.");

}

JSObject* JSTestOverloadedConstructorsWithSequence::createPrototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return JSTestOverloadedConstructorsWithSequencePrototype::create(vm, &globalObject, JSTestOverloadedConstructorsWithSequencePrototype::createStructure(vm, &globalObject, globalObject.objectPrototype()));
}

JSObject* JSTestOverloadedConstructorsWithSequence::prototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return getDOMPrototype<JSTestOverloadedConstructorsWithSequence>(vm, globalObject);
}

JSValue JSTestOverloadedConstructorsWithSequence::getConstructor(VM& vm, const JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestOverloadedConstructorsWithSequenceDOMConstructor, DOMConstructorID::TestOverloadedConstructorsWithSequence>(vm, *jsCast<const JSDOMGlobalObject*>(globalObject));
}

void JSTestOverloadedConstructorsWithSequence::destroy(JSC::JSCell* cell)
{
    JSTestOverloadedConstructorsWithSequence* thisObject = static_cast<JSTestOverloadedConstructorsWithSequence*>(cell);
    thisObject->JSTestOverloadedConstructorsWithSequence::~JSTestOverloadedConstructorsWithSequence();
}

JSC_DEFINE_CUSTOM_GETTER(jsTestOverloadedConstructorsWithSequenceConstructor, (JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, PropertyName))
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* prototype = jsDynamicCast<JSTestOverloadedConstructorsWithSequencePrototype*>(JSValue::decode(thisValue));
    if (UNLIKELY(!prototype))
        return throwVMTypeError(lexicalGlobalObject, throwScope);
    return JSValue::encode(JSTestOverloadedConstructorsWithSequence::getConstructor(JSC::getVM(lexicalGlobalObject), prototype->globalObject()));
}

JSC::GCClient::IsoSubspace* JSTestOverloadedConstructorsWithSequence::subspaceForImpl(JSC::VM& vm)
{
    return WebCore::subspaceForImpl<JSTestOverloadedConstructorsWithSequence, UseCustomHeapCellType::No>(vm,
        [] (auto& spaces) { return spaces.m_clientSubspaceForTestOverloadedConstructorsWithSequence.get(); },
        [] (auto& spaces, auto&& space) { spaces.m_clientSubspaceForTestOverloadedConstructorsWithSequence = std::forward<decltype(space)>(space); },
        [] (auto& spaces) { return spaces.m_subspaceForTestOverloadedConstructorsWithSequence.get(); },
        [] (auto& spaces, auto&& space) { spaces.m_subspaceForTestOverloadedConstructorsWithSequence = std::forward<decltype(space)>(space); }
    );
}

void JSTestOverloadedConstructorsWithSequence::analyzeHeap(JSCell* cell, HeapAnalyzer& analyzer)
{
    auto* thisObject = jsCast<JSTestOverloadedConstructorsWithSequence*>(cell);
    analyzer.setWrappedObjectForCell(cell, &thisObject->wrapped());
    if (thisObject->scriptExecutionContext())
        analyzer.setLabelForCell(cell, "url " + thisObject->scriptExecutionContext()->url().string());
    Base::analyzeHeap(cell, analyzer);
}

bool JSTestOverloadedConstructorsWithSequenceOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, AbstractSlotVisitor& visitor, const char** reason)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    UNUSED_PARAM(reason);
    return false;
}

void JSTestOverloadedConstructorsWithSequenceOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsTestOverloadedConstructorsWithSequence = static_cast<JSTestOverloadedConstructorsWithSequence*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestOverloadedConstructorsWithSequence->wrapped(), jsTestOverloadedConstructorsWithSequence);
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7TestOverloadedConstructorsWithSequence@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore38TestOverloadedConstructorsWithSequenceE[]; }
#endif
#endif

JSC::JSValue toJSNewlyCreated(JSC::JSGlobalObject*, JSDOMGlobalObject* globalObject, Ref<TestOverloadedConstructorsWithSequence>&& impl)
{

    if constexpr (std::is_polymorphic_v<TestOverloadedConstructorsWithSequence>) {
#if ENABLE(BINDING_INTEGRITY)
        const void* actualVTablePointer = getVTablePointer(impl.ptr());
#if PLATFORM(WIN)
        void* expectedVTablePointer = __identifier("??_7TestOverloadedConstructorsWithSequence@WebCore@@6B@");
#else
        void* expectedVTablePointer = &_ZTVN7WebCore38TestOverloadedConstructorsWithSequenceE[2];
#endif

        // If you hit this assertion you either have a use after free bug, or
        // TestOverloadedConstructorsWithSequence has subclasses. If TestOverloadedConstructorsWithSequence has subclasses that get passed
        // to toJS() we currently require TestOverloadedConstructorsWithSequence you to opt out of binding hardening
        // by adding the SkipVTableValidation attribute to the interface IDL definition
        RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    }
    return createWrapper<TestOverloadedConstructorsWithSequence>(globalObject, WTFMove(impl));
}

JSC::JSValue toJS(JSC::JSGlobalObject* lexicalGlobalObject, JSDOMGlobalObject* globalObject, TestOverloadedConstructorsWithSequence& impl)
{
    return wrap(lexicalGlobalObject, globalObject, impl);
}

TestOverloadedConstructorsWithSequence* JSTestOverloadedConstructorsWithSequence::toWrapped(JSC::VM&, JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSTestOverloadedConstructorsWithSequence*>(value))
        return &wrapper->wrapped();
    return nullptr;
}

}
