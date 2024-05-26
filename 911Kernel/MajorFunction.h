#pragma once
#include <fltKernel.h>

void DriverUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS PassThroughDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp);