#include "MajorFunction.h"
#include "DriverEntry.h"
#include "CallbackHelper.h"





NTSTATUS PassThroughDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;     //LastError()
	Irp->IoStatus.Information = 0;             //ReturnLength 
	IoCompleteRequest(Irp, IO_NO_INCREMENT);   //��Irp���ظ�Io������
	return STATUS_SUCCESS;
}






void DriverUnload(PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING  SymbolicLink;
	PDEVICE_OBJECT	v1 = NULL;
	PDEVICE_OBJECT  DeviceObject = NULL;

	
	UnregisterFileProtection();





	//�����豸������
	RtlInitUnicodeString(&SymbolicLink, SYMBOLIC_LINK);
	IoDeleteSymbolicLink(&SymbolicLink);

	//ͨ�������������豸����
	DeviceObject = DriverObject->DeviceObject;
	while (DeviceObject != NULL)
	{
		v1 = DeviceObject->NextDevice;
		IoDeleteDevice(DeviceObject);
		DeviceObject = v1;
	}
	DbgPrint(("DriverUnload()\r\n"));
}