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
#include "JSTestNamedSetterWithIndexedGetterAndSetter.h"

#include "ActiveDOMObject.h"
#include "ExtendedDOMClientIsoSubspaces.h"
#include "ExtendedDOMIsoSubspaces.h"
#include "IDLTypes.h"
#include "JSDOMAbstractOperations.h"
#include "JSDOMBinding.h"
#include "JSDOMConstructorNotConstructable.h"
#include "JSDOMConvertBase.h"
#include "JSDOMConvertNumbers.h"
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
#include <JavaScriptCore/PropertyNameArray.h>
#include <JavaScriptCore/SlotVisitorMacros.h>
#include <JavaScriptCore/SubspaceInlines.h>
#include <wtf/GetPtr.h>
#include <wtf/PointerPreparations.h>
#include <wtf/URL.h>


namespace WebCore {
using namespace JSC;

// Functions

static JSC_DECLARE_HOST_FUNCTION(jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_namedSetter);
static JSC_DECLARE_HOST_FUNCTION(jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_indexedSetter);

// Attributes

static JSC_DECLARE_CUSTOM_GETTER(jsTestNamedSetterWithIndexedGetterAndSetterConstructor);

class JSTestNamedSetterWithIndexedGetterAndSetterPrototype final : public JSC::JSNonFinalObject {
public:
    using Base = JSC::JSNonFinalObject;
    static JSTestNamedSetterWithIndexedGetterAndSetterPrototype* create(JSC::VM& vm, JSDOMGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSTestNamedSetterWithIndexedGetterAndSetterPrototype* ptr = new (NotNull, JSC::allocateCell<JSTestNamedSetterWithIndexedGetterAndSetterPrototype>(vm)) JSTestNamedSetterWithIndexedGetterAndSetterPrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    template<typename CellType, JSC::SubspaceAccess>
    static JSC::GCClient::IsoSubspace* subspaceFor(JSC::VM& vm)
    {
        STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSTestNamedSetterWithIndexedGetterAndSetterPrototype, Base);
        return &vm.plainObjectSpace();
    }
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSTestNamedSetterWithIndexedGetterAndSetterPrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};
STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSTestNamedSetterWithIndexedGetterAndSetterPrototype, JSTestNamedSetterWithIndexedGetterAndSetterPrototype::Base);

using JSTestNamedSetterWithIndexedGetterAndSetterDOMConstructor = JSDOMConstructorNotConstructable<JSTestNamedSetterWithIndexedGetterAndSetter>;

template<> const ClassInfo JSTestNamedSetterWithIndexedGetterAndSetterDOMConstructor::s_info = { "TestNamedSetterWithIndexedGetterAndSetter"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestNamedSetterWithIndexedGetterAndSetterDOMConstructor) };

template<> JSValue JSTestNamedSetterWithIndexedGetterAndSetterDOMConstructor::prototypeForStructure(JSC::VM& vm, const JSDOMGlobalObject& globalObject)
{
    UNUSED_PARAM(vm);
    return globalObject.functionPrototype();
}

template<> void JSTestNamedSetterWithIndexedGetterAndSetterDOMConstructor::initializeProperties(VM& vm, JSDOMGlobalObject& globalObject)
{
    putDirect(vm, vm.propertyNames->length, jsNumber(0), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    JSString* nameString = jsNontrivialString(vm, "TestNamedSetterWithIndexedGetterAndSetter"_s);
    m_originalName.set(vm, this, nameString);
    putDirect(vm, vm.propertyNames->name, nameString, JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    putDirect(vm, vm.propertyNames->prototype, JSTestNamedSetterWithIndexedGetterAndSetter::prototype(vm, globalObject), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum | JSC::PropertyAttribute::DontDelete);
}

/* Hash table for prototype */

static const HashTableValue JSTestNamedSetterWithIndexedGetterAndSetterPrototypeTableValues[] =
{
    { "constructor"_s, static_cast<unsigned>(JSC::PropertyAttribute::DontEnum), NoIntrinsic, { HashTableValue::GetterSetterType, jsTestNamedSetterWithIndexedGetterAndSetterConstructor, 0 } },
    { "namedSetter"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { HashTableValue::NativeFunctionType, jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_namedSetter, 2 } },
    { "indexedSetter"_s, static_cast<unsigned>(JSC::PropertyAttribute::Function), NoIntrinsic, { HashTableValue::NativeFunctionType, jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_indexedSetter, 1 } },
};

const ClassInfo JSTestNamedSetterWithIndexedGetterAndSetterPrototype::s_info = { "TestNamedSetterWithIndexedGetterAndSetter"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestNamedSetterWithIndexedGetterAndSetterPrototype) };

void JSTestNamedSetterWithIndexedGetterAndSetterPrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSTestNamedSetterWithIndexedGetterAndSetter::info(), JSTestNamedSetterWithIndexedGetterAndSetterPrototypeTableValues, *this);
    JSC_TO_STRING_TAG_WITHOUT_TRANSITION();
}

const ClassInfo JSTestNamedSetterWithIndexedGetterAndSetter::s_info = { "TestNamedSetterWithIndexedGetterAndSetter"_s, &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestNamedSetterWithIndexedGetterAndSetter) };

JSTestNamedSetterWithIndexedGetterAndSetter::JSTestNamedSetterWithIndexedGetterAndSetter(Structure* structure, JSDOMGlobalObject& globalObject, Ref<TestNamedSetterWithIndexedGetterAndSetter>&& impl)
    : JSDOMWrapper<TestNamedSetterWithIndexedGetterAndSetter>(structure, globalObject, WTFMove(impl))
{
}

void JSTestNamedSetterWithIndexedGetterAndSetter::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(info()));

    static_assert(!std::is_base_of<ActiveDOMObject, TestNamedSetterWithIndexedGetterAndSetter>::value, "Interface is not marked as [ActiveDOMObject] even though implementation class subclasses ActiveDOMObject.");

}

JSObject* JSTestNamedSetterWithIndexedGetterAndSetter::createPrototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return JSTestNamedSetterWithIndexedGetterAndSetterPrototype::create(vm, &globalObject, JSTestNamedSetterWithIndexedGetterAndSetterPrototype::createStructure(vm, &globalObject, globalObject.objectPrototype()));
}

JSObject* JSTestNamedSetterWithIndexedGetterAndSetter::prototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return getDOMPrototype<JSTestNamedSetterWithIndexedGetterAndSetter>(vm, globalObject);
}

JSValue JSTestNamedSetterWithIndexedGetterAndSetter::getConstructor(VM& vm, const JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestNamedSetterWithIndexedGetterAndSetterDOMConstructor, DOMConstructorID::TestNamedSetterWithIndexedGetterAndSetter>(vm, *jsCast<const JSDOMGlobalObject*>(globalObject));
}

void JSTestNamedSetterWithIndexedGetterAndSetter::destroy(JSC::JSCell* cell)
{
    JSTestNamedSetterWithIndexedGetterAndSetter* thisObject = static_cast<JSTestNamedSetterWithIndexedGetterAndSetter*>(cell);
    thisObject->JSTestNamedSetterWithIndexedGetterAndSetter::~JSTestNamedSetterWithIndexedGetterAndSetter();
}

bool JSTestNamedSetterWithIndexedGetterAndSetter::getOwnPropertySlot(JSObject* object, JSGlobalObject* lexicalGlobalObject, PropertyName propertyName, PropertySlot& slot)
{
    auto throwScope = DECLARE_THROW_SCOPE(JSC::getVM(lexicalGlobalObject));
    auto* thisObject = jsCast<JSTestNamedSetterWithIndexedGetterAndSetter*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    if (auto index = parseIndex(propertyName)) {
        if (index.value() < thisObject->wrapped().length()) {
            auto value = toJS<IDLDOMString>(*lexicalGlobalObject, throwScope, thisObject->wrapped().indexedSetter(index.value()));
            RETURN_IF_EXCEPTION(throwScope, false);
            slot.setValue(thisObject, static_cast<unsigned>(0), value);
            return true;
        }
        return JSObject::getOwnPropertySlot(object, lexicalGlobalObject, propertyName, slot);
    }
    using GetterIDLType = IDLDOMString;
    auto getterFunctor = visibleNamedPropertyItemAccessorFunctor<GetterIDLType, JSTestNamedSetterWithIndexedGetterAndSetter>([] (JSTestNamedSetterWithIndexedGetterAndSetter& thisObject, PropertyName propertyName) -> decltype(auto) {
        return thisObject.wrapped().namedItem(propertyNameToAtomString(propertyName));
    });
    if (auto namedProperty = accessVisibleNamedProperty<LegacyOverrideBuiltIns::No>(*lexicalGlobalObject, *thisObject, propertyName, getterFunctor)) {
        auto value = toJS<IDLDOMString>(*lexicalGlobalObject, throwScope, WTFMove(namedProperty.value()));
        RETURN_IF_EXCEPTION(throwScope, false);
        slot.setValue(thisObject, static_cast<unsigned>(0), value);
        return true;
    }
    return JSObject::getOwnPropertySlot(object, lexicalGlobalObject, propertyName, slot);
}

bool JSTestNamedSetterWithIndexedGetterAndSetter::getOwnPropertySlotByIndex(JSObject* object, JSGlobalObject* lexicalGlobalObject, unsigned index, PropertySlot& slot)
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* thisObject = jsCast<JSTestNamedSetterWithIndexedGetterAndSetter*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());
    if (LIKELY(index <= MAX_ARRAY_INDEX)) {
        if (index < thisObject->wrapped().length()) {
            auto value = toJS<IDLDOMString>(*lexicalGlobalObject, throwScope, thisObject->wrapped().indexedSetter(index));
            RETURN_IF_EXCEPTION(throwScope, false);
            slot.setValue(thisObject, static_cast<unsigned>(0), value);
            return true;
        }
        return JSObject::getOwnPropertySlotByIndex(object, lexicalGlobalObject, index, slot);
    }
    auto propertyName = Identifier::from(vm, index);
    using GetterIDLType = IDLDOMString;
    auto getterFunctor = visibleNamedPropertyItemAccessorFunctor<GetterIDLType, JSTestNamedSetterWithIndexedGetterAndSetter>([] (JSTestNamedSetterWithIndexedGetterAndSetter& thisObject, PropertyName propertyName) -> decltype(auto) {
        return thisObject.wrapped().namedItem(propertyNameToAtomString(propertyName));
    });
    if (auto namedProperty = accessVisibleNamedProperty<LegacyOverrideBuiltIns::No>(*lexicalGlobalObject, *thisObject, propertyName, getterFunctor)) {
        auto value = toJS<IDLDOMString>(*lexicalGlobalObject, throwScope, WTFMove(namedProperty.value()));
        RETURN_IF_EXCEPTION(throwScope, false);
        slot.setValue(thisObject, static_cast<unsigned>(0), value);
        return true;
    }
    return JSObject::getOwnPropertySlotByIndex(object, lexicalGlobalObject, index, slot);
}

void JSTestNamedSetterWithIndexedGetterAndSetter::getOwnPropertyNames(JSObject* object, JSGlobalObject* lexicalGlobalObject, PropertyNameArray& propertyNames, DontEnumPropertiesMode mode)
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto* thisObject = jsCast<JSTestNamedSetterWithIndexedGetterAndSetter*>(object);
    ASSERT_GC_OBJECT_INHERITS(object, info());
    for (unsigned i = 0, count = thisObject->wrapped().length(); i < count; ++i)
        propertyNames.add(Identifier::from(vm, i));
    for (auto& propertyName : thisObject->wrapped().supportedPropertyNames())
        propertyNames.add(Identifier::fromString(vm, propertyName));
    JSObject::getOwnPropertyNames(object, lexicalGlobalObject, propertyNames, mode);
}

bool JSTestNamedSetterWithIndexedGetterAndSetter::put(JSCell* cell, JSGlobalObject* lexicalGlobalObject, PropertyName propertyName, JSValue value, PutPropertySlot& putPropertySlot)
{
    auto* thisObject = jsCast<JSTestNamedSetterWithIndexedGetterAndSetter*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());

    if (UNLIKELY(thisObject != putPropertySlot.thisValue()))
        return JSObject::put(thisObject, lexicalGlobalObject, propertyName, value, putPropertySlot);
    auto throwScope = DECLARE_THROW_SCOPE(lexicalGlobalObject->vm());

    if (auto index = parseIndex(propertyName)) {
        auto nativeValue = convert<IDLDOMString>(*lexicalGlobalObject, value);
        RETURN_IF_EXCEPTION(throwScope, true);
        invokeFunctorPropagatingExceptionIfNecessary(*lexicalGlobalObject, throwScope, [&] { return thisObject->wrapped().indexedSetter(index.value(), WTFMove(nativeValue)); });
        return true;
    }

    if (!propertyName.isSymbol()) {
        PropertySlot slot { thisObject, PropertySlot::InternalMethodType::VMInquiry, &lexicalGlobalObject->vm() };
        JSValue prototype = thisObject->getPrototypeDirect();
        bool found = prototype.isObject() && asObject(prototype)->getPropertySlot(lexicalGlobalObject, propertyName, slot);
        slot.disallowVMEntry.reset();
        RETURN_IF_EXCEPTION(throwScope, false);
        if (!found) {
            auto nativeValue = convert<IDLDOMString>(*lexicalGlobalObject, value);
            RETURN_IF_EXCEPTION(throwScope, true);
            invokeFunctorPropagatingExceptionIfNecessary(*lexicalGlobalObject, throwScope, [&] { return thisObject->wrapped().namedSetter(propertyNameToString(propertyName), WTFMove(nativeValue)); });
            return true;
        }
    }

    throwScope.assertNoException();
    RELEASE_AND_RETURN(throwScope, JSObject::put(thisObject, lexicalGlobalObject, propertyName, value, putPropertySlot));
}

bool JSTestNamedSetterWithIndexedGetterAndSetter::putByIndex(JSCell* cell, JSGlobalObject* lexicalGlobalObject, unsigned index, JSValue value, bool shouldThrow)
{
    auto* thisObject = jsCast<JSTestNamedSetterWithIndexedGetterAndSetter*>(cell);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());

    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);

    if (LIKELY(index <= MAX_ARRAY_INDEX)) {
        auto nativeValue = convert<IDLDOMString>(*lexicalGlobalObject, value);
        RETURN_IF_EXCEPTION(throwScope, true);
        invokeFunctorPropagatingExceptionIfNecessary(*lexicalGlobalObject, throwScope, [&] { return thisObject->wrapped().indexedSetter(index, WTFMove(nativeValue)); });
        return true;
    }

    auto propertyName = Identifier::from(vm, index);
    PropertySlot slot { thisObject, PropertySlot::InternalMethodType::VMInquiry, &vm };
    JSValue prototype = thisObject->getPrototypeDirect();
    bool found = prototype.isObject() && asObject(prototype)->getPropertySlot(lexicalGlobalObject, propertyName, slot);
    slot.disallowVMEntry.reset();
    RETURN_IF_EXCEPTION(throwScope, false);
    if (!found) {
        auto nativeValue = convert<IDLDOMString>(*lexicalGlobalObject, value);
        RETURN_IF_EXCEPTION(throwScope, true);
        invokeFunctorPropagatingExceptionIfNecessary(*lexicalGlobalObject, throwScope, [&] { return thisObject->wrapped().namedSetter(propertyNameToString(propertyName), WTFMove(nativeValue)); });
        return true;
    }

    throwScope.assertNoException();
    RELEASE_AND_RETURN(throwScope, JSObject::putByIndex(cell, lexicalGlobalObject, index, value, shouldThrow));
}

bool JSTestNamedSetterWithIndexedGetterAndSetter::defineOwnProperty(JSObject* object, JSGlobalObject* lexicalGlobalObject, PropertyName propertyName, const PropertyDescriptor& propertyDescriptor, bool shouldThrow)
{
    auto* thisObject = jsCast<JSTestNamedSetterWithIndexedGetterAndSetter*>(object);
    ASSERT_GC_OBJECT_INHERITS(thisObject, info());

    auto throwScope = DECLARE_THROW_SCOPE(lexicalGlobalObject->vm());

    if (auto index = parseIndex(propertyName)) {
        if (!propertyDescriptor.isDataDescriptor())
            return false;
        auto nativeValue = convert<IDLDOMString>(*lexicalGlobalObject, propertyDescriptor.value());
        RETURN_IF_EXCEPTION(throwScope, true);
        invokeFunctorPropagatingExceptionIfNecessary(*lexicalGlobalObject, throwScope, [&] { return thisObject->wrapped().indexedSetter(index.value(), WTFMove(nativeValue)); });
        return true;
    }

    if (!propertyName.isSymbol()) {
        PropertySlot slot { thisObject, PropertySlot::InternalMethodType::VMInquiry, &lexicalGlobalObject->vm() };
        bool found = JSObject::getOwnPropertySlot(thisObject, lexicalGlobalObject, propertyName, slot);
        slot.disallowVMEntry.reset();
        RETURN_IF_EXCEPTION(throwScope, false);
        if (!found) {
            if (!propertyDescriptor.isDataDescriptor())
                return false;
            auto nativeValue = convert<IDLDOMString>(*lexicalGlobalObject, propertyDescriptor.value());
            RETURN_IF_EXCEPTION(throwScope, true);
            invokeFunctorPropagatingExceptionIfNecessary(*lexicalGlobalObject, throwScope, [&] { return thisObject->wrapped().namedSetter(propertyNameToString(propertyName), WTFMove(nativeValue)); });
            return true;
        }
    }

    PropertyDescriptor newPropertyDescriptor = propertyDescriptor;
    newPropertyDescriptor.setConfigurable(true);
    throwScope.release();
    return JSObject::defineOwnProperty(object, lexicalGlobalObject, propertyName, newPropertyDescriptor, shouldThrow);
}

JSC_DEFINE_CUSTOM_GETTER(jsTestNamedSetterWithIndexedGetterAndSetterConstructor, (JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, PropertyName))
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* prototype = jsDynamicCast<JSTestNamedSetterWithIndexedGetterAndSetterPrototype*>(JSValue::decode(thisValue));
    if (UNLIKELY(!prototype))
        return throwVMTypeError(lexicalGlobalObject, throwScope);
    return JSValue::encode(JSTestNamedSetterWithIndexedGetterAndSetter::getConstructor(JSC::getVM(lexicalGlobalObject), prototype->globalObject()));
}

static inline JSC::EncodedJSValue jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_namedSetterBody(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestNamedSetterWithIndexedGetterAndSetter>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    if (UNLIKELY(callFrame->argumentCount() < 2))
        return throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject));
    EnsureStillAliveScope argument0 = callFrame->uncheckedArgument(0);
    auto name = convert<IDLDOMString>(*lexicalGlobalObject, argument0.value());
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    EnsureStillAliveScope argument1 = callFrame->uncheckedArgument(1);
    auto value = convert<IDLDOMString>(*lexicalGlobalObject, argument1.value());
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLUndefined>(*lexicalGlobalObject, throwScope, [&]() -> decltype(auto) { return impl.namedSetter(WTFMove(name), WTFMove(value)); })));
}

JSC_DEFINE_HOST_FUNCTION(jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_namedSetter, (JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSTestNamedSetterWithIndexedGetterAndSetter>::call<jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_namedSetterBody>(*lexicalGlobalObject, *callFrame, "namedSetter");
}

static inline JSC::EncodedJSValue jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_indexedSetter1Body(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestNamedSetterWithIndexedGetterAndSetter>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    EnsureStillAliveScope argument0 = callFrame->uncheckedArgument(0);
    auto index = convert<IDLUnsignedLong>(*lexicalGlobalObject, argument0.value());
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    EnsureStillAliveScope argument1 = callFrame->uncheckedArgument(1);
    auto value = convert<IDLDOMString>(*lexicalGlobalObject, argument1.value());
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLUndefined>(*lexicalGlobalObject, throwScope, [&]() -> decltype(auto) { return impl.indexedSetter(WTFMove(index), WTFMove(value)); })));
}

static inline JSC::EncodedJSValue jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_indexedSetter2Body(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestNamedSetterWithIndexedGetterAndSetter>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    auto& impl = castedThis->wrapped();
    EnsureStillAliveScope argument0 = callFrame->uncheckedArgument(0);
    auto index = convert<IDLUnsignedLong>(*lexicalGlobalObject, argument0.value());
    RETURN_IF_EXCEPTION(throwScope, encodedJSValue());
    RELEASE_AND_RETURN(throwScope, JSValue::encode(toJS<IDLDOMString>(*lexicalGlobalObject, throwScope, impl.indexedSetter(WTFMove(index)))));
}

static inline JSC::EncodedJSValue jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_indexedSetterOverloadDispatcher(JSC::JSGlobalObject* lexicalGlobalObject, JSC::CallFrame* callFrame, typename IDLOperation<JSTestNamedSetterWithIndexedGetterAndSetter>::ClassParameter castedThis)
{
    auto& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    UNUSED_PARAM(throwScope);
    UNUSED_PARAM(callFrame);
    size_t argsCount = std::min<size_t>(2, callFrame->argumentCount());
    if (argsCount == 1) {
        RELEASE_AND_RETURN(throwScope, (jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_indexedSetter2Body(lexicalGlobalObject, callFrame, castedThis)));
    }
    if (argsCount == 2) {
        RELEASE_AND_RETURN(throwScope, (jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_indexedSetter1Body(lexicalGlobalObject, callFrame, castedThis)));
    }
    return argsCount < 1 ? throwVMError(lexicalGlobalObject, throwScope, createNotEnoughArgumentsError(lexicalGlobalObject)) : throwVMTypeError(lexicalGlobalObject, throwScope);
}

JSC_DEFINE_HOST_FUNCTION(jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_indexedSetter, (JSGlobalObject* lexicalGlobalObject, CallFrame* callFrame))
{
    return IDLOperation<JSTestNamedSetterWithIndexedGetterAndSetter>::call<jsTestNamedSetterWithIndexedGetterAndSetterPrototypeFunction_indexedSetterOverloadDispatcher>(*lexicalGlobalObject, *callFrame, "indexedSetter");
}

JSC::GCClient::IsoSubspace* JSTestNamedSetterWithIndexedGetterAndSetter::subspaceForImpl(JSC::VM& vm)
{
    return WebCore::subspaceForImpl<JSTestNamedSetterWithIndexedGetterAndSetter, UseCustomHeapCellType::No>(vm,
        [] (auto& spaces) { return spaces.m_clientSubspaceForTestNamedSetterWithIndexedGetterAndSetter.get(); },
        [] (auto& spaces, auto&& space) { spaces.m_clientSubspaceForTestNamedSetterWithIndexedGetterAndSetter = std::forward<decltype(space)>(space); },
        [] (auto& spaces) { return spaces.m_subspaceForTestNamedSetterWithIndexedGetterAndSetter.get(); },
        [] (auto& spaces, auto&& space) { spaces.m_subspaceForTestNamedSetterWithIndexedGetterAndSetter = std::forward<decltype(space)>(space); }
    );
}

void JSTestNamedSetterWithIndexedGetterAndSetter::analyzeHeap(JSCell* cell, HeapAnalyzer& analyzer)
{
    auto* thisObject = jsCast<JSTestNamedSetterWithIndexedGetterAndSetter*>(cell);
    analyzer.setWrappedObjectForCell(cell, &thisObject->wrapped());
    if (thisObject->scriptExecutionContext())
        analyzer.setLabelForCell(cell, "url " + thisObject->scriptExecutionContext()->url().string());
    Base::analyzeHeap(cell, analyzer);
}

bool JSTestNamedSetterWithIndexedGetterAndSetterOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, AbstractSlotVisitor& visitor, const char** reason)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    UNUSED_PARAM(reason);
    return false;
}

void JSTestNamedSetterWithIndexedGetterAndSetterOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsTestNamedSetterWithIndexedGetterAndSetter = static_cast<JSTestNamedSetterWithIndexedGetterAndSetter*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestNamedSetterWithIndexedGetterAndSetter->wrapped(), jsTestNamedSetterWithIndexedGetterAndSetter);
}

#if ENABLE(BINDING_INTEGRITY)
#if PLATFORM(WIN)
#pragma warning(disable: 4483)
extern "C" { extern void (*const __identifier("??_7TestNamedSetterWithIndexedGetterAndSetter@WebCore@@6B@")[])(); }
#else
extern "C" { extern void* _ZTVN7WebCore41TestNamedSetterWithIndexedGetterAndSetterE[]; }
#endif
#endif

JSC::JSValue toJSNewlyCreated(JSC::JSGlobalObject*, JSDOMGlobalObject* globalObject, Ref<TestNamedSetterWithIndexedGetterAndSetter>&& impl)
{

    if constexpr (std::is_polymorphic_v<TestNamedSetterWithIndexedGetterAndSetter>) {
#if ENABLE(BINDING_INTEGRITY)
        const void* actualVTablePointer = getVTablePointer(impl.ptr());
#if PLATFORM(WIN)
        void* expectedVTablePointer = __identifier("??_7TestNamedSetterWithIndexedGetterAndSetter@WebCore@@6B@");
#else
        void* expectedVTablePointer = &_ZTVN7WebCore41TestNamedSetterWithIndexedGetterAndSetterE[2];
#endif

        // If you hit this assertion you either have a use after free bug, or
        // TestNamedSetterWithIndexedGetterAndSetter has subclasses. If TestNamedSetterWithIndexedGetterAndSetter has subclasses that get passed
        // to toJS() we currently require TestNamedSetterWithIndexedGetterAndSetter you to opt out of binding hardening
        // by adding the SkipVTableValidation attribute to the interface IDL definition
        RELEASE_ASSERT(actualVTablePointer == expectedVTablePointer);
#endif
    }
    return createWrapper<TestNamedSetterWithIndexedGetterAndSetter>(globalObject, WTFMove(impl));
}

JSC::JSValue toJS(JSC::JSGlobalObject* lexicalGlobalObject, JSDOMGlobalObject* globalObject, TestNamedSetterWithIndexedGetterAndSetter& impl)
{
    return wrap(lexicalGlobalObject, globalObject, impl);
}

TestNamedSetterWithIndexedGetterAndSetter* JSTestNamedSetterWithIndexedGetterAndSetter::toWrapped(JSC::VM&, JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSTestNamedSetterWithIndexedGetterAndSetter*>(value))
        return &wrapper->wrapped();
    return nullptr;
}

}
