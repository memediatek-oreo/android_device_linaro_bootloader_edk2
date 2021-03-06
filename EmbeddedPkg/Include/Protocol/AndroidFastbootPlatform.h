/** @file

  Copyright (c) 2014, ARM Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __ANDROID_FASTBOOT_PLATFORM_H__
#define __ANDROID_FASTBOOT_PLATFORM_H__

extern EFI_GUID gAndroidFastbootPlatformProtocolGuid;

/*
  Protocol for platform-specific operations initiated by Android Fastboot.

  Based on Fastboot Protocol version 0.4. See
  system/core/fastboot/fastboot_protocol.txt in the AOSP source tree for more
  info.

  Doesn't support image verification.
*/

/*
  Do any initialisation that needs to be done in order to be able to respond to
  commands.

  @retval EFI_SUCCESS   Initialised successfully.
  @retval !EFI_SUCCESS  Error in initialisation.
*/
typedef
EFI_STATUS
(*FASTBOOT_PLATFORM_INIT) (
  VOID
  );

/*
  To be called when Fastboot is finished and we aren't rebooting or booting an
  image. Undo initialisation, free resrouces.
*/
typedef
VOID
(*FASTBOOT_PLATFORM_UN_INIT) (
  VOID
  );

/*
  Flash the partition named (according to a platform-specific scheme)
  PartitionName, with the image pointed to by Buffer, whose size is BufferSize.

  @param[in] PartitionName  Null-terminated name of partition to write.
  @param[in] BufferSize     Size of Buffer in byets.
  @param[in] Buffer         Data to write to partition.

  @retval EFI_NOT_FOUND     No such partition.
  @retval EFI_DEVICE_ERROR  Flashing failed.
*/
typedef
EFI_STATUS
(*FASTBOOT_PLATFORM_FLASH) (
  IN CHAR8   *PartitionName,
  IN UINTN    BufferSize,
  IN VOID    *Buffer
  );

/*
  Erase the partition named PartitionName.

  @param[in] PartitionName  Null-terminated name of partition to erase.

  @retval EFI_NOT_FOUND     No such partition.
  @retval EFI_DEVICE_ERROR  Erasing failed.
*/
typedef
EFI_STATUS
(*FASTBOOT_PLATFORM_ERASE) (
  IN CHAR8   *PartitionName
  );

/*
  If the variable referred to by Name exists, copy it (as a null-terminated
  string) into Value. If it doesn't exist, put the Empty string in Value.

  Variable names and values may not be larger than 60 bytes, excluding the
  terminal null character. This is a limitation of the Fastboot protocol.

  The Fastboot application will handle platform-nonspecific variables
  (Currently "version" is the only one of these.)

  @param[in]  Name   Null-terminated name of Fastboot variable to retrieve.
  @param[out] Value  Caller-allocated buffer for null-terminated value of
                     variable.

  @retval EFI_SUCCESS       The variable was retrieved, or it doesn't exist.
  @retval EFI_DEVICE_ERROR  There was an error looking up the variable. This
                            does _not_ include the variable not existing.
*/
typedef
EFI_STATUS
(*FASTBOOT_PLATFORM_GETVAR) (
  IN  CHAR8   *Name,
  OUT CHAR8   *Value
  );

/*
  React to an OEM-specific command.

  Future versions of this function might want to allow the platform to do some
  extra communication with the host. A way to do this would be to add a function
  to the FASTBOOT_TRANSPORT_PROTOCOL that allows the implementation of
  DoOemCommand to replace the ReceiveEvent with its own, and to restore the old
  one when it's finished.

  However at the moment although the specification allows it, the AOSP fastboot
  host application doesn't handle receiving any data from the client, and it
  doesn't support a data phase for OEM commands.

  @param[in] Command    Null-terminated command string.

  @retval EFI_SUCCESS       The command executed successfully.
  @retval EFI_NOT_FOUND     The command wasn't recognised.
  @retval EFI_DEVICE_ERROR  There was an error executing the command.
*/
typedef
EFI_STATUS
(*FASTBOOT_PLATFORM_OEM_COMMAND) (
  IN  CHAR8   *Command
  );

typedef
EFI_STATUS
(*FASTBOOT_PLATFORM_READ) (
  IN CHAR8     *PartitionName,
  IN OUT UINTN *BufferSize,
  OUT VOID    **Buffer
  );

typedef struct _FASTBOOT_PLATFORM_PROTOCOL {
  FASTBOOT_PLATFORM_INIT          Init;
  FASTBOOT_PLATFORM_UN_INIT       UnInit;
  FASTBOOT_PLATFORM_FLASH         FlashPartition;
  FASTBOOT_PLATFORM_ERASE         ErasePartition;
  FASTBOOT_PLATFORM_GETVAR        GetVar;
  FASTBOOT_PLATFORM_OEM_COMMAND   DoOemCommand;
  FASTBOOT_PLATFORM_READ          ReadPartition;
} FASTBOOT_PLATFORM_PROTOCOL;

/* See sparse_format.h in AOSP  */
#define SPARSE_HEADER_MAGIC 0xed26ff3a
#define CHUNK_TYPE_RAW      0xCAC1
#define CHUNK_TYPE_FILL     0xCAC2
#define CHUNK_TYPE_DONT_CARE    0xCAC3
#define CHUNK_TYPE_CRC32    0xCAC4

typedef struct _SPARSE_HEADER {
  UINT32    Magic;
  UINT16    MajorVersion;
  UINT16    MinorVersion;
  UINT16    FileHeaderSize;
  UINT16    ChunkHeaderSize;
  UINT32    BlockSize;
  UINT32    TotalBlocks;
  UINT32    TotalChunks;
  UINT32    ImageChecksum;
} SPARSE_HEADER;

typedef struct _CHUNK_HEADER {
  UINT16    ChunkType;
  UINT16    Reserved1;
  UINT32    ChunkSize;
  UINT32    TotalSize;
} CHUNK_HEADER;

#endif
