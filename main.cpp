#include "result.h"

#include <GL/glcorearb.h>
#include <csetjmp>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <limits.h>
#include <png.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

constexpr const char *path = "321_chekhonina_v0v5.png";
static double render_time = 60;
int width = 512;
int height = 512;

GLFWwindow *win;

struct GL {
    PFNGLACTIVETEXTUREPROC activeTexture;
    PFNGLATTACHSHADERPROC attachShader;
    PFNGLDELETEBUFFERSPROC deleteBuffers;
    PFNGLBINDFRAMEBUFFERPROC bindFramebuffer;
    PFNGLBINDIMAGETEXTUREPROC bindImageTexture;
    PFNGLBINDTEXTUREPROC bindTexture;
    PFNGLBLITFRAMEBUFFERPROC blitFramebuffer;
    PFNGLBUFFERDATAPROC bufferData;
    PFNGLCLEARPROC clear;
    PFNGLCOMPILESHADERPROC compileShader;
    PFNGLBINDBUFFERBASEPROC bindBufferBase;
    PFNGLCREATEPROGRAMPROC createProgram;
    PFNGLCREATESHADERPROC createShader;
    PFNGLDEBUGMESSAGECALLBACKPROC debugMessageCallback;
    PFNGLDELETEFRAMEBUFFERSPROC deleteFramebuffers;
    PFNGLDELETEPROGRAMPROC deleteProgram;
    PFNGLDELETESHADERPROC deleteShader;
    PFNGLDELETETEXTURESPROC deleteTextures;
    PFNGLDISPATCHCOMPUTEPROC dispatchCompute;
    PFNGLENABLEPROC enable;
    PFNGLFRAMEBUFFERTEXTURE2DPROC framebufferTexture2D;
    PFNGLGENBUFFERSPROC genBuffers;
    PFNGLGENFRAMEBUFFERSPROC genFramebuffers;
    PFNGLGENTEXTURESPROC genTextures;
    PFNGLGETINTEGERI_VPROC getIntegeri_v;
    PFNGLGETINTEGERVPROC getIntegerv;
    PFNGLGETPROGRAMIVPROC getProgramiv;
    PFNGLGETSHADERINFOLOGPROC getShaderInfoLog;
    PFNGLGETSHADERIVPROC getShaderiv;
    PFNGLGETTEXIMAGEPROC getTexImage;
    PFNGLLINKPROGRAMPROC linkProgram;
    PFNGLMEMORYBARRIERPROC memoryBarrier;
    PFNGLPIXELSTOREIPROC pixelStorei;
    PFNGLSHADERSOURCEPROC shaderSource;
    PFNGLTEXIMAGE2DPROC texImage2D;
    PFNGLTEXIMAGE3DPROC texImage3D;
    PFNGLTEXPARAMETERIPROC texParameteri;
    PFNGLUNIFORM1IPROC uniform1i;
    PFNGLUSEPROGRAMPROC useProgram;
    PFNGLVIEWPORTPROC viewport;
    PFNGLBINDBUFFERPROC bindBuffer;

    void load() {
        enable = (decltype(enable))glfwGetProcAddress("glEnable");
        deleteBuffers = (decltype(deleteBuffers))glfwGetProcAddress("glDeleteBuffers");
        bindBuffer = (decltype(bindBuffer))glfwGetProcAddress("glBindBuffer");
        bindBufferBase = (decltype(bindBufferBase))glfwGetProcAddress("glBindBufferBase");
        activeTexture = (decltype(activeTexture))glfwGetProcAddress("glActiveTexture");
        attachShader = (decltype(attachShader))glfwGetProcAddress("glAttachShader");
        bindFramebuffer = (decltype(bindFramebuffer))glfwGetProcAddress("glBindFramebuffer");
        bindImageTexture = (decltype(bindImageTexture))glfwGetProcAddress("glBindImageTexture");
        bindTexture = (decltype(bindTexture))glfwGetProcAddress("glBindTexture");
        blitFramebuffer = (decltype(blitFramebuffer))glfwGetProcAddress("glBlitFramebuffer");
        bufferData = (decltype(bufferData))glfwGetProcAddress("glBufferData");
        clear = (decltype(clear))glfwGetProcAddress("glClear");
        compileShader = (decltype(compileShader))glfwGetProcAddress("glCompileShader");
        createProgram = (decltype(createProgram))glfwGetProcAddress("glCreateProgram");
        createShader = (decltype(createShader))glfwGetProcAddress("glCreateShader");
        deleteFramebuffers = (decltype(deleteFramebuffers))glfwGetProcAddress("glDeleteFramebuffers");
        deleteProgram = (decltype(deleteProgram))glfwGetProcAddress("glDeleteProgram");
        deleteShader = (decltype(deleteShader))glfwGetProcAddress("glDeleteShader");
        deleteTextures = (decltype(deleteTextures))glfwGetProcAddress("glDeleteTextures");
        dispatchCompute = (decltype(dispatchCompute))glfwGetProcAddress("glDispatchCompute");
        framebufferTexture2D = (decltype(framebufferTexture2D))glfwGetProcAddress("glFramebufferTexture2D");
        genBuffers = (decltype(genBuffers))glfwGetProcAddress("glGenBuffers");
        genFramebuffers = (decltype(genFramebuffers))glfwGetProcAddress("glGenFramebuffers");
        genTextures = (decltype(genTextures))glfwGetProcAddress("glGenTextures");
        getIntegeri_v = (decltype(getIntegeri_v))glfwGetProcAddress("glGetIntegeri_v");
        getIntegerv = (decltype(getIntegerv))glfwGetProcAddress("glGetIntegerv");
        getProgramiv = (decltype(getProgramiv))glfwGetProcAddress("glGetProgramiv");
        getShaderInfoLog = (decltype(getShaderInfoLog))glfwGetProcAddress("glGetShaderInfoLog");
        getShaderiv = (decltype(getShaderiv))glfwGetProcAddress("glGetShaderiv");
        getTexImage = (decltype(getTexImage))glfwGetProcAddress("glGetTexImage");
        linkProgram = (decltype(linkProgram))glfwGetProcAddress("glLinkProgram");
        memoryBarrier = (decltype(memoryBarrier))glfwGetProcAddress("glMemoryBarrier");
        pixelStorei = (decltype(pixelStorei))glfwGetProcAddress("glPixelStorei");
        shaderSource = (decltype(shaderSource))glfwGetProcAddress("glShaderSource");
        texImage2D = (decltype(texImage2D))glfwGetProcAddress("glTexImage2D");
        texImage3D = (decltype(texImage3D))glfwGetProcAddress("glTexImage3D");
        texParameteri = (decltype(texParameteri))glfwGetProcAddress("glTexParameteri");
        uniform1i = (decltype(uniform1i))glfwGetProcAddress("glUniform1i");
        useProgram = (decltype(useProgram))glfwGetProcAddress("glUseProgram");
        viewport = (decltype(viewport))glfwGetProcAddress("glViewport");
    }
} gl;

static void write_png(const char *path, const uint8_t *data, size_t width, size_t height) {
    char tmp_path[PATH_MAX];
    snprintf(tmp_path, PATH_MAX - 1, "%s.new", path);
    FILE *fp = fopen(tmp_path, "wb");
    if (!fp) {
        std::cerr <<  "Cannot open file '" << path << "'" << std::endl;
        return;
    }

    png_bytep *rows = (png_bytep *)calloc(height, sizeof(*rows));
    for (size_t i = 0; i < height; i++)
        rows[i] = (png_bytep)data + width*(height - i - 1)*3;

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        std::cerr << "Cannot init png write" << std::endl;
        goto close_file;
    }

    png_infop png_info;
    if (!(png_info = png_create_info_struct(png_ptr))) {
        std::cerr << "Cannot init png info" << std::endl;
        goto destroy_write;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        std::cerr << "Error during png write" << std::endl;
        goto destroy_write;
    }

    png_init_io(png_ptr, fp);


    png_set_IHDR(png_ptr, png_info, width, height, 8, PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    png_set_rows(png_ptr, png_info, (png_bytepp)rows);
    png_write_png(png_ptr, png_info, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_end(png_ptr, png_info);

destroy_write:
    png_destroy_write_struct(&png_ptr, &png_info);
close_file:
    fclose(fp);
    free(rows);
    rename(tmp_path, path);
}

struct mapping {
    char *addr;
    size_t size;

    static mapping map(const char *path) {
        char *addr;
        int fd = open(path, O_RDONLY);
        if (fd < 0) goto e_open;

        struct stat stt;
        if (fstat(fd, &stt) < 0) goto e_open;

        addr = (char *)mmap(nullptr, stt.st_size + 1, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        if (addr == MAP_FAILED) goto e_open;

        close(fd);

        return { addr, size_t(stt.st_size) };

    e_open:
        if (fd >= 0) close(fd);
        std::cerr << "Failed to map file '" << path << "' : " << strerror(errno) << std::endl;
        return { nullptr, 0 };
    }

    ~mapping() {
        munmap(addr, size + 1);
    }
};

class Texture {
    friend class Framebuffer;
    GLuint id;
public:
    Texture(const Texture &) = delete;
    Texture(Texture &&other) { id = other.id; other.id = 0; }
    Texture &operator=(const Texture &) = delete;
    Texture &operator=(Texture &&other) { std::swap(id, other.id); return *this; };

    Texture(int width, int height, void *data = nullptr) {
        gl.genTextures(1, &id);
        gl.bindTexture(GL_TEXTURE_2D, id);
        gl.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl.texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl.pixelStorei(GL_UNPACK_ALIGNMENT, 1);
        gl.texImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data);
        gl.bindTexture(GL_TEXTURE_2D, 0);
    }
    ~Texture() {
        gl.deleteTextures(1, &id);
    }
    void bindUnit(int binding) {
        gl.bindImageTexture(binding, id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    }

    void read(void *to) {
        gl.bindTexture(GL_TEXTURE_2D, id);
        gl.getTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, to);
        gl.bindTexture(GL_TEXTURE_2D, 0);
    }
};

class TextureArray {
    friend class Framebuffer;
    GLuint id;
public:
    TextureArray(const TextureArray &) = delete;
    TextureArray(TextureArray &&other) { id = other.id; other.id = 0; }
    TextureArray &operator=(const TextureArray &) = delete;
    TextureArray &operator=(TextureArray &&other) { std::swap(id, other.id); return *this; };

    TextureArray(int width, int height, int depth, const void *data = nullptr) {
        gl.genTextures(1, &id);
        gl.bindTexture(GL_TEXTURE_2D_ARRAY, id);
        gl.texParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        gl.texParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        gl.texParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        gl.texParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
        gl.texImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        gl.bindTexture(GL_TEXTURE_2D_ARRAY, 0);
    }
    ~TextureArray() {
        gl.deleteTextures(1, &id);
    }
    void bindUnit(int binding) {
        gl.activeTexture(GL_TEXTURE0 + binding);
        gl.bindTexture(GL_TEXTURE_2D_ARRAY, id);
    }
};

class Framebuffer {
    GLuint id;
public:
    Framebuffer(const Framebuffer &) = delete;
    Framebuffer(Framebuffer &&other) { id = other.id; other.id = 0; }
    Framebuffer &operator=(const Framebuffer &) = delete;
    Framebuffer &operator=(Framebuffer &&other) { std::swap(id, other.id); return *this; };

    Framebuffer(const Texture &tex) {
        gl.genFramebuffers(1, &id);
        gl.bindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
        gl.framebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.id, 0);
        gl.bindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    ~Framebuffer() {
        gl.deleteFramebuffers(1, &id);
    }

    void bind(GLuint target = GL_DRAW_FRAMEBUFFER) {
        gl.bindFramebuffer(target, id);
    }
    void unbind(GLuint target = GL_DRAW_FRAMEBUFFER) {
        gl.bindFramebuffer(target, id);
    }
};

class ComputeShader {
    GLuint id;
public:
    ComputeShader(const ComputeShader &) = delete;
    ComputeShader(ComputeShader &&other) { id = other.id; other.id = 0; }
    ComputeShader &operator=(const ComputeShader &) = delete;
    ComputeShader &operator=(ComputeShader &&other) { std::swap(id, other.id); return *this; };

    ComputeShader(const char *src) {
        GLuint cs = gl.createShader(GL_COMPUTE_SHADER);

        gl.shaderSource(cs, 1, &src, 0);
        gl.compileShader(cs);

        id = gl.createProgram();
        gl.attachShader(id, cs);
        gl.linkProgram(id);

        gl.deleteShader(cs);
    }

    ~ComputeShader() {
        gl.deleteProgram(id);
    }

    void bind() {
        gl.useProgram(id);
    }
};

int main(int argc, char* argv[]) {
    for (int i = 0; i < argc; i++) {
        if (!strcmp(argv[i], "-w") && argv[i+1]) {
            width = height = strtol(argv[i + 1], NULL, 0);
            i++;
        } else if (!strcmp(argv[i], "-T") && argv[i+1]) {
                render_time = strtod(argv[i + 1], NULL);
                i++;
            }
    }
    if (!glfwInit()) {
        std::cerr << "Unable to initiallize OpenGL context" << std::endl;
        return 1;
    }

    // We need OpenGL 4.3 for compute shaders
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    win = glfwCreateWindow(width, height, "Tracer preview", nullptr, nullptr);
    if (!win) {
        std::cerr << "Unable to create window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(win);
    {
        gl.load();
        gl.viewport(0, 0, width, height);

        Texture dest(width, height);
        Framebuffer fbo(dest);

        TextureArray mesh(16, 16, 208*64/16/16, textures);

        auto m = mapping::map("shader.cs");
        if (!m.addr) return 1;
        ComputeShader cs(m.addr);

        cs.bind();
        fbo.bind(GL_READ_FRAMEBUFFER);
        dest.bindUnit(0);
        mesh.bindUnit(1);
        gl.uniform1i(0, 0);
        gl.uniform1i(1, 1);

        int frame{};
        std::vector<uint8_t> scrot(width*height*3);
        auto time0 = glfwGetTime();
        do {
            gl.uniform1i(2, frame);
            gl.dispatchCompute(width, height, 1);
            gl.memoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
            if (!(++frame % 64)) {
                dest.read(scrot.data());
                write_png(path, scrot.data(), width, height);
            }
            gl.clear(GL_COLOR_BUFFER_BIT);
            gl.blitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glfwSwapBuffers(win);
            glfwPollEvents();
        } while (!glfwWindowShouldClose(win) && glfwGetTime() - time0 < render_time);
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
