"""
PotatoEarth - Enhanced Version with Imagery & Atmosphere
Beautiful Earth rendering with texture mapping and atmospheric scattering
"""

import numpy as np
import OpenGL.GL as gl
import OpenGL.GLUT as glut
import OpenGL.GLU as glu
from PIL import Image
import math
import sys
import time

# =============================================================================
# CONFIGURATION
# =============================================================================

EARTH_RADIUS = 6371000.0  # meters
EARTH_SCALE = 1.0 / 1000000.0  # Scale down for rendering

# =============================================================================
# TEXTURE LOADING
# =============================================================================

def load_texture(filepath):
    """Load image texture from file"""
    try:
        img = Image.open(filepath)
        img = img.convert('RGB')
        img_data = np.array(img, dtype=np.uint8)
        
        texture_id = gl.glGenTextures(1)
        gl.glBindTexture(gl.GL_TEXTURE_2D, texture_id)
        
        gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_S, gl.GL_REPEAT)
        gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_T, gl.GL_CLAMP_TO_EDGE)
        gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MIN_FILTER, gl.GL_LINEAR_MIPMAP_LINEAR)
        gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MAG_FILTER, gl.GL_LINEAR)
        
        gl.glTexImage2D(gl.GL_TEXTURE_2D, 0, gl.GL_RGB, img.width, img.height, 0, 
                        gl.GL_RGB, gl.GL_UNSIGNED_BYTE, img_data)
        gl.glGenerateMipmap(gl.GL_TEXTURE_2D)
        
        print(f"Texture loaded: {filepath} ({img.width}x{img.height})")
        return texture_id
    except Exception as e:
        print(f"Failed to load texture: {e}")
        return None

def create_procedural_earth_texture():
    """Create a procedural earth texture if file not available"""
    width, height = 1024, 512
    img_data = np.zeros((height, width, 3), dtype=np.uint8)
    
    for y in range(height):
        lat = (y / height - 0.5) * math.pi  # -90 to 90 degrees
        for x in range(width):
            lon = (x / width) * 2 * math.pi  # 0 to 360 degrees
            
            # Simple noise for continents
            noise = math.sin(lon * 3) * math.cos(lat * 4) + \
                    math.sin(lon * 7 + 1) * math.cos(lat * 6 + 2) * 0.5 + \
                    math.sin(lon * 15 + 3) * 0.25
            
            if noise > 0.2:  # Land
                # Vary land color based on latitude (green to brown)
                if abs(lat) > 1.0:  # Polar
                    r, g, b = 240, 240, 250  # Snow/ice
                elif abs(lat) > 0.6:  # Temperate
                    r, g, b = 100, 130, 70   # Forest
                else:  # Tropical
                    r, g, b = 200, 180, 100  # Desert/savanna
            else:  # Ocean
                # Deep blue to light blue
                depth = 30 + int(50 * (0.5 + 0.5 * math.sin(lon * 2 + lat * 3)))
                r, g, b = 20, 60, depth + 100
            
            img_data[y, x] = [r, g, b]
    
    texture_id = gl.glGenTextures(1)
    gl.glBindTexture(gl.GL_TEXTURE_2D, texture_id)
    
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_S, gl.GL_REPEAT)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_T, gl.GL_CLAMP_TO_EDGE)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MIN_FILTER, gl.GL_LINEAR_MIPMAP_LINEAR)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MAG_FILTER, gl.GL_LINEAR)
    
    gl.glTexImage2D(gl.GL_TEXTURE_2D, 0, gl.GL_RGB, width, height, 0, 
                    gl.GL_RGB, gl.GL_UNSIGNED_BYTE, img_data)
    gl.glGenerateMipmap(gl.GL_TEXTURE_2D)
    
    print("Procedural earth texture created")
    return texture_id

# =============================================================================
# ATMOSPHERE SHADER
# =============================================================================

ATMOSPHERE_VERTEX = b"""
#version 120
varying vec3 v_Position;
varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main() {
    v_Position = (gl_ModelViewMatrix * gl_Vertex).xyz;
    v_Normal = gl_NormalMatrix * gl_Normal;
    v_TexCoord = gl_MultiTexCoord0.xy;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
"""

ATMOSPHERE_FRAGMENT = b"""
#version 120
uniform sampler2D u_EarthTexture;
uniform vec3 u_SunDirection;
uniform float u_SunIntensity;
uniform float u_AtmosphereStrength;

varying vec3 v_Position;
varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main() {
    vec3 normal = normalize(v_Normal);
    vec3 viewDir = normalize(-v_Position);
    
    // Sample earth texture
    vec3 earthColor = texture2D(u_EarthTexture, v_TexCoord).rgb;
    
    // Lighting
    float NdotL = max(dot(normal, u_SunDirection), 0.0);
    
    // Diffuse lighting
    vec3 diffuse = earthColor * NdotL * u_SunIntensity;
    
    // Ambient (earthshine + starlight)
    vec3 ambient = earthColor * 0.1;
    
    // Specular (ocean reflection)
    vec3 halfDir = normalize(u_SunDirection + viewDir);
    float specAngle = max(dot(normal, halfDir), 0.0);
    float specular = pow(specAngle, 32.0) * 0.3 * NdotL;
    
    // Atmospheric scattering (simple approximation)
    float atmosphere = 1.0 - max(dot(viewDir, normal), 0.0);
    atmosphere = pow(atmosphere, 3.0) * u_AtmosphereStrength;
    vec3 atmosphereColor = vec3(0.4, 0.7, 1.0) * atmosphere;
    
    // Rim lighting (day side glow)
    float rim = pow(1.0 - max(dot(viewDir, normal), 0.0), 2.0);
    vec3 rimColor = vec3(1.0, 0.9, 0.7) * rim * NdotL * 0.5;
    
    vec3 finalColor = ambient + diffuse + specular + atmosphereColor + rimColor;
    
    // Tone mapping
    finalColor = finalColor / (finalColor + vec3(1.0));
    
    // Gamma correction
    finalColor = pow(finalColor, vec3(1.0 / 2.2));
    
    gl_FragColor = vec4(finalColor, 1.0);
}
"""

SKY_VERTEX = b"""
#version 120
varying vec3 v_Position;

void main() {
    v_Position = gl_Vertex.xyz;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
"""

SKY_FRAGMENT = b"""
#version 120
uniform vec3 u_SunDirection;
uniform float u_SunIntensity;

varying vec3 v_Position;

void main() {
    vec3 viewDir = normalize(v_Position);
    
    // Sky gradient
    float horizon = max(viewDir.y, 0.0);
    vec3 skyColor = mix(
        vec3(0.02, 0.05, 0.15),  // Horizon (dark blue)
        vec3(0.0, 0.0, 0.05),     // Zenith (black/space)
        pow(1.0 - horizon, 0.5)
    );
    
    // Add horizon glow
    float horizonGlow = pow(1.0 - abs(viewDir.y), 4.0) * 0.3;
    skyColor += vec3(0.2, 0.4, 0.8) * horizonGlow;
    
    // Sun
    float sunDot = dot(viewDir, u_SunDirection);
    float sunMask = smoothstep(0.995, 1.0, sunDot);
    vec3 sunColor = vec3(1.0, 0.95, 0.8) * sunMask * u_SunIntensity;
    
    // Sun glow
    float sunGlow = pow(max(sunDot, 0.0), 20.0) * 0.5;
    skyColor += vec3(1.0, 0.9, 0.6) * sunGlow;
    
    gl_FragColor = vec4(skyColor, 1.0);
}
"""

# =============================================================================
# EARTH RENDERER
# =============================================================================

class EarthRenderer:
    def __init__(self):
        self.earth_texture = None
        self.atmosphere_shader = None
        self.sky_shader = None
        self.sky_list = None
        self.earth_list = None
        
        # Sun direction
        self.sun_direction = np.array([0.5, 0.8, 0.3])
        self.sun_direction = self.sun_direction / np.linalg.norm(self.sun_direction)
        
    def initialize(self):
        # Load or create texture
        texture_path = "G:/Cplusplus/PotatoEarth/assets/textures/earth_day.jpg"
        self.earth_texture = load_texture(texture_path)
        if self.earth_texture is None:
            self.earth_texture = create_procedural_earth_texture()
        
        # Create shaders
        self.atmosphere_shader = self.create_shader(ATMOSPHERE_VERTEX, ATMOSPHERE_FRAGMENT)
        self.sky_shader = self.create_shader(SKY_VERTEX, SKY_FRAGMENT)
        
        # Create display lists
        self.create_earth_display_list()
        self.create_sky_display_list()
        
        print("Earth renderer initialized")
    
    def create_shader(self, vertex_source, fragment_source):
        """Create OpenGL shader program"""
        vertex_shader = gl.glCreateShader(gl.GL_VERTEX_SHADER)
        gl.glShaderSource(vertex_shader, vertex_source)
        gl.glCompileShader(vertex_shader)
        
        fragment_shader = gl.glCreateShader(gl.GL_FRAGMENT_SHADER)
        gl.glShaderSource(fragment_shader, fragment_source)
        gl.glCompileShader(fragment_shader)
        
        program = gl.glCreateProgram()
        gl.glAttachShader(program, vertex_shader)
        gl.glAttachShader(program, fragment_shader)
        gl.glLinkProgram(program)
        
        gl.glDeleteShader(vertex_shader)
        gl.glDeleteShader(fragment_shader)
        
        return program
    
    def create_earth_display_list(self):
        """Create optimized earth mesh"""
        self.earth_list = gl.glGenLists(1)
        gl.glNewList(self.earth_list, gl.GL_COMPILE)
        
        # Create sphere with texture coordinates
        radius = EARTH_RADIUS * EARTH_SCALE
        stacks = 64
        slices = 128
        
        for i in range(stacks):
            lat0 = math.pi * (-0.5 + float(i) / stacks)
            lat1 = math.pi * (-0.5 + float(i + 1) / stacks)
            
            gl.glBegin(gl.GL_QUAD_STRIP)
            for j in range(slices + 1):
                lng = 2 * math.pi * float(j) / slices
                
                # Top vertex
                x0 = math.cos(lat0) * math.cos(lng)
                y0 = math.sin(lat0)
                z0 = math.cos(lat0) * math.sin(lng)
                
                # Bottom vertex
                x1 = math.cos(lat1) * math.cos(lng)
                y1 = math.sin(lat1)
                z1 = math.cos(lat1) * math.sin(lng)
                
                # Texture coordinates
                u = 1.0 - float(j) / slices
                v0 = 1.0 - float(i) / stacks
                v1 = 1.0 - float(i + 1) / stacks
                
                gl.glTexCoord2f(u, v0)
                gl.glNormal3f(x0, y0, z0)
                gl.glVertex3f(x0 * radius, y0 * radius, z0 * radius)
                
                gl.glTexCoord2f(u, v1)
                gl.glNormal3f(x1, y1, z1)
                gl.glVertex3f(x1 * radius, y1 * radius, z1 * radius)
            
            gl.glEnd()
        
        gl.glEndList()
    
    def create_sky_display_list(self):
        """Create sky dome"""
        self.sky_list = gl.glGenLists(1)
        gl.glNewList(self.sky_list, gl.GL_COMPILE)
        
        radius = EARTH_RADIUS * EARTH_SCALE * 50  # Large sky dome
        
        gl.glBegin(gl.GL_QUADS)
        
        # Simple cube map approximation
        for i in range(4):  # 4 sides
            angle = i * math.pi / 2
            next_angle = (i + 1) * math.pi / 2
            
            for j in range(8):  # Vertical segments
                y0 = -1.0 + j * 0.25
                y1 = -1.0 + (j + 1) * 0.25
                
                r0 = math.sqrt(max(0, 1 - y0 * y0))
                r1 = math.sqrt(max(0, 1 - y1 * y1))
                
                x0 = r0 * math.cos(angle)
                z0 = r0 * math.sin(angle)
                x1 = r1 * math.cos(angle)
                z1 = r1 * math.sin(angle)
                x2 = r0 * math.cos(next_angle)
                z2 = r0 * math.sin(next_angle)
                x3 = r1 * math.cos(next_angle)
                z3 = r1 * math.sin(next_angle)
                
                gl.glVertex3f(x0 * radius, y0 * radius, z0 * radius)
                gl.glVertex3f(x2 * radius, y0 * radius, z2 * radius)
                gl.glVertex3f(x3 * radius, y1 * radius, z3 * radius)
                gl.glVertex3f(x1 * radius, y1 * radius, z1 * radius)
        
        gl.glEnd()
        gl.glEndList()
    
    def render_sky(self, camera_position):
        """Render atmospheric sky"""
        gl.glDisable(gl.GL_DEPTH_TEST)
        gl.glDisable(gl.GL_LIGHTING)
        
        gl.glUseProgram(self.sky_shader)
        
        # Set uniforms
        loc = gl.glGetUniformLocation(self.sky_shader, b"u_SunDirection")
        gl.glUniform3f(loc, *self.sun_direction)
        
        loc = gl.glGetUniformLocation(self.sky_shader, b"u_SunIntensity")
        gl.glUniform1f(loc, 20.0)
        
        gl.glCallList(self.sky_list)
        
        gl.glUseProgram(0)
    
    def render_earth(self, view_matrix, projection_matrix):
        """Render earth with atmosphere"""
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_CULL_FACE)
        
        gl.glUseProgram(self.atmosphere_shader)
        
        # Bind texture
        gl.glActiveTexture(gl.GL_TEXTURE0)
        gl.glBindTexture(gl.GL_TEXTURE_2D, self.earth_texture)
        loc = gl.glGetUniformLocation(self.atmosphere_shader, b"u_EarthTexture")
        gl.glUniform1i(loc, 0)
        
        # Set uniforms
        loc = gl.glGetUniformLocation(self.atmosphere_shader, b"u_SunDirection")
        gl.glUniform3f(loc, *self.sun_direction)
        
        loc = gl.glGetUniformLocation(self.atmosphere_shader, b"u_SunIntensity")
        gl.glUniform1f(loc, 1.5)
        
        loc = gl.glGetUniformLocation(self.atmosphere_shader, b"u_AtmosphereStrength")
        gl.glUniform1f(loc, 0.8)
        
        # Render earth
        gl.glCallList(self.earth_list)
        
        gl.glUseProgram(0)
        gl.glDisable(gl.GL_CULL_FACE)

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
        x = radius * cos_lat * cos_lon * EARTH_SCALE
        y = radius * sin_lat * EARTH_SCALE
        z = radius * cos_lat * sin_lon * EARTH_SCALE
        
        return np.array([x, y, z])
    
    def apply(self):
        pos = self.get_position()
        glu.gluLookAt(pos[0], pos[1], pos[2], 0, 0, 0, 0, 1, 0)

# =============================================================================
# MAIN APPLICATION
# =============================================================================

class BeautifulEarthApp:
    def __init__(self):
        self.width = 1280
        self.height = 720
        self.camera = Camera()
        self.earth_renderer = EarthRenderer()
        self.frame_count = 0
        self.last_time = time.time()
        self.fps = 0
        
    def initialize(self):
        # OpenGL setup
        gl.glClearColor(0.0, 0.0, 0.0, 1.0)
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_TEXTURE_2D)
        
        # Initialize earth renderer
        self.earth_renderer.initialize()
        
        print("Beautiful Earth initialized")
    
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
        self.earth_renderer.render_sky(self.camera.get_position())
        
        # Render earth
        self.earth_renderer.render_earth(None, None)
        
        # Render HUD
        self.render_hud()
        
        glut.glutSwapBuffers()
        glut.glutPostRedisplay()
    
    def render_hud(self):
        """Render heads-up display"""
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
        gl.glColor4f(0.0, 0.0, 0.0, 0.7)
        gl.glBegin(gl.GL_QUADS)
        gl.glVertex2f(10, self.height - 200)
        gl.glVertex2f(380, self.height - 200)
        gl.glVertex2f(380, self.height - 10)
        gl.glVertex2f(10, self.height - 10)
        gl.glEnd()
        
        # Text
        gl.glColor3f(0.0, 1.0, 0.0)
        
        lines = [
            "PotatoEarth - Beautiful Edition",
            "=" * 35,
            f"FPS: {self.fps}",
            "",
            f"Lat: {math.degrees(self.camera.latitude):.2f}",
            f"Lon: {math.degrees(self.camera.longitude):.2f}",
            f"Height: {self.camera.height/1000:.0f} km",
            "",
            "Features:",
            "  Earth Texture Mapping",
            "  Atmospheric Scattering",
            "  Dynamic Lighting",
            "  Sky Dome",
            "",
            "[WASD] Orbit  [QE] Zoom  [ESC] Exit"
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
        if key == b'w':
            self.camera.latitude += 0.02
        elif key == b's':
            self.camera.latitude -= 0.02
        elif key == b'a':
            self.camera.longitude += 0.02
        elif key == b'd':
            self.camera.longitude -= 0.02
        elif key == b'q':
            self.camera.height = max(100000, self.camera.height - 500000)
        elif key == b'e':
            self.camera.height += 500000
        elif key == b'\x1b':
            sys.exit(0)
        
        self.camera.latitude = max(-math.pi/2 + 0.01, min(math.pi/2 - 0.01, self.camera.latitude))
    
    def reshape(self, width, height):
        self.width = width
        self.height = height
        self.camera.aspect = width / height
        gl.glViewport(0, 0, width, height)

def main():
    print("=" * 60)
    print("PotatoEarth - Beautiful Edition")
    print("=" * 60)
    print("Features:")
    print("  - Earth texture mapping")
    print("  - Atmospheric scattering")
    print("  - Dynamic lighting")
    print("  - Sky dome with sun")
    print("=" * 60)
    
    glut.glutInit()
    glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGB | glut.GLUT_DEPTH)
    glut.glutInitWindowSize(1280, 720)
    glut.glutCreateWindow(b"PotatoEarth - Beautiful Edition")
    
    app = BeautifulEarthApp()
    app.initialize()
    
    glut.glutDisplayFunc(lambda: app.display())
    glut.glutReshapeFunc(lambda w, h: app.reshape(w, h))
    glut.glutKeyboardFunc(lambda k, x, y: app.keyboard(k, x, y))
    
    glut.glutMainLoop()

if __name__ == "__main__":
    main()
