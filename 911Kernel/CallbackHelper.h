#pragma once
#include <fltKernel.h>


NTSTATUS RegisterFileProtection();
NTSTATUS UnregisterFileProtection();

VOID EnableProtectFile(POBJECT_TYPE ObjectType);
OB_PREOP_CALLBACK_STATUS PreOperation(PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION ObPreviousOperationInfo);