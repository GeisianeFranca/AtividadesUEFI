#include "TestHiiData.h"
#include "TestHii.h"

EFI_GUID mFormSetGuid = TEST_HII_FORMSET_GUID;
CHAR16 mIfrVariableName[] = L"Teste";

EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_FORM_BROWSER2_PROTOCOL      *FormBrowser2;
  EFI_HII_DATABASE_PROTOCOL       *HiiDatabase;
  //EFI_HANDLE                      DriverHandle[2] = {NULL, NULL};
  EFI_HII_HANDLE                      HiiHandle;
  DATA_CONFIGURATION              *Configuration;
  UINT                            BufferSize;

  BufferSize = sizeof (DATA_CONFIGURATION);


  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &HiiDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &FormBrowser2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HiiHandle = HiiAddPackages ( //Registra uma lista de pacotes na HII Database e retorna o HII Handle associado
          &mFormSetGuid,
          NULL,
          TestHiiVfrBin,
          //TestHiStrings,
          NULL);
  Status = gRT->GetVariable (
            mIfrVariableName,
            &mFormSetGuid,
            NULL,
            &BufferSize,
            Configuration
            );

  Status = gRT->SetVariable(
                mIfrVariableName,
                &mFormSetGuid,
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                sizeof (DATA_CONFIGURATION),
                Configuration
                );
  ActionFlag = HiiSetToDefaults (ConfigRequestHdr, EFI_HII_DEFAULT_CLASS_STANDARD);
  ASSERT (ActionFlag);
  Print(L"SetVariable: %s", mIfrVariableName);

  Status = FormBrowser2->SendForm (
		   FormBrowser2,
		   &HiiHandle,
		   1,
		   &mFormSetGuid,
		   1,
		   NULL,
		   NULL);
  if (EFI_ERROR(Status)) {
	  return Status;
  }

  return Status;

}
