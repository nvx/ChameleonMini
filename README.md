# ChameleonMini 

This is NOT the official repository of ChameleonMini, a freely programmable portable tool for NFC security analysis that can emulate and clone contactless cards, read RFID tags, and sniff/log RF data. This repository brings support for the Chameleon Tiny.

Our Project is based on the open-source project [ChameleonMini RevG](/../../../../emsec/ChameleonMini) by Kasper & Oswald. They also have their own [Webshop](https://shop.kasper.it/).

- Information:
   - Chameleon Tiny Site: **[Here](http://chameleontiny.com/)**
   - Chameleon BLE API: [**Here**](/../../../../RfidResearchGroup/ChameleonBLEAPI/blob/master/code.rar)
   - Doxygen: **[Here](/../../../../doxygen/doxygen)**
      - For viewing some of the documentation
      - Binaries: **[Here](https://www.doxygen.nl/download.html)**
      - Install: **[Here](https://www.doxygen.nl/manual/install.html)**
   - Documentation (RfidResearchgroup): 
      - Source: **[Here](https://github.com/RfidResearchGroup/ChameleonMini/tree/proxgrind/Doc/Doxygen/html)**
         - Need to manually download and view
         - Open [Doc/Doxyfile](Doc/Doxyfile) and then in application `Show HTML Output`
   - Documentation (emsec): 
      - Doxygen: **[Here](https://rawgit.com/emsec/ChameleonMini/master/Doc/Doxygen/html)**
      - Source: **[Here](/../../../../emsec/ChameleonMini/tree/master/Doc)**
- Tools:
   - Phone APP:
      - Android: **[Chameleon](https://play.google.com/store/apps/details?id=com.proxgrind.chameleon)**
         - The Android APP is currently **NOT** compatible with the official firmware
      - IOS: **[ChameleonTiny Manager](https://apps.apple.com/us/app/chameleontiny-manager/id1601341565)**
         - The IOS APP is currently **NOT** compatible with the official firmware
         - Credit to **[bettse](/../../../../bettse)**
      - Password: e4g1
   - Optional GUI (Winodws Only):
      - Windows: **[Here](http://www.icesql.se/download/ChameleonMiniGUI/publish.htm)**
      - Source: **[Here](/../../../../iceman1001/ChameleonMini-rebootedGUI)**
   - Chameleon Firmware:
      - Compile the latest firmware: **[Here](/Firmware)** 
         - Or use the precompiled: **[Here](/Firmware/Chameleon-Mini/Latest)**
      - The Chameleon Mini REV.G version uses the same firmware for both the Mini with Bluetooth version and Tiny
      - The hardware design of the RF part is exactly the same
      - The hardware is also compatible with the official firmware of the [KAOS brothers](/../../../../emsec/ChameleonMini) 
   - Terminal APP:
      - Putty or similar APP
   - DFU driver:
      - The DFU driver comes from ATMEL's official LIBUSB driver library 
      - You can download it yourself or from the repo: **[Here](/Drivers)**
   - DFU Programmer:
      - Zip file from SourceForge: **[Here](https://sourceforge.net/projects/dfu-programmer/files/dfu-programmer/0.7.2/dfu-programmer-win-0.7.2.zip/download)**  
- Resellers:
   - Our Asia & Oceania Reseller: **[Here](https://sneaktechnology.com/)**
   - Our Europe Reseller: **[Here](https://lab401.com/)**
   - Our US Reseller: **[Here](https://hackerwarehouse.com/)**
- Repository Structure:
   - **Doc:** A folder for doxygen documentation
   - **Drivers:** Chameleon drivers for Windows and Linux
   - **Dumps:** Dumps of different smartcards
   - **Hardware:** The layout and schematics of the PCB
   - **Firmware:** The complete firmware including a modified Atmel DFU bootloader and LUFA
   - **Software:** Contains a python tool for an easy configuration (and more) of the ChameleonMini
      - **WARNING:** This is currently under construction
   - **RevE:** Contains the entire contents of the discontinued RevE repository
   - **RevE-light:** Contains our development files for the RevE-light
      - **WARNING:** This is currently not supported / not functional

## A. Upgrade to the latest firmware

### A1. Upgrade Chameleon AVR Firmware
---

#### 1. Obtain and Prepare Tools:

   - Optional GUI (Windows Only)
   - Terminal APP
   - Firmware
   - DFU Driver
   - DFU Programmer

#### 2. Create a Working Directory

   1. Create a directory to work from
   2. Place the Chameleon firmware in the directory
   3. Unzip the DFU Programmer folder to the working directory
   4. If you are using the auto install on Windows, place [ChameleonFirmwareUpgrade.bat](/Firmware/Chameleon-Mini/ChameleonFirmwareUpgrade.bat) in the working directory as well

   - Further information:
      - **[Here](/Doc/Doxygen/html/_page__getting_started.html)**
      - **[emsec](/../../../../emsec/ChameleonMini/blob/master/Doc/DoxygenPages/GettingStarted.txt)**

#### 3. Enter the Chameleon Direct Firmware Update (DFU) mode:

   <details><summary>Method 1: GUI (Windows Only)</summary>

   - Mini or Tiny:
      1. If it is a Chameleon Mini, ensure it is in the `OFF` position
      2. Connect it via USB cable to your computer
         - On the Mini, it may be a good idea to remove the battery before putting the device into `DFU` mode
      3. Under the `Settings` tab, Send the `Upgrade` command while the Chameleon is connected
         - This only puts the Chameleon in `DFU` mode
         - It does **NOT** start installing the firmware at all
      4. Chameleon should now be in `DFU` mode 
    
   - Further information:
      - **[emsec](/../../../../emsec/ChameleonMini/blob/master/Doc/DoxygenPages/GettingStarted.txt)**
      - **[Iceman GUI](https://github.com/iceman1001/ChameleonMini-rebooted/wiki/Getting-started)**

   </details>

   <details><summary>Method 2: CLI</summary>

   - Mini:
      1. With the Chameleon in the `OFF` state, press and hold the black (Rev E) or yellow (Rev G) button near the USB while inserting it into the USB port
         - It may be a good idea to remove the battery before putting the device into `DFU` mode
      2. Use your favorite terminal application to connect to it. Type `upgrade` and hit `Enter`
         - This only puts the Chameleon in `DFU` mode
         - It does **NOT** start installing the firmware at all
      3. Chameleon should now be in `DFU` mode

   - Tiny:
      1. Press and hold the B button while inserting it into the USB port
      2. Use your favorite terminal application to connect to it. Type `upgrade` and hit `Enter`
         - This only puts the Chameleon in `DFU` mode
         - It does **NOT** start installing the firmware at all
      3. Chameleon should now be in `DFU` mode

   - Further information:
         - **[Here](/Doc/Doxygen/html/_page__getting_started.html)**
         - **[emsec](/../../../../emsec/ChameleonMini/blob/master/Doc/DoxygenPages/GettingStarted.txt)**

   </details>
      
#### 4. Install DFU Driver:
      
   <details><summary>Windows</summary>

   - Mini or Tiny:
      1. Download the drivers from the repo
      2. Ensure you have connected your Chameleon and that it is in `DFU` mode
      3. You should have an unknow USB device in device manager
      4. Update the device with the driver files from the repo
      5. You should now have an 'ATxmega*' device
   </details>
   
#### 5. Start Firmware Upgrade:

   <details><summary>Method 1: Automatic (Windows Only)</summary>

   - Mini or Tiny:
      1. If you haven't already, place [ChameleonFirmwareUpgrade.bat](/Firmware/Chameleon-Mini/ChameleonFirmwareUpgrade.bat) in your working directory with your firmware and other tools
      2. Ensure Chameleon is in `DFU` mode
      3. Run the `ChameleonFirmwareUpgrade.bat` file **AS ADMIN** to automatically start the firmware upgrade
      4. It usually takes 2 - 5 seconds
      5. After the progress bar is complete, the firmware upgrade is complete

   </details>

   <details><summary>Method 2: Manual</summary>
   
- Mini or Tiny:
   - Guides:
      - Icemans Guide (Windows): **[Here](/../../../../iceman1001/ChameleonMini-rebooted/wiki/Getting-started#get-ready-to-flash-bootloader-mode)** and **[Here](/../../../../iceman1001/ChameleonMini-rebooted/wiki/Getting-started#get-ready-to-flash-bootloader-mode)**
      - Icemans Guide (Linux): **[Here](/../../../../iceman1001/ChameleonMini-rebooted/wiki/Compiling-Linux-(Unix))**
      - Icemans Guide (Mac): **[Here](/../../../../iceman1001/ChameleonMini-rebooted/wiki/Compiling-OSX)**
   - Note:
      - Replace any relevant files or other parts with the resources in this repo

      </details>

### A2. Upgrade (OTA) Bluetooth Firmware (Chameleon Mini only)
---

#### 1. Obtain and Prepare Tools:

   - Optional GUI
   - Terminal APP
   - Phone APP
   - Firmware
   
#### 2. Use APP to Connect to Chameleon Using Bluetooth:

   1. Press any button on the Mini and the white battery light should come `ON`
      - You can now connect via Bluetooth
   2. Open the APP again and click `Connect` to automatically connect the Chameleon

#### 3. Start Bluetooth Firmware Upgrade:

   1. In the `Device Information` column, press `BLE CMD Version` 5 times
   2. On the `OTA upgrade` page, click `Auto Upgrade`
   3. The APP will immediately start to upgrade to the latest Bluetooth firmware that comes with it
   4. The APP will automatically exit after the upgrade is complete

## B. Crash Course

### B1. Detect Keys and Upload Card files
---

#### 1. Obtain and Prepare Tools:

  1. Tools:
      - Optional GUI
      - Terminal APP
      - Phone APP

   2. Connect to the Chameleon Mini or Tiny using the APP
      - Connect via USB or Bluetooth

   - Note:
      - Both the Chameleon Mini and Tiny support direct connection to the mobile phone USB port
         - For the Mini, an additional OTG adapter needs to be purchased
         - Tiny uses its own dual-headed TYPE-C data cable to connect directly to TYPE-C mobile phones
      - Chameleon Mini has built-in Bluetooth BLE4.0. Press any button to wake up Bluetooth
      - Turn Bluetooth `ON` on your phone and the APP will automatically connect

#### 2. Use APP to Enable Detection Mode:

   1. After connecting, click on a single card slot and select `MF_DETECTION_1K` or `MF_DETECTION_4K` in the `Card Slot` mode
   2. This card slot will now have the `Detection` mode turned `ON`
   3. Write the original card number in the `UID Card Number` column
      - If you don't know the UID number, you can fill in it at will
   4. Then click the `Clear` button below to clear the last detection record

#### 3. Use APP to Get Keys:

   1. Take the Chameleon to the access control reader and swipe it across 
   2. The key and access traces are recorded by the Chameleon when you swipe
   3. Connect back to the mobile phone and press the `Crack` button
   4. After a few seconds, the APP will automatically solve and list the results, as shown in the figure below:
   
<a href="https://ibb.co/Q6xcVkW"><img src="https://i.ibb.co/Q6xcVkW/Untitled-1.png" alt="Untitled-1" border="0"></a>

   5. The list shows which blocks the read head just visited, and what password was used for each access
   6. Click the `History` button. The APP will automatically list the keys separately and save them for other software to use
   7. If your mobile phone comes with an NFC function, you can put the original key directly on the mobile phone
   8. The APP will automatically use the key in the list to read the entire card, and after it is successful, it will automatically save the entire card data file on the mobile phone

   - Note: 
      - Multiple red LEDs are on at the same time during detection, which means the memory is full, just clear the memory.

#### 4. Use APP to Import Existing Card Files:

   1. Use QQ to send the card data file to the mobile phone QQ
      - Or connect the mobile phone to the computer and transfer the file to any directory on the mobile phone
   2. Open the APP and click the `Dump` column
   3. Click the `Scanner` in the plus sign in the upper right corner
   4. Click the three horizontal line buttons in the upper left corner and select this phone
   5. Select the root directory of the QQ receiving file or the previously copied directory, and click `Allow Access`
   6. All card data files will be automatically scanned into the `Dump` file interface, which can be uploaded or edited at will
   7. Click the card data file in the `Dump` column below, and click `Upload` below to upload to the card slot corresponding to the Chameleon

### B2. Introduction to UID Mode and SAK Mode
---

#### 1. UID Mode:

   1. Click the button `UID Changeable (GEN1a)` in the APP or directly send the command `UIDMODE = 1` to turn it `ON`
      - `UIDMODE = 0` to turn it `OFF`
   2. After the UID mode is turned `ON`, the card simulated by Chameleon will become a GEN1a card
      - Commonly known as a UID card or Chinese magic card
   3. The current card slot takes effect

#### 2. SAK Mode:

   1. Click the `SAK Mode` button in the APP or directly send the command `SAKMODE = 1` to turn it `ON`
      - `SAKMODE = 0` to turn it `OFF`
   2. After the `SAK Mode` is turned `ON`, the card will feedback the real SAK value when it is found
   3. The SAK value is determined by the 0 sector, 0 block, and the position is the position of the sixth byte immediately after the UID number
   4. If the `SAK Mode` is not turned on, the SAK is a fixed value of 08, and 0 blocks of data are ignored
      - This function is useful when special SAK values cannot be used normally after being copied
      - This achieves better compatibility
   5. The current card slot takes effect

### B3. Card Slot Functions
---

#### 1. UID Card Function Class


|Option     | Type     | Length of UID | Memory Size    |
| ------------------- |:-------------------:| -------------------:| -------------------:|
MF_classic_1K 4B/7B|M1 S50 |4 Byte / 7 Byte|1024 byte
MF_classic_4K 4B/7B | M1 S70|4 Byte / 7 Byte|4096 byte|
MF_classic_mini_4B|M1 mini S20|4 Byte / 7 Byte|320 byte|
MF_ultralight_C|M0 ultralight|7 Byte|192 byte|
MF_ultralight_EV1_80B|M0 ultralight|7 Byte|80 byte|
MF_ultralight_EV1_164B|M0 ultralight|7 Byte|164 byte|
Vicinity|-|8 Byte|8192  byte|
SL2S2002|-|8 Byte|8192 byte|
TITAGITSTANDARD|-|8 Byte|44  byte|
EM4233|-|8 Byte|208 byte|

#### 2. Cracking and Card Reading Functions


|Option     | Ability    | Cracking Type | APP Supported    |
| ------------------- |:-------------------:| -------------------:| -------------------:|
MF_DETECTION_1K|Detecting reader to obtain keys|MFKEY32V2|List results directly|
MF_DETECTION_4K|Detecting reader to obtain keys|MFKEY32V2|List results directly|
ISO14443A_READER|Reader Mode|-|Display UID|
ISO14443A_SNIFF|Sniffing|-|Not supported|
ISO15693_SNIFF|Sniffing|-|Supported|

#### 3. Custom Button Functions


|Option names    | Description    | 
| ------------------- |:-------------------:| 
NONE|Set this button to have no function|
UID_RANDOM|Randomly generated UID number in the current card slot after pressing|
UID_LEFT_INCREMENT|After pressing, the highest byte of the UID number plus one (hexadecimal)|
UID_RIGHT_INCREMENT|After pressing the lowest byte of the UID number plus one (hexadecimal)|
UID_LEFT_DECREMENT|After pressing, the highest byte of the UID number is reduced by one (hexadecimal)
UID_RIGHT_DECREMENT|After pressing, the lowest byte of the UID number is reduced by one (hexadecimal)|
CYCLE_SETTINGS|Card slot number sequence will increase after pressing|
CYCLE_SETTINGS_DEC|Card slot number sequence decreases after pressing|
STORE_MEM|Immediately after pressing, the current card data in the temporary buffer is overwritten into the memory|
RECALL_MEM|Immediately after pressing, the current card data in the memory is overwritten into the temporary buffer (Can be used to quickly restore card data)|
TOGGLE_FIELD|Click once to turn off the antenna and click again to turn on the antenna function|
STORE_LOG|Write the log data in the temporary cache to the memory, which can be saved even when power is off|
CLEAR_LOG|Clear log data immediately after pressing|
CLONE|Read the UID card number immediately after pressing, continue searching, and simulate immediately after reading the card|
CLONE_MFU|Clones a Mifare Ultralight card that is in the range of the antenna to the current slot, which is then accordingly configured to emulate it|

## C. Hardware & Lights

### C1. All Units
---

#### 1. Charging: 

   - Any time you connect USB, it will automatically start charging
   - The Mini will have a white light while the Tiny will have a red light over the USB port to indicate if it is charging

#### 2. Charging and Standby Mode:

   - Charging method: Plug in the USB at any time and start charging immediately
   - Battery type: LIR2032H replaceable lithium-ion rechargeable battery
   - Charging time: 2 hours @ 0-100%
   - Charging current: 40mA
   - Start-up current: 38mA
   - Card reading current: 65mA
   - Sleep current: 5uA (9uA-MAX)
   - Battery capacity: 70mAh
   - Duration: Swipe the card 3 times a day for 5 seconds each time, and it can be used for one year on a single charge
   - Sleeping time: Fully charged, it can be left for two years when it is turned off and sleeping
   - Port type: 
      - Mini: MicroUSB 
      - Tiny: Type-C

#### 3. Detect Memory Full:

   - If the memory is full during dense flow detection, multiple red LEDs will be abnormally lit

#### 4. Usage Guide for New Functions:

<a href="https://ibb.co/VvcGbg9"><img src="https://i.ibb.co/K9pgB2h/EUd-Gl7w-Uw-AAg-Cf6.jpg" alt="EUd-Gl7w-Uw-AAg-Cf6" border="0"></a><br />

### C2. Chameleon Mini
---

#### 1. Turn on Bluetooth and Check the Battery:

   1. When the power is `OFF`, press any button once to turn `ON` the Bluetooth power, and at the same time, display the current power with a white LED
   2. With the Bluetooth in the `ON` state, click any button to turn `OFF` the Bluetooth power, the power LED goes out, and the system sleeps
   3. Bluetooth will sleep automatically after no operation for 15 seconds
         - Press any button once to shut down immediately

#### 2. Turn On the Chameleon:

   1. With the Bluetooth in the `ON` state, double click any button
      - Or in the `OFF` state, triple click any button times to turn `ON` the Chameleon
   2. The red LED lights up to indicate the slot number
   3. Chameleon will automatically sleep and shut down after no operation for 5 seconds
  
#### 3. Bootloader/DFU Firmware Upgrade Methods:

   - Section A1: **[Here](#a1-upgrade-chameleon-avr-firmware)**

#### 4. Schematics for Chameleon Mini:

   - Chameleon Mini: **[Here](http://chameleontiny.com/downloads/)**

#### 5. Demos

   - [ChameleonMini RevG In Lab Demo / Technical Explanation of Command Line Interface](https://www.dailymotion.com/video/x3lf45p)
   - [Mifare Reader Attack: Sniffing, Cracking, Emulation, Open! LAB401 Academy - CHAMELEON MINI Tutorial](https://www.youtube.com/watch?v=pI7k5b0yhb0)
   - [How to use the new Proxgrind Chameleon Tiny and Mini with the APP](https://www.youtube.com/watch?v=PiSc9WEbeCE)

### C3. Chameleon Tiny
---

#### 1. Turn Chameleon and Bluetooth On:

   1. With the Chameleon in the `OFF` state, press the `A button` once to turn `ON` the Chameleon power
   2. The red LED lights up to indicate the slot number
   3. Chameleon will automatically sleep and shut down after no operation for 5 seconds
      - This can be adjusted in the APP

#### 2. Bootloader/DFU Firmware Upgrade

   - Section A1: **[Here](#2-write-the-firmware)**

#### 3. Schematics:

   - ~~Same as the Mini~~ ?

#### 4. Demos

   - [How to use the new Proxgrind Chameleon Tiny and Mini with the APP](https://www.youtube.com/watch?v=PiSc9WEbeCE)
   - [ChameleonTiny 8 Prox in1 & Sniff - Crack RFID ~ NFC ~ UID](https://www.youtube.com/watch?v=6yITj165tS4)
   - [Chameleontiny pro -Standalone clone of a MIFARE 1K UID](https://www.youtube.com/watch?v=Ce5DlCHyeGY)

## D. Appendix

### D1. Feature Comparison Table for Each Version
---

#### 1. Comparison Table of Specs


|-     | Rev.G Official by KAOS    | Rev.E Old Rdv2.0 by ProxGrind | Rev.G by ProxGrind    |Rev.G  Tiny by ProxGrind |
| ------------------- |:-------------------:| -------------------:| -------------------:|-------------------:
**Simulation**|Good performance, has blind area|Poor compatibility|Perfect performance|no blind area|Perfect performance|no blind area|
**As a reader**|1-2cm for white tag|0cm for keyfob|×|5-6cm for white tag|3-4cm for keyfob|3-5cm for white tag|2-3cm for keyfob|
**Read current**|170mA|×|65mA|60mA|
**BLE nrf52832**|×|×|√|×|
**Li-ion battery**|√|×|√|√|
**Battery indicator**|×|×|√|×|
**Low power sleep**|×|×|√|√|
**RF field wakeup**|×|√|√|√|
**Button wakeup**|×|√|√|√|
**Auto power off**|×|√|√|√|
**Official firmware compatible**|√|√|√|√|
**Replaceable Antenna**|×|×|√|×|
**MFKEY32 crack**|×|√|√|√|
**8 LED for slot**|×|√|√|√|
**Android APP**|×|×|√|√|
**Firmware anti lost**|×|×|√|×|

#### 2. Comparison Table of Analog Card Characteristics


||**Rev.G Official By KAOS**|**Rev.E old RDV2.0 By PROXGRIND**|**Rev.G new RDV2.0 By PROXGRIND** |**M1 white tag**|
| ------------------- |:-------------------:| -------------------:| ------------------- |-------------------:|
**122U r/w full data**|1-2 sector only|Smooth|Smooth|Smooth|
**122U Range**|61mm|41mm|73mm|71mm|
**PM3 r/w full data**|1-2 sector only|Smooth|Smooth|Smooth
**PM3 Range**|57mm|74mm|88mm|89mm|
**Phone NFC r/w full data**|No|Smooth|Smooth|Smooth
**Phone NFC Range**|25mm|18mm|33mm|32mm
**Magic back door** |By default|No|Dual mode|No
**SAK ATQA Support**|No|No|Modifiable|No|

#### 3. Table of New Commands


| Command    | Effect Range    | Description | 
| ------------------- |:-------------------:| -------------------:|
**UIDMODE?**|All slot|Returns the configuration of the all slot|
**UIDMODE=?**|All slot|Returns a list of all supported configurations|
**UIDMODE=[0;1]**|All slot|Activates(1)，deactivates(0)，the magic card mode(It will has Chinese magic card back door)|
**SAKMODE?**|Current slot|Returns the configuration of the current slot|
**SAKMODE=?**|Current slot|Returns a list of all supported configurations|
**SAKMODE=[0;1]**|Current slot|Activates(1)，deactivates(0)，the real SAK ATQA mode (the SAK ATQA will be mapped from block 0)|
**CONFIG=MF_DETECTION_1K**|Current slot|Set current slot to detection 1K mode.|(It will record the key information as log in flash)|
**CONFIG=MF_DETECTION_****4****K**|Current slot|Set current slot to detection 4K mode.|(It will record the key information as log in flash)|
**DETECTION=0**|Device|Clears the detection log memory|
**DETECTION？**|Device|Wait for an XModem connection and then downloads the binary detection log data.|

### D2. Complete Instruction List
---

#### 1. Links:

  - Instruction Sheet: **[Here](/Doc/DoxygenPages/CommandLine.txt)**
  - emsec: **[Here](/../../../../emsec/ChameleonMini/master/Doc/Doxygen/html/_page__command_line.html)**
  - [AndreasBujok/ChameleonMini](/../../../../AndreasBujok/ChameleonMini)

### D3. Supported Cards, Encoding, & Simulation Types
---

#### 1. Links:

   - emsec: **[Here](/../../../../emsec/ChameleonMini/wiki/Supported-Cards-and--Codecs)**

#### 2. Comparison Table of Encoding/Simulation Types

|Card Type|Encoding Type|Whether the hardware supports|Does the software support|Whether the application layer supports|Note|
| ------------------- |:-------------------:| -------------------:| ------------------- |:-------------------:| -------------------:|
Non13.56MHz|No|No|No| |
Mifare Ultralight|ISO14443A/106 kbit/s|Support|Support|Support|
Mifare Ultralight Ev1|ISO14443A/106 kbit/s|Support|Support|Support|
MifareClassic1K/4K 4B/7B|ISO14443A/106 kbit/s|Support|Support|Support|
Mifare DESFire|ISO14443A High Rate|Supports low rates, or possibly higher rates|Only supported Low rate|No|
Mifare DESFire EV1|ISO14443A High rate|Supports low rates, or possibly higher rates|Only supported Low rate|No|Backward compatible|
Mifare DESFire EV2|ISO14443A High rate| Supports low rates, or possibly higher rates|Only supported|Low rate|No|Backward compatible|
Mifare PLUS|ISO14443A High rate|Supports low rates, or possibly higher rates|Only supported Low rate|No
Sniff Mode NTAG|ISO14443A 106 kbit/s|Support|Support| No
LEGIC prime|LEGICprime/ ISO14443A/ ISO15693|Possible but not supported|Possible but not supported|No|
HID iCLASS|125kHz/ISO15693/ISO14443B|Possible but not supported|Possible but not supported|No|
Epass|ISO14443A/B|Supported / Supported|Low rate only / not supported|No|
TiTagIT Standard|ISO15693|Support|Support|Support|
EM4233|ISO15693|Support|Support|Support|

#### 3. Comparison Table of Sniff Modes

|Encoding type|Whether the hardware supports|Does the software support| Whether the application layer supports|Note|
| ------------------- |:-------------------:| -------------------:| ------------------- |-------------------:|
Non-13.56MHz|Not Supported|Not Supported|Not Supported|
ISO 14443 A 106 kbit/s|Reader -> card Direction sniffing|Maybe support the other direction|Currently only supported Reader -> card Direction sniffing ||
ISO 15693|Support|Support|Support|Single subcarrier only|

 #### 4. Comparison Table of Supported Reading Card Types

Card type |Encoding type|  Whether the hardware stand by|Whether the software stand by|Whether the application layer supports|Note
| ------------------- |:-------------------:| -------------------:| ------------------- |-------------------:|-------------------:|
Non13.56MHz|Not Supported|Not Supported|Not Supported|
Mifare Ultralight|ISO14443A 106 kbit/s|Support|Support|SupportCommand: dump_mfu
MifareClassic1K/4K 4B/7B|ISO14443A 106 kbit/s|Support|Support|Not Supported|No card reading instruction, encryption function has been implemented|MIFARE DESFIRE|ISO14443A High rate|Supports low rates, or possibly higher rates|  Only supported Low rate|Not Supported|No card reading instruction, encryption function is being supported|

## E. External Contributions
---

- Iceman
- Philippe Teuwen
- Willok
- DXL
- ProxGrind aka Olaf
