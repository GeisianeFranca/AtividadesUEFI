#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiApplicationEntryPoint.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Library/MemoryAllocationLib.h>

VOID CIN(CHAR16* text){
  EFI_STATUS  Status;
  UINTN Index;
  UINTN cont = 0;
  EFI_INPUT_KEY Key;
    do{
      Status = gBS->WaitForEvent (1, &gST->ConIn->WaitForKey, &Index);
      if (EFI_ERROR (Status) ) {
        Print(L"%r\n", Status);
      }

      gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
      // while(Key.UnicodeChar == 8 && cont > 0){
      //   text[cont--] = '\0';
      // }
      // if(Key.UnicodeChar == 8)
      //   text[cont] = '\0';
      text[cont] = Key.UnicodeChar;
      cont++;
      Print(L"%c", Key.UnicodeChar);
    }while(Key.UnicodeChar!=13);

  text[cont-1] = '\0';
}
EFI_STATUS
EFIAPI
Main (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
){
  CHAR16 *texto = AllocateZeroPool(sizeof(CHAR16)*512);
  CIN(texto);
  Print(L"\n");
  Print(L"Retorno:%s\n", texto);
  return EFI_SUCCESS;
}
