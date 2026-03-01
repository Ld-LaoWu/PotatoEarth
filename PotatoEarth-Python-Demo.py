import numpy as np
import OpenGL.GL as gl
import OpenGL.GLUT as glut
import OpenGL.GLU as glu
import math
import sys
from enum import Enum

# ============================================================================
# PotatoEarth - Complete Python Demo with Tile System
# ============================================================================

# WGS84 Constants
WGS84_A = 6378137.0
WGS84_B = 6356752.3142451793
WGS84_C = 6356752.3142451793

class TileState(Enum):
    UNLOADED = 0
    LOADING = 1
    LOADED = 2

class TileID:
    def __init__(self, level=0, x=0, y=0):
        self.level = level
        self.x = x
        self.y = y
    
    def __eq__(self, other):
        return self.level == other.level and self.x == other.x and self.y == other.y
    
    def __hash__(self):
        return hash((self.level, self.x, self.y))
    
    def get_child(self, index):
        """Get child tile ID (0=SW, 1=SE, 2=NW, 3=NE)"""
        return TileID(self.level + 1, self.x * 2 + (index % 2), self.y * 2 + (index // 2))

class Tile:
    """Represents a single terrain tile"""
    def __init__(self, tile_id):
        self.id = tile_id
        self.state = TileState.UNLOADED
        self.vertices = None
        self.indices = None
        self.vao = None
        self.vbo = None
        self.ibo = None
        self.bounds = self.calculate_bounds()
        
    def calculate_bounds(self):
        """Calculate geographic bounds of this tile"""
        n = 1 << self.id.level
        lon_west = (self.id.x / n) * 2 * math.pi - math.pi
        lon_east = ((self.id.x + 1) / n) * 2 * math.pi - math.pi
        
        mercator_y_n = math.pi * (1 - 2 * self.id.y / n)
        mercator_y_s = math.pi * (1 - 2 * (self.id.y + 1) / n)
        
        lat_north = math.atan(math.sinh(mercator_y_n))
        lat_south = math.atan(math.sinh(mercator_y_s))
        
        return {
            'west': lon_west, 'east': lon_east,
            'south': lat_south, 'north': lat_north
        }
    
    def generate_mesh(self):
        """Generate the tile mesh on WGS84 ellipsoid"""
        segments = 8
        vertices = []
        indices = []
        
        for iy in range(segments + 1):
            lat_frac = iy / segments
            lat = self.bounds['south'] * (1 - lat_frac) + self.bounds['north'] * lat_frac
            
            for ix in range(segments + 1):
                lon_frac = ix / segments
                lon = self.bounds['west'] * (1 - lon_frac) + self.bounds['east'] * lon_frac
                
                # Convert to Cartesian on WGS84
                cos_lat = math.cos(lat)
                sin_lat = math.sin(lat)
                cos_lon = math.cos(lon)
                sin_lon = math.sin(lon)
                
                # WGS84 radius at latitude
                radius = WGS84_A * WGS84_B / math.sqrt((WGS84_B * cos_lat)**2 + (WGS84_A * sin_lat)**2)
                
                x = radius * cos_lat * cos_lon / 1000000  # Scale down for rendering
                y = radius * sin_lat / 1000000
                z = radius * cos_lat * sin_lon / 1000000
                
                # Normal (simplified as position normalized)
                normal_len = math.sqrt(x*x + y*y + z*z)
                nx, ny, nz = x/normal_len, y/normal_len, z/normal_len
                
                # UV coordinates
                u = lon_frac
                v = lat_frac
                
                vertices.extend([x, y, z, nx, ny, nz, u, v])
        
        # Generate indices
        for iy in range(segments):
            for ix in range(segments):
                current = iy * (segments + 1) + ix
                next_row = current + segments + 1
                
                indices.extend([current, next_row, current + 1])
                indices.extend([current + 1, next_row, next_row + 1])
        
        self.vertices = np.array(vertices, dtype=np.float32)
        self.indices = np.array(indices, dtype=np.uint32)
        self.state = TileState.LOADED
        
    def render(self):
        """Render the tile"""
        if self.state != TileState.LOADED:
            return
            
        if self.vertices is None:
            self.generate_mesh()
        
        # Draw as wireframe with fill
        gl.glEnableClientState(gl.GL_VERTEX_ARRAY)
        gl.glEnableClientState(gl.GL_NORMAL_ARRAY)
        
        stride = 8 * 4  # 8 floats, 4 bytes each
        
        # Position at offset 0
        gl.glVertexPointer(3, gl.GL_FLOAT, stride, self.vertices)
        # Normal at offset 3
        gl.glNormalPointer(gl.GL_FLOAT, stride, self.vertices[3:])
        
        gl.glDrawElements(gl.GL_TRIANGLES, len(self.indices), gl.GL_UNSIGNED_INT, self.indices)
        
        gl.glDisableClientState(gl.GL_VERTEX_ARRAY)
        gl.glDisableClientState(gl.GL_NORMAL_ARRAY)

class TileQuadtree:
    """Manages the tile quadtree with LOD"""
    def __init__(self):
        self.root = None
        self.max_level = 8
        self.screen_error_threshold = 50.0
        self.tile_cache = {}
        
    def initialize(self):
        """Create root tile"""
        self.root = Tile(TileID(0, 0, 0))
        self.root.generate_mesh()
        self.tile_cache[self.root.id] = self.root
        
    def update(self, camera_pos, camera_height):
        """Update LOD based on camera position"""
        if self.root:
            self._update_node(self.root, camera_pos, camera_height)
            
    def _update_node(self, tile, camera_pos, camera_height):
        """Recursively update tile LOD"""
        # Calculate tile center
        center_lon = (tile.bounds['west'] + tile.bounds['east']) / 2
        center_lat = (tile.bounds['south'] + tile.bounds['north']) / 2
        
        # Approximate distance from camera to tile
        # Simple approximation using lat/lon difference
        dx = (center_lon - camera_pos[0]) * WGS84_A * math.cos(camera_pos[1])
        dy = (center_lat - camera_pos[1]) * WGS84_A
        distance = math.sqrt(dx*dx + dy*dy)
        
        # Tile size at this level
        tile_size = 40075000.0 / (1 << tile.id.level)
        
        # Screen space error approximation
        screen_error = tile_size / (distance + 1) * 1000
        
        should_refine = screen_error > self.screen_error_threshold and tile.id.level < self.max_level
        
        if should_refine:
            # Create children if needed
            if not hasattr(tile, 'children'):
                tile.children = []
                for i in range(4):
                    child_id = tile.id.get_child(i)
                    if child_id in self.tile_cache:
                        child = self.tile_cache[child_id]
                    else:
                        child = Tile(child_id)
                        child.generate_mesh()
                        self.tile_cache[child_id] = child
                    tile.children.append(child)
            
            # Update children
            for child in tile.children:
                self._update_node(child, camera_pos, camera_height)
        else:
            # Remove children if exist
            if hasattr(tile, 'children'):
                del tile.children
    
    def render(self):
        """Render the quadtree"""
        if self.root:
            self._render_node(self.root)
    
    def _render_node(self, tile):
        """Recursively render tiles"""
        if hasattr(tile, 'children') and tile.children:
            # Check if any child is visible (simplified - always render children if exist)
            for child in tile.children:
                self._render_node(child)
        else:
            # Render this tile
            tile.render()
            # Draw tile border for debugging
            self._draw_tile_border(tile)
    
    def _draw_tile_border(self, tile):
        """Draw tile border for debugging"""
        gl.glColor3f(0.3, 0.3, 0.3)
        gl.glLineWidth(1.0)
        gl.glBegin(gl.GL_LINE_LOOP)
        
        corners = [
            (tile.bounds['west'], tile.bounds['south']),
            (tile.bounds['east'], tile.bounds['south']),
            (tile.bounds['east'], tile.bounds['north']),
            (tile.bounds['west'], tile.bounds['north'])
        ]
        
        for lon, lat in corners:
            cos_lat = math.cos(lat)
            sin_lat = math.sin(lat)
            cos_lon = math.cos(lon)
            sin_lon = math.sin(lon)
            
            radius = WGS84_A * WGS84_B / math.sqrt((WGS84_B * cos_lat)**2 + (WGS84_A * sin_lat)**2)
            x = radius * cos_lat * cos_lon / 1000000
            y = radius * sin_lat / 1000000
            z = radius * cos_lat * sin_lon / 1000000
            
            gl.glVertex3f(x, y, z)
        
        gl.glEnd()

class EarthOrbitCamera:
    """Camera that orbits around Earth"""
    def __init__(self):
        self.longitude = math.radians(104.0)  # China
        self.latitude = math.radians(35.0)
        self.height = 15000000.0  # 15000km
        self.heading = 0.0
        self.pitch = -math.pi / 2
        self.fov = 60.0
        self.aspect = 16.0 / 9.0
        
    def get_position(self):
        """Get camera position in Cartesian"""
        cos_lat = math.cos(self.latitude)
        sin_lat = math.sin(self.latitude)
        cos_lon = math.cos(self.longitude)
        sin_lon = math.sin(self.longitude)
        
        radius = WGS84_A + self.height
        x = radius * cos_lat * cos_lon / 1000000
        y = radius * sin_lat / 1000000
        z = radius * cos_lat * sin_lon / 1000000
        
        return np.array([x, y, z])
    
    def apply(self):
        """Apply camera transform"""
        pos = self.get_position()
        
        # Look at center of Earth
        glu.gluLookAt(
            pos[0], pos[1], pos[2],
            0, 0, 0,
            0, 1, 0
        )
    
    def orbit(self, dlon, dlat):
        """Orbit around Earth"""
        self.longitude += dlon
        self.latitude += dlat
        self.latitude = max(-math.pi/2 + 0.01, min(math.pi/2 - 0.01, self.latitude))
    
    def zoom(self, delta):
        """Zoom in/out"""
        self.height = max(100000, self.height + delta)
    
    def rotate(self, dheading, dpitch):
        """Rotate view"""
        self.heading += dheading
        self.pitch += dpitch
        self.pitch = max(-math.pi/2 + 0.01, min(0.0, self.pitch))

# Global instances
camera = EarthOrbitCamera()
quadtree = TileQuadtree()
window_width = 1280
window_height = 720
show_wireframe = True
show_tiles = True

def init():
    """Initialize OpenGL"""
    gl.glClearColor(0.05, 0.05, 0.1, 1.0)
    gl.glEnable(gl.GL_DEPTH_TEST)
    gl.glEnable(gl.GL_LIGHTING)
    gl.glEnable(gl.GL_LIGHT0)
    
    # Light position
    gl.glLightfv(gl.GL_LIGHT0, gl.GL_POSITION, [1.0, 1.0, 1.0, 0.0])
    gl.glLightfv(gl.GL_LIGHT0, gl.GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0])
    gl.glLightfv(gl.GL_LIGHT0, gl.GL_AMBIENT, [0.2, 0.2, 0.2, 1.0])
    
    # Material
    gl.glMaterialfv(gl.GL_FRONT, gl.GL_DIFFUSE, [0.2, 0.4, 0.8, 1.0])
    gl.glMaterialfv(gl.GL_FRONT, gl.GL_AMBIENT, [0.1, 0.1, 0.2, 1.0])
    gl.glMaterialfv(gl.GL_FRONT, gl.GL_SPECULAR, [0.3, 0.3, 0.3, 1.0])
    gl.glMaterialf(gl.GL_FRONT, gl.GL_SHININESS, 32.0)
    
    # Initialize quadtree
    quadtree.initialize()

def display():
    """Render frame"""
    gl.glClear(gl.GL_COLOR_BUFFER_BIT | gl.GL_DEPTH_BUFFER_BIT)
    
    # Setup projection
    gl.glMatrixMode(gl.GL_PROJECTION)
    gl.glLoadIdentity()
    glu.gluPerspective(camera.fov, camera.aspect, 1.0, 100000.0)
    
    # Setup modelview
    gl.glMatrixMode(gl.GL_MODELVIEW)
    gl.glLoadIdentity()
    camera.apply()
    
    # Update quadtree LOD
    camera_geo = (camera.longitude, camera.latitude, camera.height)
    quadtree.update(camera_geo, camera.height)
    
    # Render tiles
    if show_tiles:
        gl.glEnable(gl.GL_LIGHTING)
        quadtree.render()
    
    # Draw wireframe sphere for reference
    if show_wireframe:
        gl.glDisable(gl.GL_LIGHTING)
        gl.glColor3f(0.2, 0.2, 0.4)
        gl.glLineWidth(1.0)
        gl.glPolygonMode(gl.GL_FRONT_AND_BACK, gl.GL_LINE)
        glut.glutWireSphere(WGS84_A / 1000000, 36, 18)
        gl.glPolygonMode(gl.GL_FRONT_AND_BACK, gl.GL_FILL)
    
    # Draw coordinate axes
    gl.glDisable(gl.GL_LIGHTING)
    gl.glLineWidth(2.0)
    axis_length = WGS84_A / 1000000 * 1.5
    
    # X axis (red)
    gl.glColor3f(1.0, 0.0, 0.0)
    gl.glBegin(gl.GL_LINES)
    gl.glVertex3f(0, 0, 0)
    gl.glVertex3f(axis_length, 0, 0)
    gl.glEnd()
    
    # Y axis (green)
    gl.glColor3f(0.0, 1.0, 0.0)
    gl.glBegin(gl.GL_LINES)
    gl.glVertex3f(0, 0, 0)
    gl.glVertex3f(0, axis_length, 0)
    gl.glEnd()
    
    # Z axis (blue)
    gl.glColor3f(0.0, 0.0, 1.0)
    gl.glBegin(gl.GL_LINES)
    gl.glVertex3f(0, 0, 0)
    gl.glVertex3f(0, 0, axis_length)
    gl.glEnd()
    
    # Draw HUD
    draw_hud()
    
    glut.glutSwapBuffers()

def draw_hud():
    """Draw heads-up display"""
    gl.glMatrixMode(gl.GL_PROJECTION)
    gl.glPushMatrix()
    gl.glLoadIdentity()
    gl.glOrtho(0, window_width, 0, window_height, -1, 1)
    
    gl.glMatrixMode(gl.GL_MODELVIEW)
    gl.glPushMatrix()
    gl.glLoadIdentity()
    
    gl.glDisable(gl.GL_LIGHTING)
    gl.glDisable(gl.GL_DEPTH_TEST)
    
    # Background for text
    gl.glColor4f(0.0, 0.0, 0.0, 0.7)
    gl.glBegin(gl.GL_QUADS)
    gl.glVertex2f(10, window_height - 120)
    gl.glVertex2f(300, window_height - 120)
    gl.glVertex2f(300, window_height - 10)
    gl.glVertex2f(10, window_height - 10)
    gl.glEnd()
    
    # Text color
    gl.glColor3f(1.0, 1.0, 1.0)
    
    # Position info
    y = window_height - 30
    lon_deg = math.degrees(camera.longitude)
    lat_deg = math.degrees(camera.latitude)
    
    draw_text(20, y, f"Lat: {lat_deg:.2f} Lon: {lon_deg:.2f}")
    y -= 20
    draw_text(20, y, f"Height: {camera.height/1000:.0f} km")
    y -= 20
    draw_text(20, y, f"Tiles: {len(quadtree.tile_cache)}")
    y -= 20
    draw_text(20, y, "[WASD] Orbit [QE] Zoom [T] Toggle Tiles [R] Toggle Wireframe")
    
    gl.glEnable(gl.GL_DEPTH_TEST)
    gl.glMatrixMode(gl.GL_PROJECTION)
    gl.glPopMatrix()
    gl.glMatrixMode(gl.GL_MODELVIEW)
    gl.glPopMatrix()

def draw_text(x, y, text):
    """Draw text at screen position"""
    gl.glRasterPos2f(x, y)
    for char in text:
        glut.glutBitmapCharacter(glut.GLUT_BITMAP_9_BY_15, ord(char))

def reshape(width, height):
    """Handle window resize"""
    global window_width, window_height
    window_width = width
    window_height = height
    camera.aspect = width / height
    gl.glViewport(0, 0, width, height)

def keyboard(key, x, y):
    """Handle keyboard input"""
    global show_wireframe, show_tiles
    
    if key == b'w':
        camera.orbit(0, 0.02)
    elif key == b's':
        camera.orbit(0, -0.02)
    elif key == b'a':
        camera.orbit(0.02, 0)
    elif key == b'd':
        camera.orbit(-0.02, 0)
    elif key == b'q':
        camera.zoom(-1000000)
    elif key == b'e':
        camera.zoom(1000000)
    elif key == b'r':
        show_wireframe = not show_wireframe
    elif key == b't':
        show_tiles = not show_tiles
    elif key == b'\x1b':  # ESC
        sys.exit(0)
    
    glut.glutPostRedisplay()

def special_keys(key, x, y):
    """Handle special keys (arrows)"""
    if key == glut.GLUT_KEY_UP:
        camera.rotate(0, -0.02)
    elif key == glut.GLUT_KEY_DOWN:
        camera.rotate(0, 0.02)
    elif key == glut.GLUT_KEY_LEFT:
        camera.rotate(-0.02, 0)
    elif key == glut.GLUT_KEY_RIGHT:
        camera.rotate(0.02, 0)
    
    glut.glutPostRedisplay()

def idle():
    """Idle callback"""
    glut.glutPostRedisplay()

def main():
    """Main entry point"""
    glut.glutInit()
    glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGB | glut.GLUT_DEPTH)
    glut.glutInitWindowSize(window_width, window_height)
    glut.glutCreateWindow(b"PotatoEarth - Complete Test")
    
    init()
    
    glut.glutDisplayFunc(display)
    glut.glutReshapeFunc(reshape)
    glut.glutKeyboardFunc(keyboard)
    glut.glutSpecialFunc(special_keys)
    glut.glutIdleFunc(idle)
    
    print("=" * 60)
    print("PotatoEarth - Complete Test")
    print("=" * 60)
    print("Controls:")
    print("  W/S - Orbit north/south")
    print("  A/D - Orbit east/west")
    print("  Q/E - Zoom in/out")
    print("  Arrow Keys - Rotate view")
    print("  T - Toggle tile rendering")
    print("  R - Toggle wireframe")
    print("  ESC - Exit")
    print("=" * 60)
    
    glut.glutMainLoop()

if __name__ == "__main__":
    main()
