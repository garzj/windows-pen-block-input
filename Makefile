# Helpers
rwildcard = $(wildcard $(addsuffix $2, $1)) $(foreach d,$(wildcard $(addsuffix *, $1)),$(call rwildcard,$d/,$2))
mkdir = if not exist "$1" mkdir "$1"

# Main
name = StylusBlockInput

all: build

# Build
build_dir = .\build

build_dir:
	$(call mkdir,$(build_dir))

binary_base = $(build_dir)\$(name)
binary = $(binary_base).exe

resources = resources.rc
resources_obj = $(build_dir)\resources.o

cpp_files = $(call rwildcard,src/,*.cpp)

gcc_args = -lgdi32 -static-libgcc -static-libstdc++ \
	-Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
ifneq ($(MAKECMDGOALS),dev)
	gcc_args += -mwindows
endif

build: build_dir $(cpp_files)
	windres $(resources) $(resources_obj)
	g++ \
		-o $(binary_base) $(cpp_files) $(resources_obj) \
		$(gcc_args)
	make sign

dev: build

# Allow UIAccess from insecure paths
uiaccess:
	reg add HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System /v EnableSecureUIAPaths /t REG_DWORD /d 00000000 /f \
		|| type nul

# Certificate / Code signing
cert_key = .\cert\.certkey
cert_cer = .\cert\cert.cer
cert_pfx = .\cert\cert.pfx
cert_params = $(cert_key) $(cert_cer) $(cert_pfx)

cert-create: build_dir
	if not exist $(cert_cer) \
		powershell -c \
		"Set-ExecutionPolicy -ExecutionPolicy Unrestricted -Scope Process; \
		& .\cert\certgen.ps1 $(cert_params)"

cert-install: cert-create
	certutil -user -addstore Root $(cert_cer)

cert-uninstall:
	.\cert\certdel.bat Root $(cert_cer)

cert-delete: cert-uninstall
	del /f $(cert_params)

cert-regen: cert-delete cert-create

sign: cert-create
	cmd /v /c "\
		set /p certKey=<$(cert_key) && \
		signtool sign /f $(cert_pfx) \
			/t http://timestamp.comodoca.com/authenticode \
			/p !certKey! \
			/a /fd SHA256 /v $(binary)"

# Installer
installer: build_dir build
	candle -ext WixIISExtension installer\StylusBlockInput.wxs -o "installer\\"
	light -ext WixIISExtension installer\StylusBlockInput.wixobj -o installer\StylusBlockInput-Setup-x64.msi

# DLL dump
dll-dump:
	objdump -p $(binary) | findstr "DLL Name:"

# Clean
clean: cert-delete
	rmdir /s /q $(build_dir)
