// Include standard headers
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <fstream>
#include <map>
#include <vector>
#include <tuple>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GL/glfw.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

// input reading
#include <yaml-cpp/yaml.h>

// GUI
#include <AntTweakBar.h>

#include "sphere.h"
#include "customviewer.h"
#include "controls.h"

int main( int argc, char* argv[] )
{
    std::string file("test.yaml");
    if(argc > 1)
        file = argv[1];

    if(!getInput(file))
    {
        std::cerr << "Could not read input!" << std::endl;
        exit(1);
    }
    if(!initGL())
    {
        std::cerr << "Could not initialize openGL!" << std::endl;
        exit(1);
    }
    if(!initGUI())
    {
        std::cerr << "Could not initialize GUI!" << std::endl;
        exit(1);
    }
    if(!initShaders())
    {
        std::cerr << "Could not initialize shaders!" << std::endl;
        exit(1);
    }

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    do{
        computeMatricesFromInputs();
        glm::mat4 Projection = getProjectionMatrix();
        glm::mat4 View = getViewMatrix();
        glm::mat4 Model = glm::mat4(1.0);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS); 
        // Cull triangles which normal is not towards the camera
        glEnable(GL_CULL_FACE);

        for(auto a : atomData)
        {
            if(settings.omit[a.label])
                continue;
            for(auto p : positions[a.label])
            {
                float x,y,z,s,r,g,b;
                x = std::get<0>(p)*settings.scale;
                y = std::get<1>(p)*settings.scale;
                z = std::get<2>(p)*settings.scale;

                s = a.scale*settings.scale;

                extern glm::vec3 position;
                float dx,dy,dz;
                dx = position.x - x;
                dy = position.y - y;
                dz = position.z - z;
                if(std::sqrt(dx*dx + dy*dy + dz*dz) < settings.minDrawDistance*settings.scale)
                    continue;

                r = std::get<0>(a.color);
                g = std::get<1>(a.color);
                b = std::get<2>(a.color);

                glm::mat4 translation = glm::mat4(1.0f,0.0f,0.0f,0.0f,
                                                  0.0f,1.0f,0.0f,0.0f,
                                                  0.0f,0.0f,1.0f,0.0f,
                                                  x,   y,   z,   1.0f); //this last one is x,y,z,1.0f
                glm::mat4 atomScale = glm::mat4(s,    0.0f, 0.0f, 0.0f,
                                            0.0f, s,    0.0f, 0.0f,
                                            0.0f, 0.0f, s,    0.0f,
                                            0.0f, 0.0f, 0.0f, 1.0f);
                drawCircle( glm::vec3(r, g, b), translation*atomScale*Model, View, Projection );
            }
        }
        TwDraw();
        // Swap buffers
        glfwSwapBuffers();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    // Clean up AntTweakBar 
    TwTerminate();

    // Clean up VBO and shader
    glDeleteProgram(prog);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

std::string getFileContent( const std::string &fileName)
{
    std::string line = "";
    std::string fileContent = "";

    std::ifstream fileStream(fileName.c_str(), std::ifstream::in);

    if (fileStream.is_open())
    {
        while (fileStream.good())
        {
            std::getline(fileStream,line);
            fileContent += line + "\n";
        }
        fileStream.close();
    }
    else
    {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return "";
    }
    return fileContent;
}

bool getInput(std::string file)
{
    std::ifstream fin(file);
    if(!fin)
    {
        std::cerr << "Could not open file or file does not exist!" << std::endl;
        return false;
    }
    //grab yaml stuff
    try
    {
        YAML::Parser parser(fin);
        YAML::Node doc;
        parser.GetNextDocument(doc);

        //settings
        if(doc.FindValue("settings"))
        {
            if(doc["settings"].FindValue("scale"))
            {
                doc["settings"]["scale"] >> settings.scale;
            }
            if(doc["settings"].FindValue("omit"))
            {
                for(int i = 0; i < doc["settings"]["omit"].size(); i++)
                {
                    std::string value;
                    doc["settings"]["omit"][i] >> value;
                    settings.omit[value] = true;
                }
            }
        }
        for(int i = 0; i < doc["atom_types"].size(); i++)
        {
            const YAML::Node& node = doc["atom_types"][i];
            AtomData ad;

            double r;
            double g; 
            double b;
            node["label"] >> ad.label;
            node["color"][0] >> r;
            node["color"][1] >> g;
            node["color"][2] >> b;
            ad.color = std::make_tuple(r,g,b);
            node["scale"] >> ad.scale;

            atomData.push_back(ad);

            bool hit = false;
            for(auto i = settings.omit.begin(); i != settings.omit.end(); i++)
                if(i->first == ad.label)
                    hit = true;
            if(!hit)
                settings.omit[ad.label] = false;
        }

        for(auto d : atomData)
        {
            const YAML::Node& node = doc["atom_positions"][d.label];
            for(int i = 0; i < node.size(); i++)
            {
                double x;
                double y; 
                double z;
                node[i][0] >> x;
                node[i][1] >> y;
                node[i][2] >> z;

                positions[d.label].push_back(std::make_tuple(x,y,z));
            }
        }

    }
    catch(YAML::ParserException& e)
    {
        std::cerr << "Error while reading MMC input file" << std::endl << e.what() << "\n";
        return false;
    }
    return true;
}

bool initGL()
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);

    // Open a window and create its OpenGL context
    if( !glfwOpenWindow( 1024, 768, 0,0,0,0, 32,0, GLFW_WINDOW ) )
    {
        std::cerr << "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials." << std::endl;
        glfwTerminate();
        return false;
    }

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }


    glfwSetWindowTitle( "Custom Viewer" );

    // Ensure we can capture the escape key being pressed below
    glfwEnable( GLFW_STICKY_KEYS );

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.3f, 0.0f);

    return true;
}

bool initGUI()
{
    //init gui
    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(1024, 768);
    TwBar *bar;
    bar = TwNewBar("Options");

    glfwSetMouseButtonCallback((GLFWmousebuttonfun)OnMouseButton); // - Directly redirect GLFW mouse button events to AntTweakBar
    glfwSetMousePosCallback((GLFWmouseposfun)OnMousePos);          // - Directly redirect GLFW mouse position events to AntTweakBar
    glfwSetMouseWheelCallback((GLFWmousewheelfun)OnMouseWheel);    // - Directly redirect GLFW mouse wheel events to AntTweakBar
    glfwSetKeyCallback((GLFWkeyfun)TwEventKeyGLFW);                         // - Directly redirect GLFW key events to AntTweakBar
    glfwSetCharCallback((GLFWcharfun)TwEventCharGLFW);                      // - Directly redirect GLFW char events to AntTweakBar

    for(auto i = settings.omit.begin(); i != settings.omit.end(); i++)
    {
        std::string temp = std::string(" group='Toggle Visibility' label=' Hide ") + i->first.c_str() + std::string("' help='Toggle whether or not this atom type is visible.' ");
        //current bug: if i->second is false, the button will still display as true at the beginning
        TwAddVarRW(bar, i->first.c_str(), TW_TYPE_BOOL32, &(i->second), temp.c_str());
    }

    TwAddVarRW(bar, "min Draw Dist", TW_TYPE_DOUBLE, &settings.minDrawDistance, "min=0 label='min. Draw Dist.' help='The minimum draw distance. Atoms within this radius will not be drawn.'");

    //make sure all of the options look correct
    TwRefreshBar(bar);

    return true;
}

bool initShaders()
{
    // prepare the shaders:
    std::string vs_str = getFileContent("vertex.vsh").c_str();
    std::string fs_str = getFileContent("fragment.fsh").c_str();
    const char* vs_src = vs_str.c_str();
    const char* fs_src = fs_str.c_str();

    vs = glCreateShader( GL_VERTEX_SHADER );
    fs = glCreateShader( GL_FRAGMENT_SHADER );
    prog = glCreateProgram();
    
    // at least OpenGL 3.2
    glShaderSource( vs, 1, &vs_src, NULL );
    glShaderSource( fs, 1, &fs_src, NULL );
    
    glCompileShader(vs);
    glCompileShader(fs);
    
    glAttachShader( prog, vs );
    glAttachShader( prog, fs );
    
    glLinkProgram( prog );

    glUseProgram( prog );
    
    if (glGetError() != GL_NO_ERROR) {
        std::cerr << "could not prepare shaders" << std::endl;
        return false;
    }

    return true;
}

void drawCircle( const glm::vec3 &color, const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection ) {

    // set the color:
    GLint uniformLocation = glGetUniformLocation( prog, "color" );
    glUniform3f( uniformLocation, color.r, color.g, color.b );
    // set the transformation:
    uniformLocation = glGetUniformLocation( prog, "modelView" );
    glm::mat4 modelView = view*model;
    
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(modelView));
    uniformLocation = glGetUniformLocation( prog, "projection" );
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(projection));
    
    sphere.draw();
}