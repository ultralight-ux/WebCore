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
#include "Identifier.h"
#include "InitializeThreading.h"
#include "JSCallbackObject.h"
#include "JSGlobalObject.h"
#include "JSObjectRef.h"
#include "ObjectPrototype.h"
#include "JSCInlines.h"
#include <wtf/text/StringHash.h>

using namespace JSC;

const JSClassDefinition kJSClassDefinitionEmpty = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

OpaqueJSClass::OpaqueJSClass(const JSClassDefinition* definition, OpaqueJSClass* protoClass)
    : parentClass(definition->parentClass)
      , prototypeClass(0)
      , version(definition->version)
      , m_className(String::fromUTF8(definition->className))
{
    ASSERT(version == 0 || version == 1000);

    if (version == 0)
    {
        v0 = {
            definition->initialize,
            definition->finalize,
            definition->hasProperty,
            definition->getProperty,
            definition->setProperty,
            definition->deleteProperty,
            definition->getPropertyNames,
            definition->callAsFunction,
            definition->callAsConstructor,
            definition->hasInstance,
            definition->convertToType
        };
    } else if (version == 1000)
    {
        v1000 = {
            definition->initializeEx,
            definition->finalizeEx,
            definition->hasPropertyEx,
            definition->getPropertyEx,
            definition->setPropertyEx,
            definition->deletePropertyEx,
            definition->getPropertyNamesEx,
            definition->callAsFunctionEx,
            definition->callAsConstructorEx,
            definition->hasInstanceEx,
            definition->convertToTypeEx
        };
    }

    initializeThreading();

    if (const JSStaticValue* staticValue = definition->staticValues)
    {
        m_staticValues = std::make_unique<OpaqueJSClassStaticValuesTable>();
        while (staticValue->name)
        {
            String valueName = String::fromUTF8(staticValue->name);
            if (!valueName.isNull()) {
                if (version == 0) {
                    m_staticValues->set(valueName.impl(),
                        std::make_unique<StaticValueEntry>(staticValue->getProperty,
                            staticValue->setProperty,
                            staticValue->attributes, valueName));
                }
                else if (version == 1000)
                {
                    m_staticValues->set(valueName.impl(),
                        std::make_unique<StaticValueEntry>(staticValue->getPropertyEx,
                            staticValue->setPropertyEx,
                            staticValue->attributes,
                            valueName));
                }
            }

            ++staticValue;
        }
    }

    if (const JSStaticFunction* staticFunction = definition->staticFunctions)
    {
        m_staticFunctions = std::make_unique<OpaqueJSClassStaticFunctionsTable>();
        while (staticFunction->name)
        {
            String functionName = String::fromUTF8(staticFunction->name);
            if (!functionName.isNull()) {
                if(version == 0)
                {
                    m_staticFunctions->set(functionName.impl(),
                        std::make_unique<StaticFunctionEntry>(
                            staticFunction->callAsFunction, staticFunction->attributes));
                } else if(version == 1000)
                {
                    m_staticFunctions->set(functionName.impl(), 
                        std::make_unique<StaticFunctionEntry>(
                            staticFunction->callAsFunctionEx, staticFunction->attributes));
                }
            }
            ++staticFunction;
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
    if (m_staticValues)
    {
        OpaqueJSClassStaticValuesTable::const_iterator end = m_staticValues->end();
        for (OpaqueJSClassStaticValuesTable::const_iterator it = m_staticValues->begin(); it != end; ++it)
            ASSERT(!it->key->isAtom());
    }

    if (m_staticFunctions)
    {
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
    return adoptRef(*new OpaqueJSClass(definition, 0));
}

Ref<OpaqueJSClass> OpaqueJSClass::create(const JSClassDefinition* clientDefinition)
{
    JSClassDefinition definition = *clientDefinition; // Avoid modifying client copy.

    JSClassDefinition protoDefinition = kJSClassDefinitionEmpty;
    protoDefinition.finalize = 0;
    std::swap(definition.staticFunctions, protoDefinition.staticFunctions); // Move static functions to the prototype.

    // We are supposed to use JSClassRetain/Release but since we know that we currently have
    // the only reference to this class object we cheat and use a RefPtr instead.
    RefPtr<OpaqueJSClass> protoClass = adoptRef(new OpaqueJSClass(&protoDefinition, 0));
    return adoptRef(*new OpaqueJSClass(&definition, protoClass.get()));
}

OpaqueJSClassContextData::OpaqueJSClassContextData(JSC::VM&, OpaqueJSClass* jsClass)
    : m_class(jsClass)
{
    if (jsClass->m_staticValues)
    {
        staticValues = std::make_unique<OpaqueJSClassStaticValuesTable>();
        OpaqueJSClassStaticValuesTable::const_iterator end = jsClass->m_staticValues->end();
        for (OpaqueJSClassStaticValuesTable::const_iterator it = jsClass->m_staticValues->begin(); it != end; ++it)
        {
            ASSERT(!it->key->isAtom());
            String valueName = it->key->isolatedCopy();

            staticValues->add(valueName.impl(), std::make_unique<StaticValueEntry>(*it->value, valueName));
        }
    }

    if (jsClass->m_staticFunctions)
    {
        staticFunctions = std::make_unique<OpaqueJSClassStaticFunctionsTable>();
        OpaqueJSClassStaticFunctionsTable::const_iterator end = jsClass->m_staticFunctions->end();
        for (OpaqueJSClassStaticFunctionsTable::const_iterator it = jsClass->m_staticFunctions->begin(); it != end; ++it
        )
        {
            ASSERT(!it->key->isAtom());
            staticFunctions->add(it->key->isolatedCopy(), std::make_unique<StaticFunctionEntry>(*it->value));
        }
    }
}

OpaqueJSClassContextData& OpaqueJSClass::contextData(ExecState* exec)
{
    std::unique_ptr<OpaqueJSClassContextData>& contextData = exec
                                                             ->lexicalGlobalObject()->opaqueJSClassData().add(
                                                                 this, nullptr).iterator->value;
    if (!contextData)
        contextData = std::make_unique<OpaqueJSClassContextData>(exec->vm(), this);
    return *contextData;
}

String OpaqueJSClass::className()
{
    // Make a deep copy, so that the caller has no chance to put the original into AtomStringTable.
    return m_className.isolatedCopy();
}

OpaqueJSClassStaticValuesTable* OpaqueJSClass::staticValues(JSC::ExecState* exec)
{
    return contextData(exec).staticValues.get();
}

OpaqueJSClassStaticFunctionsTable* OpaqueJSClass::staticFunctions(JSC::ExecState* exec)
{
    return contextData(exec).staticFunctions.get();
}

JSObject* OpaqueJSClass::prototype(ExecState* exec)
{
    /* Class (C++) and prototype (JS) inheritance are parallel, so:
     *     (C++)      |        (JS)
     *   ParentClass  |   ParentClassPrototype
     *       ^        |          ^
     *       |        |          |
     *  DerivedClass  |  DerivedClassPrototype
     */

    if (!prototypeClass)
        return 0;

    OpaqueJSClassContextData& jsClassData = contextData(exec);

    if (JSObject* prototype = jsClassData.cachedPrototype.get())
        return prototype;

    // Recursive, but should be good enough for our purposes
    JSObject* prototype = JSCallbackObject<JSDestructibleObject>::create(
        exec, exec->lexicalGlobalObject(), exec->lexicalGlobalObject()->callbackObjectStructure(), prototypeClass,
        &jsClassData); // set jsClassData as the object's private data, so it can clear our reference on destruction
    if (parentClass)
    {
        if (JSObject* parentPrototype = parentClass->prototype(exec))
            prototype->setPrototypeDirect(exec->vm(), parentPrototype);
    }

    jsClassData.cachedPrototype = Weak<JSObject>(prototype);
    return prototype;
}
