# Newton’s Cradle Simulation
![Newton's Cradle Simulation](images/NewtonsCradleSimulation.png)
## Section 1: Summary of What This Program Does

This program is a C++ simulation of a classic Newton’s Cradle. It uses [SFML](https://www.sfml-dev.org/) (Simple and Fast Multimedia Library) to provide the graphical output and user interaction. Key features:

- **Pendulum simulation**: Each “ball” is treated as a pendulum, complete with angle, angular velocity, and acceleration.
- **Elastic collisions**: When balls collide, momentum and energy transfer through the chain, causing the familiar “click-clack” effect of Newton’s Cradle.
- **Interactive dragging**: You can click and drag any ball to see how changing initial conditions affects the motion.

## Section 2: Detailed Installation Steps on macOS

By default a Mac usually **does not** have everything required to run this by default. Below are the steps to get everything installed:

1. **Install Xcode Command Line Tools**  
   - Open **Terminal** (Applications > Utilities).
   - Run:
     ```bash
     xcode-select --install
     ```
   - A pop-up may prompt you to install; follow the on-screen instructions.
   - This ensures you have a C++ compiler (Apple’s `clang++`) and other developer utilities.

2. **Install Homebrew** (Optional but Recommended)  
   [Homebrew](https://brew.sh/) is a popular package manager for macOS. It makes installing libraries (like SFML) simpler.  
   - In Terminal, run:
     ```bash
     /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
     ```
   - Follow the prompts and instructions. You may need your macOS user password.

3. **Install SFML**  
   - Once Homebrew is ready, run:
     ```bash
     brew update
     brew install sfml
     ```
   - This installs the SFML headers and libraries into standard paths (e.g., `/usr/local` on Intel Macs or `/opt/homebrew` on Apple Silicon).

4. **Compile the Program**  
   - Navigate to the directory containing `newtons_cradle_with_mouse.cpp`.
   - Run (using Apple’s `clang++` or a Homebrew-installed `g++`):
     ```bash
     clang++ -std=c++11 newtons_cradle_with_mouse.cpp \
         -o newtons_cradle_with_mouse \
         -lsfml-graphics -lsfml-window -lsfml-system
     ```
     or, if you specifically want GNU `g++` from Homebrew (version may vary):
     ```bash
     g++-12 -std=c++11 newtons_cradle_with_mouse.cpp \
         -o newtons_cradle_with_mouse \
         -lsfml-graphics -lsfml-window -lsfml-system
     ```
   - If you get “SFML not found” errors, add explicit include/linker paths, for example:
     ```bash
     clang++ -std=c++11 newtons_cradle_with_mouse.cpp -o newtons_cradle_with_mouse \
         -I/usr/local/include \
         -L/usr/local/lib \
         -lsfml-graphics -lsfml-window -lsfml-system
     ```
     (adjusting paths if you’re on Apple Silicon).

5. **Run the Program**  
   - In the same terminal, simply execute:
     ```bash
     ./newtons_cradle_with_mouse
     ```
   - A window should appear with the simulation.

## Section 3: Running on Windows

### Prerequisites

1. **Install Visual Studio Build Tools:**
   - Download the Visual Studio Build Tools installer from [Microsoft's website](https://visualstudio.microsoft.com/visual-cpp-build-tools/).
   - Run the installer and select the following options (refer to the image below):
     - **Desktop development with C++**
     - **MSVC v143 - VS 2022 C++ x64/x86 build tools**
     - **Windows 11 SDK**
   - Click "Install" and wait for the installation to complete.

   ![Visual Studio Build Tools Setup](images/VisualStudioBuildTools.png)

2. **Open the Correct Command Prompt:**
   - After installation, search for **x64 Native Tools Command Prompt for VS 2022** in the Windows Start Menu.
   - Open it (see the image below).

   ![x64 Native Tools Command Prompt](images/NativeToolsCommandPrompt.png)

3. **Download and Extract SFML:**
   - Download the precompiled SFML 2.6.2 library for Visual Studio from [SFML's website](https://www.sfml-dev.org/download/sfml/2.6.2/).
   - Extract the ZIP file to a directory, e.g., `C:\path\to\SFML-2.6.2`.

4. **Copy Required DLLs:**
   - Locate the following DLLs in the extracted SFML directory under `bin`:
     - `sfml-graphics-2.dll`
     - `sfml-window-2.dll`
     - `sfml-system-2.dll`
   - Copy these DLLs into the same directory as the `newtons_cradle_with_mouse.cpp` file.

---

### Building and Running the Program

1. **Navigate to the Project Directory:**
   ```cmd
   cd C:\path\to\project

2. **Compile the Program:**
   ```cmd
   cl newtons_cradle_with_mouse.cpp /I"C:\path\to\SFML-2.6.2\include" /std:c++17 /EHsc /link /LIBPATH:"C:\path\to\SFML-2.6.2\lib" /machine:x64 sfml-graphics.lib sfml-window.lib sfml-system.lib

3. **Run the program:**
   newtons_cradle_with_mouse.exe


## Section 4: Physics of How the Program Implements the Simulation

This simulation involves several core physics concepts:

1. **Pendulum Motion (Gravity-Driven)**  
   Each ball is treated like a simple pendulum characterized by:  
   - **Angle** ($\theta$) from the vertical  
   - **Angular velocity** ($\omega$)  
   - **Angular acceleration** ($\alpha$)  

   The motion is updated using the pendulum equation:  
   $\alpha = -\frac{\text{GRAVITY}}{L}\sin(\theta)$  
   where `GRAVITY` is the gravitational acceleration (set in the code) and $L$ is the length of the pendulum. These values are integrated each frame to update the angle and position.

2. **Gravitational Acceleration**  
   A constant `GRAVITY` value is used in the pendulum equation to provide the restoring force that pulls the ball toward the vertical position (straight down).

3. **Elastic Collisions (Conservation of Momentum and Energy)**  
   
   When two adjacent balls collide, their angular velocities ($v_1$ and $v_2$) are updated using 1D elastic collision equations:
   
   $v_1' = \left(\frac{m_1 - m_2}{m_1 + m_2}\right)v_1 + \left(\frac{2m_2}{m_1 + m_2}\right)v_2 $
   
   $v_2' = \left(\frac{2m_1}{m_1 + m_2}\right)v_1 + \left(\frac{m_2 - m_1}{m_1 + m_2}\right)v_2 $
   
   Here, $m_1$ and $m_2$ are the masses of the two balls, and $v_1'$ and $v_2'$ are the new angular velocities after collision. This ensures momentum and energy are (approximately) conserved.

4. **Coordinate Transformations (Angle to Position)**  
   Each ball’s position is calculated from its pivot (top anchor) using:  
   $x = x_{\text{pivot}} + L \sin(\theta)$ and $y = y_{\text{pivot}} + L \cos(\theta)$

5. **Drag-and-Drop Interaction**  
   Clicking and dragging a ball temporarily pauses its pendulum calculation. The ball’s position is set to the mouse coordinates, and on release, the simulation resumes using the new position and angle. This lets you explore different initial conditions (like pulling one ball further back) to see how momentum transfers through the cradle.


**Overall**, you see a classic Newton’s Cradle behavior: when one ball is pulled back and released, it transfers its momentum through the line of balls, causing the opposite end ball to swing out.
