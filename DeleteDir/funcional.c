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
  UINTN BufferSize = sizeof(EFI_HANDLE);
  EFI_HANDLE BufferHandle = NULL;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
  EFI_FILE_PROTOCOL *RootDir = NULL;
  EFI_FILE_PROTOCOL *FileConf = NULL;
  EFI_FILE_PROTOCOL *FPath = NULL;
  EFI_FILE_PROTOCOL *File = NULL;
  CHAR8 *BufferTemp = AllocatePool(2*sizeof(CHAR8));
  CHAR8 *Buffer = AllocatePool(512*sizeof(CHAR8));
  CHAR16 *Buffer16=AllocatePool(512*sizeof(CHAR16));

  Status = gBS->LocateHandle(
          ByProtocol,
          &gEfiSimpleFileSystemProtocolGuid,
          NULL,
          &BufferSize,
          &BufferHandle);

  if (Status == EFI_BUFFER_TOO_SMALL) {
      Print(L"Required handle buffer of size: %d\n", BufferSize);
  } else if (EFI_ERROR(Status)) {
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

  Status = FileSystem->OpenVolume(
            FileSystem,
            &RootDir);
  if (EFI_ERROR(Status)) {
      Print(L"Could not open root dir: %r\n", Status);
      //goto close_fs;
  }
  Status = RootDir->Open(
            RootDir,
            &FileConf,
            L"file_deleteion.conf",
            EFI_FILE_MODE_READ,
            0);
  if (EFI_ERROR(Status)) {
      Print(L"Could not open File : %r\n", Status);
      //goto close_file;
  }

  Status = RootDir->Open(
            RootDir,
            &File,
            L"root_deletion",
            EFI_FILE_MODE_READ,
            0);
  if (EFI_ERROR(Status)) {
      Print(L"Could not open File : %r\n", Status);
      //goto close_file;
  }



Buffer[0] = '\0';
    while (1){
        BufferSize = sizeof(CHAR8);
        Status = FileConf->Read(FileConf,&BufferSize,BufferTemp);
        if (EFI_ERROR(Status)) {
            Print(L"Could not read file: %r\n", Status);
        }

        BufferTemp[1] = '\0';
        if(*BufferTemp == '\n' && *Buffer != '\0'){
           AsciiStrToUnicodeStr(Buffer, Buffer16);
           Status = File->Open(
                     File,
                     &FPath,
                     Buffer16,
                     EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                     0);
           if (EFI_ERROR(Status)) {
               Print(L"Could not open File : %r\n", Status);
               //goto close_file;
           }

           Status = FPath->Delete(FPath);
           if(EFI_ERROR(Status)){
             Print(L"Could not delete file: %r\n", Status);
           }

           Buffer[0] = '\0';
       }
       else
          AsciiStrCat(Buffer, BufferTemp);
      if(BufferSize == 0)
          break;
    }

  bottom:
    return Status;
}
