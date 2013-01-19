#ifndef CUSTOMVIEWER_H_
#define CUSTOMVIEWER_H_

/** The struct type containing the data to describe the atoms */
struct AtomData{
    std::tuple<double,double,double> color;
    std::string label;
    double scale;
};

struct Settings{
	//settings
	double scale = 1.0;
	double minDrawDistance = 0.0;
	std::map<std::string,bool> omit;
} settings;

//shaders
GLuint vs, fs, prog;

//The sphere object used to draw all of the spheres
Sphere sphere(16);

//The data containing the information about our atoms
std::vector<AtomData> atomData;
std::map<std::string,std::vector<std::tuple<double,double,double>>> positions;

/** Draw a sphere */
void drawCircle( const glm::vec3 &color, const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection );

/** Get the contents of a file for parsing in shaders */
std::string getFileContent( const std::string &fileName);

/** Get the yaml data */
bool getInput(std::string file);

/** Initialize openGL, etc */
bool initGL();

/** Initialize the GUI */
bool initGUI();

/** Initialize the shaders */
bool initShaders();


#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#endif