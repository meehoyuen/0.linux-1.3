/*

  Linux Driver for BusLogic MultiMaster SCSI Host Adapters

  Copyright 1995 by Leonard N. Zubkoff <lnz@dandelion.com>

  This program is free software; you may redistribute and/or modify it under
  the terms of the GNU General Public License Version 2 as published by the
  Free Software Foundation, provided that none of the source code or runtime
  copyright notices are removed or modified.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY, without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for complete details.

  The author respectfully requests that any modifications to this software be
  sent directly to him for evaluation and testing.

  Special thanks to Wayne Yen and Alex Win of BusLogic, whose advice has been
  invaluable, to David Gentzel, for writing the original Linux BusLogic driver,
  and to Paul Gortmaker, for being such a dedicated test site.

*/


/*
  Define types for some of the structures that interface with the rest
  of the Linux Kernel and SCSI Subsystem.
*/

typedef struct pt_regs Registers_T;
typedef Scsi_Host_Template SCSI_Host_Template_T;
typedef struct Scsi_Host SCSI_Host_T;
typedef struct scsi_device SCSI_Device_T;
typedef struct scsi_disk SCSI_Disk_T;
typedef struct scsi_cmnd SCSI_Command_T;
typedef struct scatterlist SCSI_ScatterList_T;
typedef kdev_t KernelDevice_T;


/*
  Define prototypes for the BusLogic Driver Interface Functions.
*/

const char *BusLogic_DriverInfo(SCSI_Host_T *);
int BusLogic_DetectHostAdapter(SCSI_Host_Template_T *);
int BusLogic_ReleaseHostAdapter(SCSI_Host_T *);
int BusLogic_QueueCommand(SCSI_Command_T *,
			  void (*CompletionRoutine)(SCSI_Command_T *));
int BusLogic_AbortCommand(SCSI_Command_T *);
int BusLogic_ResetCommand(SCSI_Command_T *, unsigned int);
int BusLogic_BIOSDiskParameters(SCSI_Disk_T *, KernelDevice_T, int *);


/*
  Define the BusLogic SCSI Host Template structure.
*/

#define BUSLOGIC							 \
  { NULL,				/* Next			     */  \
    NULL,				/* Usage Count Pointer	     */  \
    NULL,				/* /proc Directory Entry     */	 \
    NULL,				/* /proc Info Function	     */	 \
    "BusLogic",				/* Driver Name		     */  \
    BusLogic_DetectHostAdapter,		/* Detect Host Adapter	     */  \
    BusLogic_ReleaseHostAdapter,	/* Release Host Adapter	     */  \
    BusLogic_DriverInfo,		/* Driver Info Function	     */  \
    NULL,				/* Command Function	     */  \
    BusLogic_QueueCommand,		/* Queue Command Function    */  \
    BusLogic_AbortCommand,		/* Abort Command Function    */  \
    BusLogic_ResetCommand,		/* Reset Command Function    */  \
    NULL,				/* Slave Attach Function     */  \
    BusLogic_BIOSDiskParameters,	/* Disk BIOS Parameters	     */  \
    0,					/* Can Queue		     */  \
    0,					/* This ID		     */  \
    0,					/* Scatter/Gather Table Size */  \
    0,					/* SCSI Commands per LUN     */  \
    0,					/* Present		     */  \
    1,					/* Default Unchecked ISA DMA */  \
    ENABLE_CLUSTERING }			/* Enable Clustering	     */


/*
  BusLogic_DriverVersion protects the private portion of this file.
*/

#ifdef BusLogic_DriverVersion


/*
  Define the maximum number of BusLogic Host Adapters that are supported.
*/

#define BusLogic_MaxHostAdapters		10


/*
  Define the maximum number of I/O Addresses that may be probed.
*/

#define BusLogic_IO_MaxProbeAddresses		16


/*
  Define the maximum number of Target Devices supported by this driver.
*/

#define BusLogic_MaxTargetDevices		16


/*
  Define the maximum number of Scatter/Gather Segments used by this driver.
  For optimal performance, it is important that this limit be at least as
  large as the maximum single request generated by the I/O Subsystem.
*/

#define BusLogic_ScatterGatherLimit		128


/*
  Define the maximum and default Queue Depth to allow for Target Devices
  depending on whether or not they support Tagged Queuing and whether or not
  ISA Bounce Buffers are required.
*/

#define BusLogic_MaxTaggedQueueDepth		31
#define BusLogic_TaggedQueueDepth_BB		2
#define BusLogic_UntaggedQueueDepth		3


/*
  Define the default amount of time in seconds to wait between a Host Adapter
  Hard Reset which initiates a SCSI Bus Reset and issuing any SCSI commands.
  Some SCSI devices get confused if they receive SCSI commands too soon after
  a SCSI Bus Reset.
*/

#define BusLogic_DefaultBusSettleTime		2


/*
  Define the possible Local Options.
*/

#define BusLogic_InhibitTargetInquiry		1


/*
  Define the possible Global Options.
*/

#define BusLogic_TraceProbe			1
#define BusLogic_TraceHardReset			2
#define BusLogic_TraceConfiguration		4
#define BusLogic_TraceErrors			8
#define BusLogic_TraceQueueDepths		16


/*
  Define the possible Error Recovery Strategy Options.
*/

#define BusLogic_ErrorRecovery_Default		0
#define BusLogic_ErrorRecovery_HardReset	1
#define BusLogic_ErrorRecovery_BusDeviceReset	2
#define BusLogic_ErrorRecovery_None		3

static char
  *BusLogic_ErrorRecoveryStrategyNames[] =
    { "Default", "Hard Reset", "Bus Device Reset", "None" },
  *BusLogic_ErrorRecoveryStrategyLetters[] =
    { "D", "H", "B", "N" };


/*
  Define a boolean data type.
*/

#define false 0
#define true  1
typedef unsigned char boolean;


/*
  Define the BusLogic SCSI Host Adapter I/O Register Offsets.
*/

#define BusLogic_IO_PortCount			4	/* I/O Registers */
#define BusLogic_ControlRegister		0	/* WO register */
#define BusLogic_StatusRegister			0	/* RO register */
#define BusLogic_CommandParameterRegister	1	/* WO register */
#define BusLogic_DataInRegister			1	/* RO register */
#define BusLogic_InterruptRegister		2	/* RO register */
#define BusLogic_GeometryRegister		3	/* RO, undocumented */


/*
  Define the bits in the write-only Control Register.
*/

#define BusLogic_ReservedCR			0x0F
#define BusLogic_SCSIBusReset			0x10
#define BusLogic_InterruptReset			0x20
#define BusLogic_SoftReset			0x40
#define BusLogic_HardReset			0x80


/*
  Define the bits in the read-only Status Register.
*/

#define BusLogic_CommandInvalid			0x01
#define BusLogic_ReservedSR			0x02
#define BusLogic_DataInRegisterReady		0x04
#define BusLogic_CommandParameterRegisterBusy	0x08
#define BusLogic_HostAdapterReady		0x10
#define BusLogic_InitializationRequired		0x20
#define BusLogic_DiagnosticFailure		0x40
#define BusLogic_DiagnosticActive		0x80


/*
  Define the bits in the read-only Interrupt Register.
*/

#define BusLogic_IncomingMailboxLoaded		0x01
#define BusLogic_OutgoingMailboxAvailable	0x02
#define BusLogic_CommandComplete		0x04
#define BusLogic_SCSIResetState			0x08
#define BusLogic_ReservedIR			0x70
#define BusLogic_InterruptValid			0x80


/*
  Define the bits in the undocumented read-only Geometry Register.
*/

#define BusLogic_Drive0Geometry			0x03
#define BusLogic_Drive1Geometry			0x0C
#define BusLogic_ReservedGR			0x70
#define BusLogic_ExtendedTranslationEnabled	0x80


/*
  Define the BusLogic SCSI Host Adapter Command Register Operation Codes.
*/

typedef enum
{
  BusLogic_TestCommandCompleteInterrupt =	0x00,	/* documented */
  BusLogic_InitializeMailbox =			0x01,	/* documented */
  BusLogic_ExecuteMailboxCommand =		0x02,	/* documented */
  BusLogic_ExecuteBIOSCommand =			0x03,	/* documented */
  BusLogic_InquireBoardID =			0x04,	/* documented */
  BusLogic_EnableOutgoingMailboxAvailableInt =	0x05,	/* documented */
  BusLogic_SetSCSISelectionTimeout =		0x06,	/* documented */
  BusLogic_SetPreemptTimeOnBus =		0x07,	/* documented */
  BusLogic_SetTimeOffBus =			0x08,	/* ISA Bus only */
  BusLogic_SetBusTransferRate =			0x09,	/* ISA Bus only */
  BusLogic_InquireInstalledDevicesID0to7 =	0x0A,	/* documented */
  BusLogic_InquireConfiguration =		0x0B,	/* documented */
  BusLogic_SetTargetMode =			0x0C,	/* now undocumented */
  BusLogic_InquireSetupInformation =		0x0D,	/* documented */
  BusLogic_WriteAdapterLocalRAM =		0x1A,	/* documented */
  BusLogic_ReadAdapterLocalRAM =		0x1B,	/* documented */
  BusLogic_WriteBusMasterChipFIFO =		0x1C,	/* documented */
  BusLogic_ReadBusMasterChipFIFO =		0x1D,	/* documented */
  BusLogic_EchoCommandData =			0x1F,	/* documented */
  BusLogic_HostAdapterDiagnostic =		0x20,	/* documented */
  BusLogic_SetAdapterOptions =			0x21,	/* documented */
  BusLogic_InquireInstalledDevicesID8to15 =	0x23,	/* Wide only */
  BusLogic_InquireDevices =			0x24,	/* "W" and "C" only */
  BusLogic_InitializeExtendedMailbox =		0x81,	/* documented */
  BusLogic_InquireFirmwareVersion3rdDigit =	0x84,	/* undocumented */
  BusLogic_InquireFirmwareVersionLetter =	0x85,	/* undocumented */
  BusLogic_InquireGenericIOPortInformation =	0x86,	/* PCI only */
  BusLogic_InquireBoardModelNumber	 =	0x8B,	/* undocumented */
  BusLogic_InquireSynchronousPeriod =		0x8C,	/* undocumented */
  BusLogic_InquireExtendedSetupInformation =	0x8D,	/* documented */
  BusLogic_EnableStrictRoundRobinMode =		0x8F,	/* documented */
  BusLogic_FetchHostAdapterLocalRAM =		0x91,	/* undocumented */
  BusLogic_ModifyIOAddress =			0x95,	/* PCI only */
  BusLogic_EnableWideModeCCB =			0x96	/* Wide only */
}
BusLogic_OperationCode_T;


/*
  Define the Inquire Board ID reply structure.
*/

typedef struct BusLogic_BoardID
{
  unsigned char BoardType;				/* Byte 0 */
  unsigned char CustomFeatures;				/* Byte 1 */
  unsigned char FirmwareVersion1stDigit;		/* Byte 2 */
  unsigned char FirmwareVersion2ndDigit;		/* Byte 3 */
}
BusLogic_BoardID_T;


/*
  Define the Inquire Installed Devices ID 0 to 7 and Inquire Installed
  Devices ID 8 to 15 reply type.  For each Target Device, a byte is returned
  where bit 0 set indicates that Logical Unit 0 exists, bit 1 set indicates
  that Logical Unit 1 exists, and so on.
*/

typedef unsigned char BusLogic_InstalledDevices8_T[8];


/*
  Define the Inquire Devices reply type.  Inquire Devices only tests Logical
  Unit 0 of each Target Device unlike Inquire Installed Devices which tests
  Logical Units 0 - 7.  Two bytes are returned, where bit 0 set indicates
  that Target Device 0 exists, and so on.
*/

typedef unsigned short BusLogic_InstalledDevices_T;


/*
  Define the Inquire Configuration reply structure.
*/

typedef struct BusLogic_Configuration
{
  unsigned char :5;					/* Byte 0 Bits 0-4 */
  boolean DMA_Channel5:1;				/* Byte 0 Bit 5 */
  boolean DMA_Channel6:1;				/* Byte 0 Bit 6 */
  boolean DMA_Channel7:1;				/* Byte 0 Bit 7 */
  boolean IRQ_Channel9:1;				/* Byte 1 Bit 0 */
  boolean IRQ_Channel10:1;				/* Byte 1 Bit 1 */
  boolean IRQ_Channel11:1;				/* Byte 1 Bit 2 */
  boolean IRQ_Channel12:1;				/* Byte 1 Bit 3 */
  unsigned char :1;					/* Byte 1 Bit 4 */
  boolean IRQ_Channel14:1;				/* Byte 1 Bit 5 */
  boolean IRQ_Channel15:1;				/* Byte 1 Bit 6 */
  unsigned char :1;					/* Byte 1 Bit 7 */
  unsigned char HostAdapterID:4;			/* Byte 2 Bits 0-3 */
  unsigned char :4;					/* Byte 2 Bits 4-7 */
}
BusLogic_Configuration_T;


/*
  Define the Inquire Setup Information reply structure.
*/

typedef struct BusLogic_SynchronousValue
{
  unsigned char Offset:4;				/* Bits 0-3 */
  unsigned char TransferPeriod:3;			/* Bits 4-6 */
  boolean Synchronous:1;				/* Bit 7 */
}
BusLogic_SynchronousValue_T;

typedef BusLogic_SynchronousValue_T
  BusLogic_SynchronousValues8_T[8];

typedef BusLogic_SynchronousValue_T
  BusLogic_SynchronousValues_T[BusLogic_MaxTargetDevices];

typedef struct BusLogic_SetupInformation
{
  boolean SynchronousInitiationEnabled:1;		/* Byte 0 Bit 0 */
  boolean ParityCheckEnabled:1;				/* Byte 0 Bit 1 */
  unsigned char :6;					/* Byte 0 Bits 2-7 */
  unsigned char BusTransferRate;			/* Byte 1 */
  unsigned char PreemptTimeOnBus;			/* Byte 2 */
  unsigned char TimeOffBus;				/* Byte 3 */
  unsigned char MailboxCount;				/* Byte 4 */
  unsigned char MailboxAddress[3];			/* Bytes 5-7 */
  BusLogic_SynchronousValues8_T SynchronousValuesID0to7; /* Bytes 8-15 */
  unsigned char DisconnectPermittedID0to7;		/* Byte 16 */
  unsigned char Signature;				/* Byte 17 */
  unsigned char CharacterD;				/* Byte 18 */
  unsigned char BusLetter;				/* Byte 19 */
  unsigned char :8;					/* Byte 20 */
  unsigned char :8;					/* Byte 21 */
  BusLogic_SynchronousValues8_T SynchronousValuesID8to15; /* Bytes 22-29 */
  unsigned char DisconnectPermittedID8to15;		/* Byte 30 */
}
BusLogic_SetupInformation_T;


/*
  Define the Initialize Extended Mailbox request structure.
*/

typedef struct BusLogic_ExtendedMailboxRequest
{
  unsigned char MailboxCount;				/* Byte 0 */
  void *BaseMailboxAddress __attribute__ ((packed));	/* Bytes 1-4 */
}
BusLogic_ExtendedMailboxRequest_T;


/*
  Define the Inquire Firmware Version 3rd Digit reply type.
*/

typedef unsigned char BusLogic_FirmwareVersion3rdDigit_T;


/*
  Define the Inquire Firmware Version Letter reply type.
*/

typedef unsigned char BusLogic_FirmwareVersionLetter_T;


/*
  Define the Inquire Generic I/O Port Information reply type.
*/

typedef struct BusLogic_GenericIOPortInformation
{
  unsigned char ISACompatibleIOPort;			/* Byte 0 */
  unsigned char PCIAssignedIRQChannel;			/* Byte 1 */
  boolean LowByteTerminated:1;				/* Byte 2 Bit 0 */
  boolean HighByteTerminated:1;				/* Byte 2 Bit 1 */
  unsigned char :2;					/* Byte 2 Bits 2-3 */
  boolean JP1:1;					/* Byte 2 Bit 4 */
  boolean JP2:1;					/* Byte 2 Bit 5 */
  boolean JP3:1;					/* Byte 2 Bit 6 */
  boolean Valid:1;					/* Byte 2 Bit 7 */
  unsigned char :8;					/* Byte 3 */
}
BusLogic_GenericIOPortInformation_T;


/*
  Define the Inquire Board Model Number reply type.
*/

typedef unsigned char BusLogic_BoardModelNumber_T[5];


/*
  Define the Inquire Synchronous Period reply type.  For each Target Device,
  a byte is returned which represents the Synchronous Transfer Period in units
  of 10 nanoseconds.
*/

typedef unsigned char BusLogic_SynchronousPeriod_T[BusLogic_MaxTargetDevices];


/*
  Define the Inquire Extended Setup Information reply structure.
*/

typedef struct BusLogic_ExtendedSetupInformation
{
  unsigned char BusType;				/* Byte 0 */
  unsigned char BIOS_Address;				/* Byte 1 */
  unsigned short ScatterGatherLimit;			/* Bytes 2-3 */
  unsigned char MailboxCount;				/* Byte 4 */
  void *BaseMailboxAddress __attribute__ ((packed));	/* Bytes 5-8 */
  struct { unsigned char :6;				/* Byte 9 Bits 0-5 */
	   boolean LevelSensitiveInterrupts:1;		/* Byte 9 Bit 6 */
	   unsigned char :1; } Misc;			/* Byte 9 Bit 7 */
  unsigned char FirmwareRevision[3];			/* Bytes 10-12 */
  boolean HostWideSCSI:1;				/* Byte 13 Bit 0 */
  boolean HostDifferentialSCSI:1;			/* Byte 13 Bit 1 */
  boolean HostAutomaticConfiguration:1;			/* Byte 13 Bit 2 */
  boolean HostUltraSCSI:1;				/* Byte 13 Bit 3 */
  unsigned char :4;					/* Byte 13 Bits 4-7 */
}
BusLogic_ExtendedSetupInformation_T;


/*
  Define the Enable Strict Round Robin Mode request type.
*/

#define BusLogic_AggressiveRoundRobinMode	0x00
#define BusLogic_StrictRoundRobinMode		0x01

typedef unsigned char BusLogic_RoundRobinModeRequest_T;


/*
  Define the Fetch Host Adapter Local RAM request type.
*/

#define BusLogic_BIOS_BaseOffset		0
#define BusLogic_AutoSCSI_BaseOffset		64

typedef struct BusLogic_FetchHostAdapterLocalRAMRequest
{
  unsigned char ByteOffset;				/* Byte 0 */
  unsigned char ByteCount;				/* Byte 1 */
}
BusLogic_FetchHostAdapterLocalRAMRequest_T;


/*
  Define the Host Adapter Local RAM Auto SCSI Byte 15 reply structure.
*/

typedef struct BusLogic_AutoSCSIByte15
{
  unsigned char LowByteTerminated:1;			/* Bit 0 */
  unsigned char :1;					/* Bit 1 */
  unsigned char HighByteTerminated:1;			/* Bit 2 */
  unsigned char :5;					/* Bits 3-7 */
}
BusLogic_AutoSCSIByte15_T;


/*
  Define the Host Adapter Local RAM Auto SCSI Byte 45 reply structure.
*/

typedef struct BusLogic_AutoSCSIByte45
{
  unsigned char ForceBusDeviceScanningOrder:1;		/* Bit 0 */
  unsigned char :7;					/* Bits 1-7 */
}
BusLogic_AutoSCSIByte45_T;


/*
  Define the Modify I/O Address request type.  On PCI Host Adapters, the
  Modify I/O Address command allows modification of the ISA compatible I/O
  Address that the Host Adapter responds to; it does not affect the PCI
  compliant I/O Address assigned at system initialization.
*/

#define BusLogic_ModifyIO_330			0x00
#define BusLogic_ModifyIO_334			0x01
#define BusLogic_ModifyIO_230			0x02
#define BusLogic_ModifyIO_234			0x03
#define BusLogic_ModifyIO_130			0x04
#define BusLogic_ModifyIO_134			0x05
#define BusLogic_ModifyIO_Disable		0x06
#define BusLogic_ModifyIO_Disable2		0x07

typedef unsigned char BusLogic_ModifyIOAddressRequest_T;


/*
  Define the Enable Wide Mode SCSI CCB request type.  Wide Mode CCBs are
  necessary to support more than 8 Logical Units per Target Device.
*/

#define BusLogic_NormalModeCCB			0x00
#define BusLogic_WideModeCCB			0x01

typedef unsigned char BusLogic_WideModeCCBRequest_T;


/*
  Define the Requested Reply Length type used by the Inquire Setup Information,
  Inquire Board Model Number, Inquire Synchronous Period, and Inquire Extended
  Setup Information commands.
*/

typedef unsigned char BusLogic_RequestedReplyLength_T;


/*
  Define a Lock data structure.  Until a true symmetric multiprocessing kernel
  with fine grained locking is available, acquiring the lock is implemented as
  saving the processor flags and disabling interrupts, and releasing the lock
  restores the saved processor flags.
*/

typedef unsigned long BusLogic_Lock_T;


/*
  Define the Outgoing Mailbox Action Codes.
*/

typedef enum
{
  BusLogic_OutgoingMailboxFree =		0,
  BusLogic_MailboxStartCommand =		1,
  BusLogic_MailboxAbortCommand =		2
}
BusLogic_ActionCode_T;


/*
  Define the Incoming Mailbox Completion Codes.
*/

typedef enum
{
  BusLogic_IncomingMailboxFree =		0,
  BusLogic_CommandCompletedWithoutError =	1,
  BusLogic_CommandAbortedAtHostRequest =	2,
  BusLogic_AbortedCommandNotFound =		3,
  BusLogic_CommandCompletedWithError =		4
}
BusLogic_CompletionCode_T;


/*
  Define the Command Control Block (CCB) Opcodes.
*/

typedef enum
{
  BusLogic_InitiatorCCB =			0x00,
  BusLogic_TargetCCB =				0x01,
  BusLogic_InitiatorCCB_ScatterGather =		0x02,
  BusLogic_InitiatorCCB_ResidualDataLength =	0x03,
  BusLogic_InitiatorCCB_ScatterGatherResidual =	0x04,
  BusLogic_BusDeviceReset =			0x81
}
BusLogic_CCB_Opcode_T;


/*
  Define the CCB Data Direction Codes.
*/

typedef enum
{
  BusLogic_UncheckedDataTransfer =		0x00,
  BusLogic_DataInLengthChecked =		0x01,
  BusLogic_DataOutLengthChecked =		0x02,
  BusLogic_NoDataTransfer =			0x03
}
BusLogic_DataDirection_T;


/*
  Define the Host Adapter Status Codes.
*/

typedef enum
{
  BusLogic_CommandCompletedNormally =		0x00,
  BusLogic_LinkedCommandCompleted =		0x0A,
  BusLogic_LinkedCommandCompletedWithFlag =	0x0B,
  BusLogic_SCSISelectionTimeout =		0x11,
  BusLogic_DataOverUnderRun =			0x12,
  BusLogic_UnexpectedBusFree =			0x13,
  BusLogic_InvalidBusPhaseRequested =		0x14,
  BusLogic_InvalidOutgoingMailboxActionCode =	0x15,
  BusLogic_InvalidCommandOperationCode =	0x16,
  BusLogic_LinkedCCBhasInvalidLUN =		0x17,
  BusLogic_InvalidCommandParameter =		0x1A,
  BusLogic_AutoRequestSenseFailed =		0x1B,
  BusLogic_TaggedQueuingMessageRejected =	0x1C,
  BusLogic_UnsupportedMessageReceived =		0x1D,
  BusLogic_HostAdapterHardwareFailed =		0x20,
  BusLogic_TargetFailedResponseToATN =		0x21,
  BusLogic_HostAdapterAssertedRST =		0x22,
  BusLogic_OtherDeviceAssertedRST =		0x23,
  BusLogic_TargetDeviceReconnectedImproperly =	0x24,
  BusLogic_HostAdapterAssertedBusDeviceReset =	0x25,
  BusLogic_AbortQueueGenerated =		0x26,
  BusLogic_HostAdapterSoftwareError =		0x27,
  BusLogic_HostAdapterHardwareTimeoutError =	0x30,
  BusLogic_SCSIParityErrorDetected =		0x34
}
BusLogic_HostAdapterStatus_T;


/*
  Define the SCSI Target Device Status Codes.
*/

typedef enum
{
  BusLogic_OperationGood =			0x00,
  BusLogic_CheckCondition =			0x02,
  BusLogic_DeviceBusy =				0x08
}
BusLogic_TargetDeviceStatus_T;


/*
  Define the Queue Tag Codes.
*/

typedef enum
{
  BusLogic_SimpleQueueTag =			0x00,
  BusLogic_HeadOfQueueTag =			0x01,
  BusLogic_OrderedQueueTag =			0x02,
  BusLogic_ReservedQT =				0x03
}
BusLogic_QueueTag_T;


/*
  Define the SCSI Command Descriptor Block (CDB).
*/

#define BusLogic_CDB_MaxLength			12

typedef unsigned char SCSI_CDB_T[BusLogic_CDB_MaxLength];


/*
  Define the SCSI Sense Data.
*/

#define BusLogic_SenseDataMaxLength		255

typedef unsigned char SCSI_SenseData_T[BusLogic_SenseDataMaxLength];


/*
  Define the Scatter/Gather Segment structure required by the Host Adapter
  Firmware Interface.
*/

typedef struct BusLogic_ScatterGatherSegment
{
  unsigned long SegmentByteCount;			/* Bytes 0-3 */
  void *SegmentDataPointer;				/* Bytes 4-7 */
}
BusLogic_ScatterGatherSegment_T;


/*
  Define the 32 Bit Mode Command Control Block (CCB) structure.  The first 40
  bytes are defined by the Host Adapter Firmware Interface.  The remaining
  components are defined by the Linux BusLogic Driver.  Wide Mode CCBs differ
  from standard 32 Bit Mode CCBs only in having the TagEnable and QueueTag
  fields moved from byte 17 to byte 1, and the Logical Unit field in byte 17
  expanded to 6 bits; unfortunately, using a union of structs containing
  enumeration type bitfields to provide both definitions leads to packing
  problems, so the following definition is used which requires setting
  TagEnable to Logical Unit bit 5 in Wide Mode CCBs.
*/

typedef struct BusLogic_CCB
{
  /*
    BusLogic Host Adapter Firmware Portion.
  */
  BusLogic_CCB_Opcode_T Opcode:8;			/* Byte 0 */
  unsigned char :3;					/* Byte 1 Bits 0-2 */
  BusLogic_DataDirection_T DataDirection:2;		/* Byte 1 Bits 3-4 */
  boolean WideModeTagEnable:1;				/* Byte 1 Bit 5 */
  BusLogic_QueueTag_T WideModeQueueTag:2;		/* Byte 1 Bits 6-7 */
  unsigned char CDB_Length;				/* Byte 2 */
  unsigned char SenseDataLength;			/* Byte 3 */
  unsigned long DataLength;				/* Bytes 4-7 */
  void *DataPointer;					/* Bytes 8-11 */
  unsigned char :8;					/* Byte 12 */
  unsigned char :8;					/* Byte 13 */
  BusLogic_HostAdapterStatus_T HostAdapterStatus:8;	/* Byte 14 */
  BusLogic_TargetDeviceStatus_T TargetDeviceStatus:8;	/* Byte 15 */
  unsigned char TargetID;				/* Byte 16 */
  unsigned char LogicalUnit:5;				/* Byte 17 Bits 0-4 */
  boolean TagEnable:1;					/* Byte 17 Bit 5 */
  BusLogic_QueueTag_T QueueTag:2;			/* Byte 17 Bits 6-7 */
  SCSI_CDB_T CDB;					/* Bytes 18-29 */
  unsigned char :8;					/* Byte 30 */
  unsigned char :8;					/* Byte 31 */
  unsigned long :32;					/* Bytes 32-35 */
  SCSI_SenseData_T *SenseDataPointer;			/* Bytes 36-39 */
  /*
    BusLogic Linux Driver Portion.
  */
  struct BusLogic_HostAdapter *HostAdapter;
  SCSI_Command_T *Command;
  enum { BusLogic_CCB_Free =	    0,
	 BusLogic_CCB_Active =	    1,
	 BusLogic_CCB_Completed =   2,
	 BusLogic_CCB_Reset =	    3 } Status;
  BusLogic_CompletionCode_T MailboxCompletionCode;
  unsigned int SerialNumber;
  struct BusLogic_CCB *Next;
  struct BusLogic_CCB *NextAll;
  BusLogic_ScatterGatherSegment_T
    ScatterGatherList[BusLogic_ScatterGatherLimit];
}
BusLogic_CCB_T;


/*
  Define the 32 Bit Mode Outgoing Mailbox structure.
*/

typedef struct BusLogic_OutgoingMailbox
{
  BusLogic_CCB_T *CCB;					/* Bytes 0-3 */
  unsigned long :24;					/* Byte 4 */
  BusLogic_ActionCode_T ActionCode:8;			/* Bytes 5-7 */
}
BusLogic_OutgoingMailbox_T;


/*
  Define the 32 Bit Mode Incoming Mailbox structure.
*/

typedef struct BusLogic_IncomingMailbox
{
  BusLogic_CCB_T *CCB;					/* Bytes 0-3 */
  BusLogic_HostAdapterStatus_T HostAdapterStatus:8;	/* Byte 4 */
  BusLogic_TargetDeviceStatus_T TargetDeviceStatus:8;	/* Byte 5 */
  unsigned char :8;					/* Byte 6 */
  BusLogic_CompletionCode_T CompletionCode:8;		/* Byte 7 */
}
BusLogic_IncomingMailbox_T;


/*
  Define the possible Bus Types.
*/

typedef enum
{
  BusLogic_Unknown_Bus =			0,
  BusLogic_ISA_Bus =				1,
  BusLogic_MCA_Bus =				2,
  BusLogic_EISA_Bus =				3,
  BusLogic_VESA_Bus =				4,
  BusLogic_PCI_Bus =				5
}
BusLogic_BusType_T;

static char
  *BusLogic_BusNames[] =
    { "Unknown", "ISA", "MCA", "EISA", "VESA", "PCI" };


/*
  Define the Linux BusLogic Driver Command Line Entry structure.
*/

typedef struct BusLogic_CommandLineEntry
{
  unsigned short IO_Address;
  unsigned short TaggedQueueDepth;
  unsigned short BusSettleTime;
  unsigned short LocalOptions;
  unsigned short TaggedQueuingPermitted;
  unsigned short TaggedQueuingPermittedMask;
  unsigned char ErrorRecoveryStrategy[BusLogic_MaxTargetDevices];
}
BusLogic_CommandLineEntry_T;


/*
  Define the Linux BusLogic Driver Host Adapter structure.
*/

typedef struct BusLogic_HostAdapter
{
  SCSI_Host_T *SCSI_Host;
  unsigned char HostNumber;
  unsigned char ModelName[9];
  unsigned char FirmwareVersion[6];
  unsigned char BoardName[18];
  unsigned char InterruptLabel[62];
  unsigned short IO_Address;
  unsigned char IRQ_Channel;
  unsigned char DMA_Channel;
  unsigned char SCSI_ID;
  BusLogic_BusType_T BusType:3;
  boolean IRQ_ChannelAcquired:1;
  boolean DMA_ChannelAcquired:1;
  boolean SynchronousInitiation:1;
  boolean ParityChecking:1;
  boolean ExtendedTranslation:1;
  boolean LevelSensitiveInterrupts:1;
  boolean HostWideSCSI:1;
  boolean HostDifferentialSCSI:1;
  boolean HostAutomaticConfiguration:1;
  boolean HostUltraSCSI:1;
  boolean TerminationInfoValid:1;
  boolean LowByteTerminated:1;
  boolean HighByteTerminated:1;
  boolean BounceBuffersRequired:1;
  boolean StrictRoundRobinModeSupported:1;
  boolean HostAdapterResetRequested:1;
  volatile boolean HostAdapterCommandCompleted:1;
  unsigned short HostAdapterScatterGatherLimit;
  unsigned short DriverScatterGatherLimit;
  unsigned short MaxTargetDevices;
  unsigned short MaxLogicalUnits;
  unsigned short MailboxCount;
  unsigned short InitialCCBs;
  unsigned short IncrementalCCBs;
  unsigned short TotalQueueDepth;
  unsigned short TaggedQueueDepth;
  unsigned short UntaggedQueueDepth;
  unsigned short BusSettleTime;
  unsigned short LocalOptions;
  unsigned short DisconnectPermitted;
  unsigned short TaggedQueuingPermitted;
  unsigned long BIOS_Address;
  BusLogic_InstalledDevices_T InstalledDevices;
  BusLogic_SynchronousValues_T SynchronousValues;
  BusLogic_SynchronousPeriod_T SynchronousPeriod;
  BusLogic_CommandLineEntry_T *CommandLineEntry;
  struct BusLogic_HostAdapter *Next;
  BusLogic_CCB_T *All_CCBs;
  BusLogic_CCB_T *Free_CCBs;
  BusLogic_CCB_T *BusDeviceResetPendingCCB[BusLogic_MaxTargetDevices];
  unsigned char ErrorRecoveryStrategy[BusLogic_MaxTargetDevices];
  unsigned char TaggedQueuingActive[BusLogic_MaxTargetDevices];
  unsigned char CommandSuccessfulFlag[BusLogic_MaxTargetDevices];
  unsigned char ActiveCommandCount[BusLogic_MaxTargetDevices];
  unsigned long TotalCommandCount[BusLogic_MaxTargetDevices];
  unsigned long LastSequencePoint[BusLogic_MaxTargetDevices];
  unsigned long LastResetTime[BusLogic_MaxTargetDevices];
  BusLogic_OutgoingMailbox_T *FirstOutgoingMailbox;
  BusLogic_OutgoingMailbox_T *LastOutgoingMailbox;
  BusLogic_OutgoingMailbox_T *NextOutgoingMailbox;
  BusLogic_IncomingMailbox_T *FirstIncomingMailbox;
  BusLogic_IncomingMailbox_T *LastIncomingMailbox;
  BusLogic_IncomingMailbox_T *NextIncomingMailbox;
}
BusLogic_HostAdapter_T;


/*
  Define a structure for the BIOS Disk Parameters.
*/

typedef struct BIOS_DiskParameters
{
  int Heads;
  int Sectors;
  int Cylinders;
}
BIOS_DiskParameters_T;


/*
  BusLogic_AcquireHostAdapterLock acquires exclusive access to Host Adapter.
*/

static inline
void BusLogic_AcquireHostAdapterLock(BusLogic_HostAdapter_T *HostAdapter,
				     BusLogic_Lock_T *Lock)
{
  save_flags(*Lock);
  cli();
}


/*
  BusLogic_ReleaseHostAdapterLock releases exclusive access to Host Adapter.
*/

static inline
void BusLogic_ReleaseHostAdapterLock(BusLogic_HostAdapter_T *HostAdapter,
				     BusLogic_Lock_T *Lock)
{
  restore_flags(*Lock);
}


/*
  BusLogic_AcquireHostAdapterLockID acquires exclusive access to Host Adapter,
  but is only called when interrupts are disabled.
*/

static inline
void BusLogic_AcquireHostAdapterLockID(BusLogic_HostAdapter_T *HostAdapter,
				       BusLogic_Lock_T *Lock)
{
}


/*
  BusLogic_ReleaseHostAdapterLockID releases exclusive access to Host Adapter,
  but is only called when interrupts are disabled.
*/

static inline
void BusLogic_ReleaseHostAdapterLockID(BusLogic_HostAdapter_T *HostAdapter,
				       BusLogic_Lock_T *Lock)
{
}


/*
  Define functions to provide an abstraction for reading and writing the
  Host Adapter I/O Registers.
*/

static inline
void BusLogic_WriteControlRegister(BusLogic_HostAdapter_T *HostAdapter,
				   unsigned char Value)
{
  outb(Value, HostAdapter->IO_Address + BusLogic_ControlRegister);
}

static inline
unsigned char BusLogic_ReadStatusRegister(BusLogic_HostAdapter_T *HostAdapter)
{
  return inb(HostAdapter->IO_Address + BusLogic_StatusRegister);
}

static inline
void BusLogic_WriteCommandParameterRegister(BusLogic_HostAdapter_T *HostAdapter,
					    unsigned char Value)
{
  outb(Value, HostAdapter->IO_Address + BusLogic_CommandParameterRegister);
}

static inline
unsigned char BusLogic_ReadDataInRegister(BusLogic_HostAdapter_T *HostAdapter)
{
  return inb(HostAdapter->IO_Address + BusLogic_DataInRegister);
}

static inline
unsigned char BusLogic_ReadInterruptRegister(BusLogic_HostAdapter_T
					     *HostAdapter)
{
  return inb(HostAdapter->IO_Address + BusLogic_InterruptRegister);
}

static inline
unsigned char BusLogic_ReadGeometryRegister(BusLogic_HostAdapter_T *HostAdapter)
{
  return inb(HostAdapter->IO_Address + BusLogic_GeometryRegister);
}


/*
  BusLogic_StartMailboxCommand issues an Execute Mailbox Command, which
  notifies the Host Adapter that an entry has been made in an Outgoing
  Mailbox.
*/

static inline
void BusLogic_StartMailboxCommand(BusLogic_HostAdapter_T *HostAdapter)
{
  BusLogic_WriteCommandParameterRegister(HostAdapter,
					 BusLogic_ExecuteMailboxCommand);
}


/*
  BusLogic_Delay waits for Seconds to elapse.
*/

static inline void BusLogic_Delay(int Seconds)
{
  unsigned long TimeoutJiffies = jiffies + Seconds * HZ;
  unsigned long ProcessorFlags;
  save_flags(ProcessorFlags);
  sti();
  while (jiffies < TimeoutJiffies) ;
  restore_flags(ProcessorFlags);
}


/*
  Define prototypes for the forward referenced BusLogic Driver
  Internal Functions.
*/

static void BusLogic_InterruptHandler(int, void *, Registers_T *);
static int BusLogic_ResetHostAdapter(BusLogic_HostAdapter_T *,
				     SCSI_Command_T *,
				     unsigned int);


#endif /* BusLogic_DriverVersion */
