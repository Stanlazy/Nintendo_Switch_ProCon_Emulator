# Nintendo_Switch_ProCon_Emulator

After refactoring, now all important code are in /core/src/ns_con_device.c and its header, while still depends on a clean copy of stm32 sub device lib.  

The example project is half of my implementation of steam controller to switch pro controller.  

The whole thing is basically a replay attack.   

The “HID” report structure is header + button + joystick + IMU data. Look at capture folder and https://github.com/FrotBot/SwitchProConLinuxUSB/blob/master/src/procon.hpp for more hints.  

