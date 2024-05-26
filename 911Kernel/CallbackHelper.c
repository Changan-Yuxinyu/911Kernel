#include "CallbackHelper.h"
#include "ObjectHelper.h"


PVOID  __CallBackHandle = NULL;

//ͨ��ϵͳ�ṩ��ObRegisterCallbacks�����ļ�����
NTSTATUS RegisterFileProtection()
{
	OB_CALLBACK_REGISTRATION  ObCallBackRegistration;//�ص������Ϣ�ṹ��
	OB_OPERATION_REGISTRATION ObOperationRegistration;//������Ϣ��ؽṹ�� ������ǰ�� �ļ���������ǰ�ص�

	NTSTATUS  Status;


	EnableProtectFile(*IoFileObjectType);


	memset(&ObCallBackRegistration, 0, sizeof(OB_CALLBACK_REGISTRATION));
	ObCallBackRegistration.Version = ObGetFilterVersion();
	ObCallBackRegistration.OperationRegistrationCount = 1;
	ObCallBackRegistration.RegistrationContext = NULL;
	RtlInitUnicodeString(&ObCallBackRegistration.Altitude, L"321000");

	memset(&ObOperationRegistration, 0, sizeof(OB_OPERATION_REGISTRATION)); //��ʼ���ṹ�����


	ObOperationRegistration.ObjectType = IoFileObjectType;
	ObOperationRegistration.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;



	ObOperationRegistration.PreOperation = (POB_PRE_OPERATION_CALLBACK)&PreOperation; //������ע��һ���ص�����ָ��

	//���������ṹ��
	ObCallBackRegistration.OperationRegistration = &ObOperationRegistration; //ע����һ�����

	//�����ص�
	Status = ObRegisterCallbacks(&ObCallBackRegistration, &__CallBackHandle);//�õ��������ж��
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
		ObUnRegisterCallbacks(__CallBackHandle);   //�ص�����
	}
}
VOID EnableProtectFile(POBJECT_TYPE ObjectType)
{
	POBJECT_TYPE_1  v1 = (POBJECT_TYPE_1)ObjectType;
	v1->TypeInfo.SupportsObjectCallbacks = 1;
}



//���˺���
/*************************************************************************************/
OB_PREOP_CALLBACK_STATUS PreOperation(PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION ObPreviousOperationInfo)
{
	//�������Ķ���
	//���ĸ����̲�����
	UNICODE_STRING v1;
	POBJECT_NAME_INFORMATION ObjectNameInfo;   //һάָ��
	PFILE_OBJECT FileObject = (PFILE_OBJECT)ObPreviousOperationInfo->Object;
	HANDLE ProcessIdentify = PsGetCurrentProcessId();   //

	if (ObPreviousOperationInfo->ObjectType != *IoFileObjectType)
	{
		return OB_PREOP_SUCCESS;  //���Ե�ǰ����Ϊ
	}
	//������Чָ��
	if (FileObject->FileName.Buffer == NULL ||
		!MmIsAddressValid(FileObject->FileName.Buffer) ||
		FileObject->DeviceObject == NULL ||
		!MmIsAddressValid(FileObject->DeviceObject))   //�ļ����������ٻ���һ���豸���� �������ڵĴ���
	{
		return OB_PREOP_SUCCESS;
	}

	if (!NT_SUCCESS(IoQueryFileDosDeviceName((PFILE_OBJECT)FileObject, &ObjectNameInfo)))//����һάָ��ĵ�ַ�������ж�̬�ڴ� ��Ҫ�ͷ�
	{
		return OB_PREOP_SUCCESS;
	}
	if (ObjectNameInfo->Name.Buffer == NULL || ObjectNameInfo->Name.Length == 0)
	{
		ExFreePool(ObjectNameInfo);
		return OB_PREOP_SUCCESS;
	}

	//�ڰ�����
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
