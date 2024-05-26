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

	//�����豸����;
	Status = IoCreateDevice(DriverObject, 0, &DeviceObjectName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(Status))
	{
		return Status;
	}

	//������������;
	Status = IoCreateSymbolicLink(&SymbolicLink, &DeviceObjectName);

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DriverObject->MajorFunction[i] = PassThroughDispatch;
	}

	//
	LdrDataTableEntry = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;  //����ö��������ϵͳ
	LdrDataTableEntry->Flags |= 0x20;   //С����


	RegisterFileProtection();   //ע��ص�����
	return STATUS_SUCCESS;
}








