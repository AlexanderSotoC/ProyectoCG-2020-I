#ifndef SPECIAL_H
#define SPECIAL_H
#include "Shape.h"

class Special: public Shape {
    private:
        double generateRandomNumber();
        void setVertices(int segmentsX, int segmentsY);
        void setColors(bool color);
    public:
        void toString();
        void draw(QOpenGLShaderProgram *shaderProgram, int segmentsX, int segmentsY, QVector<GLenum> modes,
                  bool color);
};

#endif
