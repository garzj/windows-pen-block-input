#include "log.hpp"

std::wstring GetErrorStr(DWORD err) {
  if (err == 0) return std::wstring();

  LPWSTR messageBuffer = nullptr;

  size_t size = FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    err,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPWSTR)&messageBuffer,
    0,
    NULL);

  std::wstring message(messageBuffer, size);

  LocalFree(messageBuffer);

  return message;
}

DWORD DumpLastError() {
  DWORD err = GetLastError();
  std::wcout << "Error " << err << ": " << GetErrorStr(err) << "\n";
  return err;
}
