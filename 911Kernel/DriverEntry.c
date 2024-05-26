#include "DriverEntry.h"
#include "MajorFunction.h"
#include "SystemHelper.h"
#include "CallbackHelper.h"


//Hook
//Callback  (Win7)





//bp 911Kernel!DriverEntry
NTSTATUS
DriverEntry(PDRIVER_OBJECT  DriverObject, PUNICODE_STRING  RegisterPath)
{
	PDEVICE_OBJECT DeviceObject;
	UNICODE_STRING DeviceObjectName;
	UNICODE_STRING SymbolicLink;
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	int i = 0;

	PLDR_DATA_TABLE_ENTRY LdrDataTableEntry;
	RtlInitUnicodeString(&DeviceObjectName, DEVICE_OBJECT_NAME);
	RtlInitUnicodeString(&SymbolicLink, SYMBOLIC_LINK);


	DriverObject->DriverUnload = DriverUnload;

	//创建设备对象;
	Status = IoCreateDevice(DriverObject, 0, &DeviceObjectName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(Status))
	{
		return Status;
	}

	//创建符号链接;
	Status = IoCreateSymbolicLink(&SymbolicLink, &DeviceObjectName);

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DriverObject->MajorFunction[i] = PassThroughDispatch;
	}

	//
	LdrDataTableEntry = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;  //不是枚举其他的系统
	LdrDataTableEntry->Flags |= 0x20;   //小机关


	RegisterFileProtection();   //注册回调保护
	return STATUS_SUCCESS;
}








