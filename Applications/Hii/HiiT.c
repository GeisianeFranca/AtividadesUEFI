
#include "Hii.h"
#include "HiiData.h"
EFI_GUID   mFormSetGuid = HII_TEST_FORM_GUID;



EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_FORM_BROWSER2_PROTOCOL  *Browser2Protocol;
  EFI_HII_STRING_PROTOCOL     *HiiStringProtocol;
  EFI_HII_DATABASE_PROTOCOL   *DatabaseProtocol;
  EFI_HII_HANDLE              HiiHandle;
  EFI_SCREEN_DESCRIPTOR           Screen;
  EFI_STATUS Status = EFI_SUCCESS;

  ZeroMem (&Screen, sizeof (EFI_SCREEN_DESCRIPTOR));
  gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &Screen.RightColumn, &Screen.BottomRow);
  Screen.TopRow  = 3;
  Screen.BottomRow  = Screen.BottomRow - 3;

  Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &DatabaseProtocol);
  if (EFI_ERROR (Status)) {
    Print(L"Error: %r\n", Status);
    return Status;
  }
  Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &HiiStringProtocol);
  if (EFI_ERROR (Status)) {
    Print(L"Error: %r\n", Status);
    return Status;
  }
  Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &Browser2Protocol);
  if (EFI_ERROR (Status)) {
    Print(L"Error: %r\n", Status);
    return Status;
  }

  HiiHandle = HiiAddPackages (
                 &mFormSetGuid,
                 NULL,
                 HiiTestStrings,
                 HiiTestVfrBin,
                 NULL);
  //
  Status = Browser2Protocol->SendForm (
                         Browser2Protocol,
                         &HiiHandle,
                         1,
                         &mFormSetGuid, /* Form GUID */
                         1, 			/* Form ID */
                         &Screen,
                         0
                         );
  return Status;
}
