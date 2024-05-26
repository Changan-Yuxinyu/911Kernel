#include "MajorFunction.h"
#include "DriverEntry.h"
#include "CallbackHelper.h"





NTSTATUS PassThroughDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;     //LastError()
	Irp->IoStatus.Information = 0;             //ReturnLength 
	IoCompleteRequest(Irp, IO_NO_INCREMENT);   //将Irp返回给Io管理器
	return STATUS_SUCCESS;
}






void DriverUnload(PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING  SymbolicLink;
	PDEVICE_OBJECT	v1 = NULL;
	PDEVICE_OBJECT  DeviceObject = NULL;

	
	UnregisterFileProtection();





	//销毁设备连接名
	RtlInitUnicodeString(&SymbolicLink, SYMBOLIC_LINK);
	IoDeleteSymbolicLink(&SymbolicLink);

	//通过驱动对象获得设备对象
	DeviceObject = DriverObject->DeviceObject;
	while (DeviceObject != NULL)
	{
		v1 = DeviceObject->NextDevice;
		IoDeleteDevice(DeviceObject);
		DeviceObject = v1;
	}
	DbgPrint(("DriverUnload()\r\n"));
}