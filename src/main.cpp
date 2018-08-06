#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// ############################################################################
// ## OpenGL Shader funcs
// ############################################################################

#define GL_CONTEXT_MAJOR 4
#define GL_CONTEXT_MINOR 5

typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef int GLint;
typedef int GLsizei;
typedef float GLfloat;
typedef double GLdouble;

std::string shader_load(const std::string & filePath)
{
    printf("::shader_load(%s)\n", filePath.c_str());

    std::ifstream file(filePath);

    if (!file.is_open())
    {
        printf("::shader_load file failed!\n");
        return "";
    }

    std::string data;
    std::string line;
    while (std::getline(file, line))
    {
        data += line + "\n";
    }

    return data;
}

std::string program_log(GLuint program)
{
    GLint log_len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
    GLchar * log_str = new GLchar[log_len];
    glGetProgramInfoLog(program, log_len, NULL, log_str);
    std::string result(log_str);
    delete log_str;
    return result;
}

std::string shader_log(GLuint handle)
{
    GLint log_len;
    glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_len);
    GLchar * log_str = new GLchar[log_len];
    glGetShaderInfoLog(handle, log_len, NULL, log_str);
    std::string result(log_str);
    delete log_str;
    return result;
}

void shader_attach(GLuint program, const char * source, GLenum type)
{
    printf("::shader_attach(%u)\n", program);

    GLuint handle = glCreateShader(type);

    if (!handle)
    {
        printf("::shader_attach glCreateShader failed!\n");
        return;
    }

    glShaderSource(handle, 1, &source, NULL);
    glCompileShader(handle);

    GLint status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);

    if (!status)
    {
        printf("::shader_attach glCompileShader failed! program: %u, handle: %u\n", program, handle);
        printf("%s\n", shader_log(handle).c_str());
        return;
    }

    glAttachShader(program, handle);
}

void shader_link(GLuint program)
{
    printf("::shader_link(%u)\n", program);

    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (!status)
    {
        printf("::shader_link glLinkProgram failed! program: %u\n", program);
        printf("%s\n", program_log(program).c_str());
        return;
    }
}

void shader_validate(GLuint program)
{
    printf("::shader_validate(%u)\n", program);

    glValidateProgram(program);

    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

    if (!status)
    {
        printf("::shader_attach glValidateProgram failed! program: %u\n", program);
        printf("%s\n", program_log(program).c_str());
        return;
    }
}

void shader_uniform_float(GLuint program, const char * i, GLfloat f)
{
    GLint location = glGetUniformLocation(program, i);
    glProgramUniform1f(program, location, f);
}

void shader_uniform_vec2(GLuint program, const char * i, GLfloat f1, GLfloat f2)
{
    GLfloat floats[2] = { f1, f2 };
    GLint location = glGetUniformLocation(program, i);
    glProgramUniform2fv(program, location, 1, floats);
}

int msleep(int milliseconds)
{
    return usleep(1000 * milliseconds);
}

int main(int argc, char * argv[])
{
    // init glfw3
    int glfwinit = glfwInit();

    if (!glfwinit)
    {
        printf("main:: glfwInit failed!\n");
    }

    // init glfw3 window
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_CONTEXT_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_CONTEXT_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
    GLFWwindow * window = glfwCreateWindow(512, 512, "OpenGL DemoStuff", NULL, NULL);

    if (!window)
    {
        printf("main:: glfwCreateWindow failed!\n");
        return -1;
    }

    glfwMakeContextCurrent(window);

    // init glad
    int gladgl = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    if (!gladgl)
    {
        printf("main:: gladLoadGL failed!\n");
        return -1;
    }

    printf("main:: OpenGL version major: %d, minor: %d\n", GLVersion.major, GLVersion.minor);

    // init SDL2 & SDL2 Mixer
	int sdl2init = SDL_Init(SDL_INIT_AUDIO);

	if (sdl2init != 0)
	{
        printf("main:: SDL_Init failed!\n");
        return -1;
	}

    int sdl2mixerinit = Mix_Init(MIX_INIT_OGG);

    /*if (sdl2mixerinit != MIX_INIT_OGG)
    {
        printf("main:: Mix_Init failed! Result: %d, Error: %s\n", sdl2mixerinit,  Mix_GetError());
        return -1;
    }*/

    // load main music file(s)
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    Mix_VolumeMusic(32);
    Mix_Music * music = Mix_LoadMUS("./data/demo.ogg");

    // create main shader program
    GLuint program = glCreateProgram();

    if (!program)
    {
        printf("main:: glCreateProgram failed!\n");
        return -1;
    }

    // load geometry, vertex and fragment shaders into our program
    std::string shader_g_fs_quad = shader_load("./data/g.fs_quad.glsl");
    std::string shader_v_fs_quad = shader_load("./data/v.fs_quad.glsl");
    std::string shader_f_demo = shader_load("./data/f.demo.glsl");

    shader_attach(program, shader_g_fs_quad.c_str(), GL_GEOMETRY_SHADER);
    shader_attach(program, shader_v_fs_quad.c_str(), GL_VERTEX_SHADER);
    shader_attach(program, shader_f_demo.c_str(), GL_FRAGMENT_SHADER);
    shader_link(program);
    shader_validate(program);

    // play music
    Mix_PlayMusic(music, 1);

    GLfloat iTime = 0.0f;
    float delta_time = 1.0f / 60.0f;
    while (!glfwWindowShouldClose(window))
    {
        // frame start time
        double frame_start = glfwGetTime();

        // screen fbo dimensions
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        // resize viewport
        glViewport(0, 0, width, height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        // bind main shader program
        glUseProgram(program);

        shader_uniform_vec2(program, "iResolution", static_cast<GLfloat>(width), static_cast<GLfloat>(height));
        shader_uniform_float(program, "iTime", iTime);

        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, 1);

        // frame end time + frame time + delta time
        double frame_end = glfwGetTime();
        float frame_time = frame_end - frame_start;
        float sleep_time = std::max(delta_time - frame_time, 0.0f);
        msleep(sleep_time / 1000.0f);

        // inc time
        iTime += delta_time;

        glfwSwapBuffers(window);
        glfwPollEvents();

        printf("%.5f\n", iTime);
        fflush(stdout);
    }

    // Free resources
    Mix_FreeMusic(music);
    SDL_Quit();
    glfwTerminate();

    return 0;
}