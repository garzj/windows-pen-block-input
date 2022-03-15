# windows-stylus-block-input

A windows app that blocks other user inputs while a stylus pen is used.

## Prerequesites

- MinGW with make (best installed with Chocolatey)
- [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/) -> Windows App Certification Kit  
  Make sure you add it to your PATH, it's normally in "C:\Program Files (x86)\Windows Kits\10\App Certification Kit\"
- [WiX Toolset](https://wixtoolset.org/releases/) (to create an installer)  
  This should also be added to your PATH: `%wix%\bin`

## Usage

Just type `make` and the program will be compiled and saved into a `build` directory.

Note that in order for the program to receive global input messages, it has to be granted UI access, which Windows doesn't do until the following creteria are met:

- The manifest requests an appropriate execution level (administrator)  
  That's fine.
- The application is installed in a secure location  
  This one can be disabled in the Windows Registry.
- The application is signed with a trusted digital signature  
  This one can be disabled - but at least on my machine, that just didn't work. That's why a new certificate will be generated and installed on the first compilation.

TLDR: Make sure to run `make` as administrator as it will try to install a self signed certificate and change a registry setting.

## Standalone distribution

If you want to send this program to other PCs, you'll need the following three files:

- `uiaccess.reg`: Just run this script
- `build/testcert.cer`: Install this certificate by opening it into Current User -> Automatically select the certificate store
- `build/stylus-block-input.exe`: Now start the program and it should just work in the background :)

Kill the program with `taskkill /F /IM stylus-block-input.exe`
