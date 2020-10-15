/** @file
  SPDM common library.
  It follows the SPDM Specification.

Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SPDM_RESPONDER_LIB_H__
#define __SPDM_RESPONDER_LIB_H__

#include <Library/SpdmCommonLib.h>

/**
  Process the SPDM or APP request and return the response.

  The APP message is encoded to a secured message directly in SPDM session.
  The APP message format is defined by the transport layer.
  Take MCTP as example: APP message == MCTP header (MCTP_MESSAGE_TYPE_SPDM) + SPDM message

  @param  SpdmContext                  A pointer to the SPDM context.
  @param  SessionId                    Indicates if it is a secured message protected via SPDM session.
                                       If SessionId is NULL, it is a normal message.
                                       If SessionId is NOT NULL, it is a secured message.
  @param  IsAppMessage                 Indicates if it is an APP message or SPDM message.
  @param  RequestSize                  Size in bytes of the request data.
  @param  Request                      A pointer to the request data.
  @param  ResponseSize                 Size in bytes of the response data.
                                       On input, it means the size in bytes of response data buffer.
                                       On output, it means the size in bytes of copied response data buffer if RETURN_SUCCESS is returned,
                                       and means the size in bytes of desired response data buffer if RETURN_BUFFER_TOO_SMALL is returned.
  @param  Response                     A pointer to the response data.

  @retval RETURN_SUCCESS               The request is processed and the response is returned.
  @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
  @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
  @retval RETURN_SECURITY_VIOLATION    Any verification fails.
**/
typedef
RETURN_STATUS
(EFIAPI *SPDM_GET_RESPONSE_FUNC) (
  IN     VOID                 *SpdmContext,
  IN     UINT32               *SessionId,
  IN     BOOLEAN              IsAppMessage,
  IN     UINTN                RequestSize,
  IN     VOID                 *Request,
  IN OUT UINTN                *ResponseSize,
     OUT VOID                 *Response
  );

/**
  Register an SPDM or APP message process function.

  If the default message process function cannot handle the message,
  this function will be invoked.

  @param  SpdmContext                  A pointer to the SPDM context.
  @param  GetResponseFunc              The function to process the encapsuled message.
**/
VOID
EFIAPI
SpdmRegisterGetResponseFunc (
  IN  VOID                    *SpdmContext,
  IN  SPDM_GET_RESPONSE_FUNC  GetResponseFunc
  );

/**
  This is the main dispatch function in SPDM responder.

  It receives one request message, processes it and sends the response message.

  It should be called in a while loop or an timer/interrupt handler.

  @param  SpdmContext                  A pointer to the SPDM context.

  @retval RETURN_SUCCESS               One SPDM request message is processed.
  @retval RETURN_DEVICE_ERROR          A device error occurs when communicates with the device.
  @retval RETURN_UNSUPPORTED           One request message is not supported.
**/
RETURN_STATUS
EFIAPI
SpdmResponderDispatchMessage (
  IN     VOID                 *SpdmContext
  );

/**
  Generate ERROR message.

  This function can be called in SPDM_GET_RESPONSE_FUNC.

  @param  SpdmContext                  A pointer to the SPDM context.
  @param  ErrorCode                    The error code of the message.
  @param  ErrorData                    The error data of the message.
  @param  SpdmResponseSize             Size in bytes of the response data.
                                       On input, it means the size in bytes of response data buffer.
                                       On output, it means the size in bytes of copied response data buffer if RETURN_SUCCESS is returned,
                                       and means the size in bytes of desired response data buffer if RETURN_BUFFER_TOO_SMALL is returned.
  @param  SpdmResponse                 A pointer to the response data.

  @retval RETURN_SUCCESS               The error message is generated.
  @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
**/
RETURN_STATUS
EFIAPI
SpdmGenerateErrorResponse (
  IN     VOID                 *SpdmContext,
  IN     UINT8                ErrorCode,
  IN     UINT8                ErrorData,
  IN OUT UINTN                *SpdmResponseSize,
     OUT VOID                 *SpdmResponse
  );

/**
  Generate ERROR message with extended error data.

  This function can be called in SPDM_GET_RESPONSE_FUNC.

  @param  SpdmContext                  A pointer to the SPDM context.
  @param  ErrorCode                    The error code of the message.
  @param  ErrorData                    The error data of the message.
  @param  ExtendedErrorDataSize        The size in bytes of the extended error data.
  @param  ExtendedErrorData            A pointer to the extended error data.
  @param  SpdmResponseSize             Size in bytes of the response data.
                                       On input, it means the size in bytes of response data buffer.
                                       On output, it means the size in bytes of copied response data buffer if RETURN_SUCCESS is returned,
                                       and means the size in bytes of desired response data buffer if RETURN_BUFFER_TOO_SMALL is returned.
  @param  SpdmResponse                 A pointer to the response data.

  @retval RETURN_SUCCESS               The error message is generated.
  @retval RETURN_BUFFER_TOO_SMALL      The buffer is too small to hold the data.
**/
RETURN_STATUS
EFIAPI
SpdmGenerateExtendedErrorResponse (
  IN     VOID                 *Context,
  IN     UINT8                ErrorCode,
  IN     UINT8                ErrorData,
  IN     UINTN                ExtendedErrorDataSize,
  IN     UINT8                *ExtendedErrorData,
  IN OUT UINTN                *SpdmResponseSize,
     OUT VOID                 *SpdmResponse
  );

#endif