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
#include "JSTestSetLikeWithOverriddenOperations.h"

#include "ActiveDOMObject.h"
#include "ExtendedDOMClientIsoSubspaces.h"
#include "ExtendedDOMIsoSubspaces.h"
#include "IDLTypes.h"
#include "JSDOMAttribute.h"
#include "JSDOMBinding.h"
#include "JSDOMConstructorNotConstructable.h"
#include "JSDOMConvertAny.h"
#include "JSDOMConvertBase.h"
#include "JSDOMConvertNumbers.h"
#include "JSDOMConvertStrings.h"
#include "JSDOMExceptionHandling.h"
#include "JSDOMGlobalObjectInlines.h"
#include "JSDOMOperation.h"
#include "JSDOMSetLike.h"
#include "JSDOMWrapperCache.h"
#include "ScriptExecutionContext.h"
#include "WebCoreJSClientData.h"
#include <JavaScriptCore/BuiltinNames.h>
#include <JavaScriptCore/FunctionPrototype.h>
#include <JavaScriptCore/HeapAnalyzer.h>
#include <JavaScriptCore/JSCInlines.h>
#include <JavaScriptCore/JSDestructibleObjectHeapCellType.h>
#include <JavaScriptCore/SlotVisitorMacros.h>
#include <JavaScriptCore/SubspaceInlines.h>
#include <wtf/GetPtr.h>
#include <wtf/PointerPreparations.h>
#include <wtf/URL.h>


namespace WebCore {
using namespace JSC;

// Functions

static JSC_DECLARE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_delete);
static JSC_DECLARE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_has);
static JSC_DECLARE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_entries);
static JSC_DECLARE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_keys);
static JSC_DECLARE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_values);
static JSC_DECLARE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_forEach);
static JSC_DECLARE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_clear);

// Attributes

static JSC_DECLARE_CUSTOM_GETTER(jsTestSetLikeWithOverriddenOperationsConstructor);
static JSC_DECLARE_CUSTOM_GETTER(jsTestSetLikeWithOverriddenOperations_add);
static JSC_DECLARE_CUSTOM_SETTER(setJSTestSetLikeWithOverriddenOperations_add);
static JSC_DECLARE_CUSTOM_GETTER(jsTestSetLikeWithOverriddenOperations_size);

class JSTestSetLikeWithOverriddenOperationsPrototype final : public JSC::JSNonFinalObject {
public:
    using Base = JSC::JSNonFinalObject;
    static JSTestSetLikeWithOverriddenOperationsPrototype* create(JSC::VM& vm, JSDOMGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSTestSetLikeWithOverriddenOperationsPrototype* ptr = new (NotNull, JSC::allocateCell<JSTestSetLikeWithOverriddenOperationsPrototype>(vm)) JSTestSetLikeWithOverriddenOperationsPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    template<typename CellType, JSC::SubspaceAccess>
    static JSC::GCClient::IsoSubspace* subspaceFor(JSC::VM& vm)
    {
        STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSTestSetLikeWithOverriddenOperationsPrototype, Base);
        return &vm.plainObjectSpace();
    }
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSTestSetLikeWithOverriddenOperationsPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};
STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSTestSetLikeWithOverriddenOperationsPrototype, JSTestSetLikeWithOverriddenOperationsPrototype::Base);

using JSTestSetLikeWithOverriddenOperationsDOMConstructor = JSDOMConstructorNotConstructable<JSTestSetLikeWithOverriddenOperations>;

template<> const ClassInfo JSTestSetLikeWithOverriddenOperationsDOMConstructor::s_info = { "TestSetLikeWithOverriddenOperations"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestSetLikeWithOverriddenOperationsDOMConstructor) };

template<> JSValue JSTestSetLikeWithOverriddenOperationsDOMConstructor::prototypeForStructure(JSC::VM& vm, const JSDOMGlobalObject& globalObject)
{
    UNUSED_PARAM(vm);
    return globalObject.functionPrototype();
}

template<> void JSTestSetLikeWithOverriddenOperationsDOMConstructor::initializeProperties(VM& vm, JSDOMGlobalObject& globalObject)
{
    putDirect(vm, vm.propertyNames->length, jsNumber(0), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    JSString* nameString = jsNontrivialString(vm, "TestSetLikeWithOverriddenOperations"_s);
    m_originalName.set(vm, this, nameString);
    putDirect(vm, vm.propertyNames->name, nameString, JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    putDirect(vm, vm.propertyNames->prototype, JSTestSetLikeWithOverriddenOperations::prototype(vm, globalObject), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum | JSC::PropertyAttribute::DontDelete);
}

/* Hash table for prototype */

static const HashTableValue JSTestSetLikeWithOverriddenOperationsPrototypeTableValues[] =
{
    { "constructor"_s, static_cast<unsigned>(JSC::PropertyAttribute::DontEnum), NoIntrinsic, { HashTableValue::GetterSetterType, jsTestSetLikeWithOverriddenOperationsConstructor, 0 } },
    { "add"_s, static_cast<unsigned>(JSC::PropertyAttribute::CustomAccessor | JSC::PropertyAttribute::DOMAttribute), NoIntrinsic, { HashTableValue::GetterSetterType, jsTestSetLikeWithOverriddenOperations_add, setJSTestSetLikeWithOverriddenOperations_add } },
    { "size"_s, static_cast<unsigned>(JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::CustomAccessor), NoIntrinsic, { HashTableValue::GetterSetterType, jsTestSetLikeWithOverriddenOperations_size, 0 } },
    { "delete"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { HashTableValue::NativeFunctionType, jsTestSetLikeWithOverriddenOperationsPrototypeFunction_delete, 0 } },
    { "has"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { HashTableValue::NativeFunctionType, jsTestSetLikeWithOverriddenOperationsPrototypeFunction_has, 1 } },
    { "entries"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { HashTableValue::NativeFunctionType, jsTestSetLikeWithOverriddenOperationsPrototypeFunction_entries, 0 } },
    { "keys"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { HashTableValue::NativeFunctionType, jsTestSetLikeWithOverriddenOperationsPrototypeFunction_keys, 0 } },
    { "values"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { HashTableValue::NativeFunctionType, jsTestSetLikeWithOverriddenOperationsPrototypeFunction_values, 0 } },
    { "forEach"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { HashTableValue::NativeFunctionType, jsTestSetLikeWithOverriddenOperationsPrototypeFunction_forEach, 1 } },
    { "clear"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { HashTableValue::NativeFunctionType, jsTestSetLikeWithOverriddenOperationsPrototypeFunction_clear, 0 } },
};

const ClassInfo JSTestSetLikeWithOverriddenOperationsPrototype::s_info = { "TestSetLikeWithOverriddenOperations"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestSetLikeWithOverriddenOperationsPrototype) };

void JSTestSetLikeWithOverriddenOperationsPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSTestSetLikeWithOverriddenOperations::info(), JSTestSetLikeWithOverriddenOperationsPrototypeTableValues, *this);
    putDirect(vm, vm.propertyNames->iteratorSymbol, getDirect(vm, vm.propertyNames->builtinNames().valuesPublicName()), static_cast<unsigned>(JSC::PropertyAttribute::DontEnum));
    JSC_TO_STRING_TAG_WITHOUT_TRANSITION();
}

const ClassInfo JSTestSetLikeWithOverriddenOperations::s_info = { "TestSetLikeWithOverriddenOperations"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestSetLikeWithOverriddenOperations) };

JSTestSetLikeWithOverriddenOperations::JSTestSetLikeWithOverriddenOperations(Structure* structure, JSDOMGlobalObject& globalObject, Ref<TestSetLikeWithOverriddenOperations>&& impl)
    : JSDOMWrapper<TestSetLikeWithOverriddenOperations>(structure, globalObject, WTFMove(impl))
{
}

void JSTestSetLikeWithOverriddenOperations::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));

    static_assert(!std::is_base_of<ActiveDOMObject, TestSetLikeWithOverriddenOperations>::value, "Interface is not marked as [ActiveDOMObject] even though implementation class subclasses ActiveDOMObject.");

}

JSObject* JSTestSetLikeWithOverriddenOperations::createPrototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return JSTestSetLikeWithOverriddenOperationsPrototype::create(vm, &globalObject, JSTestSetLikeWithOverriddenOperationsPrototype::createStructure(vm, &globalObject, globalObject.objectPrototype()));
}

JSObject* JSTestSetLikeWithOverriddenOperations::prototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return getDOMPrototype<JSTestSetLikeWithOverriddenOperations>(vm, globalObject);
}

JSValue JSTestSetLikeWithOverriddenOperations::getConstructor(VM& vm, const JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestSetLikeWithOverriddenOperationsDOMConstructor, DOMConstructorID::TestSetLikeWithOverriddenOperations>(vm, *jsCast<const JSDOMGlobalObject*>(globalObject));
}

void JSTestSetLikeWithOverriddenOperations::destroy(JSC::JSCell* cell)
{
    JSTestSetLikeWithOverriddenOperations* thisObject = static_cast<JSTestSetLikeWithOverriddenOperations*>(cell);
    thisObject->JSTestSetLikeWithOverriddenOperations::~JSTestSetLikeWithOverriddenOperations();
}

JSC_DEFINE_CUSTOM_GETTER(jsTestSetLikeWithOverriddenOperationsConstructor, (JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, PropertyName))
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* prototype = jsDynamicCast<JSTestSetLikeWithOverriddenOperationsPrototype*>(JSValue::decode(thisValue));
    if (UNLIKELY(!prototype))
        return throwVMTypeError(lexicalGlobalObject, throwScope);
    return JSValue::encode(JSTestSetLikeWithOverriddenOperations::getConstructor(JSC::getVM(lexicalGlobalObject), prototype->globalObject()));
}

static inline JSValue jsTestSetLikeWithOverriddenOperations_addGetter(JSGlobalObject& lexicalGlobalObject, JSTestSetLikeWithOverriddenOperations& thisObject)
{
    auto& vm = JSC::getVM(&lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto& impl = thisObject.wrapped();
    RELEASE_AND_RETURN(throwScope, (toJS<IDLLong>(lexicalGlobalObject, throwScope, impl.add())));
}

JSC_DEFINE_CUSTOM_GETTER(jsTestSetLikeWithOverriddenOperations_add, (JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, PropertyName attributeName))
{
    return IDLAttribute<JSTestSetLikeWithOverriddenOperations>::get<jsTestSetLikeWithOverriddenOperations_addGetter, CastedThisErrorBehavior::Assert>(*lexicalGlobalObject, thisValue, attributeName);
}

static inline bool setJSTestSetLikeWithOverriddenOperations_addSetter(JSGlobalObject& lexicalGlobalObject, JSTestSetLikeWithOverriddenOperations& thisObject, JSValue value)
{
    auto& vm = JSC::getVM(&lexicalGlobalObject);
    UNUSED_PARAM(vm);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto& impl = thisObject.wrapped();
    auto nativeValue = convert<IDLLong>(lexicalGlobalObject, value);
    RETURN_IF_EXCEPTION(throwScope, false);
    invokeFunctorPropagatingExceptionIfNecessary(lexicalGlobalObject, throwScope, [&] {
        return impl.setAdd(WTFMove(nativeValue));
    });
    return true;
}

JSC_DEFINE_CUSTOM_SETTER(setJSTestSetLikeWithOverriddenOperations_add, (JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, EncodedJSValue encodedValue, PropertyName attributeName))
{
    return IDLAttribute<JSTestSetLikeWithOverriddenOperations>::set<setJSTestSetLikeWithOverriddenOperations_addSetter>(*lexicalGlobalObject, thisValue, encodedValue, attributeName);
}

static inline JSValue jsTestSetLikeWithOverriddenOperations_sizeGetter(JSGlobalObject& lexicalGlobalObject, JSTestSetLikeWithOverriddenOperations& thisObject)
{
    auto& vm = JSC::getVM(&lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    RELEASE_AND_RETURN(throwScope, (toJS<IDLAny>(lexicalGlobalObject, throwScope, forwardSizeToSetLike(lexicalGlobalObject, thisObject))));
}

JSC_DEFINE_CUSTOM_GETTER(jsTestSetLikeWithOverriddenOperations_size, (JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, PropertyName attributeName))
{
    return IDLAttribute<JSTestSetLikeWithOverriddenOperations>::get<jsTestSetLikeWithOverriddenOperations_sizeGetter>(*lexicalGlobalObject, thisValue, attributeName);
}

static inline JSC::EncodedJSValue jsTestSetLikeWithOverriddenOperationsPrototypeFunction_deleteBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestSetLikeWithOverriddenOperations>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLUndefined>(*lexicalGlobalObject, throwScope, [&]() -> decltype(auto) { return impl.delete(); })));
}

JSC_DEFINE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_delete, (JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSTestSetLikeWithOverriddenOperations>::call<jsTestSetLikeWithOverriddenOperationsPrototypeFunction_deleteBody>(*lexicalGlobalObject, *callFrame, "delete");
}

static inline JSC::EncodedJSValue jsTestSetLikeWithOverriddenOperationsPrototypeFunction_hasBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestSetLikeWithOverriddenOperations>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    if (UNLIKELY(callFrame->argumentCount() < 1))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    EnsureStillAliveScope argument0 = callFrame->uncheckedArgument(0);
    auto key = convert<IDLDOMString>(*lexicalGlobalObject, argument0.value());
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLAny>(*lexicalGlobalObject, throwScope, forwardHasToSetLike(*lexicalGlobalObject, *callFrame, *castedThis, WTFMove(key)))));
}

JSC_DEFINE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_has, (JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSTestSetLikeWithOverriddenOperations>::call<jsTestSetLikeWithOverriddenOperationsPrototypeFunction_hasBody>(*lexicalGlobalObject, *callFrame, "has");
}

static inline JSC::EncodedJSValue jsTestSetLikeWithOverriddenOperationsPrototypeFunction_entriesBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestSetLikeWithOverriddenOperations>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLAny>(*lexicalGlobalObject, throwScope, forwardEntriesToSetLike(*lexicalGlobalObject, *callFrame, *castedThis))));
}

JSC_DEFINE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_entries, (JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSTestSetLikeWithOverriddenOperations>::call<jsTestSetLikeWithOverriddenOperationsPrototypeFunction_entriesBody>(*lexicalGlobalObject, *callFrame, "entries");
}

static inline JSC::EncodedJSValue jsTestSetLikeWithOverriddenOperationsPrototypeFunction_keysBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestSetLikeWithOverriddenOperations>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLAny>(*lexicalGlobalObject, throwScope, forwardKeysToSetLike(*lexicalGlobalObject, *callFrame, *castedThis))));
}

JSC_DEFINE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_keys, (JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSTestSetLikeWithOverriddenOperations>::call<jsTestSetLikeWithOverriddenOperationsPrototypeFunction_keysBody>(*lexicalGlobalObject, *callFrame, "keys");
}

static inline JSC::EncodedJSValue jsTestSetLikeWithOverriddenOperationsPrototypeFunction_valuesBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestSetLikeWithOverriddenOperations>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLAny>(*lexicalGlobalObject, throwScope, forwardValuesToSetLike(*lexicalGlobalObject, *callFrame, *castedThis))));
}

JSC_DEFINE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_values, (JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSTestSetLikeWithOverriddenOperations>::call<jsTestSetLikeWithOverriddenOperationsPrototypeFunction_valuesBody>(*lexicalGlobalObject, *callFrame, "values");
}

static inline JSC::EncodedJSValue jsTestSetLikeWithOverriddenOperationsPrototypeFunction_forEachBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestSetLikeWithOverriddenOperations>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    if (UNLIKELY(callFrame->argumentCount() < 1))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    EnsureStillAliveScope argument0 = callFrame->uncheckedArgument(0);
    auto callback = convert<IDLAny>(*lexicalGlobalObject, argument0.value());
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLAny>(*lexicalGlobalObject, throwScope, forwardForEachToSetLike(*lexicalGlobalObject, *callFrame, *castedThis, WTFMove(callback)))));
}

JSC_DEFINE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_forEach, (JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSTestSetLikeWithOverriddenOperations>::call<jsTestSetLikeWithOverriddenOperationsPrototypeFunction_forEachBody>(*lexicalGlobalObject, *callFrame, "forEach");
}

static inline JSC::EncodedJSValue jsTestSetLikeWithOverriddenOperationsPrototypeFunction_clearBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestSetLikeWithOverriddenOperations>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLAny>(*lexicalGlobalObject, throwScope, forwardClearToSetLike(*lexicalGlobalObject, *callFrame, *castedThis))));
}

JSC_DEFINE_HOST_FUNCTION(jsTestSetLikeWithOverriddenOperationsPrototypeFunction_clear, (JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSTestSetLikeWithOverriddenOperations>::call<jsTestSetLikeWithOverriddenOperationsPrototypeFunction_clearBody>(*lexicalGlobalObject, *callFrame, "clear");
}

JSC::GCClient::IsoSubspace* JSTestSetLikeWithOverriddenOperations::subspaceForImpl(JSC::VM& vm)
{
    return WebCore::subspaceForImpl<JSTestSetLikeWithOverriddenOperations, UseCustomHeapCellType::No>(vm,
        [] (auto& spaces) { return spaces.m_clientSubspaceForTestSetLikeWithOverriddenOperations.get(); },
        [] (auto& spaces, auto&& space) { spaces.m_clientSubspaceForTestSetLikeWithOverriddenOperations = std::forward<decltype(space)>(space); },
        [] (auto& spaces) { return spaces.m_subspaceForTestSetLikeWithOverriddenOperations.get(); },
        [] (auto& spaces, auto&& space) { spaces.m_subspaceForTestSetLikeWithOverriddenOperations = std::forward<decltype(space)>(space); }
    );
}

void JSTestSetLikeWithOverriddenOperations::analyzeHeap(JSCell* cell, HeapAnalyzer& analyzer)
{
    auto* thisObject = jsCast<JSTestSetLikeWithOverriddenOperations*>(cell);
    analyzer.setWrappedObjectForCell(cell, &thisObject->wrapped());
    if (thisObject->scriptExecutionContext())
        analyzer.setLabelForCell(cell, "url " + thisObject->scriptExecutionContext()->url().string());
    Base::analyzeHeap(cell, analyzer);
}

bool JSTestSetLikeWithOverriddenOperationsOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, AbstractSlotVisitor& visitor, const char** reason)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    UNUSED_PARAM(reason);
    return false;
}

void JSTestSetLikeWithOverriddenOperationsOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsTestSetLikeWithOverriddenOperations = static_cast<JSTestSetLikeWithOverriddenOperations*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestSetLikeWithOverriddenOperations->wrapped(), jsTestSetLikeWithOverriddenOperations);
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7TestSetLikeWithOverriddenOperations@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore35TestSetLikeWithOverriddenOperationsE[]; }
#endif
#endif

JSC::JSValue toJSNewlyCreated(JSC::JSGlobalObject*, JSDOMGlobalObject* globalObject, Ref<TestSetLikeWithOverriddenOperations>&& impl)
{

    if constexpr (std::is_polymorphic_v<TestSetLikeWithOverriddenOperations>) {
#if ENABLE(BINDING_INTEGRITY)
        const void* actualVTablePointer = getVTablePointer(impl.ptr());
#if PLATFORM(WIN)
        void* expectedVTablePointer = __identifier("??_7TestSetLikeWithOverriddenOperations@WebCore@@6B@");
#else
        void* expectedVTablePointer = &_ZTVN7WebCore35TestSetLikeWithOverriddenOperationsE[2];
#endif

        // If you hit this assertion you either have a use after free bug, or
        // TestSetLikeWithOverriddenOperations has subclasses. If TestSetLikeWithOverriddenOperations has subclasses that get passed
        // to toJS() we currently require TestSetLikeWithOverriddenOperations you to opt out of binding hardening
        // by adding the SkipVTableValidation attribute to the interface IDL definition
        RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    }
    return createWrapper<TestSetLikeWithOverriddenOperations>(globalObject, WTFMove(impl));
}

JSC::JSValue toJS(JSC::JSGlobalObject* lexicalGlobalObject, JSDOMGlobalObject* globalObject, TestSetLikeWithOverriddenOperations& impl)
{
    return wrap(lexicalGlobalObject, globalObject, impl);
}

TestSetLikeWithOverriddenOperations* JSTestSetLikeWithOverriddenOperations::toWrapped(JSC::VM&, JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSTestSetLikeWithOverriddenOperations*>(value))
        return &wrapper->wrapped();
    return nullptr;
}

}
