"""
PotatoEarth - Real Online Tile Viewer
Loads and displays actual ArcGIS/高德/OSM imagery tiles
"""

import numpy as np
import OpenGL.GL as gl
import OpenGL.GLUT as glut
import OpenGL.GLU as glu
from PIL import Image
import math
import sys
import time
import urllib.request
import io

# =============================================================================
# CONFIGURATION
# =============================================================================

ZOOM_LEVEL = 2  # Start with zoom level 2 (whole earth view)
TILE_SIZE = 256

# Tile providers (URL templates)
PROVIDERS = {
    'arcgis': 'https://services.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}',
    'osm': 'https://tile.openstreetmap.org/{z}/{x}/{y}.png',
    'gaode': 'https://webrd01.is.autonavi.com/appmaptile?lang=zh_cn&size=1&scale=1&style=8&x={x}&y={y}&z={z}',
    'satellite': 'https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}'
}

CURRENT_PROVIDER = 'arcgis'

# =============================================================================
# TILE LOADER
# =============================================================================

def load_tile(x, y, z, provider='arcgis'):
    """Load a single tile from online source"""
    url = PROVIDERS[provider].format(x=x, y=y, z=z)
    
    try:
        headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'}
        req = urllib.request.Request(url, headers=headers)
        
        with urllib.request.urlopen(req, timeout=10) as response:
            data = response.read()
        
        # Convert to texture
        img = Image.open(io.BytesIO(data))
        img = img.convert('RGB')
        img_data = np.array(img, dtype=np.uint8)
        
        # Create OpenGL texture
        texture_id = gl.glGenTextures(1)
        gl.glBindTexture(gl.GL_TEXTURE_2D, texture_id)
        
        gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_S, gl.GL_CLAMP_TO_EDGE)
        gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_T, gl.GL_CLAMP_TO_EDGE)
        gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MIN_FILTER, gl.GL_LINEAR)
        gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MAG_FILTER, gl.GL_LINEAR)
        
        gl.glTexImage2D(gl.GL_TEXTURE_2D, 0, gl.GL_RGB, img.width, img.height, 0,
                       gl.GL_RGB, gl.GL_UNSIGNED_BYTE, img_data)
        
        return texture_id
    except Exception as e:
        print(f"Failed to load tile {x},{y},{z}: {e}")
        return None

# =============================================================================
# EARTH RENDERER WITH TILES
# =============================================================================

class TiledEarth:
    def __init__(self):
        self.width = 1280
        self.height = 720
        
        # Camera
        self.lon = 0.0  # Longitude in radians
        self.lat = 0.0  # Latitude in radians
        self.distance = 3.0  # Distance from center
        
        # Mouse control
        self.last_mouse_x = 0
        self.last_mouse_y = 0
        self.is_dragging = False
        
        # Tiles
        self.tiles = {}  # (x, y, z) -> texture_id
        self.current_zoom = 2
        self.target_zoom = 2
        
        # Provider
        self.provider = 'arcgis'
        
        # FPS
        self.fps = 0
        self.frame_count = 0
        self.last_time = time.time()
    
    def init(self):
        gl.glClearColor(0.0, 0.0, 0.05, 1.0)
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_TEXTURE_2D)
        
        # Load initial tiles
        self.load_visible_tiles()
        
        print(f"Initialized with {len(self.tiles)} tiles")
    
    def load_visible_tiles(self):
        """Load tiles for current view"""
        z = self.current_zoom
        
        # Calculate tile range based on zoom
        num_tiles = 2 ** z
        
        # For now, load all tiles at current zoom (simplified)
        # In a real implementation, you'd only load visible tiles
        for x in range(num_tiles):
            for y in range(num_tiles):
                tile_key = (x, y, z)
                if tile_key not in self.tiles:
                    texture = load_tile(x, y, z, self.provider)
                    if texture:
                        self.tiles[tile_key] = texture
                        print(f"Loaded tile {x},{y},{z}")
    
    def render_earth(self):
        """Render earth with tiles"""
        z = self.current_zoom
        num_tiles = 2 ** z
        
        # Render each tile
        for (tx, ty, tz), texture in self.tiles.items():
            if tz != z:
                continue  # Skip tiles from other zoom levels
            
            gl.glBindTexture(gl.GL_TEXTURE_2D, texture)
            
            # Calculate tile bounds in spherical coordinates
            lon_min = (tx / num_tiles) * 2 * math.pi - math.pi
            lon_max = ((tx + 1) / num_tiles) * 2 * math.pi - math.pi
            lat_max = math.pi / 2 - (ty / num_tiles) * math.pi
            lat_min = math.pi / 2 - ((ty + 1) / num_tiles) * math.pi
            
            # Render tile as quad on sphere
            self.render_tile_quad(lon_min, lon_max, lat_min, lat_max)
    
    def render_tile_quad(self, lon_min, lon_max, lat_min, lat_max):
        """Render a single tile quad"""
        radius = 1.0
        segments = 8  # Subdivide tile for curvature
        
        gl.glBegin(gl.GL_QUADS)
        
        for i in range(segments):
            for j in range(segments):
                # Calculate normalized coordinates
                u0 = i / segments
                u1 = (i + 1) / segments
                v0 = j / segments
                v1 = (j + 1) / segments
                
                # Interpolate lat/lon
                lon0 = lon_min + u0 * (lon_max - lon_min)
                lon1 = lon_min + u1 * (lon_max - lon_min)
                lat0 = lat_min + v0 * (lat_max - lat_min)
                lat1 = lat_min + v1 * (lat_max - lat_min)
                
                # Render quad
                for lon, lat, u, v in [
                    (lon0, lat0, u0, v0),
                    (lon1, lat0, u1, v0),
                    (lon1, lat1, u1, v1),
                    (lon0, lat1, u0, v1)
                ]:
                    # Spherical coordinates
                    x = math.cos(lat) * math.cos(lon) * radius
                    y = math.sin(lat) * radius
                    z = math.cos(lat) * math.sin(lon) * radius
                    
                    # Normal
                    nx = x / radius
                    ny = y / radius
                    nz = z / radius
                    
                    gl.glTexCoord2f(u, v)
                    gl.glNormal3f(nx, ny, nz)
                    gl.glVertex3f(x, y, z)
        
        gl.glEnd()
    
    def render_sky(self):
        """Render starfield background"""
        gl.glDisable(gl.GL_DEPTH_TEST)
        gl.glDisable(gl.GL_TEXTURE_2D)
        
        # Simple gradient sky
        radius = 50.0
        
        gl.glBegin(gl.GL_QUADS)
        
        for i in range(8):
            angle0 = 2 * math.pi * i / 8
            angle1 = 2 * math.pi * (i + 1) / 8
            
            x0 = math.cos(angle0) * radius
            z0 = math.sin(angle0) * radius
            x1 = math.cos(angle1) * radius
            z1 = math.sin(angle1) * radius
            
            # Top - space black
            gl.glColor3f(0.0, 0.0, 0.02)
            gl.glVertex3f(x0, radius * 0.3, z0)
            gl.glVertex3f(x1, radius * 0.3, z1)
            
            # Bottom - horizon blue
            gl.glColor3f(0.1, 0.3, 0.6)
            gl.glVertex3f(x1, 0, z1)
            gl.glVertex3f(x0, 0, z0)
        
        gl.glEnd()
        
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_TEXTURE_2D)
    
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
        
        # Setup projection
        gl.glMatrixMode(gl.GL_PROJECTION)
        gl.glLoadIdentity()
        glu.gluPerspective(45.0, self.width / self.height, 0.1, 1000.0)
        
        # Setup modelview
        gl.glMatrixMode(gl.GL_MODELVIEW)
        gl.glLoadIdentity()
        
        # Camera
        dist = self.distance
        x = math.cos(self.lat) * math.sin(self.lon) * dist
        y = math.sin(self.lat) * dist
        z = math.cos(self.lat) * math.cos(self.lon) * dist
        glu.gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0)
        
        # Render sky
        self.render_sky()
        
        # Render earth with tiles
        self.render_earth()
        
        # Render HUD
        self.render_hud()
        
        glut.glutSwapBuffers()
        glut.glutPostRedisplay()
    
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
        gl.glDisable(gl.GL_TEXTURE_2D)
        
        # Background panel
        gl.glColor4f(0.0, 0.0, 0.0, 0.8)
        gl.glBegin(gl.GL_QUADS)
        gl.glVertex2f(10, self.height - 180)
        gl.glVertex2f(350, self.height - 180)
        gl.glVertex2f(350, self.height - 10)
        gl.glVertex2f(10, self.height - 10)
        gl.glEnd()
        
        gl.glColor3f(0.0, 1.0, 0.0)
        
        lines = [
            "PotatoEarth - Real Online Tiles",
            "=" * 35,
            f"FPS: {self.fps}",
            f"Provider: {self.provider.upper()}",
            f"Zoom: {self.current_zoom}",
            f"Tiles: {len(self.tiles)}",
            "",
            "Left Drag: Rotate",
            "Wheel: Zoom",
            "",
            "[1] ArcGIS  [2] OSM",
            "[3] Gaode   [R] Reload"
        ]
        
        y = self.height - 30
        for line in lines:
            gl.glRasterPos2f(20, y)
            for c in line:
                glut.glutBitmapCharacter(glut.GLUT_BITMAP_9_BY_15, ord(c))
            y -= 16
        
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_TEXTURE_2D)
        
        gl.glMatrixMode(gl.GL_PROJECTION)
        gl.glPopMatrix()
        gl.glMatrixMode(gl.GL_MODELVIEW)
        gl.glPopMatrix()
    
    def mouse(self, button, state, x, y):
        if button == glut.GLUT_LEFT_BUTTON:
            if state == glut.GLUT_DOWN:
                self.is_dragging = True
                self.last_mouse_x = x
                self.last_mouse_y = y
            else:
                self.is_dragging = False
        elif button == 3:  # Wheel up
            self.zoom_in()
        elif button == 4:  # Wheel down
            self.zoom_out()
    
    def motion(self, x, y):
        if self.is_dragging:
            dx = x - self.last_mouse_x
            dy = y - self.last_mouse_y
            
            self.lon += dx * 0.01
            self.lat += dy * 0.01
            
            self.lat = max(-math.pi/2 + 0.1, min(math.pi/2 - 0.1, self.lat))
            
            self.last_mouse_x = x
            self.last_mouse_y = y
    
    def zoom_in(self):
        self.current_zoom = min(5, self.current_zoom + 1)
        self.load_visible_tiles()
    
    def zoom_out(self):
        self.current_zoom = max(0, self.current_zoom - 1)
        # Clear tiles and reload
        self.tiles.clear()
        self.load_visible_tiles()
    
    def keyboard(self, key, x, y):
        if key == b'1':
            self.provider = 'arcgis'
            self.tiles.clear()
            self.load_visible_tiles()
        elif key == b'2':
            self.provider = 'osm'
            self.tiles.clear()
            self.load_visible_tiles()
        elif key == b'3':
            self.provider = 'gaode'
            self.tiles.clear()
            self.load_visible_tiles()
        elif key == b'r' or key == b'R':
            self.tiles.clear()
            self.load_visible_tiles()
        elif key == b'\x1b':
            sys.exit(0)
    
    def reshape(self, w, h):
        self.width = w
        self.height = h
        gl.glViewport(0, 0, w, h)

def main():
    print("=" * 60)
    print("PotatoEarth - Real Online Tile Viewer")
    print("=" * 60)
    print("Loading tiles from:")
    print("  - ArcGIS World Imagery")
    print("  - OpenStreetMap")
    print("  - Gaode (Amap)")
    print("=" * 60)
    print("Loading initial tiles...")
    print("This may take a moment...")
    print("=" * 60)
    
    glut.glutInit()
    glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGB | glut.GLUT_DEPTH)
    glut.glutInitWindowSize(1280, 720)
    glut.glutCreateWindow(b"PotatoEarth - Real Online Tiles")
    
    app = TiledEarth()
    app.init()
    
    # Callbacks
    glut.glutDisplayFunc(lambda: app.display())
    glut.glutReshapeFunc(lambda w, h: app.reshape(w, h))
    glut.glutMouseFunc(lambda b, s, x, y: app.mouse(b, s, x, y))
    glut.glutMotionFunc(lambda x, y: app.motion(x, y))
    glut.glutKeyboardFunc(lambda k, x, y: app.keyboard(k, x, y))
    
    print("Ready!")
    glut.glutMainLoop()

if __name__ == "__main__":
    main()
