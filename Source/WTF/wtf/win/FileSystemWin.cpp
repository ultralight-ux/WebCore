/*
 * Copyright (C) 2007-2017 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Collabora, Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include <wtf/FileSystem.h>

#include <io.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <sys/stat.h>
#include <windows.h>
#include <wtf/CryptographicallyRandomNumber.h>
#include <wtf/FileMetadata.h>
#include <wtf/HashMap.h>
#include <wtf/text/CString.h>
#include <wtf/text/StringBuilder.h>
#include <wtf/text/win/WCharStringExtras.h>
#include <wtf/win/PathWalker.h>

namespace WTF {

namespace FileSystemImpl {

static const ULONGLONG kSecondsFromFileTimeToTimet = 11644473600;

static bool getFindData(String path, WIN32_FIND_DATAW& findData)
{
    HANDLE handle = FindFirstFileW(path.wideCharacters().data(), &findData);
    if (handle == INVALID_HANDLE_VALUE)
        return false;
    FindClose(handle);
    return true;
}

static bool getFileSizeFromFindData(const WIN32_FIND_DATAW& findData, long long& size)
{
    ULARGE_INTEGER fileSize;
    fileSize.HighPart = findData.nFileSizeHigh;
    fileSize.LowPart = findData.nFileSizeLow;

    if (fileSize.QuadPart > static_cast<ULONGLONG>(std::numeric_limits<long long>::max()))
        return false;

    size = fileSize.QuadPart;
    return true;
}

static bool getFileSizeFromByHandleFileInformationStructure(const BY_HANDLE_FILE_INFORMATION& fileInformation, long long& size)
{
    ULARGE_INTEGER fileSize;
    fileSize.HighPart = fileInformation.nFileSizeHigh;
    fileSize.LowPart = fileInformation.nFileSizeLow;

    if (fileSize.QuadPart > static_cast<ULONGLONG>(std::numeric_limits<long long>::max()))
        return false;

    size = fileSize.QuadPart;
    return true;
}

static void getFileCreationTimeFromFindData(const WIN32_FIND_DATAW& findData, time_t& time)
{
    ULARGE_INTEGER fileTime;
    fileTime.HighPart = findData.ftCreationTime.dwHighDateTime;
    fileTime.LowPart = findData.ftCreationTime.dwLowDateTime;

    // Information about converting time_t to FileTime is available at http://msdn.microsoft.com/en-us/library/ms724228%28v=vs.85%29.aspx
    time = fileTime.QuadPart / 10000000 - kSecondsFromFileTimeToTimet;
}


static void getFileModificationTimeFromFindData(const WIN32_FIND_DATAW& findData, time_t& time)
{
    ULARGE_INTEGER fileTime;
    fileTime.HighPart = findData.ftLastWriteTime.dwHighDateTime;
    fileTime.LowPart = findData.ftLastWriteTime.dwLowDateTime;

    // Information about converting time_t to FileTime is available at http://msdn.microsoft.com/en-us/library/ms724228%28v=vs.85%29.aspx
    time = fileTime.QuadPart / 10000000 - kSecondsFromFileTimeToTimet;
}

bool getFileSize(const String& path, long long& size)
{
    WIN32_FIND_DATAW findData;
    if (!getFindData(path, findData))
        return false;

    return getFileSizeFromFindData(findData, size);
}

bool getFileSize(PlatformFileHandle fileHandle, long long& size)
{
    BY_HANDLE_FILE_INFORMATION fileInformation;
    if (!::GetFileInformationByHandle(fileHandle, &fileInformation))
        return false;

    return getFileSizeFromByHandleFileInformationStructure(fileInformation, size);
}

Optional<WallTime> getFileModificationTime(const String& path)
{
    WIN32_FIND_DATAW findData;
    if (!getFindData(path, findData))
        return WTF::nullopt;

    time_t time = 0;
    getFileModificationTimeFromFindData(findData, time);
    return WallTime::fromRawSeconds(time);
}

Optional<WallTime> getFileCreationTime(const String& path)
{
    WIN32_FIND_DATAW findData;
    if (!getFindData(path, findData))
        return WTF::nullopt;

    time_t time = 0;
    getFileCreationTimeFromFindData(findData, time);
    return WallTime::fromRawSeconds(time);
}

static String getFinalPathName(const String& path)
{
    auto handle = openFile(path, FileOpenMode::Read);
    if (!isHandleValid(handle))
        return String();

    Vector<UChar> buffer(MAX_PATH);
    if (::GetFinalPathNameByHandleW(handle, wcharFrom(buffer.data()), buffer.size(), VOLUME_NAME_NT) >= MAX_PATH) {
        closeFile(handle);
        return String();
    }
    closeFile(handle);

    buffer.shrink(wcslen(wcharFrom(buffer.data())));
    return String::adopt(WTFMove(buffer));
}

static inline bool isSymbolicLink(WIN32_FIND_DATAW findData)
{
    return findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT && findData.dwReserved0 == IO_REPARSE_TAG_SYMLINK;
}

static FileMetadata::Type toFileMetadataType(WIN32_FIND_DATAW findData)
{
    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return FileMetadata::Type::Directory;
    if (isSymbolicLink(findData))
        return FileMetadata::Type::SymbolicLink;
    return FileMetadata::Type::File;
}

static Optional<FileMetadata> findDataToFileMetadata(WIN32_FIND_DATAW findData)
{
    long long length;
    if (!getFileSizeFromFindData(findData, length))
        return WTF::nullopt;

    time_t modificationTime;
    getFileModificationTimeFromFindData(findData, modificationTime);

    return FileMetadata {
        WallTime::fromRawSeconds(modificationTime),
        length,
        static_cast<bool>(findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN),
        toFileMetadataType(findData)
    };
}

Optional<FileMetadata> fileMetadata(const String& path)
{
    WIN32_FIND_DATAW findData;
    if (!getFindData(path, findData))
        return WTF::nullopt;

    return findDataToFileMetadata(findData);
}

Optional<FileMetadata> fileMetadataFollowingSymlinks(const String& path)
{
    WIN32_FIND_DATAW findData;
    if (!getFindData(path, findData))
        return WTF::nullopt;

    if (isSymbolicLink(findData)) {
        String targetPath = getFinalPathName(path);
        if (targetPath.isNull())
            return WTF::nullopt;
        if (!getFindData(targetPath, findData))
            return WTF::nullopt;
    }

    return findDataToFileMetadata(findData);
}

bool createSymbolicLink(const String& targetPath, const String& symbolicLinkPath)
{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    return !::CreateSymbolicLinkW(symbolicLinkPath.wideCharacters().data(), targetPath.wideCharacters().data(), 0);
#else
  return false;
#endif
}

bool fileExists(const String& path)
{
#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
  auto handle = openFile(path, FileOpenMode::Read);
  if (!isHandleValid(handle))
    return false;

  closeFile(handle);
  return true;
#else
    WIN32_FIND_DATAW findData;
    return getFindData(path, findData);
#endif
}

bool deleteFile(const String& path)
{
    String filename = path;
    return !!DeleteFileW(filename.wideCharacters().data());
}

bool deleteEmptyDirectory(const String& path)
{
    String filename = path;
    return !!RemoveDirectoryW(filename.wideCharacters().data());
}

bool moveFile(const String& oldPath, const String& newPath)
{
    String oldFilename = oldPath;
    String newFilename = newPath;
    return !!::MoveFileEx(oldFilename.wideCharacters().data(), newFilename.wideCharacters().data(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
}

String pathByAppendingComponent(const String& path, const String& component)
{
#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    // Perform simple UTF-8 concatenation
    if (path.isEmpty())
        return component;

    if (component.isEmpty())
      return path;

    if (path.endsWith('\\') || component.startsWith('\\'))
        return path + component;
    
    return path + '\\' + component;
#else
    Vector<UChar> buffer(MAX_PATH);
    if (path.length() + 1 > buffer.size())
        return String();

    StringView(path).getCharactersWithUpconvert(buffer.data());
    buffer[path.length()] = '\0';

    if (!PathAppendW(wcharFrom(buffer.data()), component.wideCharacters().data()))
        return String();

    buffer.shrink(wcslen(wcharFrom(buffer.data())));
    return String::adopt(WTFMove(buffer));
#endif
}

String pathByAppendingComponents(StringView path, const Vector<StringView>& components)
{
    String result = path.toString();
    for (auto& component : components)
        result = pathByAppendingComponent(result, component.toString());
    return result;
}

#if !USE(CF)

CString fileSystemRepresentation(const String& path)
{
    auto characters = wcharFrom(StringView(path).upconvertedCharacters());
    int size = WideCharToMultiByte(CP_ACP, 0, characters, path.length(), 0, 0, 0, 0) - 1;

    char* buffer;
    CString string = CString::newUninitialized(size, buffer);

    WideCharToMultiByte(CP_ACP, 0, characters, path.length(), buffer, size, 0, 0);

    return string;
}

#endif // !USE(CF)

#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
/**
 * Creates all directories down to the specified path using pure WinAPI calls (no Shell API).
 *
 * The path should be absolute and not be terminated by a path separator.
 */
bool createDirectoryRecursively(const std::wstring &directory) {
  static const std::wstring separators(L"\\/");
 
  // If the specified directory name doesn't exist, do our thing
  DWORD fileAttributes = ::GetFileAttributesW(directory.c_str());
  if(fileAttributes == INVALID_FILE_ATTRIBUTES) {

    // Recursively do it all again for the parent directory, if any
    std::size_t slashIndex = directory.find_last_of(separators);
    if(slashIndex != std::wstring::npos) {
      createDirectoryRecursively(directory.substr(0, slashIndex));
    }
 
    // Create the last directory on the path (the recursive calls will have taken
    // care of the parent directories by now)
    BOOL result = ::CreateDirectoryW(directory.c_str(), nullptr);
    if(result == FALSE) {
      return false; // Could not create directory
    }
  } else { // Specified directory name already exists as a file or directory

    bool isDirectoryOrJunction =
      ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
      ((fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);
 
    if(!isDirectoryOrJunction) {
      return false; // Could not create directory because a file with the same name exists
    }
  }

  return true;
}
#endif // !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

bool makeAllDirectories(const String& path)
{
#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    return createDirectoryRecursively(path.wideCharacters().data());
#else
    String fullPath = path;
    if (SHCreateDirectoryEx(0, fullPath.wideCharacters().data(), 0) != ERROR_SUCCESS) {
        DWORD error = GetLastError();
        if (error != ERROR_FILE_EXISTS && error != ERROR_ALREADY_EXISTS) {
            LOG_ERROR("Failed to create path %s", path.ascii().data());
            return false;
        }
    }
    return true;
#endif
}

String homeDirectoryPath()
{
    return "";
}

String pathGetFileName(const String& path)
{
#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    return path.substring(path.reverseFind('/') + 1);
#else
    return String(::PathFindFileName(path.wideCharacters().data()));
#endif
}

String directoryName(const String& path)
{
    String name = path.left(path.length() - pathGetFileName(path).length());
    if (name.characterStartingAt(name.length() - 1) == '\\') {
        // Remove any trailing "\".
        name.truncate(name.length() - 1);
    }
    return name;
}

static String bundleName()
{
    static const NeverDestroyed<String> name = [] {
        String name { "WebKit"_s };

#if USE(CF)
        if (CFBundleRef bundle = CFBundleGetMainBundle()) {
            if (CFTypeRef bundleExecutable = CFBundleGetValueForInfoDictionaryKey(bundle, kCFBundleExecutableKey)) {
                if (CFGetTypeID(bundleExecutable) == CFStringGetTypeID())
                    name = reinterpret_cast<CFStringRef>(bundleExecutable);
            }
        }
#endif

        return name;
    }();

    return name;
}

static String storageDirectory(DWORD pathIdentifier)
{
#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    Vector<UChar> buffer(MAX_PATH);
    DWORD len = GetCurrentDirectoryW(MAX_PATH, wcharFrom(buffer.data()));
    if (!len)
        return String();

    buffer.shrink(wcslen(wcharFrom(buffer.data())));
    String directory = String::adopt(WTFMove(buffer));

    directory = pathByAppendingComponent(directory, "appcache\\Ultralight\\" + bundleName());
    if (!makeAllDirectories(directory))
        return String();

    return directory;
#else
    Vector<UChar> buffer(MAX_PATH);
    if (FAILED(SHGetFolderPathW(0, pathIdentifier | CSIDL_FLAG_CREATE, 0, 0, wcharFrom(buffer.data()))))
        return String();

    buffer.shrink(wcslen(wcharFrom(buffer.data())));
    String directory = String::adopt(WTFMove(buffer));

    directory = pathByAppendingComponent(directory, "Ultralight\\" + bundleName());
    if (!makeAllDirectories(directory))
        return String();

    return directory;
#endif // !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
}

static String cachedStorageDirectory(DWORD pathIdentifier)
{
    static HashMap<DWORD, String> directories;

    HashMap<DWORD, String>::iterator it = directories.find(pathIdentifier);
    if (it != directories.end())
        return it->value;

    String directory = storageDirectory(pathIdentifier);
    directories.add(pathIdentifier, directory);

    return directory;
}

static String generateTemporaryPath(const Function<bool(const String&)>& action)
{
    wchar_t tempPath[MAX_PATH];
    int tempPathLength = ::GetTempPathW(WTF_ARRAY_LENGTH(tempPath), tempPath);
    if (tempPathLength <= 0 || tempPathLength > WTF_ARRAY_LENGTH(tempPath))
        return String();

    String proposedPath;
    do {
        wchar_t tempFile[] = L"XXXXXXXX.tmp"; // Use 8.3 style name (more characters aren't helpful due to 8.3 short file names)
        const int randomPartLength = 8;
        cryptographicallyRandomValues(tempFile, randomPartLength * sizeof(wchar_t));

        // Limit to valid filesystem characters, also excluding others that could be problematic, like punctuation.
        // don't include both upper and lowercase since Windows file systems are typically not case sensitive.
        const char validChars[] = "0123456789abcdefghijklmnopqrstuvwxyz";
        for (int i = 0; i < randomPartLength; ++i)
            tempFile[i] = validChars[tempFile[i] % (sizeof(validChars) - 1)];

        ASSERT(wcslen(tempFile) == WTF_ARRAY_LENGTH(tempFile) - 1);

        proposedPath = pathByAppendingComponent(tempPath, tempFile);
        if (proposedPath.isEmpty())
            break;
    } while (!action(proposedPath));

    return proposedPath;
}

String openTemporaryFile(const String&, PlatformFileHandle& handle)
{
    handle = INVALID_HANDLE_VALUE;

    String proposedPath = generateTemporaryPath([&handle](const String& proposedPath) {
        // use CREATE_NEW to avoid overwriting an existing file with the same name
        handle = ::CreateFileW(proposedPath.wideCharacters().data(), GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);

        return isHandleValid(handle) || GetLastError() == ERROR_ALREADY_EXISTS;
    });

    if (!isHandleValid(handle))
        return String();

    return proposedPath;
}

PlatformFileHandle openFile(const String& path, FileOpenMode mode)
{
    DWORD desiredAccess = 0;
    DWORD creationDisposition = 0;
    DWORD shareMode = 0;
    switch (mode) {
    case FileOpenMode::Read:
        desiredAccess = GENERIC_READ;
        creationDisposition = OPEN_EXISTING;
        shareMode = FILE_SHARE_READ;
        break;
    case FileOpenMode::Write:
        desiredAccess = GENERIC_WRITE;
        creationDisposition = CREATE_ALWAYS;
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    String destination = path;
    return CreateFile(destination.wideCharacters().data(), desiredAccess, shareMode, 0, creationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
}

void closeFile(PlatformFileHandle& handle)
{
    if (isHandleValid(handle)) {
        ::CloseHandle(handle);
        handle = invalidPlatformFileHandle;
    }
}

long long seekFile(PlatformFileHandle handle, long long offset, FileSeekOrigin origin)
{
    DWORD moveMethod = FILE_BEGIN;

    if (origin == FileSeekOrigin::Current)
        moveMethod = FILE_CURRENT;
    else if (origin == FileSeekOrigin::End)
        moveMethod = FILE_END;

    LARGE_INTEGER largeOffset;
    largeOffset.QuadPart = offset;

    largeOffset.LowPart = SetFilePointer(handle, largeOffset.LowPart, &largeOffset.HighPart, moveMethod);

    if (largeOffset.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
        return -1;

    return largeOffset.QuadPart;
}

int writeToFile(PlatformFileHandle handle, const char* data, int length)
{
    if (!isHandleValid(handle))
        return -1;

    DWORD bytesWritten;
    bool success = WriteFile(handle, data, length, &bytesWritten, 0);

    if (!success)
        return -1;
    return static_cast<int>(bytesWritten);
}

int readFromFile(PlatformFileHandle handle, char* data, int length)
{
    if (!isHandleValid(handle))
        return -1;

    DWORD bytesRead;
    bool success = ::ReadFile(handle, data, length, &bytesRead, 0);

    if (!success)
        return -1;
    return static_cast<int>(bytesRead);
}

bool hardLink(const String& source, const String& destination)
{
    return CreateHardLink(destination.wideCharacters().data(), source.wideCharacters().data(), nullptr);
}

bool hardLinkOrCopyFile(const String& source, const String& destination)
{
    if (hardLink(source, destination))
        return true;

    // Hard link failed. Perform a copy instead.
    return !!::CopyFile(source.wideCharacters().data(), destination.wideCharacters().data(), TRUE);
}

String localUserSpecificStorageDirectory()
{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) || WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
    return cachedStorageDirectory(CSIDL_LOCAL_APPDATA);
#else
    return String();
#endif
}

String roamingUserSpecificStorageDirectory()
{
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) || WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
    return cachedStorageDirectory(CSIDL_APPDATA);
#else
    return String();
#endif
}

Vector<String> listDirectory(const String& directory, const String& filter)
{
    Vector<String> entries;

    PathWalker walker(directory, filter);
    if (!walker.isValid())
        return entries;

    do {
        if (walker.data().dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
            && (!wcscmp(walker.data().cFileName, L".") || !wcscmp(walker.data().cFileName, L"..")))
            continue;

        entries.append(directory + "\\" + reinterpret_cast<const UChar*>(walker.data().cFileName));
    } while (walker.step());

    return entries;
}

bool getVolumeFreeSpace(const String&, uint64_t&)
{
    return false;
}

Optional<int32_t> getFileDeviceId(const CString& fsFile)
{
    auto handle = openFile(fsFile.data(), FileOpenMode::Read);
    if (!isHandleValid(handle))
        return WTF::nullopt;

    BY_HANDLE_FILE_INFORMATION fileInformation = { };
    if (!::GetFileInformationByHandle(handle, &fileInformation)) {
        closeFile(handle);
        return WTF::nullopt;
    }

    closeFile(handle);

    return fileInformation.dwVolumeSerialNumber;
}

String realPath(const String& filePath)
{
    return getFinalPathName(filePath);
}

String createTemporaryDirectory()
{
    return generateTemporaryPath([](const String& proposedPath) {
        return makeAllDirectories(proposedPath);
    });
}

#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
class SearchHandleScope {
  HANDLE searchHandle;
 public:
  SearchHandleScope(HANDLE searchHandle) : searchHandle(searchHandle) {}
  ~SearchHandleScope() { ::FindClose(this->searchHandle); }
};
 
/**
 * Recursively deletes the specified directory and all its contents using plain WinAPI
 * calls (no Shell API).
 * 
 * The path should be absolute and not be terminated by a path separator.
 */
bool recursiveDeleteDirectory(const std::wstring &path) {
  static const std::wstring allFilesMask(L"\\*");
 
  WIN32_FIND_DATAW findData;
 
  // First, delete the contents of the directory, recursively for subdirectories
  std::wstring searchMask = path + allFilesMask;
  HANDLE searchHandle = ::FindFirstFileExW(
    searchMask.c_str(), FindExInfoBasic, &findData, FindExSearchNameMatch, nullptr, 0
  );
  if(searchHandle == INVALID_HANDLE_VALUE) {
    DWORD lastError = ::GetLastError();
    if(lastError != ERROR_FILE_NOT_FOUND) { // or ERROR_NO_MORE_FILES, ERROR_NOT_FOUND?
      return false; //Could not start directory enumeration
    }
  }
 
  // Did this directory have any contents? If so, delete them first
  if(searchHandle != INVALID_HANDLE_VALUE) {
    SearchHandleScope scope(searchHandle);
    for(;;) {
 
      // Do not process the obligatory '.' and '..' directories
      if(findData.cFileName[0] != '.') {
        bool isDirectory = 
          ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
          ((findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);
 
        // Subdirectories need to be handled by deleting their contents first
        std::wstring filePath = path + L'\\' + findData.cFileName;
        if(isDirectory) {
          recursiveDeleteDirectory(filePath);
        } else {
          BOOL result = ::DeleteFileW(filePath.c_str());
          if(result == FALSE) {
            return false; //Could not delete file
          }
        }
      }
 
      // Advance to the next file in the directory
      BOOL result = ::FindNextFileW(searchHandle, &findData);
      if(result == FALSE) {
        DWORD lastError = ::GetLastError();
        if(lastError != ERROR_NO_MORE_FILES) {
          return false; // Error enumerating directory
        }
        break; // All directory contents enumerated and deleted
      }
 
    } // for
  }
 
  // The directory is empty, we can now safely remove it
  BOOL result = ::RemoveDirectory(path.c_str());
  return !!result;
}
#endif // !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)

bool deleteNonEmptyDirectory(const String& directoryPath)
{
#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    return recursiveDeleteDirectory(directoryPath.wideCharacters().data());
#else
    SHFILEOPSTRUCT deleteOperation = {
        nullptr,
        FO_DELETE,
        directoryPath.wideCharacters().data(),
        L"",
        FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
        false,
        0,
        L""
    };
    return !SHFileOperation(&deleteOperation);
#endif // !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
}

MappedFileData::~MappedFileData()
{
    if (!m_fileData)
        return;
    UnmapViewOfFile(m_fileData);
}

MappedFileData::MappedFileData(const String& filePath, bool& success)
{
    success = false;
    auto file = CreateFile(filePath.wideCharacters().data(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return;

    long long size;
    if (!getFileSize(file, size) || size > std::numeric_limits<size_t>::max() || size > std::numeric_limits<decltype(m_fileSize)>::max()) {
        CloseHandle(file);
        return;
    }

    if (!size) {
        CloseHandle(file);
        success = true;
        return;
    }

    auto mapping = CreateFileMapping(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
    CloseHandle(file);
    if (!mapping)
        return;

    m_fileData = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, size);
    CloseHandle(mapping);
    if (!m_fileData)
        return;
    m_fileSize = size;
    success = true;
}

} // namespace FileSystemImpl
} // namespace WTF
