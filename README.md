# windows-stylus-block-input

A windows service that blocks other user input while a pen is used.

## Prerequesites

- [MinGW](https://www.mingw-w64.org/) + [make](https://www.gnu.org/software/make/)  
  Best installed with [Chocolatey](https://chocolatey.org/): `choco install mingw make -y`
- [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/) -> Windows App Certification Kit  
  Make sure you add it to your PATH, it's normally in "C:\Program Files (x86)\Windows Kits\10\App Certification Kit\"
- [WiX Toolset](https://wixtoolset.org/releases/) (to create the installer)  
  This should also be added to your PATH: `%wix%\bin`

## Certificates

Note that in order for the program to receive global input messages, it must be digitally signed with a trusted certificate for Windows to grant it UIAccess.

There's already a nice pregenerated certificate in the repository, but if you don't trust it, you can run `make cert-regen` to generate your own.

## Building

### Building the executable

Just type `make` and the program will automatically be compiled, signed and saved into a new `build` directory.

If you want to execute the program directly, you'll have to run `make uiaccess` as administrator first and potentially restart your computer. This will try to install the certificate and set a registry entry disabling `EnableSecureUIAPaths`, because Windows by default doesn't grant UIAccess to programs in insecure locations.

### Building the installer

You can skip the previous step, if you just run `make installer` to build a standalone `.msi` package (into `installer/`).

After installing the package, the app will automatically start in the background everytime you login until you uninstall it again.
