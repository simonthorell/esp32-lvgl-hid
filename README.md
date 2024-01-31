# _ESP32 LVGL HDI_

## NOTICE! 
As of 2024-01-31 this README file is obsolete due to complete repo refactoring. Update will follow... // ST

## Overview
This template provides a starting point for creating a GUI application on the ESP32-S3 microcontroller using the LittlevGL (LVGL) graphics library and the ESP-IDF framework. The template includes configuration for a 1.9" ST7789 LCD Screen and demonstrates the basics of using LVGL with ESP32 as well as connecting Keyboard and Mouse using USB HDI.

## Pre-requisites
- Install [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
- Install [VSCode](https://code.visualstudio.com/)
- Install [ESP-IDF Extension for VSCode](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension)
- Install [SquareLine-Studio](https://squareline.io/downloads)

## Microcontroller
- ESP32-S3 with 1.9" ST7789 LCD Screen (Example pre-built version: LILYGO T-Display-S3)
- Hardware Simulator: [Wokwi Project Simulator](https://wokwi.com/projects/380662202976325633)

## Template Setup
This template was created using the following steps:

1. **Setup IDF command line**
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

2. **Create a New Project from Template:**
- Read the following [link](https://components.espressif.com/components/espressif/esp_lvgl_port?language=en).
- Use a shell to `cd` to the folder were you want to create your project folder.
    ```bash
    # Get the idf-tools
    get_idf

    # Create the project using the template
    idf.py create-project-from-example "espressif/esp_lvgl_port^1.4.0:touchscreen"
    idf.py add-dependency "espressif/usb_host_hid^1.0.2"
    ```
3. **Setup VSCode Configuration:**
- Create the file `c_cpp_properties.json` in .vscode folder and add necessary configurations.

4. **Version Control:**
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
1. **Add Board**
  - Compress the `__ui_project_name__` folder inside the `gui` folder in the project directory. 
    - On macOS you can just: **right click -> Compress Folder**
    - Rename the folder to `t_display_s3.zip`
  - Locate the `gui` folder in this repo and copy the `t_display_s3` folder inside the `boards` folder.
  - In your applications folder (if using macOS), find your SquareLine Studio installation, right click `Show Package Contents` then find the `boards` folder and create a new folder called `LilyGo`.
  - Paste the folder from your clipboard into this folder as well as the .zip folder you created.
2. **Create GUI**
  - When you open SquarLine Studio you should now se a tab called **LilyGO** under **Create**.
  - Once the project is opened, click on **Export -> Create Template Project** and first create a folder called `squareline` in the project folder (included in .gitignore), and when prompted for **Export To** choose the `gui/__ui_project_name__/main/ui` folder in the project folder.
3. **Export GUI**
  - Export the `.ui` files by clicking **Export -> Export UI Files**.
  - Save  

## Build & Flash Wih GUI
- *TODO: Replace this with .devcontainer and GitHub Actions for auto-build!*
- Move into the UI folder, build & flash using the idf.py tool.
  ```bash 
  cd gui/__ui_project_name__                # Move to GUI folder
  get_idf                                   # Get ESP-IDF tools
  idf.py build                              # Build to project with GUI
  idf.py -p /dev/cu.usbmodem1101 flash       # Flash and monitor (replace with your USB-port)
  ```

## Build Your Project Without GUI [Optional]
- Press `CMD + Shift + P` and type `ESP-IDF: Build your Project`
- [Optional] to clean build folder press `CMD + Shift + P` and type `ESP-IDF: Full Clean Project`
- [Optional] setup a .devcontainer and use GitHub actions to build automatically when merging to main branch. 

## Flashing and Monitoring Without GUI [Optional]
After building the project, flash it to your ESP32-S3 and monitor the output:
- Press `CMD + Shift + P` and type `ESP-IDF: Flash (UART) your Project`

- [alternatively] flash it from the command line using idf.py
```bash
idf.py -p (YOUR_PORT) flash monitor
```

## Contributing
Contributions are welcome! If you have improvements or bug fixes, feel free to fork the repository and submit a pull request.

## License
This project is built using [esp_lvgl_port](https://components.espressif.com/components/espressif/esp_lvgl_port/versions/1.4.0/license) which is licensed under the Apache 2.0 License. See [Apache License Version 2.0](https://www.apache.org/licenses/LICENSE-2.0.txt) for the original license of the template.

The modifications and new code in this project are licensed under MIT License. See [LICENSE.md](LICENSE.md) for more details.
