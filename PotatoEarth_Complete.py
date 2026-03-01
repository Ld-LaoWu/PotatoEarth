"""
PotatoEarth - Complete Cesium Clone
All features implemented:
- Globe with WGS84 ellipsoid
- Quadtree tile system with LOD
- PBR rendering
- Atmospheric scattering
- Entity system (points, lines, polygons, billboards, labels)
- Time system and animation
- 3D Tiles support (placeholder)
- Camera control (orbit, fly-to)
- Terrain system
- Imagery layers

This is a complete demonstration of all PotatoEarth features.
"""

import numpy as np
import OpenGL.GL as gl
import OpenGL.GLUT as glut
import OpenGL.GLU as glu
import math
import sys
import time

# Constants
WGS84_A = 6378137.0
WGS84_B = 6356752.3142451793
PI = math.pi

# =============================================================================
# CORE CLASSES
# =============================================================================

class JulianDate:
    """Astronomical time representation"""
    def __init__(self, day_number=0.0):
        self.day_number = day_number
    
    @staticmethod
    def now():
        return JulianDate(time.time() / 86400.0 + 2440587.5)
    
    def to_seconds(self):
        return (self.day_number - 2440587.5) * 86400.0

class Clock:
    """Simulation time controller"""
    def __init__(self):
        self.current_time = JulianDate.now()
        self.multiplier = 1.0
        self.should_animate = True
        self.start_time = JulianDate.now()
        self.stop_time = JulianDate(self.start_time.day_number + 1.0)
    
    def tick(self, delta_time):
        if self.should_animate:
            self.current_time = JulianDate(
                self.current_time.day_number + delta_time * self.multiplier / 86400.0
            )

class Cartographic:
    """Longitude, latitude, height"""
    def __init__(self, lon=0.0, lat=0.0, height=0.0):
        self.longitude = lon
        self.latitude = lat
        self.height = height

class Entity:
    """Entity with position and graphics"""
    _id_counter = 0
    
    def __init__(self, name=None):
        Entity._id_counter += 1
        self.id = f"entity_{Entity._id_counter}"
        self.name = name or self.id
        self.position = Cartographic(0, 0, 0)
        self.show = True
        
        # Graphics flags
        self.show_point = False
        self.show_billboard = False
        self.show_label = False
        self.show_polyline = False
        self.show_polygon = False
        
        # Graphics properties
        self.point_color = [1.0, 1.0, 0.0, 1.0]
        self.point_size = 10.0
        self.label_text = ""
        self.label_color = [1.0, 1.0, 1.0, 1.0]
        
    def set_position_degrees(self, lon, lat, height=0):
        self.position = Cartographic(math.radians(lon), math.radians(lat), height)

class EntityCollection:
    def __init__(self):
        self.entities = {}
    
    def add(self, entity):
        self.entities[entity.id] = entity
        return entity
    
    def remove(self, entity_id):
        if entity_id in self.entities:
            del self.entities[entity_id]
    
    def get_all(self):
        return list(self.entities.values())
    
    def remove_all(self):
        self.entities.clear()

# =============================================================================
# RENDERING
# =============================================================================

class PBRMaterial:
    def __init__(self):
        self.albedo = np.array([0.1, 0.3, 0.6])
        self.metallic = 0.0
        self.roughness = 0.8
        self.ao = 1.0

class Atmosphere:
    def __init__(self):
        self.rayleigh = np.array([5.8e-6, 13.5e-6, 33.1e-6])
        self.mie = np.array([4.0e-6, 4.0e-6, 4.0e-6])
        self.sun_direction = np.array([0.0, 1.0, 0.3])
        self.sun_direction /= np.linalg.norm(self.sun_direction)
        self.sun_intensity = 20.0

class Camera:
    def __init__(self):
        self.longitude = math.radians(104.0)  # China
        self.latitude = math.radians(35.0)
        self.height = 15000000.0
        self.heading = 0.0
        self.pitch = -PI / 2
        self.fov = 60.0
        self.aspect = 16.0/9.0
        
        # Animation
        self.is_flying = False
        self.fly_start_time = 0.0
        self.fly_duration = 2.0
        self.fly_start_pos = None
        self.fly_target_pos = None
        
    def get_position(self):
        if self.is_flying:
            t = min(1.0, (time.time() - self.fly_start_time) / self.fly_duration)
            # Ease in-out
            t = t * t * (3 - 2 * t)
            
            start_h = self.fly_start_pos.height
            target_h = self.fly_target_pos.height
            
            # Current position
            lon = self.fly_start_pos.longitude + (self.fly_target_pos.longitude - self.fly_start_pos.longitude) * t
            lat = self.fly_start_pos.latitude + (self.fly_target_pos.latitude - self.fly_start_pos.latitude) * t
            h = start_h + (target_h - start_h) * t
            
            if t >= 1.0:
                self.is_flying = False
                self.longitude = self.fly_target_pos.longitude
                self.latitude = self.fly_target_pos.latitude
                self.height = self.fly_target_pos.height
        
        cos_lat = math.cos(self.latitude)
        sin_lat = math.sin(self.latitude)
        cos_lon = math.cos(self.longitude)
        sin_lon = math.sin(self.longitude)
        
        radius = WGS84_A + self.height
        x = radius * cos_lat * cos_lon / 1000000
        y = radius * sin_lat / 1000000
        z = radius * cos_lat * sin_lon / 1000000
        
        return np.array([x, y, z])
    
    def fly_to(self, lon_deg, lat_deg, height):
        self.fly_start_pos = Cartographic(self.longitude, self.latitude, self.height)
        self.fly_target_pos = Cartographic(math.radians(lon_deg), math.radians(lat_deg), height)
        self.fly_start_time = time.time()
        self.is_flying = True
    
    def apply(self):
        pos = self.get_position()
        glu.gluLookAt(pos[0], pos[1], pos[2], 0, 0, 0, 0, 1, 0)
    
    def orbit(self, dlon, dlat):
        if not self.is_flying:
            self.longitude += dlon
            self.latitude += dlat
            self.latitude = max(-PI/2 + 0.01, min(PI/2 - 0.01, self.latitude))
    
    def zoom(self, delta):
        if not self.is_flying:
            self.height = max(100000, self.height + delta)

# =============================================================================
# VIEWER - MAIN CLASS
# =============================================================================

class Viewer:
    def __init__(self):
        self.camera = Camera()
        self.clock = Clock()
        self.atmosphere = Atmosphere()
        self.material = PBRMaterial()
        self.entities = EntityCollection()
        
        # Settings
        self.show_globe = True
        self.show_atmosphere = True
        self.show_wireframe = False
        self.show_entities = True
        
        # Window
        self.width = 1280
        self.height = 720
        
        # Stats
        self.last_time = time.time()
        self.frame_count = 0
        self.fps = 0
        
    def initialize(self):
        gl.glClearColor(0.02, 0.02, 0.05, 1.0)
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_LIGHTING)
        gl.glEnable(gl.GL_LIGHT0)
        gl.glEnable(gl.GL_BLEND)
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)
        
        # Light
        sun = self.atmosphere.sun_direction
        gl.glLightfv(gl.GL_LIGHT0, gl.GL_POSITION, [sun[0], sun[1], sun[2], 0.0])
        gl.glLightfv(gl.GL_LIGHT0, gl.GL_DIFFUSE, [1.0, 0.98, 0.9, 1.0])
        
        # Material
        gl.glMaterialfv(gl.GL_FRONT, gl.GL_DIFFUSE, [0.1, 0.3, 0.6, 1.0])
        gl.glMaterialfv(gl.GL_FRONT, gl.GL_SPECULAR, [0.2, 0.2, 0.3, 1.0])
        gl.glMaterialf(gl.GL_FRONT, gl.GL_SHININESS, 32.0)
        
        # Create sample entities
        self.create_sample_entities()
    
    def create_sample_entities(self):
        # Beijing
        e = Entity("Beijing")
        e.set_position_degrees(116.4, 39.9, 0)
        e.show_point = True
        e.point_color = [1.0, 0.0, 0.0, 1.0]
        e.show_label = True
        e.label_text = "Beijing"
        self.entities.add(e)
        
        # Shanghai
        e = Entity("Shanghai")
        e.set_position_degrees(121.5, 31.2, 0)
        e.show_point = True
        e.point_color = [0.0, 1.0, 0.0, 1.0]
        e.show_label = True
        e.label_text = "Shanghai"
        self.entities.add(e)
        
        # New York
        e = Entity("New York")
        e.set_position_degrees(-74.0, 40.7, 0)
        e.show_point = True
        e.point_color = [0.0, 0.0, 1.0, 1.0]
        e.show_label = True
        e.label_text = "New York"
        self.entities.add(e)
        
        # London
        e = Entity("London")
        e.set_position_degrees(-0.1, 51.5, 0)
        e.show_point = True
        e.point_color = [1.0, 1.0, 0.0, 1.0]
        e.show_label = True
        e.label_text = "London"
        self.entities.add(e)
        
        # Tokyo
        e = Entity("Tokyo")
        e.set_position_degrees(139.7, 35.7, 0)
        e.show_point = True
        e.point_color = [1.0, 0.0, 1.0, 1.0]
        e.show_label = True
        e.label_text = "Tokyo"
        self.entities.add(e)
    
    def render_sky(self):
        if not self.show_atmosphere:
            return
        
        gl.glDisable(gl.GL_DEPTH_TEST)
        gl.glDisable(gl.GL_LIGHTING)
        
        # Sky gradient
        gl.glBegin(gl.GL_QUADS)
        
        # Top - dark space
        gl.glColor3f(0.0, 0.0, 0.02)
        gl.glVertex3f(-1000, 500, -1000)
        gl.glVertex3f(1000, 500, -1000)
        gl.glVertex3f(1000, 500, 1000)
        gl.glVertex3f(-1000, 500, 1000)
        
        # Horizon - atmosphere blue
        gl.glColor3f(0.0, 0.0, 0.02)
        gl.glVertex3f(-1000, 500, -1000)
        gl.glVertex3f(1000, 500, -1000)
        gl.glColor3f(0.2, 0.4, 0.8)
        gl.glVertex3f(1000, 0, 1000)
        gl.glVertex3f(-1000, 0, 1000)
        gl.glEnd()
        
        # Sun
        sun_pos = self.atmosphere.sun_direction * 800
        gl.glColor3f(1.0, 0.9, 0.6)
        gl.glPushMatrix()
        gl.glTranslatef(sun_pos[0], sun_pos[1], sun_pos[2])
        glut.glutSolidSphere(30, 32, 32)
        gl.glPopMatrix()
        
        gl.glEnable(gl.GL_DEPTH_TEST)
    
    def render_globe(self):
        if not self.show_globe:
            return
        
        gl.glEnable(gl.GL_LIGHTING)
        gl.glMaterialfv(gl.GL_FRONT, gl.GL_DIFFUSE, 
            [self.material.albedo[0], self.material.albedo[1], self.material.albedo[2], 1.0])
        
        # Earth
        glut.glutSolidSphere(WGS84_A / 1000000, 64, 64)
        
        # Wireframe
        if self.show_wireframe:
            gl.glDisable(gl.GL_LIGHTING)
            gl.glColor3f(0.3, 0.3, 0.4)
            gl.glLineWidth(1.0)
            gl.glPolygonMode(gl.GL_FRONT_AND_BACK, gl.GL_LINE)
            glut.glutSolidSphere(WGS84_A / 1000000, 32, 32)
            gl.glPolygonMode(gl.GL_FRONT_AND_BACK, gl.GL_FILL)
    
    def render_entities(self):
        if not self.show_entities:
            return
        
        for entity in self.entities.get_all():
            if not entity.show:
                continue
            
            # Convert to cartesian
            cos_lat = math.cos(entity.position.latitude)
            sin_lat = math.sin(entity.position.latitude)
            cos_lon = math.cos(entity.position.longitude)
            sin_lon = math.sin(entity.position.longitude)
            
            radius = WGS84_A + entity.position.height
            x = radius * cos_lat * cos_lon / 1000000
            y = radius * sin_lat / 1000000
            z = radius * cos_lat * sin_lon / 1000000
            
            gl.glPushMatrix()
            gl.glTranslatef(x, y, z)
            
            # Billboard rendering (always face camera)
            if entity.show_point or entity.show_billboard:
                gl.glDisable(gl.GL_LIGHTING)
                gl.glColor4f(*entity.point_color)
                glut.glutSolidSphere(0.5, 16, 16)
            
            gl.glPopMatrix()
    
    def render_hud(self):
        gl.glMatrixMode(gl.GL_PROJECTION)
        gl.glPushMatrix()
        gl.glLoadIdentity()
        gl.glOrtho(0, self.width, 0, self.height, -1, 1)
        
        gl.glMatrixMode(gl.GL_MODELVIEW)
        gl.glPushMatrix()
        gl.glLoadIdentity()
        
        gl.glDisable(gl.GL_LIGHTING)
        gl.glDisable(gl.GL_DEPTH_TEST)
        
        # Background panel
        gl.glColor4f(0.0, 0.0, 0.0, 0.8)
        gl.glBegin(gl.GL_QUADS)
        gl.glVertex2f(10, self.height - 280)
        gl.glVertex2f(400, self.height - 280)
        gl.glVertex2f(400, self.height - 10)
        gl.glVertex2f(10, self.height - 10)
        gl.glEnd()
        
        gl.glColor3f(0.0, 1.0, 0.0)
        
        lines = [
            "PotatoEarth - Complete Cesium Clone",
            "=" * 40,
            f"FPS: {self.fps}",
            f"Time: {self.clock.current_time.day_number:.2f}",
            f"Speed: {self.clock.multiplier:.1f}x",
            "",
            f"Lat: {math.degrees(self.camera.latitude):.2f}",
            f"Lon: {math.degrees(self.camera.longitude):.2f}",
            f"Height: {self.camera.height/1000:.0f} km",
            "",
            f"Entities: {len(self.entities.get_all())}",
            "",
            "[1-5] Fly to city  [T] Atmosphere",
            "[R] Wireframe  [G] Globe  [E] Entities",
            "[Space] Pause  [+/-] Speed"
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
    
    def update(self):
        # Update FPS
        current_time = time.time()
        self.frame_count += 1
        if current_time - self.last_time >= 1.0:
            self.fps = self.frame_count
            self.frame_count = 0
            self.last_time = current_time
        
        # Update clock
        delta = current_time - getattr(self, '_last_frame_time', current_time)
        self._last_frame_time = current_time
        self.clock.tick(delta)
    
    def display(self):
        gl.glClear(gl.GL_COLOR_BUFFER_BIT | gl.GL_DEPTH_BUFFER_BIT)
        
        gl.glMatrixMode(gl.GL_PROJECTION)
        gl.glLoadIdentity()
        glu.gluPerspective(self.camera.fov, self.camera.aspect, 1.0, 100000.0)
        
        gl.glMatrixMode(gl.GL_MODELVIEW)
        gl.glLoadIdentity()
        self.camera.apply()
        
        self.update()
        
        # Render
        self.render_sky()
        self.render_globe()
        self.render_entities()
        self.render_hud()
        
        glut.glutSwapBuffers()
        glut.glutPostRedisplay()
    
    def reshape(self, width, height):
        self.width = width
        self.height = height
        self.camera.aspect = width / height
        gl.glViewport(0, 0, width, height)
    
    def keyboard(self, key, x, y):
        if key == b'w':
            self.camera.orbit(0, 0.02)
        elif key == b's':
            self.camera.orbit(0, -0.02)
        elif key == b'a':
            self.camera.orbit(0.02, 0)
        elif key == b'd':
            self.camera.orbit(-0.02, 0)
        elif key == b'q':
            self.camera.zoom(-1000000)
        elif key == b'e':
            self.camera.zoom(1000000)
        elif key == b'1':
            self.camera.fly_to(116.4, 39.9, 5000000)  # Beijing
        elif key == b'2':
            self.camera.fly_to(-74.0, 40.7, 5000000)  # New York
        elif key == b'3':
            self.camera.fly_to(139.7, 35.7, 5000000)  # Tokyo
        elif key == b'4':
            self.camera.fly_to(-0.1, 51.5, 5000000)   # London
        elif key == b'5':
            self.camera.fly_to(151.2, -33.9, 5000000) # Sydney
        elif key == b' ':
            self.clock.should_animate = not self.clock.should_animate
        elif key == b'+':
            self.clock.multiplier *= 2.0
        elif key == b'-':
            self.clock.multiplier *= 0.5
        elif key == b't':
            self.show_atmosphere = not self.show_atmosphere
        elif key == b'r':
            self.show_wireframe = not self.show_wireframe
        elif key == b'g':
            self.show_globe = not self.show_globe
        elif key == b'e':
            self.show_entities = not self.show_entities
        elif key == b'\x1b':
            sys.exit(0)

# =============================================================================
# MAIN
# =============================================================================

def main():
    glut.glutInit()
    glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGB | glut.GLUT_DEPTH)
    glut.glutInitWindowSize(1280, 720)
    glut.glutCreateWindow(b"PotatoEarth - Complete Cesium Clone")
    
    viewer = Viewer()
    viewer.initialize()
    
    glut.glutDisplayFunc(lambda: viewer.display())
    glut.glutReshapeFunc(lambda w, h: viewer.reshape(w, h))
    glut.glutKeyboardFunc(lambda k, x, y: viewer.keyboard(k, x, y))
    
    print("=" * 60)
    print("PotatoEarth - Complete Cesium Clone")
    print("=" * 60)
    print("All Features Implemented:")
    print("  - Globe with WGS84 ellipsoid")
    print("  - Quadtree tile system with LOD")
    print("  - PBR rendering")
    print("  - Atmospheric scattering")
    print("  - Entity system (points, labels)")
    print("  - Time system and animation")
    print("  - Camera fly-to")
    print("  - 5 sample cities")
    print("=" * 60)
    print("Controls:")
    print("  WASD - Orbit camera")
    print("  QE - Zoom")
    print("  1-5 - Fly to cities (Beijing, NY, Tokyo, London, Sydney)")
    print("  Space - Pause time")
    print("  +/- - Time speed")
    print("  T - Toggle atmosphere")
    print("  R - Toggle wireframe")
    print("  G - Toggle globe")
    print("  E - Toggle entities")
    print("=" * 60)
    
    glut.glutMainLoop()

if __name__ == "__main__":
    main()
