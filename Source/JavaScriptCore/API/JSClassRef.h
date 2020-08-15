/*
 * Copyright (C) 2006 Apple Inc.  All rights reserved.
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

#ifndef JSClassRef_h
#define JSClassRef_h

#include "OpaqueJSString.h"
#include "Protect.h"
#include "Weak.h"
#include <JavaScriptCore/JSObjectRef.h>
#include <wtf/HashMap.h>
#include <wtf/text/WTFString.h>

struct StaticValueEntry {
    WTF_MAKE_FAST_ALLOCATED;
public:
    StaticValueEntry(JSObjectGetPropertyCallback _getProperty, JSObjectSetPropertyCallback _setProperty, JSPropertyAttributes _attributes, String& propertyName)
        : version(0)
        , v0{ _getProperty, _setProperty }
        , attributes(_attributes)
        , propertyNameRef(OpaqueJSString::tryCreate(propertyName))
    {
    }

    StaticValueEntry(JSObjectGetPropertyCallbackEx _getPropertyEx, JSObjectSetPropertyCallbackEx _setPropertyEx, JSPropertyAttributes _attributes, String& propertyName)
        : version(1000)
        , v1000{ _getPropertyEx, _setPropertyEx }
        , attributes(_attributes)
        , propertyNameRef(OpaqueJSString::tryCreate(propertyName))
    {
    }

    StaticValueEntry(const StaticValueEntry& other, String& valueName) : version(other.version), attributes(other.attributes), propertyNameRef(OpaqueJSString::tryCreate(valueName))
    {
        if(version == 0)
        {
            v0 = other.v0;
        } else if (version == 1000)
        {
            v1000 = other.v1000;
        }
    }

    int version;

    union {
        struct {
            JSObjectGetPropertyCallback getProperty;
            JSObjectSetPropertyCallback setProperty;
        } v0;

        struct {
            JSObjectGetPropertyCallbackEx getPropertyEx;
            JSObjectSetPropertyCallbackEx setPropertyEx;
        } v1000;
    };
    JSPropertyAttributes attributes;
    RefPtr<OpaqueJSString> propertyNameRef;
};

struct StaticFunctionEntry {
    WTF_MAKE_FAST_ALLOCATED;
public:
    StaticFunctionEntry(JSObjectCallAsFunctionCallback _callAsFunction, JSPropertyAttributes _attributes)
        : version(0), v0{ _callAsFunction }, attributes(_attributes)
    {
    }

    StaticFunctionEntry(JSObjectCallAsFunctionCallbackEx _callAsFunction, JSPropertyAttributes _attributes)
        : version(1000), v1000{ _callAsFunction }, attributes(_attributes)
    {
    }

    StaticFunctionEntry(const StaticFunctionEntry& other) : version(other.version), attributes(other.attributes)
    {
        if (version == 0)
        {
            v0 = other.v0;
        }
        else if (version == 1000)
        {
            v1000 = other.v1000;
        }
    }

    int version;

    union {
        struct {
            JSObjectCallAsFunctionCallback callAsFunction;
        } v0;

        struct {
            JSObjectCallAsFunctionCallbackEx callAsFunctionEx;
        } v1000;
    };
    JSPropertyAttributes attributes;
};

typedef HashMap<RefPtr<StringImpl>, std::unique_ptr<StaticValueEntry>> OpaqueJSClassStaticValuesTable;
typedef HashMap<RefPtr<StringImpl>, std::unique_ptr<StaticFunctionEntry>> OpaqueJSClassStaticFunctionsTable;

struct OpaqueJSClass;

// An OpaqueJSClass (JSClass) is created without a context, so it can be used with any context, even across context groups.
// This structure holds data members that vary across context groups.
struct OpaqueJSClassContextData {
    WTF_MAKE_NONCOPYABLE(OpaqueJSClassContextData); WTF_MAKE_FAST_ALLOCATED;
public:
    OpaqueJSClassContextData(JSC::VM&, OpaqueJSClass*);

    // It is necessary to keep OpaqueJSClass alive because of the following rare scenario:
    // 1. A class is created and used, so its context data is stored in VM hash map.
    // 2. The class is released, and when all JS objects that use it are collected, OpaqueJSClass
    // is deleted (that's the part prevented by this RefPtr).
    // 3. Another class is created at the same address.
    // 4. When it is used, the old context data is found in VM and used.
    RefPtr<OpaqueJSClass> m_class;

    std::unique_ptr<OpaqueJSClassStaticValuesTable> staticValues;
    std::unique_ptr<OpaqueJSClassStaticFunctionsTable> staticFunctions;
    JSC::Weak<JSC::JSObject> cachedPrototype;
};

struct OpaqueJSClass : public ThreadSafeRefCounted<OpaqueJSClass> {
    static Ref<OpaqueJSClass> create(const JSClassDefinition*);
    static Ref<OpaqueJSClass> createNoAutomaticPrototype(const JSClassDefinition*);
    JS_EXPORT_PRIVATE ~OpaqueJSClass();
    
    String className();
    OpaqueJSClassStaticValuesTable* staticValues(JSC::ExecState*);
    OpaqueJSClassStaticFunctionsTable* staticFunctions(JSC::ExecState*);
    JSC::JSObject* prototype(JSC::ExecState*);

    OpaqueJSClass* parentClass;
    OpaqueJSClass* prototypeClass;
    OpaqueJSClass* prototypeForClass;

    int version;

    union {
        struct
        {
            JSObjectInitializeCallback initialize;
            JSObjectFinalizeCallback finalize;
            JSObjectHasPropertyCallback hasProperty;
            JSObjectGetPropertyCallback getProperty;
            JSObjectSetPropertyCallback setProperty;
            JSObjectDeletePropertyCallback deleteProperty;
            JSObjectGetPropertyNamesCallback getPropertyNames;
            JSObjectCallAsFunctionCallback callAsFunction;
            JSObjectCallAsConstructorCallback callAsConstructor;
            JSObjectHasInstanceCallback hasInstance;
            JSObjectConvertToTypeCallback convertToType;
        } v0;

        struct
        {
            JSObjectInitializeCallbackEx initializeEx;
            JSObjectFinalizeCallbackEx finalizeEx;
            JSObjectHasPropertyCallbackEx hasPropertyEx;
            JSObjectGetPropertyCallbackEx getPropertyEx;
            JSObjectSetPropertyCallbackEx setPropertyEx;
            JSObjectDeletePropertyCallbackEx deletePropertyEx;
            JSObjectGetPropertyNamesCallbackEx getPropertyNamesEx;
            JSObjectCallAsFunctionCallbackEx callAsFunctionEx;
            JSObjectCallAsConstructorCallbackEx callAsConstructorEx;
            JSObjectHasInstanceCallbackEx hasInstanceEx;
            JSObjectConvertToTypeCallbackEx convertToTypeEx;

            void* privateData;
        } v1000;
    };

private:
    friend struct OpaqueJSClassContextData;

    OpaqueJSClass();
    OpaqueJSClass(const OpaqueJSClass&);
    OpaqueJSClass(const JSClassDefinition*, OpaqueJSClass* protoClass);

    OpaqueJSClassContextData& contextData(JSC::ExecState*);

    // Strings in these data members should not be put into any AtomStringTable.
    String m_className;
    std::unique_ptr<OpaqueJSClassStaticValuesTable> m_staticValues;
    std::unique_ptr<OpaqueJSClassStaticFunctionsTable> m_staticFunctions;
};

#endif // JSClassRef_h
