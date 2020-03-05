#pragma once
#ifdef _KERNEL_MODE
#include <ntddk.h>
#else
#include <windows.h>
#endif
#include "pch.h"
#include "framework.h"

#include <pshpack1.h>
typedef struct _HID_INPUT_REPORT {
	unsigned char ReportId;

	union {
		struct {
			UCHAR Modifiers;
			UCHAR Key1;
			UCHAR Key2;
			UCHAR Key3;
			UCHAR Key4;
			UCHAR Key5;
			UCHAR Key6;
		} KeyReport;
		struct {
			UCHAR Buttons;
			SHORT AbsoluteX;
			SHORT AbsoluteY;
			UCHAR Padding[2]; // Padding to keep the reports the same size
		} MouseReport;
	} Report;
} HID_INPUT_REPORT, * PHID_INPUT_REPORT;
#include <poppack.h>