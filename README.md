# Linkboy
Cross-platform Gameboy Emulator written in C++ using SFML.    
Tested on Windows and Linux

<img src="https://cloud.githubusercontent.com/assets/9373330/19225262/91d9b958-8e4d-11e6-8215-8755c3419efa.png" width="280" height="252"/>
<img src="https://cloud.githubusercontent.com/assets/9373330/19225269/9aa603e8-8e4d-11e6-8174-28bcced92611.png" width="280" height="252"/>
<img src="https://cloud.githubusercontent.com/assets/9373330/19225272/9d842798-8e4d-11e6-8da6-782e22e98ab2.png" width="280" height="252"/>    

<img src="https://cloud.githubusercontent.com/assets/9373330/19297477/3c4ead46-8ffa-11e6-815f-f1eca8caeb48.png" width="280" height="252"/>
<img src="https://cloud.githubusercontent.com/assets/9373330/19297476/388f558e-8ffa-11e6-9acd-ab64ebf5f8af.png" width="280" height="252"/>
<img src="https://cloud.githubusercontent.com/assets/9373330/19297602/74a99f42-8ffb-11e6-906d-c74a240e25a2.png" width="280" height="252"/>

## Network Multiplayer
Multiplayer is achieved through emulating the Gameboy's linkcable accessory. This is done using SFML's networking to setup UDP communication from clients to a server. The server application, which is included, manages communication from multiple linkboy clients and relays the messages between them. A server is used in hopes of later emulating a connection between 4 or more Gameboys as a few games do support.  

<img src="https://cloud.githubusercontent.com/assets/9373330/21093402/773a2ff2-c006-11e6-90e6-a84ae4a56a7d.PNG" width="460" height="252"/>   
<img src="https://cloud.githubusercontent.com/assets/9373330/21093198/671ebf3a-c005-11e6-9c21-fcb1563ed063.PNG" width="460" height="252"/>

Above is Pokemon Blue trading & Tetris 2-player

## Additional Features   
   Save/Load States    
   Increase/Decrease Speed   
   Color Palettes   
   
###Color Palettes:

<img src="https://cloud.githubusercontent.com/assets/9373330/19225263/9285c6d0-8e4d-11e6-97b0-f9d0ac09c654.png" width="260" height="234" />
<img src="https://cloud.githubusercontent.com/assets/9373330/19225329/4f23ceda-8e4f-11e6-9c14-f0a302c72917.png" width="260" height="234" />   

<img src="https://cloud.githubusercontent.com/assets/9373330/19225264/936de79e-8e4d-11e6-89ea-e58c17c2d868.png" width="260" height="234" />
   <img src="https://cloud.githubusercontent.com/assets/9373330/19225265/94bf36ac-8e4d-11e6-9d99-a564afbd2107.png" width="260" height="234" />   


Colors are changed from within the GUI


##Controls
   
&nbsp;&nbsp;Z - B Button   
&nbsp;&nbsp;X - A Button     
&nbsp;&nbsp;Enter - Start Button   
&nbsp;&nbsp;RShift - Select Button   

&nbsp;&nbsp;Arrow Keys - D-Pad

&nbsp;&nbsp;'-' - Slow Down      
&nbsp;&nbsp;'+' - Speed Up   

&nbsp;&nbsp;home - Load State   
&nbsp;&nbsp;end - Save State   

## Currently Supports: 
&nbsp;&nbsp;no MBC   
&nbsp;&nbsp;MBC1   
&nbsp;&nbsp;MBC2
&nbsp;&nbsp;MBC3 (RTC not inplace)   

## Setup
Installation and setup for SFML can be found on their website:   
http://www.sfml-dev.org/tutorials/2.4/   
dirent.h for windows is used to maintian cross-platform with linux:   
https://github.com/tronkko/dirent   
On windows Visual Studio 2015 is used for build   
On linux a makefile is provided   
   
