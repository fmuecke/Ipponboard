## Linux (WSL) Gamepad support

For the Gamepad to work with WSL you need to do the following:

1. Install `usbipd-win`

   winget install usbipd-win

2. List devices (as administrator in PowerShell)

   usbipd list

   PS C:\WINDOWS\system32> usbipd list
   Connected:
   BUSID&nbsp; VID:PID&nbsp; DEVICE&nbsp; STATE
   1-9 &nbsp; 046d:c01e &nbsp;USB Input Device&nbsp; Not shared
   1-12 &nbsp;26ce:01a2&nbsp; USB Input Device&nbsp; Not shared
   8-1 &nbsp;0f30:0112 &nbsp;USB Input Device &nbsp;<--- THAT'S THE ONE&nbsp; Not shared
   8-2 &nbsp;046d:085c&nbsp; c922 Pro Stream Webcam, C922 Pro Stream Webcam&nbsp; Not shared
   8-3 &nbsp;046a:0023 &nbsp;USB Input Device &nbsp;Not shared

3. Unplug and replug to see which device is your gamepad

4. Bind the device with the respective busid

   usbipd bind --busid <id>

5. Attach device to WSL

   PS C:\WINDOWS\system32> usbipd attach --wsl --busid <id>

   usbipd: info: Using WSL distribution 'Ubuntu' to attach; the device will be available in all WSL 2 distributions.
   usbipd: info: Loading vhci_hcd module.
   usbipd: info: Detected networking mode 'nat'.
   usbipd: info: Using IP address xxx.xxx.x.x to reach the host.

After that `usbipd list` should show the device as _Attached_.

6. Check device in WSL

   fmuecke@myhost:~/ipponboard$ lsusb

   Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
   Bus 001 Device 002: ID 0f30:0112 Jess Technology Co., Ltd Dual Analog Pad <---- THIS
   Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub

7. Configure WSL input

```bash
   # check if input group permission is required
   ls -l /dev/input/js*

   # if yes, add user to input group (running as root is not recommended)
   sudo usermod -aG input $USER
   newgrp input

   # test gamepad with evtest
   sudo apt install evtest
   sudo evtest
   ls -l /dev/input/event0
```

**Detach the device via PowerShell:**

    usbipd wsl detach --busid <id>

See https://learn.microsoft.com/en-us/windows/wsl/connect-usb#attach-a-usb-device
