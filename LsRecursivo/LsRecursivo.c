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
  UINTN NHandles;
  EFI_HANDLE *BufferHandle = NULL;
  UINTN BufferSize;
  UINTN BufferSize2;
  UINTN IndexHandle = 0;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
  EFI_FILE_PROTOCOL *RootDir = NULL;
  EFI_FILE_PROTOCOL *File = NULL;
  EFI_FILE_PROTOCOL *FileAux = NULL;
  EFI_FILE_INFO *Buffer = AllocateZeroPool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));
  EFI_FILE_INFO *Buffer2 = AllocateZeroPool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));

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
//CHAR16* b = AllocateZeroPool(sizeof(CHAR16)*512);
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

    Status = RootDir->Open(
            RootDir,
            &File,
            L"RC",
            EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
            0);
    if (EFI_ERROR(Status)) {
        Print(L"Could not open directory: %r\n", Status);
        goto close_root;
    }
    while(TRUE){
      BufferSize = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);
      Status = File->GetInfo(File, &gEfiFileInfoGuid, &BufferSize, (VOID*)Buffer);
      if(Buffer->Attribute == EFI_FILE_ARCHIVE){
        Print(L"%s\n", Buffer->FileName);
        break;
      }
      else{
        while(Buffer->Attribute == EFI_FILE_DIRECTORY){
          BufferSize2 = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);
          Buffer->FileName[BufferSize]  = '\0';
          Print(L"%s\n", Buffer->FileName);

            Print(L"Comp1: %d\n", StrCmp(Buffer->FileName,(CHAR16* )".."));
            Print(L"Comp2: %d\n", StrCmp(Buffer->FileName,(CHAR16* )"."));
            Print(L"Buffer:%s/", Buffer->FileName);
            Print(L"Comp3: %d\n", StrCmp(Buffer->FileName,(CHAR16*)"RC\0")) ;

            if(StrCmp(Buffer->FileName,(CHAR16* )"..\0") != 0 && StrCmp(Buffer->FileName,(CHAR16* )".\0") != 0){
              Print(L"%s/", Buffer->FileName);
              Status = RootDir->Read(RootDir, &BufferSize2, Buffer2);
              if(EFI_ERROR(Status)){
                Print(L"Não foi possível ler o arquivo%r\n", Status);
                break;
              }
              Buffer = Buffer2;
              continue;
              if(Buffer2->Attribute == EFI_FILE_ARCHIVE){
                Print(L"%s\n", Buffer2->FileName);
                break;
              }
              Status = RootDir->Open(RootDir, &FileAux,Buffer->FileName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,0);
              if(EFI_ERROR(Status)){
                Print(L"Não foi possível abrir o arquivo %r\n", Status);
                break;
              }
              Buffer = Buffer2;
          }
          else{
            Print(L":()");
          }
          if(BufferSize2 == 0){
            break;
          }
          break;
        }

      }
      if(BufferSize == 0)
        break;

      break;
    }
    IndexHandle++;
  }
  close_root:
      RootDir->Close(RootDir);

  close_fs:
    gBS->CloseProtocol(
        BufferHandle,
        &gEfiSimpleFileSystemProtocolGuid,
        ImageHandle,
        NULL);
  bottom:
    return Status;
}
