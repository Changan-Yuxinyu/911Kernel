#include "CallbackHelper.h"
#include "ObjectHelper.h"


PVOID  __CallBackHandle = NULL;

//通过系统提供的ObRegisterCallbacks进行文件保护
NTSTATUS RegisterFileProtection()
{
	OB_CALLBACK_REGISTRATION  ObCallBackRegistration;//回调相关信息结构体
	OB_OPERATION_REGISTRATION ObOperationRegistration;//操作信息相关结构体 包含于前者 文件保护是事前回调

	NTSTATUS  Status;


	EnableProtectFile(*IoFileObjectType);


	memset(&ObCallBackRegistration, 0, sizeof(OB_CALLBACK_REGISTRATION));
	ObCallBackRegistration.Version = ObGetFilterVersion();
	ObCallBackRegistration.OperationRegistrationCount = 1;
	ObCallBackRegistration.RegistrationContext = NULL;
	RtlInitUnicodeString(&ObCallBackRegistration.Altitude, L"321000");

	memset(&ObOperationRegistration, 0, sizeof(OB_OPERATION_REGISTRATION)); //初始化结构体变量


	ObOperationRegistration.ObjectType = IoFileObjectType;
	ObOperationRegistration.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;



	ObOperationRegistration.PreOperation = (POB_PRE_OPERATION_CALLBACK)&PreOperation; //在这里注册一个回调函数指针

	//关联两个结构体
	ObCallBackRegistration.OperationRegistration = &ObOperationRegistration; //注意这一条语句

	//启动回到
	Status = ObRegisterCallbacks(&ObCallBackRegistration, &__CallBackHandle);//得到句柄用于卸载
	if (!NT_SUCCESS(Status))
	{
		Status = STATUS_UNSUCCESSFUL;
	}
	else
	{
		Status = STATUS_SUCCESS;
	}
	return Status;
}

NTSTATUS UnregisterFileProtection()
{
	if (__CallBackHandle != NULL)
	{
		ObUnRegisterCallbacks(__CallBackHandle);   //回调函数
	}
}
VOID EnableProtectFile(POBJECT_TYPE ObjectType)
{
	POBJECT_TYPE_1  v1 = (POBJECT_TYPE_1)ObjectType;
	v1->TypeInfo.SupportsObjectCallbacks = 1;
}



//过滤函数
/*************************************************************************************/
OB_PREOP_CALLBACK_STATUS PreOperation(PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION ObPreviousOperationInfo)
{
	//被操作的对象
	//被哪个进程操作的
	UNICODE_STRING v1;
	POBJECT_NAME_INFORMATION ObjectNameInfo;   //一维指针
	PFILE_OBJECT FileObject = (PFILE_OBJECT)ObPreviousOperationInfo->Object;
	HANDLE ProcessIdentify = PsGetCurrentProcessId();   //

	if (ObPreviousOperationInfo->ObjectType != *IoFileObjectType)
	{
		return OB_PREOP_SUCCESS;  //忽略当前的行为
	}
	//过滤无效指针
	if (FileObject->FileName.Buffer == NULL ||
		!MmIsAddressValid(FileObject->FileName.Buffer) ||
		FileObject->DeviceObject == NULL ||
		!MmIsAddressValid(FileObject->DeviceObject))   //文件对象中至少会有一个设备对象 就是所在的磁盘
	{
		return OB_PREOP_SUCCESS;
	}

	if (!NT_SUCCESS(IoQueryFileDosDeviceName((PFILE_OBJECT)FileObject, &ObjectNameInfo)))//传入一维指针的地址，表明有动态内存 需要释放
	{
		return OB_PREOP_SUCCESS;
	}
	if (ObjectNameInfo->Name.Buffer == NULL || ObjectNameInfo->Name.Length == 0)
	{
		ExFreePool(ObjectNameInfo);
		return OB_PREOP_SUCCESS;
	}

	//黑白名单
	if (wcsstr(ObjectNameInfo->Name.Buffer, L"D:\\911Kernel.txt")
		|| wcsstr(ObjectNameInfo->Name.Buffer, L"C:\\911Kernel.txt"))
	{

		if (FileObject->DeleteAccess == TRUE || FileObject->WriteAccess == TRUE)
		{
			if (ObPreviousOperationInfo->Operation == OB_OPERATION_HANDLE_CREATE)
			{
				ObPreviousOperationInfo->Parameters->CreateHandleInformation.DesiredAccess = 0;
			}
			if (ObPreviousOperationInfo->Operation == OB_OPERATION_HANDLE_DUPLICATE)
			{
				ObPreviousOperationInfo->Parameters->DuplicateHandleInformation.DesiredAccess = 0;
			}
		}

	}
	RtlVolumeDeviceToDosName(FileObject->DeviceObject, &v1);
	DbgPrint("ProcessIdentify : %ld File : %wZ  %wZ\r\n", ProcessIdentify, &v1, &ObjectNameInfo->Name);
	ExFreePool(ObjectNameInfo);

	return OB_PREOP_SUCCESS;
}
