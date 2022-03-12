# Helpers
rwildcard = $(wildcard $(addsuffix $2, $1)) $(foreach d,$(wildcard $(addsuffix *, $1)),$(call rwildcard,$d/,$2))
winpath = $(subst /,\,$1)
mkdir = if not exist $(call winpath,$1) mkdir $(call winpath,$1)

# Main
name = stylus-block-input

all: build

# Build
build_dir = ./build

build_dir:
	$(call mkdir,$(build_dir))

binary_base = $(build_dir)/$(name)
binary = $(binary_base).exe

resources = resources.rc
resources_obj = $(build_dir)/resources.o

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
	make uiaccess sign

# UIAccess
uiaccess:
	reg import uiaccess.reg

# Certificate / Code signing
cert_key = $(build_dir)/cert_key
cert_cer = $(build_dir)/testcert.cer
cert_pfx = $(build_dir)/testcert.pfx
cert_params = $(cert_key) $(cert_cer) $(cert_pfx)

cert-create: build_dir
	if not exist $(cert_key) \
		powershell -c \
		"Set-ExecutionPolicy Unrestricted; \
		& .\certgen.ps1 $(call winpath,$(cert_params))"

cert-install: cert-create
	certutil -user -addstore Root $(cert_cer)

cert-uninstall:
	certdel.bat Root $(cert_cer)

cert-delete: cert-uninstall
	del /f $(call winpath,$(cert_params))

sign: cert-install
	cmd /v /c "\
		set /p certKey=<$(call winpath,$(cert_key)) && \
		signtool sign /f $(cert_pfx) \
			/t http://timestamp.comodoca.com/authenticode \
			/p !certKey! \
			/a /fd SHA256 /v $(binary)"

# DLL dump
dll-dump:
	objdump -p $(binary) | findstr "DLL Name:"

# Clean
clean: cert-delete
	rmdir /s /q $(call winpath,$(build_dir))
