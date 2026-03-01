import numpy as np
import OpenGL.GL as gl
import OpenGL.GLUT as glut
import OpenGL.GLU as glu
import math
import sys

# PBR Earth Demo with Atmospheric Scattering

WGS84_A = 6378137.0
WGS84_B = 6356752.314314

class PBRMaterial:
    def __init__(self):
        self.albedo = np.array([0.1, 0.3, 0.6])  # Ocean blue
        self.metallic = 0.0
        self.roughness = 0.8
        self.ao = 1.0
        self.emissive = np.array([0.0, 0.0, 0.0])

class Atmosphere:
    def __init__(self):
        self.rayleigh_scattering = np.array([5.802e-6, 13.558e-6, 33.1e-6])
        self.rayleigh_scale_height = 8000.0
        self.mie_scattering = np.array([3.996e-6, 3.996e-6, 3.996e-6])
        self.mie_scale_height = 1200.0
        self.mie_g = 0.8
        self.sun_direction = np.array([0.0, 1.0, 0.5])
        self.sun_direction = self.sun_direction / np.linalg.norm(self.sun_direction)
        self.sun_intensity = 20.0
        self.ground_radius = 6360000.0
        self.atmosphere_radius = 6420000.0

class Camera:
    def __init__(self):
        self.longitude = math.radians(104.0)
        self.latitude = math.radians(35.0)
        self.height = 15000000.0
        self.fov = 60.0
        self.aspect = 16.0/9.0
        
    def get_position(self):
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
        pos = self.get_position()
        glu.gluLookAt(pos[0], pos[1], pos[2], 0, 0, 0, 0, 1, 0)

camera = Camera()
atmosphere = Atmosphere()
material = PBRMaterial()
window_width = 1280
window_height = 720
show_atmosphere = True
show_wireframe = False

# PBR Shader uniforms
pbr_uniforms = {}

def init():
    gl.glClearColor(0.05, 0.05, 0.1, 1.0)
    gl.glEnable(gl.GL_DEPTH_TEST)
    gl.glEnable(gl.GL_LIGHTING)
    gl.glEnable(gl.GL_LIGHT0)
    
    # Sun light
    sun_dir = atmosphere.sun_direction
    gl.glLightfv(gl.GL_LIGHT0, gl.GL_POSITION, [sun_dir[0], sun_dir[1], sun_dir[2], 0.0])
    gl.glLightfv(gl.GL_LIGHT0, gl.GL_DIFFUSE, [1.0, 0.98, 0.9, 1.0])
    gl.glLightfv(gl.GL_LIGHT0, gl.GL_AMBIENT, [0.1, 0.1, 0.15, 1.0])
    
    # Material
    gl.glMaterialfv(gl.GL_FRONT, gl.GL_DIFFUSE, [0.1, 0.3, 0.6, 1.0])
    gl.glMaterialfv(gl.GL_FRONT, gl.GL_AMBIENT, [0.05, 0.05, 0.1, 1.0])
    gl.glMaterialfv(gl.GL_FRONT, gl.GL_SPECULAR, [0.2, 0.2, 0.3, 1.0])
    gl.glMaterialf(gl.GL_FRONT, gl.GL_SHININESS, 10.0)

def render_sky():
    """Render atmospheric sky dome"""
    if not show_atmosphere:
        return
    
    gl.glDisable(gl.GL_DEPTH_TEST)
    gl.glDisable(gl.GL_LIGHTING)
    gl.glDisable(gl.GL_CULL_FACE)
    
    # Simple sky gradient based on sun direction
    camera_pos = camera.get_position()
    
    gl.glBegin(gl.GL_QUADS)
    
    # Top - dark blue/black
    gl.glColor3f(0.0, 0.02, 0.1)
    gl.glVertex3f(-1000, 1000, -1000)
    gl.glVertex3f(1000, 1000, -1000)
    gl.glVertex3f(1000, 1000, 1000)
    gl.glVertex3f(-1000, 1000, 1000)
    
    # Horizon - blue
    gl.glColor3f(0.2, 0.4, 0.8)
    gl.glVertex3f(-1000, 0, -1000)
    gl.glVertex3f(1000, 0, -1000)
    gl.glColor3f(0.0, 0.02, 0.1)
    gl.glVertex3f(1000, 1000, 1000)
    gl.glVertex3f(-1000, 1000, 1000)
    
    gl.glEnd()
    
    # Sun
    sun_pos = atmosphere.sun_direction * 500
    gl.glColor3f(1.0, 0.9, 0.6)
    gl.glPushMatrix()
    gl.glTranslatef(sun_pos[0], sun_pos[1], sun_pos[2])
    glut.glutSolidSphere(20, 32, 32)
    gl.glPopMatrix()
    
    gl.glEnable(gl.GL_DEPTH_TEST)
    gl.glEnable(gl.GL_CULL_FACE)

def render_earth_pbr():
    """Render Earth with PBR-like lighting"""
    gl.glEnable(gl.GL_LIGHTING)
    
    # Set material
    gl.glMaterialfv(gl.GL_FRONT, gl.GL_DIFFUSE, 
        [material.albedo[0], material.albedo[1], material.albedo[2], 1.0])
    gl.glMaterialfv(gl.GL_FRONT, gl.GL_SPECULAR, 
        [material.metallic * 0.5, material.metallic * 0.5, material.metallic * 0.5, 1.0])
    
    # Draw sphere
    glut.glutSolidSphere(WGS84_A / 1000000, 64, 64)
    
    # Wireframe overlay
    if show_wireframe:
        gl.glDisable(gl.GL_LIGHTING)
        gl.glColor3f(0.3, 0.3, 0.4)
        gl.glLineWidth(1.0)
        gl.glPolygonMode(gl.GL_FRONT_AND_BACK, gl.GL_LINE)
        glut.glutSolidSphere(WGS84_A / 1000000, 32, 32)
        gl.glPolygonMode(gl.GL_FRONT_AND_BACK, gl.GL_FILL)

def display():
    gl.glClear(gl.GL_COLOR_BUFFER_BIT | gl.GL_DEPTH_BUFFER_BIT)
    
    gl.glMatrixMode(gl.GL_PROJECTION)
    gl.glLoadIdentity()
    glu.gluPerspective(camera.fov, camera.aspect, 1.0, 100000.0)
    
    gl.glMatrixMode(gl.GL_MODELVIEW)
    gl.glLoadIdentity()
    camera.apply()
    
    # Render sky first
    render_sky()
    
    # Render Earth
    render_earth_pbr()
    
    # Draw HUD
    draw_hud()
    
    glut.glutSwapBuffers()

def draw_hud():
    gl.glMatrixMode(gl.GL_PROJECTION)
    gl.glPushMatrix()
    gl.glLoadIdentity()
    gl.glOrtho(0, window_width, 0, window_height, -1, 1)
    
    gl.glMatrixMode(gl.GL_MODELVIEW)
    gl.glPushMatrix()
    gl.glLoadIdentity()
    
    gl.glDisable(gl.GL_LIGHTING)
    gl.glDisable(gl.GL_DEPTH_TEST)
    
    # Background
    gl.glColor4f(0.0, 0.0, 0.0, 0.7)
    gl.glBegin(gl.GL_QUADS)
    gl.glVertex2f(10, window_height - 200)
    gl.glVertex2f(350, window_height - 200)
    gl.glVertex2f(350, window_height - 10)
    gl.glVertex2f(10, window_height - 10)
    gl.glEnd()
    
    gl.glColor3f(1.0, 1.0, 1.0)
    
    # Text
    y = window_height - 30
    gl.glRasterPos2f(20, y)
    text = f"PBR Earth Demo - Atmospheric Scattering"
    for c in text:
        glut.glutBitmapCharacter(glut.GLUT_BITMAP_9_BY_15, ord(c))
    
    y -= 20
    gl.glRasterPos2f(20, y)
    text = f"Lat: {math.degrees(camera.latitude):.2f} Lon: {math.degrees(camera.longitude):.2f}"
    for c in text:
        glut.glutBitmapCharacter(glut.GLUT_BITMAP_9_BY_15, ord(c))
    
    y -= 20
    gl.glRasterPos2f(20, y)
    text = f"Height: {camera.height/1000:.0f} km"
    for c in text:
        glut.glutBitmapCharacter(glut.GLUT_BITMAP_9_BY_15, ord(c))
    
    y -= 20
    gl.glRasterPos2f(20, y)
    text = f"PBR - Albedo: {material.albedo[0]:.1f},{material.albedo[1]:.1f},{material.albedo[2]:.1f}"
    for c in text:
        glut.glutBitmapCharacter(glut.GLUT_BITMAP_9_BY_15, ord(c))
    
    y -= 20
    gl.glRasterPos2f(20, y)
    text = f"Metallic: {material.metallic:.1f} Roughness: {material.roughness:.1f}"
    for c in text:
        glut.glutBitmapCharacter(glut.GLUT_BITMAP_9_BY_15, ord(c))
    
    y -= 20
    gl.glRasterPos2f(20, y)
    text = f"Sun Intensity: {atmosphere.sun_intensity:.1f}"
    for c in text:
        glut.glutBitmapCharacter(glut.GLUT_BITMAP_9_BY_15, ord(c))
    
    y -= 30
    gl.glRasterPos2f(20, y)
    text = "[WASD] Move  [QE] Zoom  [1-3] Materials  [T] Atmosphere  [R] Wireframe"
    for c in text:
        glut.glutBitmapCharacter(glut.GLUT_BITMAP_9_BY_15, ord(c))
    
    gl.glEnable(gl.GL_DEPTH_TEST)
    gl.glMatrixMode(gl.GL_PROJECTION)
    gl.glPopMatrix()
    gl.glMatrixMode(gl.GL_MODELVIEW)
    gl.glPopMatrix()

def keyboard(key, x, y):
    global show_atmosphere, show_wireframe
    
    if key == b'w':
        camera.latitude += 0.02
    elif key == b's':
        camera.latitude -= 0.02
    elif key == b'a':
        camera.longitude += 0.02
    elif key == b'd':
        camera.longitude -= 0.02
    elif key == b'q':
        camera.height -= 1000000
    elif key == b'e':
        camera.height += 1000000
    elif key == b'1':
        # Ocean material
        material.albedo = np.array([0.1, 0.3, 0.6])
        material.metallic = 0.0
        material.roughness = 0.8
    elif key == b'2':
        # Forest material
        material.albedo = np.array([0.1, 0.4, 0.1])
        material.metallic = 0.0
        material.roughness = 0.9
    elif key == b'3':
        # Desert material
        material.albedo = np.array([0.8, 0.6, 0.3])
        material.metallic = 0.0
        material.roughness = 0.7
    elif key == b't':
        show_atmosphere = not show_atmosphere
    elif key == b'r':
        show_wireframe = not show_wireframe
    elif key == b'\x1b':
        sys.exit(0)
    
    camera.latitude = max(-math.pi/2 + 0.01, min(math.pi/2 - 0.01, camera.latitude))
    camera.height = max(100000, camera.height)
    
    glut.glutPostRedisplay()

def reshape(width, height):
    global window_width, window_height
    window_width = width
    window_height = height
    camera.aspect = width / height
    gl.glViewport(0, 0, width, height)

def main():
    glut.glutInit()
    glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGB | glut.GLUT_DEPTH)
    glut.glutInitWindowSize(window_width, window_height)
    glut.glutCreateWindow(b"PotatoEarth - PBR + Atmosphere Demo")
    
    init()
    
    glut.glutDisplayFunc(display)
    glut.glutReshapeFunc(reshape)
    glut.glutKeyboardFunc(keyboard)
    glut.glutIdleFunc(lambda: glut.glutPostRedisplay())
    
    print("=" * 60)
    print("PotatoEarth - PBR + Atmospheric Scattering Demo")
    print("=" * 60)
    print("Controls:")
    print("  WASD - Move camera")
    print("  QE - Zoom")
    print("  1 - Ocean material")
    print("  2 - Forest material")
    print("  3 - Desert material")
    print("  T - Toggle atmosphere")
    print("  R - Toggle wireframe")
    print("=" * 60)
    
    glut.glutMainLoop()

if __name__ == "__main__":
    main()
