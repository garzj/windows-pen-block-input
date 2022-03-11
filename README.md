# windows-stylus-block-input

A windows app that blocks other user inputs while a stylus pen is used.

## Prerequesites

- MinGW with make (best installed with Chocolatey)
- [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/) -> Windows App Certification Kit  
  Make sure you add it to your path, it's normally in "C:\Program Files (x86)\Windows Kits\10\App Certification Kit\"

## Usage

Just type `make` and the program will be compiled and saved into a `build` directory.

Note that in order for the program to receive global input messages, it has to be granted UI access, which Windows doesn't do until the following creteria are met:

- The manifest requests an appropriate execution level (administrator)  
  That's fine.
- The application is installed in a secure location  
  This one can be disabled within the Windows Group Policy Editor.  
  Go into Computer Configuration > Windows Settings > Security Settings > Local Policies > Security Options and set `User Account Control: Only elevate UIAccess applications that are installed in secure locations` to `Disabled`
- The application is signed with a trusted digital signature  
  This one can also be disabled in the Group Policy Editor - but at least on my machine, that just didn't work. That's why a new certificate will be generated and installed on the first compilation.
