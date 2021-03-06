# Hyper-V VMBusPipe Reversing

Many Hyper-V host-to-guest extensions communicate over a special named pipe transport that goes over the VMBus, called the VMBus pipe. One such feature is the Hyper-V enhanced session mode that opens a named pipe transport to the guest VM to use a special listener inside the RDP server, making it possible to connect without TCP networking available.

This reversing project was done in 2014 as an effort to find ways to create third-party VMBus extensions. This is fortunately no longer necessary because Microsoft came out with [Hyper-V sockets](https://docs.microsoft.com/en-us/virtualization/hyper-v-on-windows/user-guide/make-integration-service) years later exactly for this type of use case, opening up the VMBus as a transport for anyone wanting to leverage it.

Even with Hyper-V sockets, this project can serve as the basis for new work meant to intercept and inspect traffic happening over the VMBus pipe for extensions that use it. The VMBusPipe library contains the core API definitions and structures I reversed. The VMBusPipeStub project is meant to be compiled as vmbuspipe.dll and vmbuspiper.dll exposing the same API as the original libraries. Those stub libraries load the original unmodified libraries and forward all the API calls while logging them. The easiest way to install the replacement DLLs is to use a VM with nested virtualization, and copy the modified DLL by mounting the VHD while the VM is offline. Old test log captures can be found in the logs directory of this repository. As for the VMBusPipeTest project, it is an executable that attempts using the APIs I reversed, but from what I recall it had limited success being called outside of the Hyper-V processes. If there's a chance of making it work, it's probably through psexec.

Generate and build the project using cmake:

```
mkdir build && cd build
cmake "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Release
```

In the build directory, look under bin\Release to find the vmbuspipe.dll and vmbuspiper.dll stub DLLs. These stub DLLs export the same API as the original DLLs, but instead of implementing it they load the real DLL (rename them both to vmbuspipe_.dll and vmbuspiper_.dll) and forward the API calls while logging them to a text file. In order to replace the VMBusPipe DLLs, it is recommended to create a virtual machine with nested virtualization and Hyper-V installed, and simply mount the VHD while the VM is offline to modify the files. The logs files are created under "C:\Windows\Temp" by default and begin with "VmBusPipeStub".

I am publishing these sources years after they've been taking the dust as a private reversing project, in hope of helping security researchers and reversers in their work. Go and have fun with Hyper-V and the VMBusPipe, and let me know if you manage to make anything useful out of this!
