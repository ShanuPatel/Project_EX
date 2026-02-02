# Project_EX

**Project_EX** is a custom real-time renderer built with **C++ and OpenGL**, featuring a lightweight workflow and a modular engine architecture.

The project focuses on modern rendering techniques, a physically based rendering (PBR) pipeline designed for experimentation and learning.

---

## ✨ Overview

Project_EX implements a modern graphics pipeline with support for both **forward** 
It emphasizes clean architecture, explicit resource management, and a custom asset pipeline suitable for engine-level development.

---

## 🔥 Features

- 🔹 **Physically Based Rendering (PBR)**
- 🔹 **Image-Based Lighting (IBL)** with GGX BRDF
- 🔹 **HDR Rendering & Tone Mapping**
- 🔹 **Forward Rendering Pipelines**
- 🔹 **Custom Asset Loading Pipeline**

---

## 🛠 Technology Stack

- **Language:** C++
- **Graphics API:** OpenGL (Core Profile)
- **Math:** GLM
- **Assets:** glTF / OBJ (via custom pipeline)
- **Build System:** CMake
- **Platform:** Windows (currently)

---

## 📚 Learning & Inspiration

This project is heavily inspired by educational resources and industry best practices, especially:

- **:contentReference[oaicite:0]{index=0}**  
  For engine architecture, rendering abstractions, and practical C++ patterns.

- **:contentReference[oaicite:1]{index=1}**  
  For foundational OpenGL concepts, PBR theory, and IBL workflows.

Project_EX builds upon these ideas and extends them with a custom engine structure workflow.

---

## 📦 Dependencies

Third-party libraries are managed through a dedicated **ExternalLibrary** module (via Git submodules), keeping engine code clean and isolated.

> ⚠️ **Important:** This project uses Git submodules.

---

## ⬇️ Cloning the Repository

To clone the project **with all dependencies included**, use:

```bash
git clone --recurse-submodules https://github.com/ShanuPatel/Project_EX.git
```
If you already cloned without submodules:
```bash
git submodule update --init --recursive
```
## 🚧 Status
This project is actively developed and primarily intended for:

Learning advanced rendering techniques

Experimenting with engine architecture

Demonstrating real-time graphics concepts

It is not intended as a production-ready engine.

## 📜 License
This project is open-source and provided for educational purposes.
Third-party libraries are subject to their respective licenses.

## 🙌 Acknowledgements
Special thanks to the graphics programming community and open educational resources that make projects like this possible.

## 🙏 Special Mentions

Huge shout out to the following resources and creators whose work greatly influenced this project:

- For his in-depth C++ engine development video series and the open-source **Hazel** engine, which inspired core engine architecture and rendering abstractions.  
  Original project: TheCherno/Hazel

- For clear, practical explanations of OpenGL fundamentals, PBR theory, image-based lighting, and modern rendering workflows.

- For providing deep insight into real-world engine design, systems thinking, and large-scale engine structure.

These resources were invaluable for learning and experimentation, and this project build
