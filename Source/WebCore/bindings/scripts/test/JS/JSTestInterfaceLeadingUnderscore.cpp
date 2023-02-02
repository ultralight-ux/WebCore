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
#include "JSTestInterfaceLeadingUnderscore.h"

#include "ActiveDOMObject.h"
#include "DOMIsoSubspaces.h"
#include "JSDOMAttribute.h"
#include "JSDOMBinding.h"
#include "JSDOMConstructorNotConstructable.h"
#include "JSDOMConvertStrings.h"
#include "JSDOMExceptionHandling.h"
#include "JSDOMWrapperCache.h"
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

// Attributes

JSC::EncodedJSValue jsTestInterfaceLeadingUnderscoreConstructor(JSC::JSGlobalObject*, JSC::EncodedJSValue, JSC::PropertyName);
bool setJSTestInterfaceLeadingUnderscoreConstructor(JSC::JSGlobalObject*, JSC::EncodedJSValue, JSC::EncodedJSValue);
JSC::EncodedJSValue jsTestInterfaceLeadingUnderscoreReadonly(JSC::JSGlobalObject*, JSC::EncodedJSValue, JSC::PropertyName);

class JSTestInterfaceLeadingUnderscorePrototype final : public JSC::JSNonFinalObject {
public:
    using Base = JSC::JSNonFinalObject;
    static JSTestInterfaceLeadingUnderscorePrototype* create(JSC::VM& vm, JSDOMGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSTestInterfaceLeadingUnderscorePrototype* ptr = new (NotNull, JSC::allocateCell<JSTestInterfaceLeadingUnderscorePrototype>(vm.heap)) JSTestInterfaceLeadingUnderscorePrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    template<typename CellType, JSC::SubspaceAccess>
    static JSC::IsoSubspace* subspaceFor(JSC::VM& vm)
    {
        STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSTestInterfaceLeadingUnderscorePrototype, Base);
        return &vm.plainObjectSpace;
    }
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSTestInterfaceLeadingUnderscorePrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};
STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSTestInterfaceLeadingUnderscorePrototype, JSTestInterfaceLeadingUnderscorePrototype::Base);

using JSTestInterfaceLeadingUnderscoreConstructor = JSDOMConstructorNotConstructable<JSTestInterfaceLeadingUnderscore>;

template<> JSValue JSTestInterfaceLeadingUnderscoreConstructor::prototypeForStructure(JSC::VM& vm, const JSDOMGlobalObject& globalObject)
{
    UNUSED_PARAM(vm);
    return globalObject.functionPrototype();
}

template<> void JSTestInterfaceLeadingUnderscoreConstructor::initializeProperties(VM& vm, JSDOMGlobalObject& globalObject)
{
    putDirect(vm, vm.propertyNames->prototype, JSTestInterfaceLeadingUnderscore::prototype(vm, globalObject), JSC::PropertyAttribute::DontDelete | JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    putDirect(vm, vm.propertyNames->name, jsNontrivialString(vm, "TestInterfaceLeadingUnderscore"_s), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
    putDirect(vm, vm.propertyNames->length, jsNumber(0), JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::DontEnum);
}

template<> const ClassInfo JSTestInterfaceLeadingUnderscoreConstructor::s_info = { "TestInterfaceLeadingUnderscore", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestInterfaceLeadingUnderscoreConstructor) };

/* Hash table for prototype */

static const HashTableValue JSTestInterfaceLeadingUnderscorePrototypeTableValues[] =
{
    { "constructor", static_cast<unsigned>(JSC::PropertyAttribute::DontEnum), NoIntrinsic, { (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestInterfaceLeadingUnderscoreConstructor), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(setJSTestInterfaceLeadingUnderscoreConstructor) } },
    { "readonly", static_cast<unsigned>(JSC::PropertyAttribute::ReadOnly | JSC::PropertyAttribute::CustomAccessor | JSC::PropertyAttribute::DOMAttribute), NoIntrinsic, { (intptr_t)static_cast<PropertySlot::GetValueFunc>(jsTestInterfaceLeadingUnderscoreReadonly), (intptr_t) static_cast<PutPropertySlot::PutValueFunc>(0) } },
};

const ClassInfo JSTestInterfaceLeadingUnderscorePrototype::s_info = { "TestInterfaceLeadingUnderscore", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestInterfaceLeadingUnderscorePrototype) };

void JSTestInterfaceLeadingUnderscorePrototype::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    reifyStaticProperties(vm, JSTestInterfaceLeadingUnderscore::info(), JSTestInterfaceLeadingUnderscorePrototypeTableValues, *this);
    JSC_TO_STRING_TAG_WITHOUT_TRANSITION();
}

const ClassInfo JSTestInterfaceLeadingUnderscore::s_info = { "TestInterfaceLeadingUnderscore", &Base::s_info, nullptr, nullptr, CREATE_METHOD_TABLE(JSTestInterfaceLeadingUnderscore) };

JSTestInterfaceLeadingUnderscore::JSTestInterfaceLeadingUnderscore(Structure* structure, JSDOMGlobalObject& globalObject, Ref<TestInterfaceLeadingUnderscore>&& impl)
    : JSDOMWrapper<TestInterfaceLeadingUnderscore>(structure, globalObject, WTFMove(impl))
{
}

void JSTestInterfaceLeadingUnderscore::finishCreation(VM& vm)
{
    Base::finishCreation(vm);
    ASSERT(inherits(vm, info()));

    static_assert(!std::is_base_of<ActiveDOMObject, TestInterfaceLeadingUnderscore>::value, "Interface is not marked as [ActiveDOMObject] even though implementation class subclasses ActiveDOMObject.");

}

JSObject* JSTestInterfaceLeadingUnderscore::createPrototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return JSTestInterfaceLeadingUnderscorePrototype::create(vm, &globalObject, JSTestInterfaceLeadingUnderscorePrototype::createStructure(vm, &globalObject, globalObject.objectPrototype()));
}

JSObject* JSTestInterfaceLeadingUnderscore::prototype(VM& vm, JSDOMGlobalObject& globalObject)
{
    return getDOMPrototype<JSTestInterfaceLeadingUnderscore>(vm, globalObject);
}

JSValue JSTestInterfaceLeadingUnderscore::getConstructor(VM& vm, const JSGlobalObject* globalObject)
{
    return getDOMConstructor<JSTestInterfaceLeadingUnderscoreConstructor>(vm, *jsCast<const JSDOMGlobalObject*>(globalObject));
}

void JSTestInterfaceLeadingUnderscore::destroy(JSC::JSCell* cell)
{
    JSTestInterfaceLeadingUnderscore* thisObject = static_cast<JSTestInterfaceLeadingUnderscore*>(cell);
    thisObject->JSTestInterfaceLeadingUnderscore::~JSTestInterfaceLeadingUnderscore();
}

template<> inline JSTestInterfaceLeadingUnderscore* IDLAttribute<JSTestInterfaceLeadingUnderscore>::cast(JSGlobalObject& lexicalGlobalObject, EncodedJSValue thisValue)
{
    return jsDynamicCast<JSTestInterfaceLeadingUnderscore*>(JSC::getVM(&lexicalGlobalObject), JSValue::decode(thisValue));
}

EncodedJSValue jsTestInterfaceLeadingUnderscoreConstructor(JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, PropertyName)
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* prototype = jsDynamicCast<JSTestInterfaceLeadingUnderscorePrototype*>(vm, JSValue::decode(thisValue));
    if (UNLIKELY(!prototype))
        return throwVMTypeError(lexicalGlobalObject, throwScope);
    return JSValue::encode(JSTestInterfaceLeadingUnderscore::getConstructor(JSC::getVM(lexicalGlobalObject), prototype->globalObject()));
}

bool setJSTestInterfaceLeadingUnderscoreConstructor(JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, EncodedJSValue encodedValue)
{
    VM& vm = JSC::getVM(lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto* prototype = jsDynamicCast<JSTestInterfaceLeadingUnderscorePrototype*>(vm, JSValue::decode(thisValue));
    if (UNLIKELY(!prototype)) {
        throwVMTypeError(lexicalGlobalObject, throwScope);
        return false;
    }
    // Shadowing a built-in constructor
    return prototype->putDirect(vm, vm.propertyNames->constructor, JSValue::decode(encodedValue));
}

static inline JSValue jsTestInterfaceLeadingUnderscoreReadonlyGetter(JSGlobalObject& lexicalGlobalObject, JSTestInterfaceLeadingUnderscore& thisObject)
{
    auto& vm = JSC::getVM(&lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    auto& impl = thisObject.wrapped();
    RELEASE_AND_RETURN(throwScope, (toJS<IDLDOMString>(lexicalGlobalObject, throwScope, impl.readonly())));
}

EncodedJSValue jsTestInterfaceLeadingUnderscoreReadonly(JSGlobalObject* lexicalGlobalObject, EncodedJSValue thisValue, PropertyName)
{
    return IDLAttribute<JSTestInterfaceLeadingUnderscore>::get<jsTestInterfaceLeadingUnderscoreReadonlyGetter, CastedThisErrorBehavior::Assert>(*lexicalGlobalObject, thisValue, "readonly");
}

JSC::IsoSubspace* JSTestInterfaceLeadingUnderscore::subspaceForImpl(JSC::VM& vm)
{
    auto& clientData = *static_cast<JSVMClientData*>(vm.clientData);
    auto& spaces = clientData.subspaces();
    if (auto* space = spaces.m_subspaceForTestInterfaceLeadingUnderscore.get())
        return space;
    static_assert(std::is_base_of_v<JSC::JSDestructibleObject, JSTestInterfaceLeadingUnderscore> || !JSTestInterfaceLeadingUnderscore::needsDestruction);
    if constexpr (std::is_base_of_v<JSC::JSDestructibleObject, JSTestInterfaceLeadingUnderscore>)
        spaces.m_subspaceForTestInterfaceLeadingUnderscore = makeUnique<IsoSubspace> ISO_SUBSPACE_INIT(vm.heap, vm.destructibleObjectHeapCellType.get(), JSTestInterfaceLeadingUnderscore);
    else
        spaces.m_subspaceForTestInterfaceLeadingUnderscore = makeUnique<IsoSubspace> ISO_SUBSPACE_INIT(vm.heap, vm.cellHeapCellType.get(), JSTestInterfaceLeadingUnderscore);
    auto* space = spaces.m_subspaceForTestInterfaceLeadingUnderscore.get();
IGNORE_WARNINGS_BEGIN("unreachable-code")
IGNORE_WARNINGS_BEGIN("tautological-compare")
    if (&JSTestInterfaceLeadingUnderscore::visitOutputConstraints != &JSC::JSCell::visitOutputConstraints)
        clientData.outputConstraintSpaces().append(space);
IGNORE_WARNINGS_END
IGNORE_WARNINGS_END
    return space;
}

void JSTestInterfaceLeadingUnderscore::analyzeHeap(JSCell* cell, HeapAnalyzer& analyzer)
{
    auto* thisObject = jsCast<JSTestInterfaceLeadingUnderscore*>(cell);
    analyzer.setWrappedObjectForCell(cell, &thisObject->wrapped());
    if (thisObject->scriptExecutionContext())
        analyzer.setLabelForCell(cell, "url " + thisObject->scriptExecutionContext()->url().string());
    Base::analyzeHeap(cell, analyzer);
}

bool JSTestInterfaceLeadingUnderscoreOwner::isReachableFromOpaqueRoots(JSC::Handle<JSC::Unknown> handle, void*, SlotVisitor& visitor, const char** reason)
{
    UNUSED_PARAM(handle);
    UNUSED_PARAM(visitor);
    UNUSED_PARAM(reason);
    return false;
}

void JSTestInterfaceLeadingUnderscoreOwner::finalize(JSC::Handle<JSC::Unknown> handle, void* context)
{
    auto* jsTestInterfaceLeadingUnderscore = static_cast<JSTestInterfaceLeadingUnderscore*>(handle.slot()->asCell());
    auto& world = *static_cast<DOMWrapperWorld*>(context);
    uncacheWrapper(world, &jsTestInterfaceLeadingUnderscore->wrapped(), jsTestInterfaceLeadingUnderscore);
}

JSC::JSValue toJSNewlyCreated(JSC::JSGlobalObject*, JSDOMGlobalObject* globalObject, Ref<TestInterfaceLeadingUnderscore>&& impl)
{
    // If you hit this failure the interface definition has the ImplementationLacksVTable
    // attribute. You should remove that attribute. If the class has subclasses
    // that may be passed through this toJS() function you should use the SkipVTableValidation
    // attribute to TestInterfaceLeadingUnderscore.
    static_assert(!std::is_polymorphic<TestInterfaceLeadingUnderscore>::value, "TestInterfaceLeadingUnderscore is polymorphic but the IDL claims it is not");
    return createWrapper<TestInterfaceLeadingUnderscore>(globalObject, WTFMove(impl));
}

JSC::JSValue toJS(JSC::JSGlobalObject* lexicalGlobalObject, JSDOMGlobalObject* globalObject, TestInterfaceLeadingUnderscore& impl)
{
    return wrap(lexicalGlobalObject, globalObject, impl);
}

TestInterfaceLeadingUnderscore* JSTestInterfaceLeadingUnderscore::toWrapped(JSC::VM& vm, JSC::JSValue value)
{
    if (auto* wrapper = jsDynamicCast<JSTestInterfaceLeadingUnderscore*>(vm, value))
        return &wrapper->wrapped();
    return nullptr;
}

}
