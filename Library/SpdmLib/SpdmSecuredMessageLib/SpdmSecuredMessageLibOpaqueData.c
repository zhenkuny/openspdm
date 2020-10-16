/** @file
  SPDM transport library.
  It follows the SPDM Specification.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/SpdmSecuredMessageLib.h>
#include <IndustryStandard/SpdmSecuredMessage.h>

/**
  Return the size in bytes of opaque data version selection.

  This function should be called in KEY_EXCHANGE/PSK_EXCHANGE response generation.

  @param  SpdmContext                  A pointer to the SPDM context.

  @return the size in bytes of opaque data version selection.
**/
UINTN
EFIAPI
SpdmGetOpaqueDataVersionSelectionDataSize (
  IN     VOID                 *SpdmContext
  )
{
  UINTN  Size;
  Size = sizeof(SECURED_MESSAGE_GENERAL_OPAQUE_DATA_TABLE_HEADER) +
         sizeof(SECURED_MESSAGE_OPAQUE_ELEMENT_TABLE_HEADER) +
         sizeof(SECURED_MESSAGE_OPAQUE_ELEMENT_VERSION_SELECTION);
  return (Size + 3) & ~3;
}

/**
  Return the size in bytes of opaque data supproted version.

  This function should be called in KEY_EXCHANGE/PSK_EXCHANGE request generation.

  @param  SpdmContext                  A pointer to the SPDM context.

  @return the size in bytes of opaque data supproted version.
**/
UINTN
EFIAPI
SpdmGetOpaqueDataSupportedVersionDataSize (
  IN     VOID                 *SpdmContext
  )
{
  UINTN  Size;
  Size = sizeof(SECURED_MESSAGE_GENERAL_OPAQUE_DATA_TABLE_HEADER) +
         sizeof(SECURED_MESSAGE_OPAQUE_ELEMENT_TABLE_HEADER) +
         sizeof(SECURED_MESSAGE_OPAQUE_ELEMENT_SUPPORTED_VERSION) +
         sizeof(SPDM_VERSION_NUMBER);
  return (Size + 3) & ~3;
}

/**
  Build opaque data supported version.

  This function should be called in KEY_EXCHANGE/PSK_EXCHANGE request generation.

  @param  SpdmContext                  A pointer to the SPDM context.
  @param  DataOutSize                  Size in bytes of the DataOut.
                                       On input, it means the size in bytes of DataOut buffer.
                                       On output, it means the size in bytes of copied DataOut buffer if RETURN_SUCCESS is returned,
                                       and means the size in bytes of desired DataOut buffer if RETURN_BUFFER_TOO_SMALL is returned.
  @param  DataOut                      A pointer to the desination buffer to store the opaque data supported version.

  @retval RETURN_SUCCESS               The opaque data supported version is built successfully.
  @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
**/
RETURN_STATUS
EFIAPI
SpdmBuildOpaqueDataSupportedVersionData (
  IN     VOID                 *SpdmContext,
  IN OUT UINTN                *DataOutSize,
     OUT VOID                 *DataOut
  )
{
  UINTN                                              FinalDataSize;
  SECURED_MESSAGE_GENERAL_OPAQUE_DATA_TABLE_HEADER   *GeneralOpaqueDataTableHeader;
  SECURED_MESSAGE_OPAQUE_ELEMENT_TABLE_HEADER        *OpaqueElementTableHeader;
  SECURED_MESSAGE_OPAQUE_ELEMENT_SUPPORTED_VERSION   *OpaqueElementSupportVersion;
  SPDM_VERSION_NUMBER                                *VersionsList;

  FinalDataSize = SpdmGetOpaqueDataSupportedVersionDataSize(SpdmContext);
  if (*DataOutSize < FinalDataSize) {
    *DataOutSize = FinalDataSize;
    return RETURN_BUFFER_TOO_SMALL;
  }

  GeneralOpaqueDataTableHeader = DataOut;
  GeneralOpaqueDataTableHeader->SpecId = SECURED_MESSAGE_OPAQUE_DATA_SPEC_ID;
  GeneralOpaqueDataTableHeader->OpaqueVersion = SECURED_MESSAGE_OPAQUE_VERSION;
  GeneralOpaqueDataTableHeader->TotalElements = 1;
  GeneralOpaqueDataTableHeader->Reserved = 0;

  OpaqueElementTableHeader = (VOID *)(GeneralOpaqueDataTableHeader + 1);
  OpaqueElementTableHeader->Id = SPDM_EXTENDED_ALGORITHM_REGISTRY_ID_DMTF;
  OpaqueElementTableHeader->VendorLen = 0;
  OpaqueElementTableHeader->OpaqueElementDataLen = sizeof(SECURED_MESSAGE_OPAQUE_ELEMENT_SUPPORTED_VERSION) + sizeof(SPDM_VERSION_NUMBER);

  OpaqueElementSupportVersion = (VOID *)(OpaqueElementTableHeader + 1);
  OpaqueElementSupportVersion->SMDataVersion = SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_DATA_VERSION;
  OpaqueElementSupportVersion->SMDataID = SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_ID_SUPPORTED_VERSION;
  OpaqueElementSupportVersion->VersionCount = 1;

  VersionsList = (VOID *)(OpaqueElementSupportVersion + 1);
  VersionsList->Alpha = 0;
  VersionsList->UpdateVersionNumber = 0;
  VersionsList->MinorVersion = 1;
  VersionsList->MajorVersion = 1;

  return RETURN_SUCCESS;
}

/**
  Process opaque data supported version.

  This function should be called in KEY_EXCHANGE/PSK_EXCHANGE request parsing in responder.

  @param  SpdmContext                  A pointer to the SPDM context.
  @param  DataInSize                   Size in bytes of the DataIn.
  @param  DataIn                       A pointer to the buffer to store the opaque data supported version.

  @retval RETURN_SUCCESS               The opaque data supported version is processed successfully.
  @retval RETURN_UNSUPPORTED           The DataIn is NOT opaque data supported version.
**/
RETURN_STATUS
EFIAPI
SpdmProcessOpaqueDataSupportedVersionData (
  IN     VOID                 *SpdmContext,
  IN     UINTN                DataInSize,
  IN     VOID                 *DataIn
  )
{
  SECURED_MESSAGE_GENERAL_OPAQUE_DATA_TABLE_HEADER   *GeneralOpaqueDataTableHeader;
  SECURED_MESSAGE_OPAQUE_ELEMENT_TABLE_HEADER        *OpaqueElementTableHeader;
  SECURED_MESSAGE_OPAQUE_ELEMENT_SUPPORTED_VERSION   *OpaqueElementSupportVersion;
  SPDM_VERSION_NUMBER                                *VersionsList;

  if (DataInSize != SpdmGetOpaqueDataSupportedVersionDataSize(SpdmContext)) {
    return RETURN_UNSUPPORTED;
  }
  GeneralOpaqueDataTableHeader = DataIn;
  if ((GeneralOpaqueDataTableHeader->SpecId != SECURED_MESSAGE_OPAQUE_DATA_SPEC_ID) ||
      (GeneralOpaqueDataTableHeader->OpaqueVersion != SECURED_MESSAGE_OPAQUE_VERSION) ||
      (GeneralOpaqueDataTableHeader->TotalElements != 1) ||
      (GeneralOpaqueDataTableHeader->Reserved != 0) ) {
    return RETURN_UNSUPPORTED;
  }
  OpaqueElementTableHeader = (VOID *)(GeneralOpaqueDataTableHeader + 1);
  if ((OpaqueElementTableHeader->Id != SPDM_EXTENDED_ALGORITHM_REGISTRY_ID_DMTF) ||
      (OpaqueElementTableHeader->VendorLen != 0) ||
      (OpaqueElementTableHeader->OpaqueElementDataLen != sizeof(SECURED_MESSAGE_OPAQUE_ELEMENT_SUPPORTED_VERSION) + sizeof(SPDM_VERSION_NUMBER)) ) {
    return RETURN_UNSUPPORTED;
  }
  OpaqueElementSupportVersion = (VOID *)(OpaqueElementTableHeader + 1);
  if ((OpaqueElementSupportVersion->SMDataVersion != SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_DATA_VERSION) ||
      (OpaqueElementSupportVersion->SMDataID != SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_ID_SUPPORTED_VERSION) ||
      (OpaqueElementSupportVersion->VersionCount != 1) ) {
    return RETURN_UNSUPPORTED;
  }
  VersionsList = (VOID *)(OpaqueElementSupportVersion + 1);
  if ((VersionsList->MinorVersion != 1) ||
      (VersionsList->MajorVersion != 1) ) {
    return RETURN_UNSUPPORTED;
  }

  return RETURN_SUCCESS;
}

/**
  Build opaque data version selection.

  This function should be called in KEY_EXCHANGE/PSK_EXCHANGE response generation.

  @param  SpdmContext                  A pointer to the SPDM context.
  @param  DataOutSize                  Size in bytes of the DataOut.
                                       On input, it means the size in bytes of DataOut buffer.
                                       On output, it means the size in bytes of copied DataOut buffer if RETURN_SUCCESS is returned,
                                       and means the size in bytes of desired DataOut buffer if RETURN_BUFFER_TOO_SMALL is returned.
  @param  DataOut                      A pointer to the desination buffer to store the opaque data version selection.

  @retval RETURN_SUCCESS               The opaque data version selection is built successfully.
  @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
**/
RETURN_STATUS
EFIAPI
SpdmBuildOpaqueDataVersionSelectionData (
  IN     VOID                 *SpdmContext,
  IN OUT UINTN                *DataOutSize,
     OUT VOID                 *DataOut
  )
{
  UINTN                                              FinalDataSize;
  SECURED_MESSAGE_GENERAL_OPAQUE_DATA_TABLE_HEADER   *GeneralOpaqueDataTableHeader;
  SECURED_MESSAGE_OPAQUE_ELEMENT_TABLE_HEADER        *OpaqueElementTableHeader;
  SECURED_MESSAGE_OPAQUE_ELEMENT_VERSION_SELECTION   *OpaqueElementVersionSection;

  FinalDataSize = SpdmGetOpaqueDataVersionSelectionDataSize(SpdmContext);
  if (*DataOutSize < FinalDataSize) {
    *DataOutSize = FinalDataSize;
    return RETURN_BUFFER_TOO_SMALL;
  }

  GeneralOpaqueDataTableHeader = DataOut;
  GeneralOpaqueDataTableHeader->SpecId = SECURED_MESSAGE_OPAQUE_DATA_SPEC_ID;
  GeneralOpaqueDataTableHeader->OpaqueVersion = SECURED_MESSAGE_OPAQUE_VERSION;
  GeneralOpaqueDataTableHeader->TotalElements = 1;
  GeneralOpaqueDataTableHeader->Reserved = 0;

  OpaqueElementTableHeader = (VOID *)(GeneralOpaqueDataTableHeader + 1);
  OpaqueElementTableHeader->Id = SPDM_EXTENDED_ALGORITHM_REGISTRY_ID_DMTF;
  OpaqueElementTableHeader->VendorLen = 0;
  OpaqueElementTableHeader->OpaqueElementDataLen = sizeof(SECURED_MESSAGE_OPAQUE_ELEMENT_VERSION_SELECTION);

  OpaqueElementVersionSection = (VOID *)(OpaqueElementTableHeader + 1);
  OpaqueElementVersionSection->SMDataVersion = SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_DATA_VERSION;
  OpaqueElementVersionSection->SMDataID = SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_ID_VERSION_SELECTION;
  OpaqueElementVersionSection->SelectedVersion.Alpha = 0;
  OpaqueElementVersionSection->SelectedVersion.UpdateVersionNumber = 0;
  OpaqueElementVersionSection->SelectedVersion.MinorVersion = 1;
  OpaqueElementVersionSection->SelectedVersion.MajorVersion = 1;

  return RETURN_SUCCESS;
}

/**
  Process opaque data version selection.

  This function should be called in KEY_EXCHANGE/PSK_EXCHANGE response parsing in requester.

  @param  SpdmContext                  A pointer to the SPDM context.
  @param  DataInSize                   Size in bytes of the DataIn.
  @param  DataIn                       A pointer to the buffer to store the opaque data version selection.

  @retval RETURN_SUCCESS               The opaque data version selection is processed successfully.
  @retval RETURN_UNSUPPORTED           The DataIn is NOT opaque data version selection.
**/
RETURN_STATUS
EFIAPI
SpdmProcessOpaqueDataVersionSelectionData (
  IN     VOID                 *SpdmContext,
  IN     UINTN                DataInSize,
  IN     VOID                 *DataIn
  )
{
  SECURED_MESSAGE_GENERAL_OPAQUE_DATA_TABLE_HEADER   *GeneralOpaqueDataTableHeader;
  SECURED_MESSAGE_OPAQUE_ELEMENT_TABLE_HEADER        *OpaqueElementTableHeader;
  SECURED_MESSAGE_OPAQUE_ELEMENT_VERSION_SELECTION   *OpaqueElementVersionSection;

  if (DataInSize != SpdmGetOpaqueDataVersionSelectionDataSize(SpdmContext)) {
    return RETURN_UNSUPPORTED;
  }
  GeneralOpaqueDataTableHeader = DataIn;
  if ((GeneralOpaqueDataTableHeader->SpecId != SECURED_MESSAGE_OPAQUE_DATA_SPEC_ID) ||
      (GeneralOpaqueDataTableHeader->OpaqueVersion != SECURED_MESSAGE_OPAQUE_VERSION) ||
      (GeneralOpaqueDataTableHeader->TotalElements != 1) ||
      (GeneralOpaqueDataTableHeader->Reserved != 0) ) {
    return RETURN_UNSUPPORTED;
  }
  OpaqueElementTableHeader = (VOID *)(GeneralOpaqueDataTableHeader + 1);
  if ((OpaqueElementTableHeader->Id != SPDM_EXTENDED_ALGORITHM_REGISTRY_ID_DMTF) ||
      (OpaqueElementTableHeader->VendorLen != 0) ||
      (OpaqueElementTableHeader->OpaqueElementDataLen != sizeof(SECURED_MESSAGE_OPAQUE_ELEMENT_VERSION_SELECTION)) ) {
    return RETURN_UNSUPPORTED;
  }
  OpaqueElementVersionSection = (VOID *)(OpaqueElementTableHeader + 1);
  if ((OpaqueElementVersionSection->SMDataVersion != SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_DATA_VERSION) ||
      (OpaqueElementVersionSection->SMDataID != SECURED_MESSAGE_OPAQUE_ELEMENT_SMDATA_ID_VERSION_SELECTION) ||
      (OpaqueElementVersionSection->SelectedVersion.MinorVersion != 1) ||
      (OpaqueElementVersionSection->SelectedVersion.MajorVersion != 1) ) {
    return RETURN_UNSUPPORTED;
  }

  return RETURN_SUCCESS;
}