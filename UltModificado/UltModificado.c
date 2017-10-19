#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiApplicationEntryPoint.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>





UINTN Compara(EFI_TIME *FileInfo1, EFI_TIME *FileInfo2){
  if(FileInfo1->Year < FileInfo2->Year){
    return 1;
    //  *FileInfo1 = *FileInfo2;
    //  StrCpy(FileInfo1->FileName, FileInfo2->FileName);
  }
  else if(FileInfo1->Year == FileInfo2->Year){
    if(FileInfo1->Month < FileInfo2->Month){
      return 1;
    }
    else if(FileInfo1->Month == FileInfo2->Month){
      if(FileInfo1->Day < FileInfo2->Day){
        return 1;
      }
      else if(FileInfo1->Day == FileInfo2->Day){
        if(FileInfo1->Hour < FileInfo2->Hour){
          return 1;
        }
        else if(FileInfo1->Hour == FileInfo2->Hour){
          if(FileInfo1->Minute < FileInfo2->Minute){
            return 1;
          }
            else if(FileInfo1->Minute == FileInfo2->Minute){
              if(FileInfo1->Second < FileInfo2->Second){
                return 1;
                }
            }
        }
      }
    }
  }
  return -1;
}

VOID Verifica(EFI_FILE_PROTOCOL *File, EFI_FILE_INFO *LastModification){
  EFI_FILE_PROTOCOL* FileAux = NULL;
//CHAR16 *Temp = AllocateZeroPool(512*sizeof(CHAR16));
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
    if((Buffer->Attribute & EFI_FILE_DIRECTORY )== EFI_FILE_DIRECTORY){
      Status = File->Open(File,&FileAux, Buffer->FileName, EFI_FILE_MODE_READ,0);
      if(EFI_ERROR(Status)){
        goto close_file;
      }


      Verifica(FileAux, LastModification);
    }else{
      if(Compara(&LastModification->ModificationTime, &Buffer->ModificationTime) == 1){
        *LastModification = *Buffer;
        StrCpy(LastModification->FileName, Buffer->FileName);
      }


    }
  }

    close_file:
      File->Close(File);

}

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    EFI_STATUS Status = EFI_SUCCESS;
//UINTN BufferSize ;
    UINTN QtdHandles;
    EFI_HANDLE *BufferHandle = NULL;
    UINTN IndexHandle = 0;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
    EFI_FILE_PROTOCOL *RootDir = NULL;
    EFI_FILE_INFO *LastModification = AllocateZeroPool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));
    LastModification->FileName[0]='\0';
  //  EFI_FILE_INFO *Buffer = AllocatePool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));

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

    for(IndexHandle=0; IndexHandle<QtdHandles; IndexHandle++){
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
      //Status = RootDir->Open(RootDir,&RootDir, L"RC", EFI_FILE_MODE_READ, 0);
        Verifica(RootDir, LastModification);
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
