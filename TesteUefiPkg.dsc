[Defines]
  PLATFORM_NAME = TesteUefiPkg
  PLATFORM_GUID = 84d120d9-d01a-4fa0-beec-144423230680
  PLATFORM_VERSION = 0.1
  DSC_SPECIFICATION = 0x00010005
  OUTPUT_DIRECTORY = Build/TesteUefiPkg
  SUPPORTED_ARCHITECTURES = IA32|IPF|X64|EBC
  BUILD_TARGETS = DEBUG|RELEASE
  SKUID_IDENTIFIER = DEFAULT

[LibraryClasses]
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  DebugLib|MdePkg/Library/UefiDebugLibStdErr/UefiDebugLibStdErr.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf

[Components]
  TesteUefiPkg/Applications/MyApplication.inf
  TesteUefiPkg/WriteTest/WriteTest.inf
  TesteUefiPkg/DeleteDir/DeleteDir.inf
  TesteUefiPkg/VerificaArquivo/VerificaArquivo.inf
  TesteUefiPkg/UltModificado/UltModificado.inf
  TesteUefiPkg/LsRecursivo/LsRecursivo.inf
  TesteUefiPkg/FindRecursivo/FindRecursivo.inf
  TesteUefiPkg/LerImagem/LerImagem.inf
  TesteUefiPkg/Teste/Teste.inf
  TesteUefiPkg/Hash/Hash.inf
