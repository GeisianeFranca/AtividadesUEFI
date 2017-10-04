#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  Print(L"Hello there fellow Programmer.\n");
  Print(L"Welcome to the world of EDK II.\n");

  return EFI_SUCCESS;
}
