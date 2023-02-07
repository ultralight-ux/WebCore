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
#include "JSTestStringifierReadOnlyAttribute.h"

#include "ActiveDOMObject.h"
#include "ExtendedDOMClientIsoSubspaces.h"
#include "ExtendedDOMIsoSubspaces.h"
#include "JSDOMAttribute.h"
#include "JSDOMBinding.h"
#include "JSDOMConstructorNotConstructable.h"
#include "JSDOMConvertStrings.h"
#include "JSDOMExceptionHandling.h"
#include "JSDOMGlobalObjectInlines.h"
#include "JSDOMOperation.h"
#include "JSDOMWrapperCache.h"
#include "ScriptExecutionContext.h"
#include "WebCoreJSClientData.h"
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

static JSC_DECLARE_HOST_FUNCTION(jsTestStringifierReadOnlyAttributePrototypeFunction_toString);

// Attributes

static JSC_DECLARE_CUSTOM_GETTER(jsTestStringifierReadOnlyAttributeConstructor);
static JSC_DECLARE_CUSTOM_GETTER(jsTestStringifierReadOnlyAttribute_identifier);

class JSTestStringifierReadOnlyAttributePrototype final : public JSC::JSNonFinalObject {
public:
    using Base = JSC::JSNonFinalObject;
    static JSTestStringifierReadOnlyAttributePrototype* create(JSC::VM& vm, JSDOMGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSTestStringifierReadOnlyAttributePrototype* ptr = new (NotNull, JSC::allocateCell<JSTestStringifierReadOnlyAttributePrototype>(vm)) JSTestStringifierReadOnlyAttributePrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    template<typename CellType, JSC::SubspaceAccess>
    static JSC::GCClient::IsoSubspace* subspaceFor(JSC::VM& vm)
    {
        STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSTestStringifierReadOnlyAttributePrototype, Base);
        return &vm.plainObjectSpace();
    }
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSTestStringifierReadOnlyAttributePrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};
STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSTestStringifierReadOnlyAttributePrototype, JSTestStringifierReadOnlyAttributePrototype::Base);

using JSTestStringifierReadOnlyAttributeDOMConstructor = JSDOMConstructorNotConstructable<JSTestStringifierReadOnlyAttribute>;

template<> const ClassInfo JSTestStringifierReadOnlyAttributeDOMConstructor::s_info = { "TestStringifierReadOnlyAttribute"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestStringifierReadOnlyAttributeDOMConstructor) };

template<> JSValue JSTestStringifierReadOnlyAttributeDOMConstructor::prototypeForStructure(JSC::VM& vm, const JSDOMGlobalObject& globalObject)
{
    UNUSED_PARAM(vm);
    return globalObject.functionPrototype();
}

template<> void JSTestStringifierReadOnlyAttributeDOMConstructor::initializeProperties(VM& vm, JSDOMGlobalObject& globalObject)
{
    putDirect(vm, vm.propertyNames->length, jsNumber(0), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    JSString* nameString = jsNontrivialString(vm, "TestStringifierReadOnlyAttribute"_s);
    m_originalName.set(vm, this, nameString);
    putDirect(vm, vm.propertyNames->name, nameString, JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    putDirect(vm, vm.propertyNames->prototype, JSTestStringifierReadOnlyAttribute::prototype(vm, globalObject), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum | JSC::PropertyAttribute::DontDelete);
}

/* Hash table for prototype */

static const HashTableValue JSTestStringifierReadOnlyAttributePrototypeTableValues[] =
{
    { "constructor"_s, static_cast<unsigned>(JSC::PropertyAttribute::DontEnum), NoIntrinsic, { HashTableValue::GetterSetterType, jsTestStringifierReadOnlyAttributeConstructor, 0 } },
    { "identifier"_s, static_cast<unsigned>(JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::CustomAccessor | JSC::PropertyAttribute::DOMAttribute), NoIntrinsic, { HashTableValue::GetterSetterType, jsTestStringifierReadOnlyAttribute_identifier, 0 } },
    { "toString"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { HashTableValue::NativeFunctionType, jsTestStringifierReadOnlyAttributePrototypeFunction_toString, 0 } },
};

const ClassInfo JSTestStringifierReadOnlyAttributePrototype::s_info = { "TestStringifierReadOnlyAttribute"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestStringifierReadOnlyAttributePrototype) };

void JSTestStringifierReadOnlyAttributePrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSTestStringifierReadOnlyAttribute::info(), JSTestStringifierReadOnlyAttributePrototypeTableValues, *this);
    JSC_TO_STRING_TAG_WITHOUT_TRANSITION();
}

const ClassInfo JSTestStringifierReadOnlyAttribute::s_info = { "TestStringifierReadOnlyAttribute"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestStringifierReadOnlyAttribute) };

JSTestStringifierReadOnlyAttribute::JSTestStringifierReadOnlyAttribute(Structure* structure, JSDOMGlobalObject& globalObject, Ref<TestStringifierReadOnlyAttribute>&& impl)
    : JSDOMWrapper<TestStringifierReadOnlyAttribute>(structure, globalObject, WTFMove(impl))
{
}

void JSTestStringifierReadOnlyAttribute::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));

    static_assert(!std::is_base_of<ActiveDOMObject, TestStringifierReadOnlyAttribute>::value, "Interface is not marked as [ActiveDOMObject] even though implementation class subclasses ActiveDOMObject.");

}

JSObject* JSTestStringifierReadOnlyAttribute::createPrototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return JSTestStringifierReadOnlyAttributePrototype::create(vm, &globalObject, JSTestStringifierReadOnlyAttributePrototype::createStructure(vm, &globalObject, globalObject.objectPrototype()));
}

JSObject* JSTestStringifierReadOnlyAttribute::prototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return getDOMPrototype<JSTestStringifierReadOnlyAttribute>(vm, globalObject);
}

JSValue JSTestStringifierReadOnlyAttribute::getConstructor(VM& vm, const JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestStringifierReadOnlyAttributeDOMConstructor, DOMConstructorID::TestStringifierReadOnlyAttribute>(vm, *jsCast<const JSDOMGlobalObject*>(globalObject));
}

void JSTestStringifierReadOnlyAttribute::destroy(JSC::JSCell* cell)
{
    JSTestStringifierReadOnlyAttribute* thisObject = static_cast<JSTestStringifierReadOnlyAttribute*>(cell);
    thisObject->JSTestStringifierReadOnlyAttribute::~JSTestStringifierReadOnlyAttribute();
}

JSC_DEFINE_CUSTOM_GETTER(jsTestStringifierReadOnlyAttributeConstructor, (JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, PropertyName))
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* prototype = jsDynamicCast<JSTestStringifierReadOnlyAttributePrototype*>(JSValue::decode(thisValue));
    if (UNLIKELY(!prototype))
        return throwVMTypeError(lexicalGlobalObject, throwScope);
    return JSValue::encode(JSTestStringifierReadOnlyAttribute::getConstructor(JSC::getVM(lexicalGlobalObject), prototype->globalObject()));
}

static inline JSValue jsTestStringifierReadOnlyAttribute_identifierGetter(JSGlobalObject& lexicalGlobalObject, JSTestStringifierReadOnlyAttribute& thisObject)
{
    auto& vm = JSC::getVM(&lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto& impl = thisObject.wrapped();
    RELEASE_AND_RETURN(throwScope, (toJS<IDLDOMString>(lexicalGlobalObject, throwScope, impl.identifier())));
}

JSC_DEFINE_CUSTOM_GETTER(jsTestStringifierReadOnlyAttribute_identifier, (JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, PropertyName attributeName))
{
    return IDLAttribute<JSTestStringifierReadOnlyAttribute>::get<jsTestStringifierReadOnlyAttribute_identifierGetter, CastedThisErrorBehavior::Assert>(*lexicalGlobalObject, thisValue, attributeName);
}

static inline JSC::EncodedJSValue jsTestStringifierReadOnlyAttributePrototypeFunction_toStringBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestStringifierReadOnlyAttribute>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLDOMString>(*lexicalGlobalObject, throwScope, impl.identifier())));
}

JSC_DEFINE_HOST_FUNCTION(jsTestStringifierReadOnlyAttributePrototypeFunction_toString, (JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSTestStringifierReadOnlyAttribute>::call<jsTestStringifierReadOnlyAttributePrototypeFunction_toStringBody>(*lexicalGlobalObject, *callFrame, "toString");
}

JSC::GCClient::IsoSubspace* JSTestStringifierReadOnlyAttribute::subspaceForImpl(JSC::VM& vm)
{
    return WebCore::subspaceForImpl<JSTestStringifierReadOnlyAttribute, UseCustomHeapCellType::No>(vm,
        [] (auto& spaces) { return spaces.m_clientSubspaceForTestStringifierReadOnlyAttribute.get(); },
        [] (auto& spaces, auto&& space) { spaces.m_clientSubspaceForTestStringifierReadOnlyAttribute = std::forward<decltype(space)>(space); },
        [] (auto& spaces) { return spaces.m_subspaceForTestStringifierReadOnlyAttribute.get(); },
        [] (auto& spaces, auto&& space) { spaces.m_subspaceForTestStringifierReadOnlyAttribute = std::forward<decltype(space)>(space); }
    );
}

void JSTestStringifierReadOnlyAttribute::analyzeHeap(JSCell* cell, HeapAnalyzer& analyzer)
{
    auto* thisObject = jsCast<JSTestStringifierReadOnlyAttribute*>(cell);
    analyzer.setWrappedObjectForCell(cell, &thisObject->wrapped());
    if (thisObject->scriptExecutionContext())
        analyzer.setLabelForCell(cell, "url " + thisObject->scriptExecutionContext()->url().string());
    Base::analyzeHeap(cell, analyzer);
}

bool JSTestStringifierReadOnlyAttributeOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, AbstractSlotVisitor& visitor, const char** reason)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    UNUSED_PARAM(reason);
    return false;
}

void JSTestStringifierReadOnlyAttributeOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsTestStringifierReadOnlyAttribute = static_cast<JSTestStringifierReadOnlyAttribute*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestStringifierReadOnlyAttribute->wrapped(), jsTestStringifierReadOnlyAttribute);
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7TestStringifierReadOnlyAttribute@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore32TestStringifierReadOnlyAttributeE[]; }
#endif
#endif

JSC::JSValue toJSNewlyCreated(JSC::JSGlobalObject*, JSDOMGlobalObject* globalObject, Ref<TestStringifierReadOnlyAttribute>&& impl)
{

    if constexpr (std::is_polymorphic_v<TestStringifierReadOnlyAttribute>) {
#if ENABLE(BINDING_INTEGRITY)
        const void* actualVTablePointer = getVTablePointer(impl.ptr());
#if PLATFORM(WIN)
        void* expectedVTablePointer = __identifier("??_7TestStringifierReadOnlyAttribute@WebCore@@6B@");
#else
        void* expectedVTablePointer = &_ZTVN7WebCore32TestStringifierReadOnlyAttributeE[2];
#endif

        // If you hit this assertion you either have a use after free bug, or
        // TestStringifierReadOnlyAttribute has subclasses. If TestStringifierReadOnlyAttribute has subclasses that get passed
        // to toJS() we currently require TestStringifierReadOnlyAttribute you to opt out of binding hardening
        // by adding the SkipVTableValidation attribute to the interface IDL definition
        RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    }
    return createWrapper<TestStringifierReadOnlyAttribute>(globalObject, WTFMove(impl));
}

JSC::JSValue toJS(JSC::JSGlobalObject* lexicalGlobalObject, JSDOMGlobalObject* globalObject, TestStringifierReadOnlyAttribute& impl)
{
    return wrap(lexicalGlobalObject, globalObject, impl);
}

TestStringifierReadOnlyAttribute* JSTestStringifierReadOnlyAttribute::toWrapped(JSC::VM&, JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSTestStringifierReadOnlyAttribute*>(value))
        return &wrapper->wrapped();
    return nullptr;
}

}
