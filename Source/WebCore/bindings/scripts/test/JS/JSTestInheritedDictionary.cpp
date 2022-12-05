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
#include "JSTestInheritedDictionary.h"

#include "Document.h"
#include "JSDOMConvertBoolean.h"
#include "JSDOMConvertCallbacks.h"
#include "JSDOMConvertNumbers.h"
#include "JSDOMConvertStrings.h"
#include "JSDOMGlobalObject.h"
#include "JSVoidCallback.h"
#include "Settings.h"
#include <JavaScriptCore/JSCInlines.h>
#include <JavaScriptCore/ObjectConstructor.h>


namespace WebCore {
using namespace JSC;

template<> TestInheritedDictionary convertDictionary<TestInheritedDictionary>(JSGlobalObject& lexicalGlobalObject, JSValue value)
{
    VM& vm = JSC::getVM(&lexicalGlobalObject);
    auto throwScope = DECLARE_THROW_SCOPE(vm);
    bool isNullOrUndefined = value.isUndefinedOrNull();
    auto* object = isNullOrUndefined ? nullptr : value.getObject();
    if (UNLIKELY(!isNullOrUndefined && !object)) {
        throwTypeError(&lexicalGlobalObject, throwScope);
        return { };
    }
    TestInheritedDictionary result;
    JSValue boolMemberValue;
    if (isNullOrUndefined)
        boolMemberValue = jsUndefined();
    else {
        boolMemberValue = object->get(&lexicalGlobalObject, Identifier::fromString(vm, "boolMember"));
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    if (!boolMemberValue.isUndefined()) {
        result.boolMember = convert<IDLBoolean>(lexicalGlobalObject, boolMemberValue);
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    JSValue callbackMemberValue;
    if (isNullOrUndefined)
        callbackMemberValue = jsUndefined();
    else {
        callbackMemberValue = object->get(&lexicalGlobalObject, Identifier::fromString(vm, "callbackMember"));
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    if (!callbackMemberValue.isUndefined()) {
        result.callbackMember = convert<IDLCallbackFunction<JSVoidCallback>>(lexicalGlobalObject, callbackMemberValue, *jsCast<JSDOMGlobalObject*>(&lexicalGlobalObject));
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    JSValue partialBooleanMemberValue;
    if (isNullOrUndefined)
        partialBooleanMemberValue = jsUndefined();
    else {
        partialBooleanMemberValue = object->get(&lexicalGlobalObject, Identifier::fromString(vm, "partialBooleanMember"));
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    if (!partialBooleanMemberValue.isUndefined()) {
        result.partialBooleanMember = convert<IDLBoolean>(lexicalGlobalObject, partialBooleanMemberValue);
        RETURN_IF_EXCEPTION(throwScope, { });
    }
#if ENABLE(Conditional15)
    JSValue partialBooleanMemberWithConditionalValue;
    if (isNullOrUndefined)
        partialBooleanMemberWithConditionalValue = jsUndefined();
    else {
        partialBooleanMemberWithConditionalValue = object->get(&lexicalGlobalObject, Identifier::fromString(vm, "partialBooleanMemberWithConditional"));
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    if (!partialBooleanMemberWithConditionalValue.isUndefined()) {
        result.partialBooleanMemberWithConditional = convert<IDLBoolean>(lexicalGlobalObject, partialBooleanMemberWithConditionalValue);
        RETURN_IF_EXCEPTION(throwScope, { });
    }
#endif
    JSValue partialCallbackMemberValue;
    if (isNullOrUndefined)
        partialCallbackMemberValue = jsUndefined();
    else {
        partialCallbackMemberValue = object->get(&lexicalGlobalObject, Identifier::fromString(vm, "partialCallbackMember"));
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    if (!partialCallbackMemberValue.isUndefined()) {
        result.partialCallbackMember = convert<IDLCallbackFunction<JSVoidCallback>>(lexicalGlobalObject, partialCallbackMemberValue, *jsCast<JSDOMGlobalObject*>(&lexicalGlobalObject));
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    JSValue partialRequiredLongMemberValue;
    if (isNullOrUndefined)
        partialRequiredLongMemberValue = jsUndefined();
    else {
        partialRequiredLongMemberValue = object->get(&lexicalGlobalObject, Identifier::fromString(vm, "partialRequiredLongMember"));
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    if (!partialRequiredLongMemberValue.isUndefined()) {
        result.partialRequiredLongMember = convert<IDLLong>(lexicalGlobalObject, partialRequiredLongMemberValue);
        RETURN_IF_EXCEPTION(throwScope, { });
    } else {
        throwRequiredMemberTypeError(lexicalGlobalObject, throwScope, "partialRequiredLongMember", "TestInheritedDictionary", "long");
        return { };
    }
    JSValue partialStringMemberValue;
    if (isNullOrUndefined)
        partialStringMemberValue = jsUndefined();
    else {
        partialStringMemberValue = object->get(&lexicalGlobalObject, Identifier::fromString(vm, "partialStringMember"));
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    if (!partialStringMemberValue.isUndefined()) {
        result.partialStringMember = convert<IDLDOMString>(lexicalGlobalObject, partialStringMemberValue);
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    JSValue partialStringMemberWithEnabledBySettingValue;
    if (isNullOrUndefined)
        partialStringMemberWithEnabledBySettingValue = jsUndefined();
    else {
        partialStringMemberWithEnabledBySettingValue = object->get(&lexicalGlobalObject, Identifier::fromString(vm, "partialStringMemberWithEnabledBySetting"));
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    if (!partialStringMemberWithEnabledBySettingValue.isUndefined()) {
        result.partialStringMemberWithEnabledBySetting = convert<IDLDOMString>(lexicalGlobalObject, partialStringMemberWithEnabledBySettingValue);
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    JSValue partialUnsignedLongMemberWithImplementedAsValue;
    if (isNullOrUndefined)
        partialUnsignedLongMemberWithImplementedAsValue = jsUndefined();
    else {
        partialUnsignedLongMemberWithImplementedAsValue = object->get(&lexicalGlobalObject, Identifier::fromString(vm, "partialUnsignedLongMemberWithImplementedAs"));
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    if (!partialUnsignedLongMemberWithImplementedAsValue.isUndefined()) {
        result.partialUnsignedLongMember = convert<IDLUnsignedLong>(lexicalGlobalObject, partialUnsignedLongMemberWithImplementedAsValue);
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    JSValue stringMemberValue;
    if (isNullOrUndefined)
        stringMemberValue = jsUndefined();
    else {
        stringMemberValue = object->get(&lexicalGlobalObject, Identifier::fromString(vm, "stringMember"));
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    if (!stringMemberValue.isUndefined()) {
        result.stringMember = convert<IDLDOMString>(lexicalGlobalObject, stringMemberValue);
        RETURN_IF_EXCEPTION(throwScope, { });
    }
    return result;
}

JSC::JSObject* convertDictionaryToJS(JSC::JSGlobalObject& lexicalGlobalObject, JSDOMGlobalObject& globalObject, const TestInheritedDictionary& dictionary)
{
    auto& vm = JSC::getVM(&lexicalGlobalObject);

    auto result = constructEmptyObject(&lexicalGlobalObject, globalObject.objectPrototype());

    if (!IDLBoolean::isNullValue(dictionary.boolMember)) {
        auto boolMemberValue = toJS<IDLBoolean>(IDLBoolean::extractValueFromNullable(dictionary.boolMember));
        result->putDirect(vm, JSC::Identifier::fromString(vm, "boolMember"), boolMemberValue);
    }
    if (!IDLCallbackFunction<JSVoidCallback>::isNullValue(dictionary.callbackMember)) {
        auto callbackMemberValue = toJS<IDLCallbackFunction<JSVoidCallback>>(lexicalGlobalObject, globalObject, IDLCallbackFunction<JSVoidCallback>::extractValueFromNullable(dictionary.callbackMember));
        result->putDirect(vm, JSC::Identifier::fromString(vm, "callbackMember"), callbackMemberValue);
    }
    if (!IDLBoolean::isNullValue(dictionary.partialBooleanMember)) {
        auto partialBooleanMemberValue = toJS<IDLBoolean>(IDLBoolean::extractValueFromNullable(dictionary.partialBooleanMember));
        result->putDirect(vm, JSC::Identifier::fromString(vm, "partialBooleanMember"), partialBooleanMemberValue);
    }
#if ENABLE(Conditional15)
    if (!IDLBoolean::isNullValue(dictionary.partialBooleanMemberWithConditional)) {
        auto partialBooleanMemberWithConditionalValue = toJS<IDLBoolean>(IDLBoolean::extractValueFromNullable(dictionary.partialBooleanMemberWithConditional));
        result->putDirect(vm, JSC::Identifier::fromString(vm, "partialBooleanMemberWithConditional"), partialBooleanMemberWithConditionalValue);
    }
#endif
    if (!IDLCallbackFunction<JSVoidCallback>::isNullValue(dictionary.partialCallbackMember)) {
        auto partialCallbackMemberValue = toJS<IDLCallbackFunction<JSVoidCallback>>(lexicalGlobalObject, globalObject, IDLCallbackFunction<JSVoidCallback>::extractValueFromNullable(dictionary.partialCallbackMember));
        result->putDirect(vm, JSC::Identifier::fromString(vm, "partialCallbackMember"), partialCallbackMemberValue);
    }
    auto partialRequiredLongMemberValue = toJS<IDLLong>(dictionary.partialRequiredLongMember);
    result->putDirect(vm, JSC::Identifier::fromString(vm, "partialRequiredLongMember"), partialRequiredLongMemberValue);
    if (!IDLDOMString::isNullValue(dictionary.partialStringMember)) {
        auto partialStringMemberValue = toJS<IDLDOMString>(lexicalGlobalObject, IDLDOMString::extractValueFromNullable(dictionary.partialStringMember));
        result->putDirect(vm, JSC::Identifier::fromString(vm, "partialStringMember"), partialStringMemberValue);
    }
    if (downcast<Document>(jsCast<JSDOMGlobalObject*>(&globalObject)->scriptExecutionContext())->settings().testSettingEnabled()) {
        if (!IDLDOMString::isNullValue(dictionary.partialStringMemberWithEnabledBySetting)) {
            auto partialStringMemberWithEnabledBySettingValue = toJS<IDLDOMString>(lexicalGlobalObject, IDLDOMString::extractValueFromNullable(dictionary.partialStringMemberWithEnabledBySetting));
            result->putDirect(vm, JSC::Identifier::fromString(vm, "partialStringMemberWithEnabledBySetting"), partialStringMemberWithEnabledBySettingValue);
        }
    }
    if (!IDLUnsignedLong::isNullValue(dictionary.partialUnsignedLongMember)) {
        auto partialUnsignedLongMemberWithImplementedAsValue = toJS<IDLUnsignedLong>(IDLUnsignedLong::extractValueFromNullable(dictionary.partialUnsignedLongMember));
        result->putDirect(vm, JSC::Identifier::fromString(vm, "partialUnsignedLongMemberWithImplementedAs"), partialUnsignedLongMemberWithImplementedAsValue);
    }
    if (!IDLDOMString::isNullValue(dictionary.stringMember)) {
        auto stringMemberValue = toJS<IDLDOMString>(lexicalGlobalObject, IDLDOMString::extractValueFromNullable(dictionary.stringMember));
        result->putDirect(vm, JSC::Identifier::fromString(vm, "stringMember"), stringMemberValue);
    }
    return result;
}

} // namespace WebCore
