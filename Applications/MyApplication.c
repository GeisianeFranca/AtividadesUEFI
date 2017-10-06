#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiApplicationEntryPoint.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Library/MemoryAllocationLib.h>


EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HANDLE NtfsHandle = NULL;
    UINTN BufferSize = sizeof(EFI_HANDLE);
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
    EFI_FILE_PROTOCOL *RootDir = NULL;
    EFI_FILE_PROTOCOL *File = NULL;
    EFI_FILE_INFO *Buffer = AllocatePool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));

    Status = gBS->LocateHandle(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &BufferSize,
        &NtfsHandle);
    if (Status == EFI_BUFFER_TOO_SMALL) {
        Print(L"Required handle buffer of size: %d\n", BufferSize);
    } else if (EFI_ERROR(Status)) {
        Print(L"Could not find any NTFS volume: %r\n", Status);
        goto bottom;
    }
    //Print(L"Found a NTFS file system\n");

    Status = gBS->OpenProtocol(
        NtfsHandle,
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
            &File,
            L"Hello",
            EFI_FILE_MODE_READ,
            EFI_FILE_READ_ONLY);
    if (EFI_ERROR(Status)) {
        Print(L"Could not open directory: %r\n", Status);
        goto close_root;
    }

    //   BufferSize = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);
    // File->GetInfo(File, &gEfiFileInfoGuid, &BufferSize, Buffer);
    //   Print(L"Att:%x\n",Buffer->Attribute);

    // CHAR16 Teste4;
    // CHAR8 * Teste3 = (CHAR8 *)"..\0";
    // CHAR8 * Teste2 = (CHAR8 *)"..\0";
    // CHAR16 Teste;
    while(TRUE){
      BufferSize = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);
      Status = File->Read(File, &BufferSize, (VOID*) Buffer);
        if(EFI_ERROR(Status)|| BufferSize==0){
          goto close_file;
          break;
        }
      // AsciiStrToUnicodeStr(Teste2, &Teste);
      // AsciiStrToUnicodeStr(Teste3, &Teste4);
      // if(StrCmp(Buffer->FileName, &Teste) && StrCmp(Buffer->FileName, &Teste4))
      //if(Buffer->Attribute == EFI_FILE_DIRECTORY)
        Print(L"%s\n",Buffer->FileName);
    }


/*
//Ler Arquivo
  BufferSize = 4;
  Status = File->Read(File, &BufferSize, CharBuffer);
  if (EFI_ERROR(Status)) {
      Print(L"Could not read file: %r\n", Status);
      //goto close_file;
  }

  CharBuffer[BufferSize] = '\0';
  Print(L"Read from file: %a\n", CharBuffer);
*/

close_file:
    File->Close(File);

close_root:
    RootDir->Close(RootDir);

  close_fs:
    gBS->CloseProtocol(
        NtfsHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        ImageHandle,
        NULL);


  bottom:
      return Status;
}
