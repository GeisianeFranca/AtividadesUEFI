#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiApplicationEntryPoint.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimpleFileSystem.h>
#include  <Guid/FileInfo.h>
#include  <Library/MemoryAllocationLib.h>
#include  <Library/BaseLib.h>

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN BufferSize = 2;
  EFI_HANDLE *BufferHandle = NULL;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
  EFI_FILE_PROTOCOL *RootDir = NULL;
  EFI_FILE_PROTOCOL *FileConf = NULL;
  UINTN IndexHandle;

  EFI_FILE_PROTOCOL *File = NULL;
  EFI_FILE_PROTOCOL *FPath = NULL;
  CHAR8 *BufferTemp = AllocatePool(2*sizeof(CHAR8));
  CHAR8 *Buffer = AllocatePool(512*sizeof(CHAR8));
  CHAR16 *Buffer16=AllocatePool(512*sizeof(CHAR16));

  Status = gBS->LocateHandleBuffer(
          ByProtocol,
          &gEfiSimpleFileSystemProtocolGuid,
          NULL,
          &BufferSize,
          &BufferHandle);

  if (EFI_ERROR(Status)) {
      Print(L"Could not find any FAT volume: %r\n", Status);
      goto bottom;
  }


for(IndexHandle = 0; IndexHandle<BufferSize; IndexHandle++){
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
    Status = RootDir->Open(
              RootDir,
              &FileConf,
              L"file_deletion.conf",
              EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
              0);
    if (EFI_ERROR(Status)) {
        Print(L"Could not open File : %r\n", Status);
    }
    if (Status != EFI_NOT_FOUND) {
        Status = RootDir->Open(
                  RootDir,
                  &File,
                  L"root_deletion",
                  EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                  0);
        if (EFI_ERROR(Status)) {
            Print(L"Could not open File : %r\n", Status);
        }
        if (Status != EFI_NOT_FOUND) {
              break;
        }
    }
}

Buffer[0] = '\0';
while(1){
  BufferSize = sizeof(CHAR8);
  Status = FileConf->Read(FileConf,&BufferSize,BufferTemp);
  if (EFI_ERROR(Status)) {
      Print(L"Could not read file: %r\n", Status);
  }
  BufferTemp[1] = '\0';
   if(*BufferTemp == '\n' && *Buffer != '\0'){
     AsciiStrToUnicodeStr(Buffer, Buffer16);
     Print(L"Buffer: %s\n", Buffer16);
      Status = File->Open(
                File,
                &FPath,
                Buffer16,
                EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                0);
      if (EFI_ERROR(Status)) {
          Print(L"Could not open File : %r\n", Status);
      }
      else{
        Status = FPath->Delete(FPath);
        if(EFI_ERROR(Status)){
          Print(L"Could not delete file: %r\n", Status);
        }
      }
      Buffer[0] = '\0';

   }
   else{
     AsciiStrCat(Buffer, BufferTemp);
   }

   if(BufferSize == 0)
        break;
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
