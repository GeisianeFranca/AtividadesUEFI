#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiApplicationEntryPoint.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Library/MemoryAllocationLib.h>

VOID LsRecursivo(EFI_FILE_PROTOCOL *File, CHAR16 *Path){
  EFI_FILE_PROTOCOL* FileAux = NULL;
  CHAR16 *Temp = AllocateZeroPool(512*sizeof(CHAR16));
  UINTN BufferSize;
  EFI_FILE_INFO *Buffer = AllocateZeroPool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));
  EFI_STATUS Status = EFI_SUCCESS;
  while(TRUE){
    BufferSize = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);
    Status = File->Read(File, &BufferSize, (VOID*) Buffer);
    if(EFI_ERROR(Status)|| BufferSize==0){
      goto close_file;
    }
    if((StrCmp(Buffer->FileName, (CHAR16*)L"..") == 0 || StrCmp(Buffer->FileName, (CHAR16*)L".") == 0)){
      continue;
    }
    FreePool(Temp);
    Temp = AllocateZeroPool(512*sizeof(CHAR16));
    StrCat(Temp,Path);
    StrCat(Temp,Buffer->FileName);
    Print(L"%s\n",Temp);
    if((Buffer->Attribute & EFI_FILE_DIRECTORY )== EFI_FILE_DIRECTORY){
      Status = File->Open(File,&FileAux, Buffer->FileName, EFI_FILE_MODE_READ,0);
      if(EFI_ERROR(Status)){
        goto close_file;
      }
      LsRecursivo(FileAux,StrCat(Temp, L"/"));
    }
  }
  close_file:
      File->Close(File);
      if(Temp != NULL){
        FreePool(Temp);
      }
}

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN NHandles;
  EFI_HANDLE *BufferHandle = NULL;
  UINTN BufferSize;
  UINTN IndexHandle = 0;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
  EFI_FILE_PROTOCOL *RootDir = NULL;

  Status = gBS->LocateHandleBuffer(
    ByProtocol,
    &gEfiSimpleFileSystemProtocolGuid,
    NULL,
    &NHandles,
    &BufferHandle
  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
      Print(L"Required handle buffer of size: %d\n", BufferSize);
  } else if (EFI_ERROR(Status)) {
      Print(L"Could not find any NTFS volume: %r\n", Status);
      goto bottom;
  }
  while(IndexHandle < NHandles){
    Status = gBS->OpenProtocol(
      BufferHandle[IndexHandle],
      &gEfiSimpleFileSystemProtocolGuid,
      (VOID **) &FileSystem,
      ImageHandle,
      NULL,
      EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(Status)) {
        Print(L"Could not open FAT sile system protocol: %r\n", Status);
        goto bottom;
    }

    Status = FileSystem->OpenVolume(FileSystem,&RootDir);
    if (EFI_ERROR(Status)) {
        Print(L"Could not open root dir: %r\n", Status);
        goto close_fs;
    }

    CHAR16* Path = (CHAR16*)"/\0";
    LsRecursivo(RootDir, Path);
    IndexHandle++;
  }

  close_fs:
    gBS->CloseProtocol(
        BufferHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        ImageHandle,
        NULL);
  bottom:
    return Status;
}
