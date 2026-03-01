# PotatoEarth - 完整Cesium功能设计文档

## 1. 渲染架构 (PBR + 高级渲染)

### 1.1 材质系统 (Material System)
```cpp
class Material {
    // PBR 参数
    vec3 albedo;           // 基础颜色
    float metallic;        // 金属度
    float roughness;       // 粗糙度
    float ao;              // 环境光遮蔽
    
    // 纹理贴图
    Texture2D albedoMap;
    Texture2D normalMap;
    Texture2D metallicMap;
    Texture2D roughnessMap;
    Texture2D aoMap;
    Texture2D emissiveMap;
};
```

### 1.2 大气散射 (Atmospheric Scattering)
```cpp
class Atmosphere {
    // Rayleigh 散射
    vec3 rayleighScattering;
    float rayleighScaleHeight;
    
    // Mie 散射
    vec3 mieScattering;
    float mieScaleHeight;
    float miePreferredDirection;
    
    // 渲染方法
    void RenderSky(RenderTarget& target, Camera& camera);
    void RenderAerialPerspective(RenderTarget& target);
};
```

### 1.3 光照系统
```cpp
class LightingSystem {
    // 太阳光
    DirectionalLight sunLight;
    
    // 环境光 (IBL)
    Cubemap environmentMap;
    Cubemap irradianceMap;
    Cubemap prefilterMap;
    Texture2D brdfLUT;
    
    // HDR/Tone Mapping
    float exposure;
    int toneMappingMode;  // ACES, Reinhard, etc.
};
```

### 1.4 后期处理 (Post Processing)
```cpp
class PostProcessStack {
    // 1. Bloom
    void BloomPass(Texture2D& input);
    
    // 2. SSAO
    void SSAOPass(Texture2D& depth, Texture2D& normal);
    
    // 3. SSR (屏幕空间反射)
    void SSRPass(Texture2D& color, Texture2D& depth);
    
    // 4. Tone Mapping
    void ToneMappingPass(Texture2D& input);
    
    // 5. FXAA/TAA
    void AntiAliasingPass(Texture2D& input);
};
```

## 2. 地球核心功能

### 2.1 地形系统 (Terrain)
```cpp
class TerrainProvider {
    // 高程数据源
    enum TerrainSource {
        ELLIPSOID,           // 纯椭球体
        STK_TERRAIN,         // Cesium STK地形
        MAPBOX_TERRAIN,      // Mapbox地形
        CUSTOM_TERRAIN       // 自定义地形
    };
    
    // 地形瓦片
    class TerrainTile {
        TileID id;
        float* heightmap;    // 高度图数据
        vec3* normals;       // 法线
        Mesh mesh;           // 网格
    };
    
    // 采样高程
    float SampleHeight(Cartographic position);
    
    // 获取地形瓦片
    TerrainTile* GetTile(TileID id);
};
```

### 2.2 影像系统 (Imagery)
```cpp
class ImageryLayerCollection {
    // 影像提供者
    vector<ImageryLayer*> layers;
    
    // 混合模式
    enum BlendMode {
        OPACITY,             // 透明度混合
        ADD,                 // 加法混合
        MULTIPLY             // 乘法混合
    };
};

class ImageryLayer {
    string name;
    ImageryProvider* provider;
    float alpha;
    float brightness;
    float contrast;
    float saturation;
    float gamma;
    
    // 裁剪区域
    Rectangle rectangle;
    
    // 最小/最大显示层级
    int minimumTerrainLevel;
    int maximumTerrainLevel;
};

// 支持的影像源
class ImageryProviders {
    // 在线地图
    BingMapsImageryProvider;
    OpenStreetMapImageryProvider;
    MapboxImageryProvider;
    GoogleEarthImageryProvider;
    SentinelImageryProvider;    // 卫星影像
    
    // 自定义
    SingleTileImageryProvider;   // 单张图片
    TileMapServiceImageryProvider; // TMS服务
    UrlTemplateImageryProvider;   // URL模板
};
```

### 2.3 矢量数据 (Vector Data)
```cpp
class VectorDataLayer {
    // 几何类型
    enum GeometryType {
        POINT,               // 点
        POLYLINE,            // 线
        POLYGON,             // 面
        BILLBOARD,           // 公告板
        LABEL,               // 标签
        MODEL                // 模型
    };
    
    // 数据源
    GeoJsonDataSource;
    KmlDataSource;
    CzmlDataSource;
    ShapefileDataSource;
};

// 实体系统 (Entity System)
class Entity {
    string id;
    string name;
    
    // 位置
    PositionProperty position;
    
    // 方向
    OrientationProperty orientation;
    
    // 可视化
    union {
        PointGraphics point;
        BillboardGraphics billboard;
        LabelGraphics label;
        PolylineGraphics polyline;
        PolygonGraphics polygon;
        ModelGraphics model;
    };
    
    // 时间动态
    bool hasTimeDynamic;
    vector<KeyFrame> keyframes;
};
```

## 3. 场景与相机

### 3.1 相机系统
```cpp
class CameraController {
    // 相机模式
    enum CameraMode {
        FREE,                // 自由相机
        ORBIT,               // 轨道相机
        FOLLOW,              // 跟随模式
        FLY_TO,              // 飞行模式
        MORPHING             // 变形模式
    };
    
    // 视锥体
    Frustum frustum;
    
    // 可见性检测
    bool IsBoundingVolumeVisible(BoundingVolume& bv);
    
    // 拾取
    Ray GetPickRay(vec2 screenPosition);
    Cartographic PickEllipsoid(vec2 screenPosition);
    Entity* PickEntity(vec2 screenPosition);
};
```

### 3.2 场景树 (Scene Graph)
```cpp
class Scene {
    // 根节点
    QuadtreePrimitive globe;
    
    // 矢量图层
    VectorDataLayerCollection vectorLayers;
    
    // 3D模型
    ModelCollection models;
    
    // 粒子系统
    ParticleSystemCollection particles;
    
    // 后处理
    PostProcessStack postProcess;
    
    // 天空盒
    SkyBox skyBox;
    
    // 太阳/月亮
    Sun sun;
    Moon moon;
};
```

## 4. 瓦片系统优化

### 4.1 瓦片加载策略
```cpp
class TileLoadingStrategy {
    // 距离优先级
    float ComputePriority(Tile& tile, Camera& camera);
    
    // 视锥体裁剪
    bool FrustumCull(Tile& tile, Frustum& frustum);
    
    // 遮挡查询
    bool OcclusionCull(Tile& tile);
    
    // 几何误差
    float ComputeScreenSpaceError(Tile& tile, Camera& camera);
    
    //  refine决策
    bool ShouldRefine(Tile& tile, Camera& camera) {
        return ComputeScreenSpaceError(tile, camera) > maxScreenSpaceError;
    }
};
```

### 4.2 缓存管理
```cpp
class TileCache {
    // LRU缓存
    LRUCache<TileID, Tile> tileCache;
    
    // 显存管理
    size_t gpuMemoryLimit;
    size_t currentGpuMemory;
    
    // 卸载策略
    void UnloadLeastRecentlyUsed();
    void UnloadTilesOutsideView();
};
```

## 5. 时间系统与动画

### 5.1 时钟系统
```cpp
class Clock {
    // 当前时间
    JulianDate currentTime;
    
    // 时间缩放
    float multiplier;  // 1.0 = 实时, >1 = 加速, <0 = 倒退
    
    // 时间范围
    JulianDate startTime;
    JulianDate stopTime;
    
    // 动画控制
    void Tick(float deltaTime);
    void Play();
    void Pause();
    void Reverse();
};
```

### 5.2 属性系统 (Property System)
```cpp
// 时间动态属性
template<typename T>
class SampledProperty {
    vector<TimeInterval<T>> samples;
    
    T GetValue(JulianDate time) {
        // 插值
        return Interpolate(samples, time);
    }
};

// 支持的属性类型
class PositionProperty : SampledProperty<Cartesian3> {};
class OrientationProperty : SampledProperty<Quaternion> {};
class ColorProperty : SampledProperty<Color> {};
class NumberProperty : SampledProperty<float> {};
```

## 6. 事件与交互

### 6.1 事件系统
```cpp
class EventManager {
    // 鼠标事件
    Event<MouseMoveEvent> onMouseMove;
    Event<MouseClickEvent> onMouseClick;
    Event<MouseWheelEvent> onMouseWheel;
    
    // 键盘事件
    Event<KeyEvent> onKeyDown;
    Event<KeyEvent> onKeyUp;
    
    // 相机事件
    Event<CameraChangeEvent> onCameraChange;
    
    // 选择事件
    Event<EntitySelectEvent> onEntitySelect;
};
```

### 6.2 交互部件
```cpp
class Widgets {
    // 导航器
    NavigationHelpButton;
    
    // 时间轴
    TimelineWidget;
    
    // 动画控制
    AnimationWidget;
    
    // 底图选择
    BaseLayerPicker;
    
    // 全屏
    FullscreenButton;
    
    // 位置信息
    Geocoder;
    
    // 缩放控制
    ZoomControl;
    
    // 指南针
    Compass;
    
    // 距离比例尺
    DistanceLegend;
};
```

## 7. 性能优化

### 7.1 Level of Detail (LOD)
```cpp
class LODSystem {
    // 几何LOD
    void ComputeGeometricError(Tile& tile);
    
    // 纹理LOD
    void ComputeTextureLOD(Tile& tile, Camera& camera);
    
    // 法线LOD
    void GenerateNormalMapLOD(Tile& tile);
};
```

### 7.2 渲染优化
```cpp
class RenderingOptimizations {
    // 实例化渲染
    void InstancedRendering(vector<Entity>& entities);
    
    // 遮挡查询
    void HardwareOcclusionQuery();
    
    // 细节层次混合
    void MorphTargets(Tile& tile, float morphFactor);
    
    // 异步上传
    void AsyncTextureUpload();
    void AsyncBufferUpload();
};
```

## 8. 扩展功能

### 8.1 测量工具
```cpp
class MeasurementTools {
    // 距离测量
    float MeasureDistance(Cartographic p1, Cartographic p2);
    
    // 面积测量
    float MeasureArea(vector<Cartographic> polygon);
    
    // 高度测量
    float MeasureHeight(Cartographic p1, Cartographic p2);
    
    // 角度测量
    float MeasureAngle(Cartographic center, Cartographic p1, Cartographic p2);
};
```

### 8.2 3D Tiles 支持
```cpp
class Cesium3DTiles {
    // 3D Tiles 格式支持
    Batched3DModel (b3dm);      // 批量模型
    Instanced3DModel (i3dm);    // 实例化模型
    PointCloud (pnts);          // 点云
    Composite (cmpt);           // 复合模型
    
    // 流式加载
    void StreamTiles();
    
    // 样式化
    void ApplyStyle(StyleExpression& style);
};
```

### 8.3 VR/AR 支持
```cpp
class XRSupport {
    // VR
    void InitializeVR();
    void RenderStereo(Camera& leftEye, Camera& rightEye);
    
    // AR
    void InitializeAR();
    void RenderAROverlay();
};
```

## 9. 完整API示例

```cpp
// 初始化地球
auto viewer = new Viewer("#container");

// 添加影像图层
auto imageryLayer = new ImageryLayer(new BingMapsImageryProvider());
viewer->imageryLayers->Add(imageryLayer);

// 添加地形
auto terrainProvider = new CesiumTerrainProvider({
    url: "https://assets.agi.com/stk-terrain/world"
});
viewer->terrainProvider = terrainProvider;

// 添加实体
auto entity = viewer->entities->Add({
    position: Cartesian3::FromDegrees(-122.0, 45.0, 1000.0),
    point: {
        pixelSize: 10,
        color: Color::Yellow()
    },
    label: {
        text: "Portland",
        font: "14pt sans-serif"
    }
});

// 相机飞行
viewer->camera->FlyTo({
    destination: Cartesian3::FromDegrees(-122.0, 45.0, 5000.0),
    orientation: {
        heading: Math::ToRadians(0.0),
        pitch: Math::ToRadians(-90.0),
        roll: 0.0
    }
});

// 时间动画
viewer->clock->multiplier = 100.0;  // 100x speed
viewer->clock->shouldAnimate = true;
```

---

## 开发优先级

### Phase 1 (当前) - 核心渲染 ✅
- [x] 基础地球渲染
- [x] 瓦片四叉树
- [x] 轨道相机
- [x] 基础光照

### Phase 2 - PBR渲染
- [ ] 材质系统
- [ ] IBL环境光
- [ ] 大气散射
- [ ] HDR/Tone Mapping

### Phase 3 - 高级功能
- [ ] 地形高程
- [ ] 影像图层
- [ ] 矢量数据
- [ ] 实体系统

### Phase 4 - 完整Cesium功能
- [ ] 时间系统
- [ ] 动画系统
- [ ] 3D Tiles
- [ ] 测量工具
- [ ] VR/AR支持
