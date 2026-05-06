<div align="center">
  
# Robotic Arm Kinematics Simulator

**A comprehensive and interactive 2D simulation tool for Inverse (IK) and Forward Kinematics (FK) of a robotic arm, built purely in C using SDL3.**

</div>

---

## Showcase

<p align="center">
  <img width="49%" src="https://github.com/user-attachments/assets/153a55c4-6c85-43ee-8299-b2d3402cca69" alt="Simulator Showcase 1" />
  <img width="49%" src="https://github.com/user-attachments/assets/687b4832-f637-4102-8315-d8a43f3cb8be" alt="Simulator Showcase 2" />
</p>
<p align="center">
  <img width="49%" src="https://github.com/user-attachments/assets/a41d6147-4222-44f5-aa17-52b48c022ffd" alt="Simulator Showcase 3" />
  <img width="49%" src="https://github.com/user-attachments/assets/f6e35c2b-d73c-48fa-9426-6fd146df74c2" alt="Simulator Showcase 4" />
</p>

## Key Features

- **Interactive UI**: Real-time rendering with user-friendly custom UI components.
- **Inverse Kinematics (IK) Mode**: Click anywhere to set a target position, and the arm automatically calculates the required joint angles to reach it.
- **Forward Kinematics (FK) Mode**: Manually input joint angles to see exactly where the end-effector will be positioned.
- **Adjustable Segments**: Dynamically add (`+`) or remove (`-`) links to the robotic arm on the fly.
- **Real-time Telemetry**: Monitor end-effector position, IK error margins, and individual joint angles (both absolute and relative).
- **Mathematical Transparency**: Toggleable formula view to see the underlying mathematics powering the kinematics.

## Prerequisites

- **Compiler**: GCC (e.g., MinGW-w64 on Windows)
- **Library**: [SDL3](https://github.com/libsdl-org/SDL) (Ensure SDL3 is installed and accessible)
- **Environment**: Windows (PowerShell is used for the build script)

> **Note:** Ensure that your SDL3 installation path matches the `$basePath` referenced in `build.ps1`, or update the script to match your local path:
> ```powershell
> $basePath = "C:/SDL3/SDL3-3.4.8/x86_64-w64-mingw32"
> ```

## Building and Running

A simple PowerShell script is provided to streamline the build and execution process.

1. Clone the repository and navigate to the project directory.
2. Open your terminal (PowerShell).
3. Execute the build script:

```powershell
.\build.ps1
```

If successful, the script will automatically compile all source files, copy `SDL3.dll` if it is missing, and launch the application.

## Controls

| Action | Control |
|---|---|
| **Set IK Target** | Left Mouse Click (in IK Mode) |
| **Input Values** | Keyboard |
| **Delete Character** | `Backspace` |
| **Next Input Field** | `Tab` |
| **Toggle Formula View** | `F1` |
| **Switch Modes (IK/FK)** | UI Button |
| **Add/Remove Segments** | UI Buttons (`+` / `-`) |

## Project Structure

- `main.c` - Entry point and main event loop handling.
- `kinematics.c` / `.h` - Core mathematical logic for Forward and Inverse Kinematics.
- `math_utils.c` / `.h` - Helper functions for vector math and geometry.
- `ui.c` / `.h` - User interface elements, text input handling, and button logic.
- `render_utils.c` / `.h` - Graphical routines for shapes, grid, layout, and text rendering.

---
*Built with C and SDL3.*
