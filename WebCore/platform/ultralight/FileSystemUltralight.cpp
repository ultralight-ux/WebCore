#include "config.h"
#include "FileSystem.h"
#include "NotImplemented.h"
#include <Ultralight/private/Painter.h>
#include <Ultralight/Platform/FileSystem.h>
#include <Ultralight/Platform/Platform.h>
#include "StringUltralight.h"

namespace WebCore {

using ultralight::Convert;

inline ultralight::FileSystem* fileSystem() { return ultralight::Platform::instance().file_system(); }

bool fileExists(const String& path) {
  auto fs = fileSystem(); if (!fs) return false;
  return fs->FileExists(Convert(path));
}

bool deleteFile(const String& path) {
  auto fs = fileSystem(); if (!fs) return false;
  return fs->DeleteFile_(Convert(path));
}

bool deleteEmptyDirectory(const String& path) {
  auto fs = fileSystem(); if (!fs) return false;
  return fs->DeleteEmptyDirectory(Convert(path));
}

bool moveFile(const String& oldPath, const String& newPath) {
  auto fs = fileSystem(); if (!fs) return false;
  return fs->MoveFile_(Convert(oldPath), Convert(newPath));
}

bool getFileSize(const String& path, long long& result) {
  auto fs = fileSystem(); if (!fs) return false;
  return fs->GetFileSize(Convert(path), result);
}

bool getFileSize(PlatformFileHandle handle, long long& result) {
  auto fs = fileSystem(); if (!fs) return false;
  return fs->GetFileSize((ultralight::FileHandle)handle, result);
}

bool getFileModificationTime(const String& path, time_t& result) {
  auto fs = fileSystem(); if (!fs) return false;
  return fs->GetFileModificationTime(Convert(path), result);
}

bool getFileCreationTime(const String& path, time_t& result) {
  auto fs = fileSystem(); if (!fs) return false;
  return fs->GetFileCreationTime(Convert(path), result);
}

bool getFileMetadata(const String& path, FileMetadata&) { 
  notImplemented();
  return false;
}

String pathByAppendingComponent(const String& path, const String& component) {
  auto fs = fileSystem(); if (!fs) return String();
  return Convert(fs->GetPathByAppendingComponent(Convert(path), Convert(component)));
}

//String lastComponentOfPathIgnoringTrailingSlash(const String& path) { notImplemented(); return String(); }

bool makeAllDirectories(const String& path) {
  auto fs = fileSystem(); if (!fs) return false;
  return fs->CreateDirectory_(Convert(path));
}

String homeDirectoryPath() {
  auto fs = fileSystem(); if (!fs) return String();
  return Convert(fs->GetHomeDirectory());
}

String pathGetFileName(const String& path) {
  auto fs = fileSystem(); if (!fs) return String();
  return Convert(fs->GetFilenameFromPath(Convert(path)));
}

String directoryName(const String& path) {
  auto fs = fileSystem(); if (!fs) return String();
  return Convert(fs->GetDirectoryNameFromPath(Convert(path)));
}

bool getVolumeFreeSpace(const String& path, uint64_t& result) {
  auto fs = fileSystem(); if (!fs) return false;
  return fs->GetVolumeFreeSpace(Convert(path), result);
}

std::optional<int32_t> getFileDeviceId(const CString& path) {
  auto fs = fileSystem(); if (!fs) return std::optional<int32_t>();
  String wtf_path = String::fromUTF8(path);
  return fs->GetVolumeId(Convert(wtf_path));
}

//void setMetadataURL(String& URLString, const String& referrer, const String& path) { notImplemented(); }

//bool canExcludeFromBackup() { notImplemented(); return false; }
//bool excludeFromBackup(const String& path) { notImplemented(); return false; }

Vector<String> listDirectory(const String& path, const String& filter) {
  auto fs = fileSystem(); if (!fs) return Vector<String>();
  auto items = fs->ListDirectory(Convert(path), Convert(filter));
  Vector<String> result(items.size());
  for (size_t i = 0; i < items.size(); ++i)
    result[i] = Convert(items[i]);

  return result;
}

//CString fileSystemRepresentation(const String& path) { notImplemented(); return CString(); }
//String stringFromFileSystemRepresentation(const char*) { notImplemented(); return String(); }

String openTemporaryFile(const String& prefix, PlatformFileHandle& handle) {
  auto fs = fileSystem(); if (!fs) return String();
  return Convert(fs->OpenTemporaryFile(Convert(prefix), handle));
}

PlatformFileHandle openFile(const String& path, FileOpenMode mode) {
  auto fs = fileSystem(); if (!fs) return PlatformFileHandle();
  return fs->OpenFile(Convert(path), mode == OpenForWrite);
}

void closeFile(PlatformFileHandle& handle) {
  auto fs = fileSystem(); if (!fs) return;
  fs->CloseFile(handle);
}

// Returns the resulting offset from the beginning of the file if successful, -1 otherwise.
long long seekFile(PlatformFileHandle handle, long long offset, FileSeekOrigin origin) {
  auto fs = fileSystem(); if (!fs) return -1;
  return fs->SeekFile(handle, offset, (ultralight::FileSeekOrigin)origin);
}

bool truncateFile(PlatformFileHandle handle, long long offset) {
  auto fs = fileSystem(); if (!fs) return false;
  return fs->TruncateFile(handle, offset);
}

// Returns number of bytes actually read if successful, -1 otherwise.
int writeToFile(PlatformFileHandle handle, const char* data, int length) {
  auto fs = fileSystem(); if (!fs) return -1;
  return fs->WriteToFile(handle, data, length);
}

// Returns number of bytes actually written if successful, -1 otherwise.
int readFromFile(PlatformFileHandle handle, char* data, int length) {
  auto fs = fileSystem(); if (!fs) return -1;
  return fs->ReadFromFile(handle, data, length);
}

// Appends the contents of the file found at 'path' to the open PlatformFileHandle.
// Returns true if the write was successful, false if it was not.
//bool appendFileContentsToFileHandle(const String& path, PlatformFileHandle&) { notImplemented(); return false; }

// Hard links a file if possible, copies it if not.
bool hardLinkOrCopyFile(const String& source, const String& destination) {
  auto fs = fileSystem(); if (!fs) return false;
  return fs->CopyFile_(Convert(source), Convert(destination));
}

#if USE(FILE_LOCK)
bool lockFile(PlatformFileHandle, FileLockMode) { notImplemented(); return false; }
bool unlockFile(PlatformFileHandle) { notImplemented(); return false; }
#endif

// Functions for working with loadable modules.
bool unloadModule(PlatformModule) { notImplemented(); return false; }

//String encodeForFileName(const String& path) { notImplemented(); return String(); }
//String decodeFromFilename(const String& path) { notImplemented(); return String(); }

CString fileSystemRepresentation(const String& path)
{
  // TODO: is this right?
  return path.utf8();
}

String stringFromFileSystemRepresentation(const char* fileSystemRepresentation)
{
  // TODO: is this right?
  return String::fromUTF8(fileSystemRepresentation);
}

}  // namespace WebCore
