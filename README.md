# 💡 Computer Graphics - OpenGL Implementation

This project showcases a 3D graphics renderer built with modern OpenGL. It includes model loading, real-time lighting (directional, point, and spotlight), texture mapping, and a rotatable skybox environment. It was developed for educational purposes as part of a Computer Graphics course.

> 🔗 [Project Drive Link](https://drive.google.com/file/d/1zaOoxtrSPdpZ3LsEyvuvA54merhzsLGV/view?usp=sharing)

---

## 📸 Demo - Menu Interface & Models

The application features an interactive right-click menu to load different 3D models and skybox backgrounds.

<img width="586" height="593" alt="螢幕擷取畫面 2025-10-02 112531" src="https://github.com/user-attachments/assets/51e1851c-23f6-4037-8e05-bb3a3a02f700" />


---

## 📁 Key Features

- ✔️ OBJ model loading and rendering
- ✔️ Phong shading with ambient, diffuse, and specular lighting
- ✔️ Directional, point, and spotlight implementation
- ✔️ Texture mapping (`map_Kd`)
- ✔️ Rotatable skybox with multiple HDRI background options
- ✔️ Right-click menu to switch models and backgrounds

---

## 🧱 Code Highlights

### `RenderSceneCB()`
- Computes MVP and normal matrices
- Iterates over submeshes to bind texture if available
- Renders main model and skybox
- Skybox rotation handled via `curSkyboxRotationY`

### `Menu(int choice)`
- Maps menu selections to:
  - 9 models (Arcanine, Ferrari, Gengar, etc.)
  - 3 backgrounds (Photo studio, Sunflower, Veranda)
- Automatically reloads models or skyboxes

### `SetupRenderState()`
- Enables depth testing
- Initializes GLUT right-click menu

---

## 🧵 Shaders

### Vertex Shader (`phong_shading_demo.vs`)
- Inputs: `Position`, `Normal`, `Texcoord`
- Outputs: `iPosWorld`, `iNormalWorld`, `iTexCoord`
- Supports world-space transformations and normal matrix

### Fragment Shader (`phong_shading_demo.fs`)
- Inputs:
  - Lighting uniforms: `ambientLight`, `dirLightRadiance`, `pointLightPos`, `spotLightIntensity`, etc.
  - Material: `Ka`, `Kd`, `Ks`, `Ns`, `map_Kd`
- Calculates:
  - Ambient + Diffuse + Specular lighting per light source
  - Spotlight attenuation using inner/outer cone with degrees + distance
  - Final `FragColor` output via `vec3 iLightingColor`

---

## 🧊 Skybox

### `Skybox::Render()`
- Applies scale and Y-axis rotation matrix (`RotMatrix`)
- Multiplies into MVP and renders cube background

---

## 📦 Mesh Loading

### `TriangleMesh::LoadMaterialsFromFile()`
- Parses `.mtl` file and loads `map_Kd` texture using relative path

### `TriangleMesh::Rendering()`
- Sets up `glVertexAttribPointer` for position, normal, and texcoord
- Uses `glDrawElements()` to draw geometry

---

## 🛠 How to Build

```bash
# Install required libraries (GLFW, GLEW, GLM, etc.)
mkdir build && cd build
cmake ..
make
./CG_OpenGL_Implementation
```

---

## 🧪 Controls

- Right-click → Choose model / background
- Camera or model/skybox rotation via keyboard or mouse (if implemented)
- Shader preview & real-time updates

---

## 🧱 Project Structure

```
CG_OpenGL_Implementation/
├── src/
│   ├── CG2023_HW3.cpp
│   ├── trianglemesh.cpp/h
│   ├── skybox.cpp/h
│   └── shaders/
│       ├── phong_shading_demo.vs
│       └── phong_shading_demo.fs
├── assets/
│   ├── TestModels_HW3/
│   └── TestTextures_HW3/
├── CMakeLists.txt
└── README.md
```

---

Developed by Hope Chen, National Taipei University
