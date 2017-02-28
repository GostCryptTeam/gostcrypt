:: Usage: BuildDriver <-build|-rebuild|-clean> <-release|-debug> <-x86|-x64> <dir1> [dir2] ...

@echo off
set GST_ARG_CMD=%~1
shift
set GST_ARG_TYPE=%~1
shift
set GST_ARG_ARCH=%~1
shift


:: Windows Driver Kit build number

set GST_WINDDK_BUILD=7600.16385.1


:: Check for spaces in the current directory path

cd | find " " >NUL:

if %ERRORLEVEL% == 0 (
	echo BuildDriver.cmd: error: MS Build does not support building of projects stored in a path containing spaces. >&2
	exit /B 1
)


:: Build options

set GST_C_DEFINES=-D_WIN32 -DGST_WINDOWS_DRIVER
set GST_C_FLAGS=-nologo -I..
set GST_C_WARNING_LEVEL=-W4
set GST_C_DISABLED_WARNINGS=-wd4057 -wd4100 -wd4127 -wd4152 -wd4201 -wd4701 -wd4702 -wd4706
set GST_LIBRARIAN_FLAGS=-nologo
set GST_LINKER_FLAGS=-nologo
set GST_TEST_SIGN=0


:: Windows Driver Kit root

REM echo "[*] WDK Path : %GST_WINDDK_ROOT%\bin\setenv.bat"

set GST_WINDDK_ROOT=%SYSTEMDRIVE%\WinDDK\%GST_WINDDK_BUILD%
if exist "%GST_WINDDK_ROOT%\bin\setenv.bat" goto ddk_found

set GST_WINDDK_ROOT=%WINDDK_ROOT%\%GST_WINDDK_BUILD%
if exist "%GST_WINDDK_ROOT%\bin\setenv.bat" goto ddk_found

set GST_WINDDK_ROOT=%WINDDK_ROOT%
if exist "%GST_WINDDK_ROOT%\bin\setenv.bat" goto ddk_found

echo BuildDriver.cmd: error: Windows Driver Development Kit not found in the default directory. Set WINDDK_ROOT environment variable to point to your Windows DDK installation directory. >&2
exit /B 1

:ddk_found

:: Create the symlink to allow the use of the WDDK in Program Files.
REM set GST_TMP_SYMLINK=C:\TmpPath
REM mklink /J %GST_TMP_SYMLINK% "%WINDDK_ROOT%"
REM set GST_WINDDK_ROOT=%GST_TMP_SYMLINK%

:: CPU architecture

if "%GST_ARG_ARCH%"=="-x64" (
	set GST_BUILD_ARCH=x64 WNET
	set GST_BUILD_ARCH_DIR=amd64
	set GST_ARCH=x64
	set GST_ARCH_SUFFIX=-x64
	set GST_C_DISABLED_WARNINGS=%GST_C_DISABLED_WARNINGS% -wd4328 -wd4366
	set GST_LINKER_FLAGS=%GST_LINKER_FLAGS% -LTCG
	if defined GST_KERNEL_TEST_CERTIFICATE_NAME set GST_TEST_SIGN=1
) else (
	set GST_BUILD_ARCH=WXP
	set GST_BUILD_ARCH_DIR=i386
	set GST_ARCH=x86
	set GST_ARCH_SUFFIX=
)


:: Build type

if "%GST_ARG_TYPE%"=="-debug" (
	set GST_BUILD_TYPE=chk
	set GST_C_DEFINES=%GST_C_DEFINES% -DDEBUG -D_DEBUG
	set GST_BUILD_ALT_DIR=_driver_debug
	set GST_COPY_DIR="..\Debug"
) else (
	set GST_BUILD_TYPE=fre
	set GST_BUILD_ALT_DIR=_driver_release
	set GST_COPY_DIR="..\Release"
	set GST_TEST_SIGN=0
)


:: WDK environment

pushd .
call %GST_WINDDK_ROOT%\bin\setenv.bat %GST_WINDDK_ROOT% %GST_BUILD_TYPE% %GST_BUILD_ARCH% no_oacr || exit /B %errorlevel%
popd


:: At the end, remove the symlink.
REM rmdir %GST_TMP_SYMLINK%


:: Build

if "%GST_ARG_CMD%"=="-rebuild" (set GST_BUILD_OPTS=-c -Z)

pushd .
:build_dirs

	if "%~1"=="" goto done
	cd /D "%~1" || exit /B %errorlevel%

	if "%GST_ARG_CMD%"=="-clean" (
		rd /s /q obj%GST_BUILD_ALT_DIR%\%GST_BUILD_ARCH_DIR% 2>NUL:
		rd /q obj%GST_BUILD_ALT_DIR% 2>NUL:
	) else (

		set USER_C_FLAGS=%GST_C_FLAGS% %GST_C_DISABLED_WARNINGS% -FAcs -Fa%~1\obj%GST_BUILD_ALT_DIR%\%GST_BUILD_ARCH_DIR%\
		set MSC_WARNING_LEVEL=%GST_C_WARNING_LEVEL%
		set C_DEFINES=%GST_C_DEFINES%
		set RCOPTIONS=/I %MFC_INC_PATH%
		set LIBRARIAN_FLAGS=%GST_LIBRARIAN_FLAGS%
		set LINKER_FLAGS=%GST_LINKER_FLAGS%
		set BUILD_ALT_DIR=%GST_BUILD_ALT_DIR%

		build %GST_BUILD_OPTS% -w -nmake /S -nmake /C 2>build_errors.log 1>&2
		
		if errorlevel 1 (
			type build_errors.log
			type build_errors_asm.log 2>NUL:
			exit /B 1
		)
		del /q build_errors.log build_errors_asm.log build%BUILD_ALT_DIR%.* 2>NUL:
	)

	shift
	
goto build_dirs
:done
popd


if "%GST_ARG_CMD%"=="-clean" exit /B 0

md "%GST_COPY_DIR%\Setup Files" >NUL: 2>NUL:
copy /B /Y obj%GST_BUILD_ALT_DIR%\%GST_BUILD_ARCH_DIR%\gostcrypt.sys "%GST_COPY_DIR%\Setup Files\gostcrypt%GST_ARCH_SUFFIX%.sys" >NUL:

if errorlevel 1 (
	echo BuildDriver.cmd: error: Cannot copy target. >&2
	exit /B 1
)

if %GST_TEST_SIGN% equ 1 (
	signtool sign /s "%GST_KERNEL_TEST_CERTIFICATE_STORE%" /n "%GST_KERNEL_TEST_CERTIFICATE_NAME%" "%GST_COPY_DIR%\Setup Files\gostcrypt%GST_ARCH_SUFFIX%.sys" >NUL:

	if errorlevel 1 (
		echo BuildDriver.cmd: error: Cannot test-sign target. >&2
		exit /B 1
	)
)


exit /B 0
