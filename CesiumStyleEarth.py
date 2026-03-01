"""
PotatoEarth - Cesium-Style with Online Tiles and Mouse Control
Supports ArcGIS and Gaode (Amap) imagery tiles
"""

import numpy as np
import OpenGL.GL as gl
import OpenGL.GLUT as glut
import OpenGL.GLU as glu
import math
import sys
import time
import urllib.request
import io
from PIL import Image
import threading
import queue

# =============================================================================
# CONFIGURATION
# =============================================================================

EARTH_RADIUS = 6378137.0  # WGS84 equatorial radius
SCALE = 1.0 / 1000000.0

# Tile providers
TILE_PROVIDERS = {
    'arcgis': {
        'url': 'https://services.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}',
        'max_zoom': 18,
        'attribution': 'ArcGIS'
    },
    'gaode': {
        'url': 'https://webrd0{s}.is.autonavi.com/appmaptile?lang=zh_cn&size=1&scale=1&style=8&x={x}&y={y}&z={z}',
        'subdomains': ['1', '2', '3', '4'],
        'max_zoom': 18,
        'attribution': 'Gaode'
    },
    'osm': {
        'url': 'https://tile.openstreetmap.org/{z}/{x}/{y}.png',
        'max_zoom': 19,
        'attribution': 'OpenStreetMap'
    },
    'cesium': {
        'url': 'https://t{s}.tianditu.gov.cn/img_w/wmts?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=img&STYLE=default&TILEMATRIXSET=w&FORMAT=tiles&TILEMATRIX={z}&TILEROW={y}&TILECOL={x}&tk=YOUR_TOKEN',
        'subdomains': ['0', '1', '2', '3', '4', '5', '6', '7'],
        'max_zoom': 18,
        'attribution': 'Tianditu'
    }
}

# =============================================================================
# TILE CACHE AND LOADING
# =============================================================================

class TileCache:
    """LRU cache for tiles"""
    def __init__(self, max_size=100):
        self.max_size = max_size
        self.cache = {}
        self.access_order = []
        self.lock = threading.Lock()
    
    def get(self, tile_id):
        with self.lock:
            if tile_id in self.cache:
                # Move to front (most recently used)
                self.access_order.remove(tile_id)
                self.access_order.append(tile_id)
                return self.cache[tile_id]
            return None
    
    def put(self, tile_id, texture_id):
        with self.lock:
            if tile_id in self.cache:
                self.access_order.remove(tile_id)
            elif len(self.cache) >= self.max_size:
                # Remove least recently used
                lru = self.access_order.pop(0)
                old_texture = self.cache.pop(lru)
                gl.glDeleteTextures([old_texture])
            
            self.cache[tile_id] = texture_id
            self.access_order.append(tile_id)
    
    def clear(self):
        with self.lock:
            for texture_id in self.cache.values():
                gl.glDeleteTextures([texture_id])
            self.cache.clear()
            self.access_order.clear()

class TileLoader:
    """Async tile loader"""
    def __init__(self, provider='arcgis'):
        self.provider = provider
        self.config = TILE_PROVIDERS[provider]
        self.cache = TileCache(max_size=50)
        self.load_queue = queue.Queue()
        self.loaded_queue = queue.Queue()
        self.running = False
        self.loader_thread = None
    
    def start(self):
        self.running = True
        self.loader_thread = threading.Thread(target=self._load_loop)
        self.loader_thread.daemon = True
        self.loader_thread.start()
    
    def stop(self):
        self.running = False
        if self.loader_thread:
            self.loader_thread.join(timeout=1.0)
    
    def _load_loop(self):
        """Background thread for loading tiles"""
        while self.running:
            try:
                tile_id = self.load_queue.get(timeout=0.1)
                x, y, z = tile_id
                
                # Check cache first
                texture = self.cache.get(tile_id)
                if texture is not None:
                    self.loaded_queue.put((tile_id, texture))
                    continue
                
                # Download tile
                url = self._build_url(x, y, z)
                try:
                    headers = {'User-Agent': 'PotatoEarth/1.0'}
                    req = urllib.request.Request(url, headers=headers)
                    with urllib.request.urlopen(req, timeout=5) as response:
                        data = response.read()
                        
                    # Create texture
                    texture = self._create_texture(data)
                    if texture:
                        self.cache.put(tile_id, texture)
                        self.loaded_queue.put((tile_id, texture))
                except Exception as e:
                    print(f"Failed to load tile {tile_id}: {e}")
                    
            except queue.Empty:
                continue
    
    def _build_url(self, x, y, z):
        """Build tile URL"""
        url = self.config['url']
        url = url.replace('{x}', str(x)).replace('{y}', str(y)).replace('{z}', str(z))
        
        if '{s}' in url:
            subdomain = self.config.get('subdomains', [''])[0]
            url = url.replace('{s}', subdomain)
        
        return url
    
    def _create_texture(self, image_data):
        """Create OpenGL texture from image data"""
        try:
            img = Image.open(io.BytesIO(image_data))
            img = img.convert('RGB')
            img_data = np.array(img, dtype=np.uint8)
            
            texture_id = gl.glGenTextures(1)
            gl.glBindTexture(gl.GL_TEXTURE_2D, texture_id)
            
            gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_S, gl.GL_CLAMP_TO_EDGE)
            gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_T, gl.GL_CLAMP_TO_EDGE)
            gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MIN_FILTER, gl.GL_LINEAR_MIPMAP_LINEAR)
            gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MAG_FILTER, gl.GL_LINEAR)
            
            gl.glTexImage2D(gl.GL_TEXTURE_2D, 0, gl.GL_RGB, img.width, img.height, 0,
                           gl.GL_RGB, gl.GL_UNSIGNED_BYTE, img_data)
            gl.glGenerateMipmap(gl.GL_TEXTURE_2D)
            
            return texture_id
        except Exception as e:
            print(f"Failed to create texture: {e}")
            return None
    
    def request_tile(self, x, y, z):
        """Request a tile to be loaded"""
        tile_id = (x, y, z)
        
        # Check if already cached
        texture = self.cache.get(tile_id)
        if texture is not None:
            return texture
        
        # Add to load queue
        try:
            self.load_queue.put_nowait(tile_id)
        except queue.Full:
            pass
        
        return None
    
    def get_loaded_tiles(self):
        """Get tiles that have finished loading"""
        tiles = []
        while not self.loaded_queue.empty():
            try:
                tiles.append(self.loaded_queue.get_nowait())
            except queue.Empty:
                break
        return tiles
    
    def set_provider(self, provider):
        """Change tile provider"""
        if provider in TILE_PROVIDERS:
            self.provider = provider
            self.config = TILE_PROVIDERS[provider]
            self.cache.clear()

# =============================================================================
# MOUSE CONTROLLER (Cesium-style)
# =============================================================================

class MouseController:
    """Cesium-style mouse interaction"""
    def __init__(self, camera):
        self.camera = camera
        self.last_x = 0
        self.last_y = 0
        self.is_left_down = False
        self.is_right_down = False
        self.is_middle_down = False
        
        # Sensitivity
        self.rotate_speed = 0.005
        self.zoom_speed = 0.1
        self.pan_speed = 0.01
    
    def on_mouse_down(self, button, x, y):
        if button == glut.GLUT_LEFT_BUTTON:
            self.is_left_down = True
        elif button == glut.GLUT_RIGHT_BUTTON:
            self.is_right_down = True
        elif button == glut.GLUT_MIDDLE_BUTTON:
            self.is_middle_down = True
        
        self.last_x = x
        self.last_y = y
    
    def on_mouse_up(self, button, x, y):
        if button == glut.GLUT_LEFT_BUTTON:
            self.is_left_down = False
        elif button == glut.GLUT_RIGHT_BUTTON:
            self.is_right_down = False
        elif button == glut.GLUT_MIDDLE_BUTTON:
            self.is_middle_down = False
    
    def on_mouse_move(self, x, y):
        dx = x - self.last_x
        dy = y - self.last_y
        self.last_x = x
        self.last_y = y
        
        if self.is_left_down:
            # Left button: rotate around earth
            self.camera.longitude += dx * self.rotate_speed
            self.camera.latitude += dy * self.rotate_speed
            self.camera.latitude = max(-math.pi/2 + 0.01, min(math.pi/2 - 0.01, self.camera.latitude))
        
        elif self.is_right_down:
            # Right button: pan (change target) - simplified to zoom for now
            pass
        
        elif self.is_middle_down:
            # Middle button: zoom
            self.camera.height *= (1.0 + dy * self.zoom_speed)
            self.camera.height = max(100000, min(50000000, self.camera.height))
    
    def on_mouse_wheel(self, delta, x, y):
        # Zoom with mouse wheel
        self.camera.height *= (1.0 - delta * self.zoom_speed * 0.1)
        self.camera.height = max(100000, min(50000000, self.camera.height))

# =============================================================================
# CAMERA
# =============================================================================

class Camera:
    def __init__(self):
        self.longitude = math.radians(104.0)  # China
        self.latitude = math.radians(35.0)
        self.height = 15000000.0
        self.fov = 45.0
        self.aspect = 16.0 / 9.0
    
    def get_position(self):
        cos_lat = math.cos(self.latitude)
        sin_lat = math.sin(self.latitude)
        cos_lon = math.cos(self.longitude)
        sin_lon = math.sin(self.longitude)
        
        radius = EARTH_RADIUS + self.height
        x = radius * cos_lat * cos_lon * SCALE
        y = radius * sin_lat * SCALE
        z = radius * cos_lat * sin_lon * SCALE
        
        return np.array([x, y, z])
    
    def apply(self):
        pos = self.get_position()
        glu.gluLookAt(pos[0], pos[1], pos[2], 0, 0, 0, 0, 1, 0)
    
    def get_view_direction(self):
        """Get camera view direction"""
        return -self.get_position()  # Looking at origin

# =============================================================================
# MAIN APPLICATION
# =============================================================================

class CesiumEarth:
    def __init__(self):
        self.width = 1280
        self.height = 720
        self.camera = Camera()
        self.mouse = MouseController(self.camera)
        self.tile_loader = TileLoader(provider='arcgis')
        
        self.frame_count = 0
        self.last_time = time.time()
        self.fps = 0
        
        # Simple earth display list
        self.earth_list = None
        self.current_provider = 'arcgis'
    
    def init(self):
        gl.glClearColor(0.05, 0.05, 0.1, 1.0)
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_TEXTURE_2D)
        
        # Create earth sphere
        self.create_earth_sphere()
        
        # Start tile loader
        self.tile_loader.start()
        
        print("Cesium Earth initialized")
        print(f"Current provider: {self.current_provider}")
    
    def create_earth_sphere(self):
        """Create earth sphere display list"""
        self.earth_list = gl.glGenLists(1)
        gl.glNewList(self.earth_list, gl.GL_COMPILE)
        
        radius = EARTH_RADIUS * SCALE
        stacks = 64
        slices = 128
        
        for i in range(stacks):
            lat0 = math.pi * (-0.5 + float(i) / stacks)
            lat1 = math.pi * (-0.5 + float(i + 1) / stacks)
            
            gl.glBegin(gl.GL_TRIANGLE_STRIP)
            for j in range(slices + 1):
                lng = 2 * math.pi * float(j) / slices
                
                for lat in [lat0, lat1]:
                    x = math.cos(lat) * math.cos(lng)
                    y = math.sin(lat)
                    z = math.cos(lat) * math.sin(lng)
                    
                    # Texture coordinates
                    u = 1.0 - float(j) / slices
                    v = 1.0 - (lat + math.pi / 2) / math.pi
                    
                    gl.glTexCoord2f(u, v)
                    gl.glNormal3f(x, y, z)
                    gl.glVertex3f(x * radius, y * radius, z * radius)
            gl.glEnd()
        
        gl.glEndList()
    
    def display(self):
        # Update FPS
        current_time = time.time()
        self.frame_count += 1
        if current_time - self.last_time >= 1.0:
            self.fps = self.frame_count
            self.frame_count = 0
            self.last_time = current_time
        
        # Clear
        gl.glClear(gl.GL_COLOR_BUFFER_BIT | gl.GL_DEPTH_BUFFER_BIT)
        
        # Setup matrices
        gl.glMatrixMode(gl.GL_PROJECTION)
        gl.glLoadIdentity()
        glu.gluPerspective(self.camera.fov, self.camera.aspect, 1.0, 100000.0)
        
        gl.glMatrixMode(gl.GL_MODELVIEW)
        gl.glLoadIdentity()
        self.camera.apply()
        
        # Render sky
        self.render_sky()
        
        # Render earth
        self.render_earth()
        
        # Render HUD
        self.render_hud()
        
        glut.glutSwapBuffers()
        glut.glutPostRedisplay()
    
    def render_sky(self):
        """Render simple sky gradient"""
        gl.glDisable(gl.GL_DEPTH_TEST)
        gl.glDisable(gl.GL_TEXTURE_2D)
        
        # Simple sky dome
        radius = EARTH_RADIUS * SCALE * 100
        
        gl.glBegin(gl.GL_QUADS)
        
        # Top - space black
        gl.glColor3f(0.0, 0.0, 0.05)
        for _ in range(4):
            gl.glVertex3f(0, radius, 0)
        
        # Sides - gradient
        for i in range(8):
            angle0 = 2 * math.pi * i / 8
            angle1 = 2 * math.pi * (i + 1) / 8
            
            x0 = math.cos(angle0) * radius
            z0 = math.sin(angle0) * radius
            x1 = math.cos(angle1) * radius
            z1 = math.sin(angle1) * radius
            
            # Space color
            gl.glColor3f(0.0, 0.0, 0.05)
            gl.glVertex3f(x0, radius * 0.5, z0)
            gl.glVertex3f(x1, radius * 0.5, z1)
            
            # Horizon color
            gl.glColor3f(0.1, 0.3, 0.6)
            gl.glVertex3f(x1, 0, z1)
            gl.glVertex3f(x0, 0, z0)
        
        gl.glEnd()
        
        gl.glEnable(gl.GL_DEPTH_TEST)
    
    def render_earth(self):
        """Render earth with tiles"""
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_CULL_FACE)
        
        # For now, use a default blue color since online tiles need more complex mapping
        gl.glDisable(gl.GL_TEXTURE_2D)
        gl.glColor3f(0.1, 0.3, 0.6)
        
        # Add simple lighting
        gl.glEnable(gl.GL_LIGHTING)
        gl.glEnable(gl.GL_LIGHT0)
        
        light_pos = [1.0, 1.0, 1.0, 0.0]
        gl.glLightfv(gl.GL_LIGHT0, gl.GL_POSITION, light_pos)
        gl.glLightfv(gl.GL_LIGHT0, gl.GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0])
        
        gl.glMaterialfv(gl.GL_FRONT, gl.GL_DIFFUSE, [0.1, 0.3, 0.6, 1.0])
        gl.glMaterialfv(gl.GL_FRONT, gl.GL_AMBIENT, [0.05, 0.05, 0.1, 1.0])
        gl.glMaterialfv(gl.GL_FRONT, gl.GL_SPECULAR, [0.3, 0.3, 0.3, 1.0])
        gl.glMaterialf(gl.GL_FRONT, gl.GL_SHININESS, 32.0)
        
        gl.glCallList(self.earth_list)
        
        gl.glDisable(gl.GL_LIGHTING)
        gl.glDisable(gl.GL_CULL_FACE)
    
    def render_hud(self):
        """Render HUD"""
        gl.glMatrixMode(gl.GL_PROJECTION)
        gl.glPushMatrix()
        gl.glLoadIdentity()
        gl.glOrtho(0, self.width, 0, self.height, -1, 1)
        
        gl.glMatrixMode(gl.GL_MODELVIEW)
        gl.glPushMatrix()
        gl.glLoadIdentity()
        
        gl.glDisable(gl.GL_DEPTH_TEST)
        
        # Background
        gl.glColor4f(0.0, 0.0, 0.0, 0.7)
        gl.glBegin(gl.GL_QUADS)
        gl.glVertex2f(10, self.height - 200)
        gl.glVertex2f(380, self.height - 200)
        gl.glVertex2f(380, self.height - 10)
        gl.glVertex2f(10, self.height - 10)
        gl.glEnd()
        
        gl.glColor3f(0.0, 1.0, 0.0)
        
        lines = [
            "PotatoEarth - Cesium Style",
            "=" * 30,
            f"FPS: {self.fps}",
            f"Provider: {self.current_provider.upper()}",
            "",
            f"Lat: {math.degrees(self.camera.latitude):.2f}",
            f"Lon: {math.degrees(self.camera.longitude):.2f}",
            f"Height: {self.camera.height/1000:.0f} km",
            "",
            "Controls:",
            "  Left Drag: Rotate",
            "  Right Drag: Pan",
            "  Wheel: Zoom",
            "",
            "[1] ArcGIS  [2] Gaode",
            "[3] OSM     [ESC] Exit"
        ]
        
        y = self.height - 30
        for line in lines:
            gl.glRasterPos2f(20, y)
            for c in line:
                glut.glutBitmapCharacter(glut.GLUT_BITMAP_9_BY_15, ord(c))
            y -= 18
        
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glMatrixMode(gl.GL_PROJECTION)
        gl.glPopMatrix()
        gl.glMatrixMode(gl.GL_MODELVIEW)
        gl.glPopMatrix()
    
    def keyboard(self, key, x, y):
        if key == b'1':
            self.tile_loader.set_provider('arcgis')
            self.current_provider = 'arcgis'
            print("Switched to ArcGIS")
        elif key == b'2':
            self.tile_loader.set_provider('gaode')
            self.current_provider = 'gaode'
            print("Switched to Gaode")
        elif key == b'3':
            self.tile_loader.set_provider('osm')
            self.current_provider = 'osm'
            print("Switched to OSM')
        elif key == b'\x1b':
            self.tile_loader.stop()
            sys.exit(0)
    
    def reshape(self, w, h):
        self.width = w
        self.height = h
        self.camera.aspect = w / h
        gl.glViewport(0, 0, w, h)

def main():
    print("=" * 60)
    print("PotatoEarth - Cesium Style")
    print("=" * 60)
    print("Features:")
    print("  - Cesium-style mouse control")
    print("  - Online tile support (ArcGIS, Gaode, OSM)")
    print("  - Async tile loading")
    print("=" * 60)
    
    glut.glutInit()
    glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGB | glut.GLUT_DEPTH)
    glut.glutInitWindowSize(1280, 720)
    glut.glutCreateWindow(b"PotatoEarth - Cesium Style")
    
    app = CesiumEarth()
    app.init()
    
    # Register callbacks
    glut.glutDisplayFunc(lambda: app.display())
    glut.glutReshapeFunc(lambda w, h: app.reshape(w, h))
    glut.glutKeyboardFunc(lambda k, x, y: app.keyboard(k, x, y))
    
    # Mouse callbacks
    glut.glutMouseFunc(lambda button, state, x, y: 
        app.mouse.on_mouse_down(button, x, y) if state == glut.GLUT_DOWN 
        else app.mouse.on_mouse_up(button, x, y))
    
    glut.glutMotionFunc(lambda x, y: app.mouse.on_mouse_move(x, y))
    glut.glutPassiveMotionFunc(lambda x, y: app.mouse.on_mouse_move(x, y))
    
    # Mouse wheel (if available)
    try:
        glut.glutMouseWheelFunc(lambda wheel, direction, x, y: 
            app.mouse.on_mouse_wheel(direction, x, y))
    except:
        pass  # Not all GLUT versions support this
    
    print("Ready! Click and drag to rotate.")
    glut.glutMainLoop()

if __name__ == "__main__":
    main()
