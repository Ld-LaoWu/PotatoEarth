"""
PotatoEarth - Ultra Realistic Edition
Multi-layer Earth with atmosphere, clouds, and night lights
"""

import numpy as np
import OpenGL.GL as gl
import OpenGL.GLUT as glut
import OpenGL.GLU as glu
from PIL import Image, ImageFilter
import math
import sys
import time

# =============================================================================
# CONFIGURATION
# =============================================================================

EARTH_RADIUS = 6371000.0
SCALE = 1.0 / 1000000.0
ATMOSPHERE_RADIUS = EARTH_RADIUS * 1.025

# =============================================================================
# ADVANCED TEXTURE GENERATION
# =============================================================================

def create_earth_texture():
    """Create high-quality earth texture with continents and oceans"""
    width, height = 2048, 1024
    img = np.zeros((height, width, 3), dtype=np.uint8)
    
    # Simplex noise-like function for continents
    def noise(x, y, z):
        return math.sin(x) * math.cos(y) * math.sin(z)
    
    for y in range(height):
        lat = (y / height - 0.5) * math.pi
        for x in range(width):
            lon = (x / width) * 2 * math.pi
            
            # Multiple octaves of noise for realistic continents
            nx = math.cos(lat) * math.cos(lon)
            ny = math.sin(lat)
            nz = math.cos(lat) * math.sin(lon)
            
            # Continental noise
            n = noise(nx * 2, ny * 2, nz * 2) * 0.5 + \
                noise(nx * 4 + 1, ny * 4 + 2, nz * 4 + 3) * 0.25 + \
                noise(nx * 8 + 3, ny * 8 + 1, nz * 8 + 2) * 0.125 + \
                noise(nx * 16, ny * 16, nz * 16) * 0.0625
            
            # Determine biome based on latitude and noise
            abs_lat = abs(lat)
            
            if n > 0.15:  # Land
                if abs_lat > 1.2:  # Polar - ice/snow
                    r, g, b = 240, 245, 250
                elif abs_lat > 0.8:  # Tundra
                    r, g, b = 180, 190, 170
                elif abs_lat > 0.4:  # Temperate
                    if n > 0.4:  # Mountains
                        r, g, b = 120, 110, 100
                    else:  # Forest
                        r, g, b = 60, 100, 50
                else:  # Tropical
                    if n > 0.5:  # Desert
                        r, g, b = 210, 180, 120
                    else:  # Jungle
                        r, g, b = 50, 120, 40
            else:  # Ocean
                # Depth variation
                depth = 30 + int(40 * (0.5 + 0.5 * n))
                r, g, b = 20, 50, depth + 120
            
            img[y, x] = [r, g, b]
    
    return img

def create_cloud_texture():
    """Create cloud texture"""
    width, height = 2048, 1024
    img = np.zeros((height, width, 4), dtype=np.uint8)
    
    for y in range(height):
        lat = (y / height - 0.5) * math.pi
        for x in range(width):
            lon = (x / width) * 2 * math.pi
            
            # Cloud noise
            nx = math.cos(lat) * math.cos(lon)
            ny = math.sin(lat)
            nz = math.cos(lat) * math.sin(lon)
            
            n = math.sin(nx * 3) * math.cos(ny * 4) * 0.5 + \
                math.sin(nx * 7 + 2) * math.cos(ny * 6 + 1) * 0.3 + \
                math.sin(nx * 15) * 0.2
            
            # More clouds at equator, less at poles
            lat_factor = 1.0 - abs(lat) / (math.pi / 2) * 0.5
            
            if n > 0.3 * lat_factor:
                alpha = int((n - 0.3) * 400 * lat_factor)
                alpha = min(200, alpha)  # Not fully opaque
                img[y, x] = [255, 255, 255, alpha]
            else:
                img[y, x] = [255, 255, 255, 0]
    
    return img

def create_night_lights_texture():
    """Create city lights texture for night side"""
    width, height = 2048, 1024
    img = np.zeros((height, width, 4), dtype=np.uint8)
    
    # City clusters (simplified)
    cities = [
        # lon, lat, intensity
        (-74.0, 40.7, 1.0),   # New York
        (-0.1, 51.5, 0.9),    # London
        (116.4, 39.9, 1.0),   # Beijing
        (139.7, 35.7, 1.0),   # Tokyo
        (-43.2, -22.9, 0.7),  # Rio
        (151.2, -33.9, 0.8),  # Sydney
        (37.6, 55.8, 0.9),    # Moscow
        (-118.2, 34.0, 0.9),  # Los Angeles
        (28.9, 41.0, 0.7),    # Istanbul
        (72.8, 19.0, 0.9),    # Mumbai
        (103.8, 1.3, 0.8),    # Singapore
        (31.2, 30.0, 0.6),    # Cairo
        # European cluster
        (10.0, 50.0, 0.8),
        (2.0, 48.0, 0.8),
        (13.0, 52.0, 0.8),
        # US East Coast
        (-80.0, 35.0, 0.7),
        (-87.0, 41.0, 0.8),
        # Asia
        (121.5, 31.2, 0.9),   # Shanghai
        (126.9, 37.5, 0.8),   # Seoul
        (77.2, 28.6, 0.8),    # Delhi
    ]
    
    for lon_deg, lat_deg, intensity in cities:
        lon = math.radians(lon_deg)
        lat = math.radians(lat_deg)
        
        x = int((lon / (2 * math.pi) + 0.5) * width) % width
        y = int((0.5 - lat / math.pi) * height)
        
        # Draw city glow
        radius = 30
        for dy in range(-radius, radius + 1):
            for dx in range(-radius, radius + 1):
                px = (x + dx) % width
                py = y + dy
                if 0 <= py < height:
                    dist = math.sqrt(dx * dx + dy * dy)
                    if dist < radius:
                        alpha = int(intensity * (1 - dist / radius) * 255)
                        if alpha > img[py, px, 3]:
                            # Gold/orange city light color
                            img[py, px] = [255, 200, 100, alpha]
    
    return img

def create_starfield_texture():
    """Create starfield background"""
    width, height = 2048, 1024
    img = np.zeros((height, width, 3), dtype=np.uint8)
    
    import random
    random.seed(42)
    
    # Background - deep space
    for y in range(height):
        for x in range(width):
            # Slight gradient
            val = 5 + int(10 * (y / height))
            img[y, x] = [val // 3, val // 4, val]
    
    # Add stars
    num_stars = 2000
    for _ in range(num_stars):
        x = random.randint(0, width - 1)
        y = random.randint(0, height - 1)
        brightness = random.randint(100, 255)
        size = random.choice([1, 1, 1, 2, 2, 3])  # Mostly small stars
        
        for dy in range(-size, size + 1):
            for dx in range(-size, size + 1):
                px = (x + dx) % width
                py = y + dy
                if 0 <= py < height:
                    dist = math.sqrt(dx * dx + dy * dy)
                    if dist <= size:
                        intensity = int(brightness * (1 - dist / (size + 1)))
                        img[py, px] = [intensity, intensity, intensity]
    
    return img

def create_textures():
    """Create all textures and return their IDs"""
    textures = {}
    
    # Earth day texture
    print("Generating Earth texture...")
    earth_data = create_earth_texture()
    textures['earth'] = gl.glGenTextures(1)
    gl.glBindTexture(gl.GL_TEXTURE_2D, textures['earth'])
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_S, gl.GL_REPEAT)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_T, gl.GL_CLAMP_TO_EDGE)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MIN_FILTER, gl.GL_LINEAR_MIPMAP_LINEAR)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MAG_FILTER, gl.GL_LINEAR)
    gl.glTexImage2D(gl.GL_TEXTURE_2D, 0, gl.GL_RGB, earth_data.shape[1], earth_data.shape[0], 
                    0, gl.GL_RGB, gl.GL_UNSIGNED_BYTE, earth_data)
    gl.glGenerateMipmap(gl.GL_TEXTURE_2D)
    
    # Cloud texture
    print("Generating cloud texture...")
    cloud_data = create_cloud_texture()
    textures['clouds'] = gl.glGenTextures(1)
    gl.glBindTexture(gl.GL_TEXTURE_2D, textures['clouds'])
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_S, gl.GL_REPEAT)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_T, gl.GL_CLAMP_TO_EDGE)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MIN_FILTER, gl.GL_LINEAR_MIPMAP_LINEAR)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MAG_FILTER, gl.GL_LINEAR)
    gl.glTexImage2D(gl.GL_TEXTURE_2D, 0, gl.GL_RGBA, cloud_data.shape[1], cloud_data.shape[0], 
                    0, gl.GL_RGBA, gl.GL_UNSIGNED_BYTE, cloud_data)
    gl.glGenerateMipmap(gl.GL_TEXTURE_2D)
    
    # Night lights texture
    print("Generating night lights texture...")
    night_data = create_night_lights_texture()
    textures['night'] = gl.glGenTextures(1)
    gl.glBindTexture(gl.GL_TEXTURE_2D, textures['night'])
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_S, gl.GL_REPEAT)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_T, gl.GL_CLAMP_TO_EDGE)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MIN_FILTER, gl.GL_LINEAR_MIPMAP_LINEAR)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MAG_FILTER, gl.GL_LINEAR)
    gl.glTexImage2D(gl.GL_TEXTURE_2D, 0, gl.GL_RGBA, night_data.shape[1], night_data.shape[0], 
                    0, gl.GL_RGBA, gl.GL_UNSIGNED_BYTE, night_data)
    gl.glGenerateMipmap(gl.GL_TEXTURE_2D)
    
    # Starfield texture
    print("Generating starfield texture...")
    star_data = create_starfield_texture()
    textures['stars'] = gl.glGenTextures(1)
    gl.glBindTexture(gl.GL_TEXTURE_2D, textures['stars'])
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_S, gl.GL_REPEAT)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_WRAP_T, gl.GL_CLAMP_TO_EDGE)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MIN_FILTER, gl.GL_LINEAR_MIPMAP_LINEAR)
    gl.glTexParameteri(gl.GL_TEXTURE_2D, gl.GL_TEXTURE_MAG_FILTER, gl.GL_LINEAR)
    gl.glTexImage2D(gl.GL_TEXTURE_2D, 0, gl.GL_RGB, star_data.shape[1], star_data.shape[0], 
                    0, gl.GL_RGB, gl.GL_UNSIGNED_BYTE, star_data)
    gl.glGenerateMipmap(gl.GL_TEXTURE_2D)
    
    print("All textures generated!")
    return textures

# =============================================================================
# SHADERS
# =============================================================================

EARTH_VERTEX = b"""
#version 120
varying vec3 v_Position;
varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main() {
    v_Position = (gl_ModelViewMatrix * gl_Vertex).xyz;
    v_Normal = normalize(gl_NormalMatrix * gl_Normal);
    v_TexCoord = gl_MultiTexCoord0.xy;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
"""

EARTH_FRAGMENT = b"""
#version 120
uniform sampler2D u_EarthTexture;
uniform sampler2D u_CloudTexture;
uniform sampler2D u_NightTexture;
uniform vec3 u_SunDirection;
uniform float u_Time;

varying vec3 v_Position;
varying vec3 v_Normal;
varying vec2 v_TexCoord;

void main() {
    vec3 normal = normalize(v_Normal);
    
    // Lighting
    float NdotL = dot(normal, u_SunDirection);
    float sunIntensity = max(NdotL, 0.0);
    
    // Sample textures
    vec3 earthColor = texture2D(u_EarthTexture, v_TexCoord).rgb;
    vec4 cloudColor = texture2D(u_CloudTexture, v_TexCoord + vec2(u_Time * 0.001, 0.0));
    vec4 nightColor = texture2D(u_NightTexture, v_TexCoord);
    
    // Day/night mixing
    float dayFactor = smoothstep(-0.1, 0.1, NdotL);
    float nightFactor = 1.0 - dayFactor;
    
    // Day side
    vec3 dayColor = earthColor * (0.3 + 0.7 * sunIntensity);
    
    // Night side with city lights
    vec3 nightSideColor = earthColor * 0.05 + nightColor.rgb * nightColor.a;
    
    // Mix day and night
    vec3 finalColor = mix(nightSideColor, dayColor, dayFactor);
    
    // Add clouds
    float cloudAlpha = cloudColor.a * 0.8;
    vec3 cloudLit = vec3(1.0) * (0.4 + 0.6 * sunIntensity);
    finalColor = mix(finalColor, cloudLit, cloudAlpha);
    
    // Specular reflection on oceans (simplified)
    if (earthColor.b > earthColor.g + 0.1) {  // Ocean
        vec3 viewDir = normalize(-v_Position);
        vec3 halfDir = normalize(u_SunDirection + viewDir);
        float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);
        finalColor += vec3(0.3) * spec * sunIntensity;
    }
    
    // Atmospheric rim glow
    float rim = 1.0 - max(dot(normalize(-v_Position), normal), 0.0);
    rim = pow(rim, 2.0);
    vec3 atmosphereColor = vec3(0.4, 0.7, 1.0) * rim * 0.5;
    
    finalColor += atmosphereColor;
    
    // Tone mapping
    finalColor = finalColor / (finalColor + vec3(1.0));
    
    // Gamma correction
    finalColor = pow(finalColor, vec3(1.0 / 2.2));
    
    gl_FragColor = vec4(finalColor, 1.0);
}
"""

ATMOSPHERE_VERTEX = b"""
#version 120
varying vec3 v_Position;
varying vec3 v_Normal;

void main() {
    v_Position = (gl_ModelViewMatrix * gl_Vertex).xyz;
    v_Normal = normalize(gl_NormalMatrix * gl_Normal);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
"""

ATMOSPHERE_FRAGMENT = b"""
#version 120
uniform vec3 u_SunDirection;

varying vec3 v_Position;
varying vec3 v_Normal;

void main() {
    vec3 normal = normalize(v_Normal);
    vec3 viewDir = normalize(-v_Position);
    
    // Rayleigh scattering approximation
    float cosTheta = dot(viewDir, u_SunDirection);
    float rayleigh = 0.5 + 0.5 * cosTheta;
    
    // Rim effect
    float rim = 1.0 - max(dot(viewDir, normal), 0.0);
    rim = pow(rim, 3.0);
    
    // Atmosphere colors
    vec3 dayColor = vec3(0.5, 0.8, 1.0);
    vec3 sunsetColor = vec3(1.0, 0.6, 0.3);
    vec3 atmosphereColor = mix(dayColor, sunsetColor, pow(1.0 - rayleigh, 2.0));
    
    float alpha = rim * 0.6;
    
    gl_FragColor = vec4(atmosphereColor, alpha);
}
"""

SKY_VERTEX = b"""
#version 120
varying vec3 v_TexCoord;

void main() {
    v_TexCoord = gl_Vertex.xyz;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
"""

SKY_FRAGMENT = b"""
#version 120
uniform sampler2D u_StarTexture;
uniform vec3 u_SunDirection;
uniform float u_SunIntensity;

varying vec3 v_TexCoord;

void main() {
    vec3 viewDir = normalize(v_TexCoord);
    
    // Sample starfield
    vec2 texCoord = vec2(
        atan(viewDir.x, viewDir.z) / (2.0 * 3.14159) + 0.5,
        asin(viewDir.y) / 3.14159 + 0.5
    );
    vec3 stars = texture2D(u_StarTexture, texCoord).rgb;
    
    // Sun
    float sunDot = dot(viewDir, u_SunDirection);
    float sunMask = smoothstep(0.9995, 1.0, sunDot);
    vec3 sunColor = vec3(1.0, 0.95, 0.8) * sunMask * u_SunIntensity;
    
    // Sun glow
    float sunGlow = pow(max(sunDot, 0.0), 100.0) * 0.5;
    stars += vec3(1.0, 0.9, 0.7) * sunGlow;
    
    gl_FragColor = vec4(stars, 1.0);
}
"""

def compile_shader(vertex_source, fragment_source):
    """Compile shader program"""
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

# =============================================================================
# MAIN APPLICATION
# =============================================================================

class UltraEarth:
    def __init__(self):
        self.width = 1280
        self.height = 720
        self.camera_pos = np.array([0.0, 0.0, 3.0])
        self.camera_rot = [0.0, 0.0]
        self.time = 0.0
        self.fps = 0
        self.frame_count = 0
        self.last_time = time.time()
        
        self.textures = {}
        self.shaders = {}
        self.display_lists = {}
        
    def init(self):
        gl.glClearColor(0.0, 0.0, 0.0, 1.0)
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_BLEND)
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA)
        
        # Create textures
        self.textures = create_textures()
        
        # Compile shaders
        self.shaders['earth'] = compile_shader(EARTH_VERTEX, EARTH_FRAGMENT)
        self.shaders['atmosphere'] = compile_shader(ATMOSPHERE_VERTEX, ATMOSPHERE_FRAGMENT)
        self.shaders['sky'] = compile_shader(SKY_VERTEX, SKY_FRAGMENT)
        
        # Create display lists
        self.create_sphere_display_list()
        self.create_sky_display_list()
        
        print("Ultra Earth initialized!")
    
    def create_sphere_display_list(self):
        # Earth sphere
        self.display_lists['earth'] = gl.glGenLists(1)
        gl.glNewList(self.display_lists['earth'], gl.GL_COMPILE)
        self.render_sphere(1.0, 128, 64)
        gl.glEndList()
        
        # Atmosphere sphere (slightly larger)
        self.display_lists['atmosphere'] = gl.glGenLists(1)
        gl.glNewList(self.display_lists['atmosphere'], gl.GL_COMPILE)
        self.render_sphere(1.025, 64, 32)
        gl.glEndList()
    
    def create_sky_display_list(self):
        self.display_lists['sky'] = gl.glGenLists(1)
        gl.glNewList(self.display_lists['sky'], gl.GL_COMPILE)
        
        # Render large inverted sphere for sky
        radius = 50.0
        gl.glBegin(gl.GL_QUADS)
        stacks = 8
        slices = 16
        
        for i in range(stacks):
            lat0 = math.pi * (-0.5 + float(i) / stacks)
            lat1 = math.pi * (-0.5 + float(i + 1) / stacks)
            
            for j in range(slices):
                lng0 = 2 * math.pi * float(j) / slices
                lng1 = 2 * math.pi * float(j + 1) / slices
                
                # Four corners of quad
                for lat, lng in [(lat0, lng0), (lat0, lng1), (lat1, lng1), (lat1, lng0)]:
                    x = math.cos(lat) * math.cos(lng) * radius
                    y = math.sin(lat) * radius
                    z = math.cos(lat) * math.sin(lng) * radius
                    gl.glVertex3f(x, y, z)
        
        gl.glEnd()
        gl.glEndList()
    
    def render_sphere(self, radius, slices, stacks):
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
                    
                    u = 1.0 - float(j) / slices
                    v = 1.0 - (lat + math.pi / 2) / math.pi
                    
                    gl.glTexCoord2f(u, v)
                    gl.glNormal3f(x, y, z)
                    gl.glVertex3f(x * radius, y * radius, z * radius)
            gl.glEnd()
    
    def display(self):
        # Update
        self.time += 0.01
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
        glu.gluPerspective(45.0, self.width / self.height, 0.1, 1000.0)
        
        gl.glMatrixMode(gl.GL_MODELVIEW)
        gl.glLoadIdentity()
        
        # Camera
        dist = 3.0
        x = math.sin(self.camera_rot[0]) * math.cos(self.camera_rot[1]) * dist
        y = math.sin(self.camera_rot[1]) * dist
        z = math.cos(self.camera_rot[0]) * math.cos(self.camera_rot[1]) * dist
        glu.gluLookAt(x, y, z, 0, 0, 0, 0, 1, 0)
        
        # Sun direction (for lighting)
        sun_dir = np.array([0.5, 0.5, 1.0])
        sun_dir = sun_dir / np.linalg.norm(sun_dir)
        
        # Render sky
        gl.glDisable(gl.GL_DEPTH_TEST)
        gl.glUseProgram(self.shaders['sky'])
        gl.glActiveTexture(gl.GL_TEXTURE0)
        gl.glBindTexture(gl.GL_TEXTURE_2D, self.textures['stars'])
        loc = gl.glGetUniformLocation(self.shaders['sky'], b"u_StarTexture")
        gl.glUniform1i(loc, 0)
        loc = gl.glGetUniformLocation(self.shaders['sky'], b"u_SunDirection")
        gl.glUniform3f(loc, *sun_dir)
        loc = gl.glGetUniformLocation(self.shaders['sky'], b"u_SunIntensity")
        gl.glUniform1f(loc, 20.0)
        gl.glCallList(self.display_lists['sky'])
        
        # Render atmosphere (behind earth)
        gl.glEnable(gl.GL_DEPTH_TEST)
        gl.glEnable(gl.GL_CULL_FACE)
        gl.glCullFace(gl.GL_FRONT)  # Render inside of atmosphere sphere
        gl.glUseProgram(self.shaders['atmosphere'])
        loc = gl.glGetUniformLocation(self.shaders['atmosphere'], b"u_SunDirection")
        gl.glUniform3f(loc, *sun_dir)
        gl.glCallList(self.display_lists['atmosphere'])
        
        # Render earth
        gl.glCullFace(gl.GL_BACK)
        gl.glUseProgram(self.shaders['earth'])
        
        # Bind textures
        gl.glActiveTexture(gl.GL_TEXTURE0)
        gl.glBindTexture(gl.GL_TEXTURE_2D, self.textures['earth'])
        loc = gl.glGetUniformLocation(self.shaders['earth'], b"u_EarthTexture")
        gl.glUniform1i(loc, 0)
        
        gl.glActiveTexture(gl.GL_TEXTURE1)
        gl.glBindTexture(gl.GL_TEXTURE_2D, self.textures['clouds'])
        loc = gl.glGetUniformLocation(self.shaders['earth'], b"u_CloudTexture")
        gl.glUniform1i(loc, 1)
        
        gl.glActiveTexture(gl.GL_TEXTURE2)
        gl.glBindTexture(gl.GL_TEXTURE_2D, self.textures['night'])
        loc = gl.glGetUniformLocation(self.shaders['earth'], b"u_NightTexture")
        gl.glUniform1i(loc, 2)
        
        loc = gl.glGetUniformLocation(self.shaders['earth'], b"u_SunDirection")
        gl.glUniform3f(loc, *sun_dir)
        loc = gl.glGetUniformLocation(self.shaders['earth'], b"u_Time")
        gl.glUniform1f(loc, self.time)
        
        gl.glCallList(self.display_lists['earth'])
        
        gl.glUseProgram(0)
        gl.glDisable(gl.GL_CULL_FACE)
        
        # HUD
        self.render_hud()
        
        glut.glutSwapBuffers()
        glut.glutPostRedisplay()
    
    def render_hud(self):
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
        gl.glVertex2f(10, self.height - 220)
        gl.glVertex2f(400, self.height - 220)
        gl.glVertex2f(400, self.height - 10)
        gl.glVertex2f(10, self.height - 10)
        gl.glEnd()
        
        gl.glColor3f(0.0, 1.0, 0.0)
        
        lines = [
            "PotatoEarth - Ultra Realistic Edition",
            "=" * 40,
            f"FPS: {self.fps}",
            "",
            "Features:",
            "  Multi-layer Earth Rendering",
            "  Procedural Textures",
            "  Day/Night Cycle",
            "  City Lights",
            "  Cloud Layer",
            "  Atmospheric Scattering",
            "  Starfield Background",
            "",
            "[WASD] Rotate  [QE] Zoom"
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
        if key == b'a':
            self.camera_rot[0] -= 0.05
        elif key == b'd':
            self.camera_rot[0] += 0.05
        elif key == b'w':
            self.camera_rot[1] = min(math.pi / 2 - 0.1, self.camera_rot[1] + 0.05)
        elif key == b's':
            self.camera_rot[1] = max(-math.pi / 2 + 0.1, self.camera_rot[1] - 0.05)
        elif key == b'q':
            pass  # Zoom out
        elif key == b'e':
            pass  # Zoom in
        elif key == b'\x1b':
            sys.exit(0)
    
    def reshape(self, w, h):
        self.width = w
        self.height = h
        gl.glViewport(0, 0, w, h)

def main():
    print("=" * 60)
    print("PotatoEarth - Ultra Realistic Edition")
    print("=" * 60)
    print("Generating procedural textures...")
    print("This may take a moment...")
    print("=" * 60)
    
    glut.glutInit()
    glut.glutInitDisplayMode(glut.GLUT_DOUBLE | glut.GLUT_RGB | glut.GLUT_DEPTH)
    glut.glutInitWindowSize(1280, 720)
    glut.glutCreateWindow(b"PotatoEarth - Ultra Realistic")
    
    app = UltraEarth()
    app.init()
    
    glut.glutDisplayFunc(lambda: app.display())
    glut.glutReshapeFunc(lambda w, h: app.reshape(w, h))
    glut.glutKeyboardFunc(lambda k, x, y: app.keyboard(k, x, y))
    
    print("Ready!")
    glut.glutMainLoop()

if __name__ == "__main__":
    main()
