# Project_EX

**Project_EX** is a custom real-time renderer built with **C++ and OpenGL**, featuring a lightweight workflow and a modular engine architecture.

The project focuses on modern rendering techniques and a **Physically Based Rendering (PBR)** pipeline designed primarily for experimentation and learning.

---

## ✨ Overview

Project_EX implements a modern graphics pipeline with support for **forward rendering**.  
It emphasizes clean architecture, explicit resource management, and a custom asset pipeline suitable for engine-level development and renderer experimentation.

---

## 🔥 Features

- 🔹 **Physically Based Rendering (PBR)**
- 🔹 **Image-Based Lighting (IBL)** with GGX BRDF
- 🔹 **HDR Rendering & Tone Mapping**
- 🔹 **Forward Rendering Pipeline**
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

## 🎥 Demo Video

📺 **YouTube Showcase**  
https://www.youtube.com/watch?v=8VYoi8kVgz4

This video demonstrates real-time PBR rendering, image-based lighting response and the overall renderer workflow.

---

## 🖼️ Screenshots

![PBR Scene](screenshots/pbr_scene.png)
![IBL Lighting](screenshots/ibl_lighting.png)
![Editor View](screenshots/editor_view.png)

---

## 📚 Learning & Inspiration

This project is heavily inspired by educational resources and industry best practices, especially:

- **Yan Chernikov (The Cherno)**  
  For engine architecture patterns, rendering abstractions, and practical C++ workflows.

- **LearnOpenGL.com**  
  For foundational OpenGL concepts, PBR theory, and image-based lighting workflows.

Project_EX builds upon these ideas and extends them with a custom renderer structure and experimentation-focused workflow.

---

## 📦 Dependencies

Third-party libraries are managed through a dedicated **ExternalLibrary** module using **Git submodules**, keeping engine code clean and isolated.

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
This project is actively developed and primarily intended for learning advanced rendering techniques, experimenting with engine architecture, and demonstrating real-time graphics concepts.

It is not intended to be a production-ready engine.

## 📜 License
This project is open-source and provided for educational purposes.
Third-party libraries are subject to their respective licenses.

## 🙏 Special Mentions
Huge shout out to the following resources and creators whose work greatly influenced this project:

Yan Chernikov (The Cherno)
For his in-depth C++ engine development video series and the open-source Hazel engine.
Original project: https://github.com/TheCherno/Hazel

LearnOpenGL.com
For clear, practical explanations of OpenGL fundamentals, PBR theory, and modern rendering workflows.

Game Engine Architecture by Jason Gregory
For deep insight into real-world engine design, systems thinking, and large-scale engine structure.

These resources were invaluable for learning and experimentation, and this project builds upon those ideas with a custom implementation.
