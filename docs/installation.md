# Installing Edito

This guide will help you get Edito up and running on your computer.  
Currently, the only way to install Edito is to **build it from source**, because pre‑built installers are not yet available.

> **Prefer a ready‑to‑run package?** We’re planning to offer Windows installers and Linux AppImages in the future. If you’d like to help us package Edito, see [CONTRIBUTING.md](../CONTRIBUTING.md).

---

## System Requirements

- **Operating System:** Windows 10/11, Linux (Ubuntu 20.04+), macOS (untested)
- **Qt 6.9 or later** (Widgets module)
- **C++ compiler:** MinGW 64‑bit (Windows), GCC 9+ (Linux), Clang (macOS)
- **Git** (to clone the repository)
- **Hunspell** (the spellchecking library – included in the source, but your system may need the runtime library)

No database or internet connection is required.

---

## Building from Source (All Platforms)

### 1. Install Prerequisites

#### Windows
1. Download the **Qt Online Installer** from [qt.io/download](https://www.qt.io/download-qt-installer).
2. During installation, select:
   - Qt 6.9.1 or later
   - **MinGW 64‑bit** (under Qt → Qt 6.9.x → MinGW 64-bit)
   - **Qt Creator** (recommended, but you can use any IDE)
3. Install **Git** from [git‑scm.com](https://git-scm.com/).

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential qt6-base-dev libhunspell-dev git
```

#### macOS
```bash
brew install qt@6 hunspell git
```

### 2. Clone the Repository

Open a terminal (Command Prompt, PowerShell, or your favourite shell) and run:

```bash
git clone https://github.com/Yovsky/Edito.git
cd Edito
```

### 3. Build with Qt Creator (Recommended)

- Launch **Qt Creator**.
- Click **File → Open File or Project…** and select the `CMakeLists.txt` (or `.pro` file) inside the `Edito` folder.
- Qt Creator will ask you to configure a **Kit**. Choose the one that matches your Qt version and compiler (e.g., `Qt 6.9.1 MinGW 64-bit`).
- Click the **hammer icon** (or press `Ctrl+B`) to build.
- Once the build finishes, click the **green play button** to run Edito.

### 4. Build from Command Line (Alternative)

If you prefer the command line, use **CMake** (or qmake) directly:

```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.9.1/gcc_64   # adjust for your system
cmake --build .
```

After building, the executable will be inside the `build/` directory.

> On Windows, make sure the MinGW `bin` folder (e.g. `C:\Qt\6.9.1\mingw_64\bin`) is in your `PATH`, or run Edito directly from Qt Creator.

### 5. Run Edito

- **Qt Creator:** Use the green play button.
- **Command line:** From the build folder, run `./Edito` (Linux/macOS) or `Edito.exe` (Windows).

The application window will open immediately, ready for editing!

---

## Spellchecking Dictionaries

Edito uses Hunspell for spellchecking and looks for dictionary files (`.aff` and `.dic`) in the `dictionaries/` folder next to the executable.

If you built from source, this folder is already in the repository and will be copied automatically during the build process.  
To add a new dictionary:

1. Download the `.aff` and `.dic` files for your language from [LibreOffice dictionaries](https://cgit.freedesktop.org/libreoffice/dictionaries/tree/) or [SCOWL](http://wordlist.aspell.net/).
2. Place both files inside the `dictionaries/` folder.
3. Restart Edito — the new language should appear in the spellcheck menu.

If spellchecking doesn’t work, make sure the `dictionaries` folder isn’t empty and that the files are named correctly (e.g., `en_US.aff` and `en_US.dic`).

---

## Verifying the Installation

1. Launch Edito.
2. Type a misspelled word (e.g., “speling”).
3. If you see a red wavy underline, spellchecking is active.  
   Right‑click the word to see suggestions.

If the editor opens without any error messages, the installation was successful! 🎉

---

## Troubleshooting

| Problem | Solution |
|---------|----------|
| **“Qt6Widgets.dll not found”** | Make sure Qt’s `bin` folder is in your `PATH`, or run Edito from Qt Creator. |
| **Build fails with “hunspell/hunspell.hxx not found”** | Hunspell is included in `third-party/hunspell`. Ensure the folder is intact and that the `CMakeLists.txt` / `.pro` file correctly references it. Re-run CMake. |
| **Spellcheck doesn’t work** | Confirm the `dictionaries/` folder contains at least one `.aff` and `.dic` pair. Check that the files are not corrupted. |
| **Application crashes on startup** | Rebuild with debug symbols (`cmake .. -DCMAKE_BUILD_TYPE=Debug`) and run through a debugger to see the error. Report the bug if it persists. |
| **“No C++ compiler found”** | Install MinGW‑w64 (Windows) or `build-essential` (Linux). On macOS, install Xcode Command Line Tools with `xcode-select --install`. |
