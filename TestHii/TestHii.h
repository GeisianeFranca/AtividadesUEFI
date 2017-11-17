#ifndef _TEST_HII_
#define _TEST_HII_
#include <Uefi.h>

#include <Protocol/HiiConfigRouting.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>

#include <Guid/MdeModuleHii.h>

#include <Library/HiiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/HiiDatabase.h>

extern UINT8 TestHiiVfrBin[];

extern UINT8 TestHiiStrings[];

EFI_HII_HANDLE gStringPackHandle;


#endif
