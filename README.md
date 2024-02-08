# _ESP32 LVGL HDI_

## Overview
This repository provides a robust starting point for developing GUI applications on the LilyGo T-Display-S3 ESP32 MCU. Leveraging the capabilities of the ESP32-S3 microcontroller and the powerful LittlevGL (LVGL) graphics library within the ESP-IDF framework, this template is designed for high performance and reliability.

### Features
- **Display Interface:** Configuration for a 1.9" ST7789 LCD Screen, demonstrating the integration and usage of LVGL with the ESP32.
- **Keyboard Connectivity:** Support for connecting a keyboard using USB HID, enhancing user interaction capabilities.
- **Secure Wi-Fi Connection:** Implements secure Wi-Fi communication, ensuring data protection and network integrity.
- **TLS MQTT Connection:** Utilizes TLS for MQTT connections, safeguarding message confidentiality and integrity. This includes CA certificate verification for trusted communication.
- **AES256 End-to-End Encryption:** Employs AES256 encryption for end-to-end data security, ensuring that sensitive information remains confidential and tamper-proof during transmission.
- **Firmware Updates Over the Air (FOTA):** Supports FOTA, enabling seamless and secure firmware updates, ensuring that the device can receive the latest features, improvements, and security patches remotely and with minimal downtime.

With these features, the repository offers a comprehensive suite for developing advanced GUI applications, focusing on security, user experience, and ease of maintenance.

## Pre-requisites
- Install [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
- Install [VSCode](https://code.visualstudio.com/)
- Install [ESP-IDF Extension for VSCode](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension)
- Install [SquareLine-Studio](https://squareline.io/downloads)

## Microcontroller
- ESP32-S3 with 1.9" ST7789 LCD Screen (Example pre-built version: LILYGO T-Display-S3)

## Template Setup
This template was created using the following steps:

### 1. Setup IDF command line
- This is only tested and working on Apple Silicon macOS using `.zsh` shell.
    ```bash
    # Edit your shell profile
    nano ~/.zshrc

    # Add the following lines
    alias get_idf='. $HOME/esp/esp-idf/export.sh'
    export IDF_PATH="$HOME/esp/esp-idf:$PATH"
    export PATH="$HOME/esp/esp-idf/tools/xtensa-esp-elf/esp-13.2.0_20230928/xtensa-esp-elf/bin:$PATH"
    export PATH="$HOME/esp/esp-idf/tools:$PATH"
    ```

### 2. Create a New Project from Template:
  - Read the following [link](https://components.espressif.com/components/espressif/esp_lvgl_port?language=en).
  - Use a shell to `cd` to the folder were you want to create your project folder.
      ```bash
      # Get the idf-tools
      get_idf

      # Create the project using the template
      idf.py create-project-from-example "espressif/esp_lvgl_port^1.4.0:touchscreen"
      idf.py add-dependency "espressif/usb_host_hid^1.0.2"
      ```

### 3. Setup VSCode Configuration: [optional]
- Create the file `c_cpp_properties.json` in .vscode folder and add necessary configurations.

### 4. Version Control:
  - Create a new repository on GitHub for your project.
  - Initialize git in your project folder:
    ```bash
    git init
    git add .
    git commit -m "Inital commit and build"
    ```
  - Create a development branch [Recommended]
    ```bash
    git checkout -b dev
    ```
  - Once you are ready for next build you make a Pull Request from dev branch on Github and merge it into the main branch.

## Build GUI using SquareLine Studio
### 1. Add Board
- Compress the `main` folder inside the project directory. 
  - On macOS you can just: **right click -> Compress Folder**
  - Rename the folder to `t_display_s3.zip`
- Locate the `boards` folder and copy the `t_display_s3` folder.
- In your applications folder (if using macOS), find your SquareLine Studio installation, right click `Show Package Contents` then find the `boards` folder and create a new folder called `LilyGo`.
- Paste the folder from your clipboard into this folder as well as the .zip folder you created.

### 2. Create GUI
- When you open SquarLine Studio you should now se a tab called **LilyGO** under **Create**.
- Once the project is opened, click on **Export -> Create Template Project** and first create a folder called `squareline` in the project folder (included in .gitignore), and when prompted for **Export To** choose the `main/ui` folder in the project folder.

### 3. Export GUI
- Export the `.ui` files by clicking **Export -> Export UI Files**.
- Save  

## Build Project & Flash to ESP32
- *TODO: Replace this with .devcontainer and GitHub Actions for auto-build and firmware update release!*
- Move into project folder, build & flash using the idf.py tool.
  ```bash
  get_idf                                      # Get ESP-IDF tools
  idf.py build                                 # Build to project with GUI
  idf.py -p /dev/cu.usbmodem1101 flash monitor  # Flash and monitor (replace with your USB-port)
  ```
- If you do not like to monitor the output just use:
  ```bash
  idf.py -p /dev/cu.usbmodem1101 flash  # Flash and monitor (replace with your USB-port)
  ```
- If you need to clean the project you can use the following commands:
  ```bash
  idf.py clean
  idf.py fullclean
  ```

## Build Your Project *(using VSCode extension)* [optional]
- Press `CMD + Shift + P` and type `ESP-IDF: Build your Project`
- [Optional] to clean build folder press `CMD + Shift + P` and type `ESP-IDF: Full Clean Project`
- [Optional] setup a .devcontainer and use GitHub actions to build automatically when merging to main branch. 

## Flashing and Monitoring *(using VSCode extension)* [optional]
After building the project, flash it to your ESP32-S3 and monitor the output:
- Press `CMD + Shift + P` and type `ESP-IDF: Flash (UART) your Project`

- [alternatively] flash it from the command line using idf.py
```bash
idf.py -p (YOUR_PORT) flash monitor
# Press Ctrl+Å to close (Nordic keyboard)
```

## Setup Firmware Over The Air (FOTA)
- See file `partition_table.csv` and main folder as well as `firmware.bin` and `firmware.json`in the bin-folder.
- In `ESP-IDF menuconfig` search for *"Partition table"*. In this project option 2 is used as the firmware is larger than the standard size of 1 MB.
  - Option 1. Use *"Factory app, two OTA definitions"* to use standard partition size (1MB per partition). 
  - Option 2. Use *"Custom partition table CSV"* and add the `partition_table.csv` file. You can edit the size of the partitions directly in this file depending on your specific ESP32 microcontroller.
- The firmware check is done in the FreeRTOS task: *"fota_task"* defined in `main.c`. For details of implementation, see the file `esp_firmware.c`.
- In the src `CMakeLists.txt`, the following has to be added to the **REQUIRES**: `esp_http_client esp_https_ota json` under **idf_component_register**.
- When building the project, `check_and_copy.cmake` and src `CMakeLists.txt` will find and copy the built .bin-file to the firmware folder automatically.
- Update the paths in `main.cpp` and `firmware.json` to your own firmware update path. In this project we use GitHub only.
- When you are ready to release and push the firmware update, change the version numbers in `main.cpp` and `firmware.json` - then commit to main branch. Monitor the MCU over UART to check that the device is correctly requesting and getting the latest update. 
- Once the MCU has downloaded and installed the new firmware, it will reboot to the opposite ota partition and display firmware version on the LCD.

## Known Issues
### 1. Crash on Simultaneous Button Presses
- **Issue Description:** ESP32 may crash occasionally when multiple buttons are pressed simultaneously. Potential causes include inadequate debounce handling in the `espressif/button` library or task-related conflicts.
- **Impact:** This issue affects applications relying on multi-button interactions.
- **Status:** Currently under investigation. Future updates or fixes will be documented.

## Contributing
Contributions are welcome! 
If you have improvements or bug fixes, feel free to fork the repository and submit a pull request.

## License
Distributed under the MIT License. See [LICENSE.md] for more information.
