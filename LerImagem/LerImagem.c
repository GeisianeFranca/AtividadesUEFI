#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiApplicationEntryPoint.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/LoadedImage.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/BlockIo.h>

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
      text[cont] = Key.UnicodeChar;
      cont++;
      Print(L"%c", Key.UnicodeChar);
    }while(Key.UnicodeChar!=13);

  text[cont-1] = '\0';
}


EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  CHAR16 *NameImg = AllocateZeroPool(sizeof(CHAR16)*100);
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_HANDLE BufferHandle= NULL;
  EFI_HANDLE AppImageHandle = NULL;
  UINTN BufferSize = sizeof(EFI_HANDLE);
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
  EFI_LOADED_IMAGE_PROTOCOL *Image = NULL;
  EFI_DEVICE_PATH_PROTOCOL *PathImage = NULL;

  Status = gBS->LocateHandle(
    ByProtocol,
    &gEfiSimpleFileSystemProtocolGuid,
    NULL,
    &BufferSize,
    &BufferHandle);
  if(EFI_ERROR(Status)){
    Print(L"Could not find any FAT volume: %r\n", Status);
    goto bottom;
  }

  Status = gBS->OpenProtocol(
      BufferHandle,
      &gEfiSimpleFileSystemProtocolGuid,
      (VOID**) &FileSystem,
      ImageHandle,
      NULL,
      EFI_OPEN_PROTOCOL_GET_PROTOCOL);
  if (EFI_ERROR(Status)) {
      Print(L"Could not open FAT sile system protocol: %r\n", Status);
      goto bottom;
  }

  CIN(NameImg);
  Print(L"\n");
  PathImage = FileDevicePath(BufferHandle, NameImg);
  Print(L"%s\n",ConvertDevicePathToText(PathImage, FALSE, FALSE));
  Status = gBS->LoadImage(
          FALSE,
          ImageHandle,
          PathImage,
          (VOID*) NULL,
          0,
          &AppImageHandle);
  if(EFI_ERROR(Status)){
    Print(L"Could not load image: %r\n", Status);
    goto bottom;
  }
  else{
    Print(L"Loaded the image with success\n");
  }
  Status = gBS->OpenProtocol(
          AppImageHandle,
          &gEfiLoadedImageProtocolGuid,
          (VOID**) &Image,
          ImageHandle,
          (VOID*) NULL,
          EFI_OPEN_PROTOCOL_GET_PROTOCOL
          );
  if (Status != EFI_SUCCESS) {
      Print(L"Protocol is not supported - %r\n", Status);
      return Status;
  }
  else{
      Print(L"ImageInfo opened\n");
  }

  Print(L"Image start:\n");
  Status = gBS->StartImage(AppImageHandle, (UINTN*) NULL, (CHAR16**) NULL);

  bottom:
    return Status;
}
