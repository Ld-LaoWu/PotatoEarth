# PotatoEarth

A geographic rendering engine inspired by CesiumJS, built with C++ and OpenGL.

## Features

- [x] WGS84 Geospatial Core: Accurate ellipsoid and coordinate transformations
- [x] Dynamic LOD: Quadtree-based tile system with adaptive detail levels
- [x] Orbit Camera: Geographic camera controls (longitude, latitude, height)
- [x] Scene System: Entity-component architecture for rendering
- [x] Imagery Support: Async tile loading with disk caching
- [x] Cross-Platform: Designed for Windows with Qt frontend

## Architecture

```
PotatoEarth/
├── EarthCore/          # Core engine
│   ├── Core/           # Application, Events, Math
│   ├── Geospatial/     # WGS84, Cartographic, Ellipsoid
│   ├── Render/         # Shader, Buffer, Camera, Texture
│   └── Scene/          # Scene, Entity, Tile, Quadtree
├── Platform/           # Platform abstraction
│   ├── Opengl/         # OpenGL implementations
│   └── Windows/        # Windows platform
└── extern/             # Third-party libraries
    ├── glm/            # Math library
    ├── glad/           # OpenGL loader
    ├── spdlog/         # Logging
    └── imgui/          # UI library
```

## Building

Requirements:
- Visual Studio 2022
- Qt 5.15+ (for frontend)
- Premake5

```bash
# Generate Visual Studio project
premake5 vs2022

# Build
msbuild PotatoEarth.sln
```

## Controls

| Key | Action |
|-----|--------|
| W/S | Orbit north/south |
| A/D | Orbit east/west |
| Q/E | Zoom in/out |
| Up/Down | Rotate pitch |
| Left/Right | Rotate heading |

## Progress

- [x] Core framework (Event system, Window abstraction)
- [x] Geospatial (WGS84, coordinate transforms)
- [x] Rendering (Shader, Buffer, VertexArray)
- [x] Camera (EarthOrbitCamera with geographic controls)
- [x] Scene/Entity system
- [x] Tile quadtree with dynamic LOD
- [x] Texture loading
- [x] Imagery layer (OSM support)
- [ ] Real-time imagery integration (in progress)
- [ ] Terrain height data
- [ ] Atmosphere rendering

## License

MIT
