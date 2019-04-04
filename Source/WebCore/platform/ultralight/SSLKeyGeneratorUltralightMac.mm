#include "WebCorePrefix.h"
#import "config.h"
#import "SSLKeyGenerator.h"

#import "LocalizedStrings.h"
#import "URL.h"
#import <Security/SecAsn1Coder.h>
#import <Security/SecAsn1Templates.h>
#import <Security/SecEncodeTransform.h>
#import <Security/cssm.h>
#import <wtf/RetainPtr.h>
#import <wtf/Scope.h>
#import <wtf/spi/cocoa/SecuritySPI.h>
#import <wtf/text/Base64.h>

// These are in Security.framework but not declared in a public header.
extern const SecAsn1Template kSecAsn1AlgorithmIDTemplate[];
extern const SecAsn1Template kSecAsn1SubjectPublicKeyInfoTemplate[];

namespace WebCore {
    
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    
    static RetainPtr<CFStringRef> ToCFString(const String& str)
    {
        if (str.is8Bit())
            return adoptCF(CFStringCreateWithBytes(0, reinterpret_cast<const UInt8*>(str.characters8()), str.length(), kCFStringEncodingISOLatin1, false));
        
        return adoptCF(CFStringCreateWithCharacters(0, reinterpret_cast<const UniChar*>(str.characters16()), str.length()));
    }
    
    struct PublicKeyAndChallenge {
        CSSM_X509_SUBJECT_PUBLIC_KEY_INFO subjectPublicKeyInfo;
        CSSM_DATA challenge;
    };
    
    struct SignedPublicKeyAndChallenge {
        PublicKeyAndChallenge publicKeyAndChallenge;
        CSSM_X509_ALGORITHM_IDENTIFIER algorithmIdentifier;
        CSSM_DATA signature;
    };
    
    static const SecAsn1Template publicKeyAndChallengeTemplate[] {
        { SEC_ASN1_SEQUENCE, 0, nullptr, sizeof(PublicKeyAndChallenge) },
        { SEC_ASN1_INLINE, offsetof(PublicKeyAndChallenge, subjectPublicKeyInfo), kSecAsn1SubjectPublicKeyInfoTemplate, 0},
        { SEC_ASN1_INLINE, offsetof(PublicKeyAndChallenge, challenge), kSecAsn1IA5StringTemplate, 0 },
        { 0, 0, 0, 0}
    };
    
    static const SecAsn1Template signedPublicKeyAndChallengeTemplate[] {
        { SEC_ASN1_SEQUENCE, 0, nullptr, sizeof(SignedPublicKeyAndChallenge) },
        { SEC_ASN1_INLINE, offsetof(SignedPublicKeyAndChallenge, publicKeyAndChallenge), publicKeyAndChallengeTemplate, 0 },
        { SEC_ASN1_INLINE, offsetof(SignedPublicKeyAndChallenge, algorithmIdentifier), kSecAsn1AlgorithmIDTemplate, 0 },
        { SEC_ASN1_BIT_STRING, offsetof(SignedPublicKeyAndChallenge, signature), 0, 0 },
        { 0, 0, 0, 0 }
    };
    
    static bool getSubjectPublicKey(CSSM_CSP_HANDLE cspHandle, SecKeyRef publicKey, CSSM_KEY_PTR subjectPublicKey)
    {
        const CSSM_KEY* cssmPublicKey;
        if (SecKeyGetCSSMKey(publicKey, &cssmPublicKey) != noErr)
            return false;
        
        CSSM_ACCESS_CREDENTIALS credentials { };
        CSSM_CC_HANDLE ccHandle;
        if (CSSM_CSP_CreateSymmetricContext(cspHandle, CSSM_ALGID_NONE, CSSM_ALGMODE_NONE, &credentials, nullptr, nullptr, CSSM_PADDING_NONE, 0, &ccHandle) != noErr)
            return false;
        
        auto deleteContext = makeScopeExit([&] {
            CSSM_DeleteContext(ccHandle);
        });
        
        CSSM_CONTEXT_ATTRIBUTE publicKeyFormatAttribute;
        publicKeyFormatAttribute.AttributeType = CSSM_ATTRIBUTE_PUBLIC_KEY_FORMAT;
        publicKeyFormatAttribute.AttributeLength = sizeof(uint32);
        publicKeyFormatAttribute.Attribute.Data = (CSSM_DATA_PTR)CSSM_KEYBLOB_RAW_FORMAT_X509;
        
        if (CSSM_UpdateContextAttributes(ccHandle, 1, &publicKeyFormatAttribute) != noErr)
            return false;
        
        if (CSSM_WrapKey(ccHandle, &credentials, cssmPublicKey, nullptr, subjectPublicKey) != noErr)
            return false;
        
        return true;
    }
    
    static bool signPublicKeyAndChallenge(CSSM_CSP_HANDLE cspHandle, const CSSM_DATA* plainText, SecKeyRef privateKey, CSSM_ALGORITHMS algorithmID, CSSM_DATA& signature)
    {
        ASSERT(!signature.Data);
        ASSERT(!signature.Length);
        
        const CSSM_KEY* cssmPrivateKey;
        if (SecKeyGetCSSMKey(privateKey, &cssmPrivateKey) != noErr)
            return false;
        
        const CSSM_ACCESS_CREDENTIALS* credentials;
        if (SecKeyGetCredentials(privateKey, CSSM_ACL_AUTHORIZATION_SIGN, kSecCredentialTypeDefault, &credentials) != noErr)
            return false;
        
        CSSM_CC_HANDLE ccHandle;
        if (CSSM_CSP_CreateSignatureContext(cspHandle, algorithmID, credentials, cssmPrivateKey, &ccHandle) != noErr)
            return false;
        auto deleteContext = makeScopeExit([&] {
            CSSM_DeleteContext(ccHandle);
        });
        
        if (CSSM_SignData(ccHandle, plainText, 1, CSSM_ALGID_NONE, &signature) != noErr)
            return false;
        
        return true;
    }
    
    static String signedPublicKeyAndChallengeString(unsigned keySize, const CString& challenge, const String& keyDescription)
    {
        ASSERT(keySize >= 2048);
        
        SignedPublicKeyAndChallenge signedPublicKeyAndChallenge { };
        
        SecAccessRef accessRef { nullptr };
        if (SecAccessCreate(ToCFString(keyDescription).get(), nullptr, &accessRef) != noErr)
            return String();
        RetainPtr<SecAccessRef> access = adoptCF(accessRef);
        
        CFArrayRef aclsRef { nullptr };
        if (SecAccessCopySelectedACLList(access.get(), CSSM_ACL_AUTHORIZATION_DECRYPT, &aclsRef) != noErr)
            return String();
        RetainPtr<CFArrayRef> acls = adoptCF(aclsRef);
        
        SecACLRef acl = (SecACLRef)(CFArrayGetValueAtIndex(acls.get(), 0));
        
        // Passing nullptr to SecTrustedApplicationCreateFromPath tells that function to assume the application bundle.
        SecTrustedApplicationRef trustedAppRef { nullptr };
        if (SecTrustedApplicationCreateFromPath(nullptr, &trustedAppRef) != noErr)
            return String();
        RetainPtr<SecTrustedApplicationRef> trustedApp = adoptCF(trustedAppRef);
        
        const CSSM_ACL_KEYCHAIN_PROMPT_SELECTOR defaultSelector = {
            CSSM_ACL_KEYCHAIN_PROMPT_CURRENT_VERSION, 0
        };
        if (SecACLSetSimpleContents(acl, (__bridge CFArrayRef)@[ (__bridge id)trustedApp.get() ], ToCFString(keyDescription).get(), &defaultSelector) != noErr)
            return String();
        
        SecKeyRef publicKeyRef { nullptr };
        SecKeyRef privateKeyRef { nullptr };
        if (SecKeyCreatePair(nullptr, CSSM_ALGID_RSA, keySize, 0, CSSM_KEYUSE_ANY, CSSM_KEYATTR_PERMANENT | CSSM_KEYATTR_EXTRACTABLE | CSSM_KEYATTR_RETURN_REF, CSSM_KEYUSE_ANY, CSSM_KEYATTR_SENSITIVE | CSSM_KEYATTR_RETURN_REF | CSSM_KEYATTR_PERMANENT | CSSM_KEYATTR_EXTRACTABLE, access.get(), &publicKeyRef, &privateKeyRef) != noErr)
            return String();
        RetainPtr<SecKeyRef> publicKey = adoptCF(publicKeyRef);
        RetainPtr<SecKeyRef> privateKey = adoptCF(privateKeyRef);
        
        CSSM_CSP_HANDLE cspHandle;
        if (SecKeyGetCSPHandle(privateKey.get(), &cspHandle) != noErr)
            return String();
        
        CSSM_KEY subjectPublicKey { };
        if (!getSubjectPublicKey(cspHandle, publicKey.get(), &subjectPublicKey))
            return String();
        auto freeSubjectPublicKey = makeScopeExit([&] {
            CSSM_FreeKey(cspHandle, nullptr, &subjectPublicKey, CSSM_FALSE);
        });
        
        SecAsn1CoderRef coder = nullptr;
        if (SecAsn1CoderCreate(&coder) != noErr)
            return String();
        auto releaseCoder = makeScopeExit([&] {
            SecAsn1CoderRelease(coder);
        });
        
        if (SecAsn1DecodeData(coder, &subjectPublicKey.KeyData, kSecAsn1SubjectPublicKeyInfoTemplate, &signedPublicKeyAndChallenge.publicKeyAndChallenge.subjectPublicKeyInfo) != noErr)
            return String();
        
        ASSERT(challenge.data());
        
        // Length needs to account for the null terminator.
        signedPublicKeyAndChallenge.publicKeyAndChallenge.challenge.Length = challenge.length() + 1;
        signedPublicKeyAndChallenge.publicKeyAndChallenge.challenge.Data = (uint8_t*)challenge.data();
        
        CSSM_DATA encodedPublicKeyAndChallenge { 0, nullptr };
        if (SecAsn1EncodeItem(coder, &signedPublicKeyAndChallenge.publicKeyAndChallenge, publicKeyAndChallengeTemplate, &encodedPublicKeyAndChallenge) != noErr)
            return String();
        
        CSSM_DATA signature { };
        if (!signPublicKeyAndChallenge(cspHandle, &encodedPublicKeyAndChallenge, privateKey.get(), CSSM_ALGID_MD5WithRSA, signature))
            return String();
        auto freeSignatureData = makeScopeExit([&] {
            CSSM_API_MEMORY_FUNCS memoryFunctions;
            if (CSSM_GetAPIMemoryFunctions(cspHandle, &memoryFunctions) != noErr)
                return;
            
            memoryFunctions.free_func(signature.Data, memoryFunctions.AllocRef);
        });
        
        uint8 encodeNull[2] { SEC_ASN1_NULL, 0 };
        signedPublicKeyAndChallenge.algorithmIdentifier.algorithm = CSSMOID_MD5WithRSA;
        signedPublicKeyAndChallenge.algorithmIdentifier.parameters.Data = (uint8 *)encodeNull;
        signedPublicKeyAndChallenge.algorithmIdentifier.parameters.Length = 2;
        signedPublicKeyAndChallenge.signature = signature;
        
        // We want the signature length to be in bits.
        signedPublicKeyAndChallenge.signature.Length *= 8;
        
        CSSM_DATA encodedSignedPublicKeyAndChallenge { 0, nullptr };
        if (SecAsn1EncodeItem(coder, &signedPublicKeyAndChallenge, signedPublicKeyAndChallengeTemplate, &encodedSignedPublicKeyAndChallenge) != noErr)
            return String();
        
        return base64Encode(encodedSignedPublicKeyAndChallenge.Data, encodedSignedPublicKeyAndChallenge.Length);
    }
    
#pragma clang diagnostic pop
    
    void getSupportedKeySizes(Vector<String>& supportedKeySizes)
    {
        ASSERT(supportedKeySizes.isEmpty());
        supportedKeySizes.append("2048 (High Grade)");
    }
    
    String signedPublicKeyAndChallengeString(unsigned keySizeIndex, const String& challengeString, const URL& url)
    {
        // This switch statement must always be synced with the UI strings returned by getSupportedKeySizes.
        UInt32 keySize;
        switch (keySizeIndex) {
            case 0:
                keySize = 2048;
                break;
            default:
                ASSERT_NOT_REACHED();
                return String();
        }
        
        auto challenge = challengeString.containsOnlyASCII() ? challengeString.ascii() : "";
        
        return signedPublicKeyAndChallengeString(keySize, challenge, String("Key from ") + url.host());
    }
    
}
