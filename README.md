# Nintendo_Switch_ProCon_Emulator

BareMetal, up to the point where switch can recognize it as a controller.  
Need to enable pro controller wired mode.  
Some lib files supplied by stm32cubemx have been modified.  

The whole thing is basically a replay attack.  
The “HID” report structure is header + button + joystick + IMU data. Look at reference and capture folder for more hints.  

https://github.com/shinyquagsire23/HID-Joy-Con-Whispering and https://github.com/FrotBot/SwitchProConLinuxUSB/blob/master/src/procon.hpp are helpful references.  
