name = stylus-block-input

rwildcard = $(wildcard $(addsuffix $2, $1)) $(foreach d,$(wildcard $(addsuffix *, $1)),$(call rwildcard,$d/,$2))
winpath = $(subst /,\,$1)
ifeq ($(OS),Windows_NT)
	mkdir = if not exist $1 mkdir $1
else
	mkdir = mkdir -p $1
endif

build_dir = build

binary_name = $(build_dir)/$(name)
binary = $(binary_name).exe

resources = resources.rc
resources_obj = $(build_dir)/resources.o

cert_key = $(build_dir)/cert_key
cert_cer = $(build_dir)/testcert.cer
cert_pfx = $(build_dir)/testcert.pfx
cert_params = $(cert_key) $(cert_cer) $(cert_pfx)

cpp_files = $(call rwildcard,src/,*.cpp)

gcc_args = -lgdi32 -static-libgcc -static-libstdc++ \
	-Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive
ifneq ($(MAKECMDGOALS),dev)
	gcc_args += -mwindows
endif

all: build

build: build_dir $(cpp_files)
	windres $(resources) $(resources_obj)
	g++ \
		-o $(binary_name) $(cpp_files) $(resources_obj) \
		$(gcc_args)
	make enable-uiaccess

run:
	$(call winpath,$(binary))

dev: all run

watch:
	nodemon --watch ./src/** --ext cpp,hpp,c,h --exec make dev

build_dir:
	$(call mkdir,$(build_dir))

enable-uiaccess: sign
	reg add HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\System /v EnableSecureUIAPaths /t REG_DWORD /d 0 /f

add-cert: build_dir
	powershell -c \
		"Set-ExecutionPolicy Unrestricted; \
		& .\certgen.ps1 $(call winpath,$(cert_params))"
	certutil -user -addstore Root $(cert_cer)

sign:
	if not exist $(cert_key) make add-cert
	cmd /v /c "\
		set /p certKey=<$(call winpath,$(cert_key)) && \
		signtool sign /f $(cert_pfx) \
			/t http://timestamp.comodoca.com/authenticode \
			/p !certKey! \
			/a /fd SHA256 /v $(binary)"

del-cert:
	certdel.bat Root $(cert_cer)
	del $(call winpath,$(cert_params))

dll-dump:
	objdump -p $(binary) | findstr "DLL Name:"

clean: del-cert
	rmdir /s /q $(build_dir)
