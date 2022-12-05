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
#include "JSTestOverrideBuiltins.h"

#include "ActiveDOMObject.h"
#include "DOMIsoSubspaces.h"
#include "JSDOMAbstractOperations.h"
#include "JSDOMBinding.h"
#include "JSDOMConstructorNotConstructable.h"
#include "JSDOMConvertInterface.h"
#include "JSDOMConvertStrings.h"
#include "JSDOMExceptionHandling.h"
#include "JSDOMGlobalObject.h"
#include "JSDOMOperation.h"
#include "JSDOMWrapperCache.h"
#include "JSNode.h"
#include "ScriptExecutionContext.h"
#include "WebCoreJSClientData.h"
#include <JavaScriptCore/FunctionPrototype.h>
#include <JavaScriptCore/HeapAnalyzer.h>
#include <JavaScriptCore/JSCInlines.h>
#include <JavaScriptCore/JSDestructibleObjectHeapCellType.h>
#include <JavaScriptCore/SubspaceInlines.h>
#include <wtf/GetPtr.h>
#include <wtf/PointerPreparations.h>
#include <wtf/URL.h>


namespace WebCore {
using namespace JSC;

// Functions

JSC::EncodedJSValue JSC_HOST_CALL jsTestOverrideBuiltinsPrototypeFunctionNamedItem(JSC::JSGlobalObject*, JSC::CallFrame*);

// Attributes

JSC::EncodedJSValue jsTestOverrideBuiltinsConstructor(JSC::JSGlobalObject*, JSC::EncodedJSValue, JSC::PropertyName);
bool setJSTestOverrideBuiltinsConstructor(JSC::JSGlobalObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);

class JSTestOverrideBuiltinsPrototype final : public JSC::JSNonFinalObject {
public:
    using Base = JSC::JSNonFinalObject;
    static JSTestOverrideBuiltinsPrototype* create(JSC::VM& vm, JSDOMGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSTestOverrideBuiltinsPrototype* ptr = new (NotNull, JSC::allocateCell<JSTestOverrideBuiltinsPrototype>(vm.heap)) JSTestOverrideBuiltinsPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    template<typename CellType, JSC::SubspaceAccess>
    static JSC::IsoSubspace* subspaceFor(JSC::VM& vm)
    {
        STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSTestOverrideBuiltinsPrototype, Base);
        return &vm.plainObjectSpace;
    }
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSTestOverrideBuiltinsPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};
STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSTestOverrideBuiltinsPrototype, JSTestOverrideBuiltinsPrototype::Base);

using JSTestOverrideBuiltinsConstructor = JSDOMConstructorNotConstructable<JSTestOverrideBuiltins>;

template<> JSValue JSTestOverrideBuiltinsConstructor::prototypeForStructure(JSC::VM& vm, const JSDOMGlobalObject& globalObject)
{
    UNUSED_PARAM(vm);
    return globalObject.functionPrototype();
}

template<> void JSTestOverrideBuiltinsConstructor::initializeProperties(VM& vm, JSDOMGlobalObject& globalObject)
{
    putDirect(vm, vm.propertyNames->prototype, JSTestOverrideBuiltins::prototype(vm, globalObject), JSC::PropertyAttribute::DontDelete | JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(vm, "TestOverrideBuiltins"_s), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
}

template<> const ClassInfo JSTestOverrideBuiltinsConstructor::s_info = { "TestOverrideBuiltins", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestOverrideBuiltinsConstructor) };

/* Hash table for prototype */

static const HashTableValue JSTestOverrideBuiltinsPrototypeTableValues[] =
{
    { "constructor", static_cast<unsigned>(JSC::PropertyAttribute::DontEnum), NoIntrinsic, { (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestOverrideBuiltinsConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSTestOverrideBuiltinsConstructor) } },
    { "namedItem", static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { (intptr_t)static_cast<RawNativeFunction>(jsTestOverrideBuiltinsPrototypeFunctionNamedItem), (intptr_t) (1) } },
};

const ClassInfo JSTestOverrideBuiltinsPrototype::s_info = { "TestOverrideBuiltins", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestOverrideBuiltinsPrototype) };

void JSTestOverrideBuiltinsPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSTestOverrideBuiltins::info(), JSTestOverrideBuiltinsPrototypeTableValues, *this);
    JSC_TO_STRING_TAG_WITHOUT_TRANSITION();
}

const ClassInfo JSTestOverrideBuiltins::s_info = { "TestOverrideBuiltins", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestOverrideBuiltins) };

JSTestOverrideBuiltins::JSTestOverrideBuiltins(Structure* structure, JSDOMGlobalObject& globalObject, Ref<TestOverrideBuiltins>&& impl)
    : JSDOMWrapper<TestOverrideBuiltins>(structure, globalObject, WTFMove(impl))
{
}

void JSTestOverrideBuiltins::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(vm, info()));

    static_assert(!std::is_base_of<ActiveDOMObject, TestOverrideBuiltins>::value, "Interface is not marked as [ActiveDOMObject] even though implementation class subclasses ActiveDOMObject.");

}

JSObject* JSTestOverrideBuiltins::createPrototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return JSTestOverrideBuiltinsPrototype::create(vm, &globalObject, JSTestOverrideBuiltinsPrototype::createStructure(vm, &globalObject, globalObject.objectPrototype()));
}

JSObject* JSTestOverrideBuiltins::prototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return getDOMPrototype<JSTestOverrideBuiltins>(vm, globalObject);
}

JSValue JSTestOverrideBuiltins::getConstructor(VM& vm, const JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestOverrideBuiltinsConstructor>(vm, *jsCast<const JSDOMGlobalObject*>(globalObject));
}

void JSTestOverrideBuiltins::destroy(JSC::JSCell* cell)
{
    JSTestOverrideBuiltins* thisObject = static_cast<JSTestOverrideBuiltins*>(cell);
    thisObject->JSTestOverrideBuiltins::~JSTestOverrideBuiltins();
}

bool JSTestOverrideBuiltins::getOwnPropertySlot(JSObject* object, JSGlobalObject* lexicalGlobalObject, PropertyName propertyName, PropertySlot& slot)
{
    auto* thisObject = jsCast<JSTestOverrideBuiltins*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    using GetterIDLType = IDLInterface<Node>;
    auto getterFunctor = [] (auto& thisObject, auto propertyName) -> Optional<typename GetterIDLType::ImplementationType> {
        auto result = thisObject.wrapped().namedItem(propertyNameToAtomString(propertyName));
        if (!GetterIDLType::isNullValue(result))
            return typename GetterIDLType::ImplementationType { GetterIDLType::extractValueFromNullable(result) };
        return WTF::nullopt;
    };
    if (auto namedProperty = accessVisibleNamedProperty<OverrideBuiltins::Yes>(*lexicalGlobalObject, *thisObject, propertyName, getterFunctor)) {
        auto value = toJS<IDLInterface<Node>>(*lexicalGlobalObject, *thisObject->globalObject(), WTFMove(namedProperty.value()));
        slot.setValue(thisObject, static_cast<unsigned>(JSC::PropertyAttribute::ReadOnly), value);
        return true;
    }
    return JSObject::getOwnPropertySlot(object, lexicalGlobalObject, propertyName, slot);
}

bool JSTestOverrideBuiltins::getOwnPropertySlotByIndex(JSObject* object, JSGlobalObject* lexicalGlobalObject, unsigned index, PropertySlot& slot)
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto* thisObject = jsCast<JSTestOverrideBuiltins*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    auto propertyName = Identifier::from(vm, index);
    using GetterIDLType = IDLInterface<Node>;
    auto getterFunctor = [] (auto& thisObject, auto propertyName) -> Optional<typename GetterIDLType::ImplementationType> {
        auto result = thisObject.wrapped().namedItem(propertyNameToAtomString(propertyName));
        if (!GetterIDLType::isNullValue(result))
            return typename GetterIDLType::ImplementationType { GetterIDLType::extractValueFromNullable(result) };
        return WTF::nullopt;
    };
    if (auto namedProperty = accessVisibleNamedProperty<OverrideBuiltins::Yes>(*lexicalGlobalObject, *thisObject, propertyName, getterFunctor)) {
        auto value = toJS<IDLInterface<Node>>(*lexicalGlobalObject, *thisObject->globalObject(), WTFMove(namedProperty.value()));
        slot.setValue(thisObject, static_cast<unsigned>(JSC::PropertyAttribute::ReadOnly), value);
        return true;
    }
    return JSObject::getOwnPropertySlotByIndex(object, lexicalGlobalObject, index, slot);
}

void JSTestOverrideBuiltins::getOwnPropertyNames(JSObject* object, JSGlobalObject* lexicalGlobalObject, PropertyNameArray& propertyNames, EnumerationMode mode)
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto* thisObject = jsCast<JSTestOverrideBuiltins*>(object);
    ASSERT_GC_OBJECT_INHERITS(object, info());
    for (auto& propertyName : thisObject->wrapped().supportedPropertyNames())
        propertyNames.add(Identifier::fromString(vm, propertyName));
    JSObject::getOwnPropertyNames(object, lexicalGlobalObject, propertyNames, mode);
}

template<> inline JSTestOverrideBuiltins* IDLOperation<JSTestOverrideBuiltins>::cast(JSGlobalObject& lexicalGlobalObject, CallFrame& callFrame)
{
    return jsDynamicCast<JSTestOverrideBuiltins*>(JSC::getVM(&lexicalGlobalObject), callFrame.thisValue());
}

EncodedJSValue jsTestOverrideBuiltinsConstructor(JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, PropertyName)
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* prototype = jsDynamicCast<JSTestOverrideBuiltinsPrototype*>(vm, JSValue::decode(thisValue));
    if (UNLIKELY(!prototype))
        return throwVMTypeError(lexicalGlobalObject, throwScope);
    return JSValue::encode(JSTestOverrideBuiltins::getConstructor(JSC::getVM(lexicalGlobalObject), prototype->globalObject()));
}

bool setJSTestOverrideBuiltinsConstructor(JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* prototype = jsDynamicCast<JSTestOverrideBuiltinsPrototype*>(vm, JSValue::decode(thisValue));
    if (UNLIKELY(!prototype)) {
        throwVMTypeError(lexicalGlobalObject, throwScope);
        return false;
    }
    // Shadowing a built-in constructor
    return prototype->putDirect(vm, vm.propertyNames->constructor, JSValue::decode(encodedValue));
}

static inline JSC::EncodedJSValue jsTestOverrideBuiltinsPrototypeFunctionNamedItemBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestOverrideBuiltins>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    if (UNLIKELY(callFrame->argumentCount() < 1))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    EnsureStillAliveScope argument0 = callFrame->uncheckedArgument(0);
    auto name = convert<IDLDOMString>(*lexicalGlobalObject, argument0.value());
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLInterface<Node>>(*lexicalGlobalObject, *castedThis->globalObject(), impl.namedItem(WTFMove(name)))));
}

EncodedJSValue JSC_HOST_CALL jsTestOverrideBuiltinsPrototypeFunctionNamedItem(JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame)
{
    return IDLOperation<JSTestOverrideBuiltins>::call<jsTestOverrideBuiltinsPrototypeFunctionNamedItemBody>(*lexicalGlobalObject, *callFrame, "namedItem");
}

JSC::IsoSubspace* JSTestOverrideBuiltins::subspaceForImpl(JSC::VM& vm)
{
    auto& clientData = *static_cast<JSVMClientData*>(vm.clientData);
    auto& spaces = clientData.subspaces();
    if (auto* space = spaces.m_subspaceForTestOverrideBuiltins.get())
        return space;
    static_assert(std::is_base_of_v<JSC::JSDestructibleObject, JSTestOverrideBuiltins> || !JSTestOverrideBuiltins::needsDestruction);
    if constexpr (std::is_base_of_v<JSC::JSDestructibleObject, JSTestOverrideBuiltins>)
        spaces.m_subspaceForTestOverrideBuiltins = makeUnique<IsoSubspace> ISO_SUBSPACE_INIT(vm.heap, vm.destructibleObjectHeapCellType.get(), JSTestOverrideBuiltins);
    else
        spaces.m_subspaceForTestOverrideBuiltins = makeUnique<IsoSubspace> ISO_SUBSPACE_INIT(vm.heap, vm.cellHeapCellType.get(), JSTestOverrideBuiltins);
    auto* space = spaces.m_subspaceForTestOverrideBuiltins.get();
IGNORE_WARNINGS_BEGIN("unreachable-code")
IGNORE_WARNINGS_BEGIN("tautological-compare")
    if (&JSTestOverrideBuiltins::visitOutputConstraints != &JSC::JSCell::visitOutputConstraints)
        clientData.outputConstraintSpaces().append(space);
IGNORE_WARNINGS_END
IGNORE_WARNINGS_END
    return space;
}

void JSTestOverrideBuiltins::analyzeHeap(JSCell* cell, HeapAnalyzer& analyzer)
{
    auto* thisObject = jsCast<JSTestOverrideBuiltins*>(cell);
    analyzer.setWrappedObjectForCell(cell, &thisObject->wrapped());
    if (thisObject->scriptExecutionContext())
        analyzer.setLabelForCell(cell, "url " + thisObject->scriptExecutionContext()->url().string());
    Base::analyzeHeap(cell, analyzer);
}

bool JSTestOverrideBuiltinsOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor, const char** reason)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    UNUSED_PARAM(reason);
    return false;
}

void JSTestOverrideBuiltinsOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsTestOverrideBuiltins = static_cast<JSTestOverrideBuiltins*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestOverrideBuiltins->wrapped(), jsTestOverrideBuiltins);
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7TestOverrideBuiltins@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore20TestOverrideBuiltinsE[]; }
#endif
#endif

JSC::JSValue toJSNewlyCreated(JSC::JSGlobalObject*, JSDOMGlobalObject* globalObject, Ref<TestOverrideBuiltins>&& impl)
{

#if ENABLE(BINDING_INTEGRITY)
    const void* actualVTablePointer = getVTablePointer(impl.ptr());
#if PLATFORM(WIN)
    void* expectedVTablePointer = __identifier("??_7TestOverrideBuiltins@WebCore@@6B@");
#else
    void* expectedVTablePointer = &_ZTVN7WebCore20TestOverrideBuiltinsE[2];
#endif

    // If this fails TestOverrideBuiltins does not have a vtable, so you need to add the
    // ImplementationLacksVTable attribute to the interface definition
    static_assert(std::is_polymorphic<TestOverrideBuiltins>::value, "TestOverrideBuiltins is not polymorphic");

    // If you hit this assertion you either have a use after free bug, or
    // TestOverrideBuiltins has subclasses. If TestOverrideBuiltins has subclasses that get passed
    // to toJS() we currently require TestOverrideBuiltins you to opt out of binding hardening
    // by adding the SkipVTableValidation attribute to the interface IDL definition
    RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    return createWrapper<TestOverrideBuiltins>(globalObject, WTFMove(impl));
}

JSC::JSValue toJS(JSC::JSGlobalObject* lexicalGlobalObject, JSDOMGlobalObject* globalObject, TestOverrideBuiltins& impl)
{
    return wrap(lexicalGlobalObject, globalObject, impl);
}

TestOverrideBuiltins* JSTestOverrideBuiltins::toWrapped(JSC::VM& vm, JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSTestOverrideBuiltins*>(vm, value))
        return &wrapper->wrapped();
    return nullptr;
}

}
