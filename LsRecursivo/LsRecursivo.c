#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiApplicationEntryPoint.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include <Library/MemoryAllocationLib.h>



 // EFI_FILE_PROTOCOL LsRec(EFI_FILE_PROTOCOL *File){
 //   CHAR16 *Teste = AllocatePool(1024*sizeof(CHAR16));
 //   if(!IsDir(File))
 //      return File;
 //  return ;
 // }

CHAR16* teste(EFI_FILE_PROTOCOL *File, EFI_FILE_PROTOCOL *RootDir){
  CHAR16 *var = AllocateZeroPool(sizeof(CHAR16)*512);
  EFI_STATUS Status= EFI_SUCCESS;
  UINTN BufferSize = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);
  EFI_FILE_INFO *Buffer = AllocatePool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));
  EFI_FILE_INFO *BufferAux = AllocatePool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));
  EFI_FILE_PROTOCOL *FileAux = NULL;
  File->GetInfo(File, &gEfiFileInfoGuid, &BufferSize, Buffer);
  Buffer->FileName[BufferSize] = '/';
  Buffer->FileName[BufferSize+1] = '\0';
  if(Buffer->Attribute == EFI_FILE_ARCHIVE){
    var = StrCat(StrCat(var, Buffer->FileName),(CHAR16*)"/\0");
      Print(L"Var: %s\n", var);
    return var;
  }
  else{
    if(Buffer->Attribute == EFI_FILE_DIRECTORY){
      while(TRUE){
        BufferSize = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);
        //BufferAux[BufferSize]
        Status = File->Read(File, &BufferSize, BufferAux);
        if (EFI_ERROR(Status)) {
            Print(L"Could not read directory: %r\n", Status);
            File->Close(File);
            return (CHAR16*)Status;
          }
        Print(L"BufferAux: %s\n", BufferAux->FileName);
        while(!StrCmp(BufferAux->FileName, (CHAR16*)"..") && !StrCmp(BufferAux->FileName, (CHAR16*)".")){
            Status = RootDir->Open(
              RootDir,
              &FileAux,
              BufferAux->FileName,
              EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
            0);

          if (EFI_ERROR(Status)) {
              Print(L"Could not open directory: %r\n", Status);
              RootDir->Close(RootDir);
              return (CHAR16*)Status;
          }
          BufferAux->FileName[BufferSize] = '/';
          BufferAux->FileName[BufferSize+1] = '\0';
          var = StrCat(var, StrCat(Buffer->FileName, (CHAR16*)"/\0"));
          return StrCat(var, teste(FileAux, File));
        }
        if(EFI_ERROR(Status)|| BufferSize==0){
          File->Close(File);
          break;
        }
      }


      //var = StrCat(StrCat(var, Buffer->FileName),(CHAR16*)"/\0");
      //return var;
    }
    // if(Buffer->Attribute == EFI_FILE_DIRECTORY){
    //   BufferSize = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);
    //   Status = File->Read(File, &BufferSize, BufferAux);
    //   if (EFI_ERROR(Status)) {
    //       Print(L"Could not read file: %r\n", Status);
    //   }
    //   RootDir->Open(
    //     RootDir,
    //     &FileAux,
    //     (VOID*)BufferAux,
    //     EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
    //     0);
    //     if (EFI_ERROR(Status)) {
    //         Print(L"Could not open directory: %r\n", Status);
    //         RootDir->Close(RootDir);
    //         return (CHAR16*)Status;
    //     }
    //
    //   var = StrCat(var,(CHAR16*)BufferAux->Attribute);
    //
    //   return StrCat(var,teste(FileAux, RootDir));
    // }
    return var;
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
  EFI_FILE_PROTOCOL *File = NULL;
//  EFI_FILE_PROTOCOL *Aux = NULL;
  //EFI_FILE_INFO *Buffer = AllocatePool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));
  //EFI_FILE_INFO *Buffer2 = AllocatePool(SIZE_OF_EFI_FILE_INFO+512*sizeof(CHAR16));

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
CHAR16* b = AllocateZeroPool(sizeof(CHAR16)*512);
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

    b = teste(File, RootDir);
    Print(L"%s\n", b);
    IndexHandle++;
    // CHAR8 * Dir1 = (CHAR8 *)".\0";
    // CHAR8 * Dir2 = (CHAR8 *)"..\0";
    // CHAR16 Dir1_16;
    // CHAR16 Dir2_16;
    // AsciiStrToUnicodeStr(Dir1, &Dir1_16);
    // AsciiStrToUnicodeStr(Dir2, &Dir2_16);
    // while(TRUE){
    //   BufferSize = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);
    //   Status = File->Read(File, &BufferSize, (VOID*) Buffer);
    //     if(EFI_ERROR(Status)|| BufferSize==0){
    //       goto close_file;
    //       break;
    //     }
    //     if(Buffer->Attribute == EFI_FILE_DIRECTORY){
    //       Status = RootDir->Open(
    //               RootDir,
    //               &Aux,
    //               Buffer->FileName,
    //               EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
    //               0);
    //       if (EFI_ERROR(Status)) {
    //           Print(L"Could not open directory: %r\n", Status);
    //           goto close_root;
    //       }
    //       while(TRUE){
    //         BufferSize = SIZE_OF_EFI_FILE_INFO + 512*sizeof(CHAR16);
    //         if(!(StrCmp(Buffer->FileName, &Dir1_16) && StrCmp(Buffer->FileName, &Dir2_16))){
    //           Status = Aux->Read(Aux, &BufferSize, (VOID*) Buffer2);
    //           if(EFI_ERROR(Status)|| BufferSize==0){
    //             goto close_file;
    //             break;
    //           }
    //             Print(L"%s/%s\n",Buffer->FileName, Buffer2->FileName);
    //         }
    //
    //       }
    //     }
    //       Print(L"%s\n",Buffer->FileName);
    // }
  }

  // close_file:
  //   File->Close(File);

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
