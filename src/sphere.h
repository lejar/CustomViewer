#ifndef SPHERE_H_
#define SPHERE_H_

class Sphere {
public:
    Sphere( unsigned int detail = 8 );
    ~Sphere();

    void draw();

private:
    glm::vec3 positionOnSphere( int sliceNumber, int stackNumber );

    void addTriangle( unsigned int sl0, unsigned int st0,
                      unsigned int sl1, unsigned int st1,
                      unsigned int sl2, unsigned int st2,
                      unsigned int &posOffset );

    unsigned int stacks;
    unsigned int slices;
    unsigned int vertexCount;
    unsigned int elementCount;
    GLuint arrayBuffer;
    float *vertexData;
    float radius;
    bool uploadedDataToGPU;
};

#endif