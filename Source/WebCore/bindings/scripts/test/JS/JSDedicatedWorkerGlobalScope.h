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

#pragma once

#include "JSDOMWrapper.h"
#include "JSWorkerGlobalScope.h"

namespace WebCore {

class DedicatedWorkerGlobalScope;

class JSDedicatedWorkerGlobalScope : public JSWorkerGlobalScope {
public:
    using Base = JSWorkerGlobalScope;
    using DOMWrapped = DedicatedWorkerGlobalScope;
    static JSDedicatedWorkerGlobalScope* create(JSC::VM& vm, JSC::Structure* structure, Ref<DedicatedWorkerGlobalScope>&& impl, JSC::JSProxy* proxy)
    {
        JSDedicatedWorkerGlobalScope* ptr = new (NotNull, JSC::allocateCell<JSDedicatedWorkerGlobalScope>(vm)) JSDedicatedWorkerGlobalScope(vm, structure, WTFMove(impl));
        ptr->finishCreation(vm, proxy);
        return ptr;
    }


    DECLARE_INFO;

    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::GlobalObjectType, StructureFlags), info(), JSC::NonArray);
    }

    static JSC::JSValue getConstructor(JSC::VM&, const JSC::JSGlobalObject*);
    template<typename, JSC::SubspaceAccess mode> static JSC::GCClient::IsoSubspace* subspaceFor(JSC::VM& vm)
    {
        if constexpr (mode == JSC::SubspaceAccess::Concurrently)
            return nullptr;
        return subspaceForImpl(vm);
    }
    static JSC::GCClient::IsoSubspace* subspaceForImpl(JSC::VM& vm);
    static void analyzeHeap(JSCell*, JSC::HeapAnalyzer&);
    DedicatedWorkerGlobalScope& wrapped() const
    {
        return static_cast<DedicatedWorkerGlobalScope&>(Base::wrapped());
    }
public:
    static constexpr unsigned StructureFlags = Base::StructureFlags | JSC::HasStaticPropertyTable;
protected:
    JSDedicatedWorkerGlobalScope(JSC::VM&, JSC::Structure*, Ref<DedicatedWorkerGlobalScope>&&);
    void finishCreation(JSC::VM&, JSC::JSProxy*);
};


class JSDedicatedWorkerGlobalScopePrototype final : public JSC::JSNonFinalObject {
public:
    using Base = JSC::JSNonFinalObject;
    static JSDedicatedWorkerGlobalScopePrototype* create(JSC::VM& vm, JSDOMGlobalObject* globalObject, JSC::Structure* structure)
    {
        JSDedicatedWorkerGlobalScopePrototype* ptr = new (NotNull, JSC::allocateCell<JSDedicatedWorkerGlobalScopePrototype>(vm)) JSDedicatedWorkerGlobalScopePrototype(vm, globalObject, structure);
        ptr->finishCreation(vm);
        return ptr;
    }

    DECLARE_INFO;
    template<typename CellType, JSC::SubspaceAccess>
    static JSC::GCClient::IsoSubspace* subspaceFor(JSC::VM& vm)
    {
        STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSDedicatedWorkerGlobalScopePrototype, Base);
        return &vm.plainObjectSpace();
    }
    static JSC::Structure* createStructure(JSC::VM& vm, JSC::JSGlobalObject* globalObject, JSC::JSValue prototype)
    {
        return JSC::Structure::create(vm, globalObject, prototype, JSC::TypeInfo(JSC::ObjectType, StructureFlags), info());
    }

private:
    JSDedicatedWorkerGlobalScopePrototype(JSC::VM& vm, JSC::JSGlobalObject*, JSC::Structure* structure)
        : JSC::JSNonFinalObject(vm, structure)
    {
    }

    void finishCreation(JSC::VM&);
};
STATIC_ASSERT_ISO_SUBSPACE_SHARABLE(JSDedicatedWorkerGlobalScopePrototype, JSDedicatedWorkerGlobalScopePrototype::Base);

template<> struct JSDOMWrapperConverterTraits<DedicatedWorkerGlobalScope> {
    using WrapperClass = JSDedicatedWorkerGlobalScope;
    using ToWrappedReturnType = DedicatedWorkerGlobalScope*;
};

} // namespace WebCore
