/*
 * Copyright (C) 2006, 2007 Apple Inc.  All rights reserved.
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
#include "JSClassRef.h"

#include "APICast.h"
#include "InitializeThreading.h"
#include "JSCInlines.h"
#include "JSCallbackObject.h"

using namespace JSC;

const JSClassDefinition kJSClassDefinitionEmpty = { 0, 0, nullptr, nullptr, { { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr } }, nullptr };

OpaqueJSClass::OpaqueJSClass(const JSClassDefinition* definition, OpaqueJSClass* protoClass)
    : parentClass(definition->parentClass)
    , prototypeClass(nullptr)
    , version(definition->version)
    , m_className(String::fromUTF8(definition->className))
{
    ASSERT(version == 0 || version == 1000);

    JSC::initialize();

    // Initialize callbacks based on version
    if (version == 0) {
        privateData = nullptr; // No private data for version 0
        v0.initialize = definition->initialize;
        v0.finalize = definition->finalize;
        v0.hasProperty = definition->hasProperty;
        v0.getProperty = definition->getProperty;
        v0.setProperty = definition->setProperty;
        v0.deleteProperty = definition->deleteProperty;
        v0.getPropertyNames = definition->getPropertyNames;
        v0.callAsFunction = definition->callAsFunction;
        v0.callAsConstructor = definition->callAsConstructor;
        v0.hasInstance = definition->hasInstance;
        v0.convertToType = definition->convertToType;

        if (const JSStaticValue* staticValue = definition->staticValues) {
            m_staticValues = makeUnique<OpaqueJSClassStaticValuesTable>();
            while (staticValue->name) {
                String valueName = String::fromUTF8(staticValue->name);
                if (!valueName.isNull())
                    m_staticValues->set(valueName.impl(), makeUnique<StaticValueEntry>(staticValue->getProperty, staticValue->setProperty, staticValue->attributes, valueName));
                ++staticValue;
            }
        }

        if (const JSStaticFunction* staticFunction = definition->staticFunctions) {
            m_staticFunctions = makeUnique<OpaqueJSClassStaticFunctionsTable>();
            while (staticFunction->name) {
                String functionName = String::fromUTF8(staticFunction->name);
                if (!functionName.isNull())
                    m_staticFunctions->set(functionName.impl(), makeUnique<StaticFunctionEntry>(staticFunction->callAsFunction, staticFunction->attributes));
                ++staticFunction;
            }
        }
    } else if (version == 1000) {
        v1000.initializeEx = definition->initializeEx;
        v1000.finalizeEx = definition->finalizeEx;
        v1000.hasPropertyEx = definition->hasPropertyEx;
        v1000.getPropertyEx = definition->getPropertyEx;
        v1000.setPropertyEx = definition->setPropertyEx;
        v1000.deletePropertyEx = definition->deletePropertyEx;
        v1000.getPropertyNamesEx = definition->getPropertyNamesEx;
        v1000.callAsFunctionEx = definition->callAsFunctionEx;
        v1000.callAsConstructorEx = definition->callAsConstructorEx;
        v1000.hasInstanceEx = definition->hasInstanceEx;
        v1000.convertToTypeEx = definition->convertToTypeEx;
        privateData = definition->privateData;

        if (const JSStaticValueEx* staticValue = definition->staticValuesEx) {
            m_staticValues = makeUnique<OpaqueJSClassStaticValuesTable>();
            while (staticValue->name) {
                String valueName = String::fromUTF8(staticValue->name);
                if (!valueName.isNull())
                    m_staticValues->set(valueName.impl(), makeUnique<StaticValueEntry>(staticValue->getPropertyEx, staticValue->setPropertyEx, staticValue->attributes, valueName));
                ++staticValue;
            }
        }

        if (const JSStaticFunctionEx* staticFunction = definition->staticFunctionsEx) {
            m_staticFunctions = makeUnique<OpaqueJSClassStaticFunctionsTable>();
            while (staticFunction->name) {
                String functionName = String::fromUTF8(staticFunction->name);
                if (!functionName.isNull())
                    m_staticFunctions->set(functionName.impl(), makeUnique<StaticFunctionEntry>(staticFunction->callAsFunctionEx, staticFunction->attributes));
                ++staticFunction;
            }
        }
    }

    if (protoClass)
        prototypeClass = JSClassRetain(protoClass);
}

OpaqueJSClass::~OpaqueJSClass()
{
    // The empty string is shared across threads & is an identifier, in all other cases we should have done a deep copy in className(), below. 
    ASSERT(!m_className.length() || !m_className.impl()->isAtom());

#ifndef NDEBUG
    if (m_staticValues) {
        OpaqueJSClassStaticValuesTable::const_iterator end = m_staticValues->end();
        for (OpaqueJSClassStaticValuesTable::const_iterator it = m_staticValues->begin(); it != end; ++it)
            ASSERT(!it->key->isAtom());
    }

    if (m_staticFunctions) {
        OpaqueJSClassStaticFunctionsTable::const_iterator end = m_staticFunctions->end();
        for (OpaqueJSClassStaticFunctionsTable::const_iterator it = m_staticFunctions->begin(); it != end; ++it)
            ASSERT(!it->key->isAtom());
    }
#endif
    
    if (prototypeClass)
        JSClassRelease(prototypeClass);
}

Ref<OpaqueJSClass> OpaqueJSClass::createNoAutomaticPrototype(const JSClassDefinition* definition)
{
    return adoptRef(*new OpaqueJSClass(definition, nullptr));
}

Ref<OpaqueJSClass> OpaqueJSClass::create(const JSClassDefinition* clientDefinition)
{
    JSClassDefinition definition = *clientDefinition; // Avoid modifying client copy.

    JSClassDefinition protoDefinition = kJSClassDefinitionEmpty;
    protoDefinition.finalize = nullptr;
    std::swap(definition.staticFunctions, protoDefinition.staticFunctions); // Move static functions to the prototype.
    
    // We are supposed to use JSClassRetain/Release but since we know that we currently have
    // the only reference to this class object we cheat and use a RefPtr instead.
    RefPtr<OpaqueJSClass> protoClass = adoptRef(new OpaqueJSClass(&protoDefinition, nullptr));
    return adoptRef(*new OpaqueJSClass(&definition, protoClass.get()));
}

OpaqueJSClassContextData::OpaqueJSClassContextData(JSC::VM&, OpaqueJSClass* jsClass)
    : m_class(jsClass)
{
    if (jsClass->m_staticValues) {
        staticValues = makeUnique<OpaqueJSClassStaticValuesTable>();
        OpaqueJSClassStaticValuesTable::const_iterator end = jsClass->m_staticValues->end();
        for (OpaqueJSClassStaticValuesTable::const_iterator it = jsClass->m_staticValues->begin(); it != end; ++it) {
            ASSERT(!it->key->isAtom());
            String valueName = it->key->isolatedCopy();
            staticValues->add(valueName.impl(), makeUnique<StaticValueEntry>(
                it->value->version,
                it->value->version == 0 ? it->value->v0.getProperty : nullptr,
                it->value->version == 0 ? it->value->v0.setProperty : nullptr,
                it->value->version == 1000 ? it->value->v1000.getPropertyEx : nullptr,
                it->value->version == 1000 ? it->value->v1000.setPropertyEx : nullptr,
                it->value->attributes, valueName));
        }
    }

    if (jsClass->m_staticFunctions) {
        staticFunctions = makeUnique<OpaqueJSClassStaticFunctionsTable>();
        OpaqueJSClassStaticFunctionsTable::const_iterator end = jsClass->m_staticFunctions->end();
        for (OpaqueJSClassStaticFunctionsTable::const_iterator it = jsClass->m_staticFunctions->begin(); it != end; ++it) {
            ASSERT(!it->key->isAtom());
            staticFunctions->add(it->key->isolatedCopy(), makeUnique<StaticFunctionEntry>(
                it->value->version,
                it->value->version == 0 ? it->value->v0.callAsFunction : nullptr,
                it->value->version == 1000 ? it->value->v1000.callAsFunctionEx : nullptr,
                it->value->attributes));
        }
    }
}

OpaqueJSClassContextData& OpaqueJSClass::contextData(JSGlobalObject* globalObject)
{
    std::unique_ptr<OpaqueJSClassContextData>& contextData = globalObject->opaqueJSClassData().add(this, nullptr).iterator->value;
    if (!contextData)
        contextData = makeUnique<OpaqueJSClassContextData>(globalObject->vm(), this);
    return *contextData;
}

String OpaqueJSClass::className()
{
    // Make a deep copy, so that the caller has no chance to put the original into AtomStringTable.
    return m_className.isolatedCopy();
}

OpaqueJSClassStaticValuesTable* OpaqueJSClass::staticValues(JSC::JSGlobalObject* globalObject)
{
    return contextData(globalObject).staticValues.get();
}

OpaqueJSClassStaticFunctionsTable* OpaqueJSClass::staticFunctions(JSC::JSGlobalObject* globalObject)
{
    return contextData(globalObject).staticFunctions.get();
}

JSObject* OpaqueJSClass::prototype(JSGlobalObject* globalObject)
{
    /* Class (C++) and prototype (JS) inheritance are parallel, so:
     *     (C++)      |        (JS)
     *   ParentClass  |   ParentClassPrototype
     *       ^        |          ^
     *       |        |          |
     *  DerivedClass  |  DerivedClassPrototype
     */

    if (!prototypeClass)
        return nullptr;

    OpaqueJSClassContextData& jsClassData = contextData(globalObject);

    if (JSObject* prototype = jsClassData.cachedPrototype.get())
        return prototype;

    // Recursive, but should be good enough for our purposes
    JSObject* prototype = JSCallbackObject<JSNonFinalObject>::create(globalObject, globalObject->callbackObjectStructure(), prototypeClass, &jsClassData); // set jsClassData as the object's private data, so it can clear our reference on destruction
    if (parentClass) {
        if (JSObject* parentPrototype = parentClass->prototype(globalObject))
            prototype->setPrototypeDirect(globalObject->vm(), parentPrototype);
    }

    jsClassData.cachedPrototype = Weak<JSObject>(prototype);
    return prototype;
}
