GST_ASFLAGS = -Xvc -Ox

!if "$(GST_ARCH)" == "x86"
GST_ASFLAGS = $(GST_ASFLAGS) -f win32 --prefix _ -D MS_STDCALL -D DLL_EXPORT
!else
GST_ASFLAGS = $(GST_ASFLAGS) -f win64
!endif

GST_ASM_ERR_LOG = ..\Driver\build_errors_asm.log
