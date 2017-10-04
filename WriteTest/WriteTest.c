#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiApplicationEntryPoint.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Library/BaseLib.h>
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
  //  EFI_FILE_INFO *Buffer = AllocatePool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));
    //CHAR8 Buffer[6];

    Status = gBS->LocateHandle(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &BufferSize,
        &NtfsHandle);

    if (Status == EFI_BUFFER_TOO_SMALL) {
        Print(L"Required handle buffer of size: %d\n", BufferSize);
    } else if (EFI_ERROR(Status)) {
        Print(L"Could not find any FAT volume: %r\n", Status);
        goto bottom;
    }
    //Print(L"Found a FAT file system\n");

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
              L"Hello.txt",
              EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE,
              0);
    if (EFI_ERROR(Status)) {
        Print(L"Could not open file: %r\n", Status);
        goto close_root;
    }


    /*  while(TRUE){
        BufferSize = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);
        Status = File->Read(File, &BufferSize, (VOID*) Buffer);
          if(EFI_ERROR(Status)|| BufferSize==0){
            goto close_file;
            break;
          }
          Print(L"%s\n", Buffer->FileName);

      }*/

    CHAR16 *Origem = L"Teste";
    BufferSize = sizeof(CHAR8)*(StrLen(Origem));
    CHAR8 *Buffer = AllocatePool(sizeof(CHAR8)*(StrLen(Origem)+1));
    UnicodeStrToAsciiStr(Origem, Buffer);

    UINT64 Position;
    Status = File->GetPosition(File, &Position);
    Print(L"Position:%d\n",Position);
    if(EFI_ERROR(Status)){
      Print(L"%r\n", Status);
      goto close_file;
    }


    Status = File->Write(File, &BufferSize, Buffer);
    Print(L"BufferSize:%d\n", BufferSize);
    if(EFI_ERROR(Status)){
      Print(L"%r\n", Status);
      goto close_file;
    }

    Status = File->GetPosition(File, &Position);
    Print(L"Position:%d\n",Position);
    if(EFI_ERROR(Status)){
      Print(L"%r\n", Status);
      goto close_file;
    }
    //Position = 0;
    // Status = File->SetPosition(File, Position);
    // if(EFI_ERROR(Status)){
    //   Print(L"%r\n", Status);
    //   goto close_file;
    // }

    Status = File->Flush(File);
    if(EFI_ERROR(Status)){
      Print(L"%r\n", Status);
      goto close_file;
    }

    Status = File->GetPosition(File, &Position);
    Print(L"Position:%d\n",Position);
    if(EFI_ERROR(Status)){
      Print(L"%r\n", Status);
      goto close_file;
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
