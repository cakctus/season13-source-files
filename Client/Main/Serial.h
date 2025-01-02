#include "sha1.h"
#include <winbase.h>

#if !defined(AFX_PROTECT_H__F6763674_2EFE_4243_9D00_45971CECFBB8__INCLUDED_)
#define AFX_PROTECT_H__F6763674_2EFE_4243_9D00_45971CECFBB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)

static DWORD HDTYPE;
static char HDNAME[16];
static char HDSERIAL[18];
static SYSTEM_INFO HDSYSTEM;

#define ENC_NUMBER 850

#define HD_TYPE_1 895742 //NORMAL
#define HD_TYPE_2 106671 //TEMP
#define HD_TYPE_3 789541 //EXTENDED

#define  SMART_GET_VERSION        0x00074080
#define  SMART_RCV_DRIVE_DATA     0x0007c088
#define  IDE_ATAPI_IDENTIFY       0xA1
#define  IDE_ATA_IDENTIFY         0xEC

struct DRIVER_STATUS
{
    BYTE  bDriverError;     //  Error code from driver, or 0 if no error.
    BYTE  bIDEStatus;       //  Contents of IDE Error register.
    BYTE  bReserved[10];    //  Reserved for future expansion.
};

struct DRIVER_DATA
{
    DWORD cBufferSize;          //  Size of bBuffer in bytes
    DRIVER_STATUS DriverStatus; //  Driver status structure.
    BYTE bBuffer[512];          //  Buffer of arbitrary length in which to store the data read from the drive.
};

struct DRIVER_VERSION
{
    BYTE bVersion;           // Binary driver version.
    BYTE bRevision;          // Binary driver revision.
    BYTE bReserved;          // Not used.
    BYTE bIDEDeviceMap;      // Bit map of IDE devices.
    DWORD fCapabilities;     // Bit mask of driver capabilities.
    DWORD dwReserved[4];     // For future use.
};

struct DRIVER_INFO
{
    BYTE bFeaturesReg;       // Used for specifying SMART "commands".
    BYTE bSectorCountReg;    // IDE sector count register
    BYTE bSectorNumberReg;   // IDE sector number register
    BYTE bCylLowReg;         // IDE low order cylinder value
    BYTE bCylHighReg;        // IDE high order cylinder value
    BYTE bDriveHeadReg;      // IDE drive/head register
    BYTE bCommandReg;        // Actual IDE command.
    BYTE bReserved;          // reserved for future use.  Must be zero.
};

struct DRIVER_BASE
{
    DWORD cBufferSize;       // Buffer size in bytes
    DRIVER_INFO DriverInfo;  // Structure with drive register values.
    BYTE bDriveNumber;       // Physical drive number to send command to (0,1,2,3).
    BYTE bReserved[16];      // Reserved for future expansion.
};

#pragma pack(1)

typedef struct _IDENTIFY_DATA {
    USHORT GeneralConfiguration;            // 00 00
    USHORT NumberOfCylinders;               // 02  1
    USHORT Reserved1;                       // 04  2
    USHORT NumberOfHeads;                   // 06  3
    USHORT UnformattedBytesPerTrack;        // 08  4
    USHORT UnformattedBytesPerSector;       // 0A  5
    USHORT SectorsPerTrack;                 // 0C  6
    USHORT VendorUnique1[3];                // 0E  7-9
    USHORT SerialNumber[10];                // 14  10-19
    USHORT BufferType;                      // 28  20
    USHORT BufferSectorSize;                // 2A  21
    USHORT NumberOfEccBytes;                // 2C  22
    USHORT FirmwareRevision[4];             // 2E  23-26
    USHORT ModelNumber[20];                 // 36  27-46
    UCHAR  MaximumBlockTransfer;            // 5E  47
    UCHAR  VendorUnique2;                   // 5F
    USHORT DoubleWordIo;                    // 60  48
    USHORT Capabilities;                    // 62  49
    USHORT Reserved2;                       // 64  50
    UCHAR  VendorUnique3;                   // 66  51
    UCHAR  PioCycleTimingMode;              // 67
    UCHAR  VendorUnique4;                   // 68  52
    UCHAR  DmaCycleTimingMode;              // 69
    USHORT TranslationFieldsValid:1;        // 6A  53
    USHORT Reserved3:15;
    USHORT NumberOfCurrentCylinders;        // 6C  54
    USHORT NumberOfCurrentHeads;            // 6E  55
    USHORT CurrentSectorsPerTrack;          // 70  56
    ULONG  CurrentSectorCapacity;           // 72  57-58
    USHORT CurrentMultiSectorSetting;       //     59
    ULONG  UserAddressableSectors;          //     60-61
    USHORT SingleWordDMASupport : 8;        //     62
    USHORT SingleWordDMAActive : 8;
    USHORT MultiWordDMASupport : 8;         //     63
    USHORT MultiWordDMAActive : 8;
    USHORT AdvancedPIOModes : 8;            //     64
    USHORT Reserved4 : 8;
    USHORT MinimumMWXferCycleTime;          //     65
    USHORT RecommendedMWXferCycleTime;      //     66
    USHORT MinimumPIOCycleTime;             //     67
    USHORT MinimumPIOCycleTimeIORDY;        //     68
    USHORT Reserved5[2];                    //     69-70
    USHORT ReleaseTimeOverlapped;           //     71
    USHORT ReleaseTimeServiceCommand;       //     72
    USHORT MajorRevision;                   //     73
    USHORT MinorRevision;                   //     74
    USHORT Reserved6[50];                   //     75-126
    USHORT SpecialFunctionsEnabled;         //     127
    USHORT Reserved7[128];                  //     128-255
}IDENTIFY_DATA,*PIDENTIFY_DATA;

char FinalHash[256];

#pragma pack()

class CSerial
{
public:
	CSerial()
	{
	}

	virtual ~CSerial()
	{
	}

	static char* GetSerial()
    {
		char serial[256];
		char Sys[256];		

		char temptext[256];
		
		SYSTEM_INFO system;
		
		MEMORYSTATUS mi;
		memset( &mi, 0, sizeof(MEMORYSTATUS) );
		mi.dwLength = sizeof(MEMORYSTATUS);
		GlobalMemoryStatus( &mi );
		
	    int hd = 0;

	    wchar_t name[256];

        swprintf(name,L"\\\\.\\PhysicalDrive%d",hd);

	    HANDLE Drive = CreateFileW(name,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0,NULL);

	    if(Drive == INVALID_HANDLE_VALUE)
	    {
			return "0";
	    }

        DWORD TempSerial = 0;
		GetVolumeInformation("C:\\",NULL,0,&TempSerial,NULL,NULL,NULL,0);
		wsprintf(serial,"%X",TempSerial);
		CloseHandle(Drive);
		
		GetSystemInfo(&system);

		wsprintf(Sys,"%X|%X|%X|%X|%X|%X|%X",
			system.dwActiveProcessorMask,
			system.dwNumberOfProcessors,
			system.dwProcessorType,
			system.wProcessorArchitecture,
			system.wProcessorLevel,
			system.wProcessorRevision,
			mi.dwTotalPhys
		);

		wsprintf(temptext,"%s|%s",Sys,serial);

		SHA1 sha;
		unsigned message_digest1[5];

		sha.Reset();
		sha << temptext;
		sha.Result(message_digest1);

		wsprintf(FinalHash,"%08X-%08X-%08X-%08X-%08X",message_digest1[0],message_digest1[1],message_digest1[2],message_digest1[3],message_digest1[4]);

		return FinalHash;
	}
};

#endif // !defined(AFX_PROTECT_H__F6763674_2EFE_4243_9D00_45971CECFBB8__INCLUDED_)