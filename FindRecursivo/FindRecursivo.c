#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiApplicationEntryPoint.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Library/MemoryAllocationLib.h>
UINTN qtdeFiles= 0;
VOID Verifica(EFI_FILE_PROTOCOL *File, CHAR16 *Name, CHAR16 *Path){
  EFI_FILE_PROTOCOL* FileAux = NULL;
  CHAR16 *Temp = AllocateZeroPool(512*sizeof(CHAR16));
  UINTN BufferSize;
  EFI_FILE_INFO *Buffer = AllocateZeroPool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));
  EFI_STATUS Status = EFI_SUCCESS;

//  AsciiStrToUnicodeStr((CHAR16*)Archive, (CHAR8*)Archive);
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
  //  Print(L"%s\n", Archive);

    if((Buffer->Attribute & EFI_FILE_DIRECTORY )== EFI_FILE_DIRECTORY){
      Status = File->Open(File,&FileAux, Buffer->FileName, EFI_FILE_MODE_READ,0);
      if(EFI_ERROR(Status)){
        goto close_file;
      }
      Verifica(FileAux,Name, StrCat(Temp, L"/"));
    }else{
       if(StrCmp(Buffer->FileName, Name) == 0){
         Print(L"%s\n", Temp);
         qtdeFiles++;
       }
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
  UINTN IndexHandle;
  EFI_HANDLE *BufferHandle = NULL;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
  EFI_FILE_PROTOCOL *RootDir = NULL;
  //EFI_FILE_PROTOCOL *File = NULL;
  //UINTN BufferSize = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);

  Status = gBS->LocateHandleBuffer(
          ByProtocol,
          &gEfiSimpleFileSystemProtocolGuid,
          NULL,
          &NHandles,
          &BufferHandle);
    if (EFI_ERROR(Status)) {
      Print(L"Could not find any FAT volume: %r\n", Status);
      goto bottom;
    }

    for(IndexHandle = 0; IndexHandle<NHandles; IndexHandle++){
      Status = gBS->OpenProtocol(
              BufferHandle[IndexHandle],
              &gEfiSimpleFileSystemProtocolGuid,
              (VOID**) &FileSystem,
              ImageHandle,
              NULL,
              EFI_OPEN_PROTOCOL_GET_PROTOCOL);

      if (EFI_ERROR(Status)) {
          Print(L"Could not open FAT sile system protocol: %r\n", Status);
          goto bottom;
      }
      Status = FileSystem->OpenVolume(
          FileSystem,
          &RootDir);
      if (EFI_ERROR(Status)) {
          Print(L"Could not open root dir: %r\n", Status);
          goto close_fs;
      }
      CHAR16* Archive = L"WriteTeste.efi\0";
      CHAR16* Path = L"/\0";
      Verifica(RootDir, Archive, Path);
      Print(L"%d files found.\n",qtdeFiles);
      // Status = RootDir->Open(
      //         RootDir,
      //         &File,
      //         L"WriteTeste.efi",
      //         EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
      //         0);
      // if(Status != EFI_NOT_FOUND){
      //   if (EFI_ERROR(Status))
      //       Print(L"%r\n", Status);
      //   else{
      //       if(EFI_ERROR(Status)|| BufferSize==0){
      //         Print(L"%r\n", Status);
      //       }
      //       Print(L"Existe!\n");
      //       break;
      //   }
      // }
    }

    if(Status == EFI_NOT_FOUND)
      Print(L"%r\n", Status);

close_fs:
  gBS->CloseProtocol(
      BufferHandle,
      &gEfiSimpleFileSystemProtocolGuid,
      ImageHandle,
      NULL);

  bottom:
    return Status;
}
