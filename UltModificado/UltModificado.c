#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiApplicationEntryPoint.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN BufferSize ;
    UINTN QtdHandles;
    EFI_HANDLE *BufferHandle = NULL;
    UINTN IndexHandle = 0;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
    EFI_FILE_PROTOCOL *RootDir = NULL;
    EFI_FILE_INFO *Buffer = AllocatePool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));
    EFI_FILE_INFO *LastModification = AllocateZeroPool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));

    // EFI_FILE_INFO LastModification;
    //
    // ZeroMem(LastModification, sizeof(EFI_FILE_INFO));

    Status = gBS->LocateHandleBuffer(
      ByProtocol,
      &gEfiSimpleFileSystemProtocolGuid,
      NULL,
      &QtdHandles,
      &BufferHandle);
    if (EFI_ERROR(Status)) {
        Print(L"Could not find any FAT volume: %r\n", Status);
        goto bottom;
      }

    while(IndexHandle<QtdHandles){
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

      while(TRUE){
        BufferSize = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);
        Status = RootDir->Read(RootDir,&BufferSize,(VOID *)Buffer);
        if(EFI_ERROR(Status)|| BufferSize==0){
          IndexHandle++;
          break;
        }

        Print(L"ModificationTime: %t\n", Buffer->ModificationTime);
         if(LastModification->ModificationTime.Year < Buffer->ModificationTime.Year){
           LastModification = Buffer;
         }
         else if(LastModification->ModificationTime.Month < Buffer->ModificationTime.Month){
           LastModification = Buffer;
            }
            else if(LastModification->ModificationTime.Month == Buffer->ModificationTime.Month){
              if(LastModification->ModificationTime.Day < Buffer->ModificationTime.Day){
                LastModification = Buffer;
              }
              else if(LastModification->ModificationTime.Day == Buffer->ModificationTime.Day){
                if(LastModification->ModificationTime.Hour < Buffer->ModificationTime.Hour){
                  LastModification = Buffer;
                }
                else if(LastModification->ModificationTime.Hour == Buffer->ModificationTime.Hour){
                  if(LastModification->ModificationTime.Minute < Buffer->ModificationTime.Minute){
                    LastModification = Buffer;
                  }
                }

              }


            }
      }
    }

    Print(L"LastModification: %s\n", LastModification->FileName);

close_fs:
  gBS->CloseProtocol(
      BufferHandle,
      &gEfiSimpleFileSystemProtocolGuid,
      ImageHandle,
      NULL);

  bottom:
    return Status;
}
