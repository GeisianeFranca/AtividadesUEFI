#ifndef _TESTE_HII_DATA_
#define _TESTE_HII_DATA_

#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/HiiFormMapMethodGuid.h>
#include <Guid/DriverSampleHii.h>
#include <Guid/ZeroGuid.h>

#define TEST_HII_FORMSET_GUID {0xddb9df6c, 0x2040, 0x4fb6, {0xad, 0xe0, 0x60, 0xdf, 0x3a, 0x97, 0x6a, 0xbd}}

#define TEST_HII_VAR_GUID {0xc8617911, 0xce2f 0x42b5, {0x94, 0x85, 0x27, 0xb2, 0xb1, 0x9d, 0x4c, 0x49} }

#pragma pack(1)
typedef struct{
  UINT8 Checkbox;
}DATA_CONFIGURATION;
#pragma pack()

#endif
