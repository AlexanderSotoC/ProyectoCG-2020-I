#include <QDebug>
#include <QVector3D>
#include <math.h>

#include "Cube.h"

#define PI 3.14159265358979323846

using namespace std;

void Cube::toString() {
    qDebug() << "Cone shape was selected \n";
}

void Cube::setVertices(int segX, int segY) {
    QVector3D top1 = QVector3D(1, 2, -1);
    QVector3D top2 = QVector3D(1, 2, 1);
    QVector3D top3 = QVector3D(-1, 2, 1);
    QVector3D top4 = QVector3D(-1, 2, -1);

    QVector3D bottom1 = QVector3D(1, 0, -1);
    QVector3D bottom2 = QVector3D(1, 0, 1);
    QVector3D bottom3 = QVector3D(-1, 0, 1);
    QVector3D bottom4 = QVector3D(-1, 0, -1);

    drawFace(top2, bottom2, bottom3, top3, segX, segY); // FRONT
    drawFace(top4, bottom4, bottom1, top1, segX, segY); // BACK

    drawFace(top3, bottom3, bottom4, top4, segX, segY); // LEFT
    drawFace(top1, bottom1, bottom2, top2, segX, segY); // RIGHT

    drawFace(top1, top2, top3, top4, segX, segY); // TOP
    drawFace(bottom1, bottom2, bottom3, bottom4, segX, segY); // BOTTOM
}

void Cube::setColors(bool color) {
    float r, g, b;

    srand(6667);

    for (int i = 0; i < vertices.size(); i++)
    {
        if(!color){
            colors.push_back(QVector3D(0.8,0.8,0.8));
            continue;
        }

        if (i % 3 == 0)
        {
            r = (rand() % 256) / 255.0f;
            g = (rand() % 256) / 255.0f;
            b = (rand() % 256) / 255.0f;
        }

        colors.push_back(QVector3D(r, g, b));
    }
}

void Cube::draw(QOpenGLShaderProgram *shaderProgram, int segmentsX, int segmentsY, QVector<GLenum> modes,
                bool color) {
    if(modes.isEmpty()) return;

    glDisable(GL_POLYGON_STIPPLE);

    for(int i=0;i<modes.size();i++){
        glPolygonMode(GL_FRONT_AND_BACK, modes[i]);
        vertices.clear();
        colors.clear();

        setVertices(segmentsX, segmentsY);

        if(modes[i] == GL_LINE) {
            solidColor();
            glLineWidth(3.0);
            glEnable(GL_LINE_SMOOTH);
        }else if(modes[i]== GL_POINT){
            setColors(color);
            glPointSize(5.0);
            glEnable( GL_POINT_SMOOTH );
        }
        else if(modes[i]==GL_POLYGON_STIPPLE){
            setColors(color);
            glEnable(GL_POLYGON_STIPPLE); // punteado
            GLubyte fly[] = {
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x03, 0x80, 0x01, 0xC0, 0x06, 0xC0, 0x03, 0x60,
                0x04, 0x60, 0x06, 0x20, 0x04, 0x30, 0x0C, 0x20,
                0x04, 0x18, 0x18, 0x20, 0x04, 0x0C, 0x30, 0x20,
                0x04, 0x06, 0x60, 0x20, 0x44, 0x03, 0xC0, 0x22,
                0x44, 0x01, 0x80, 0x22, 0x44, 0x01, 0x80, 0x22,
                0x44, 0x01, 0x80, 0x22, 0x44, 0x01, 0x80, 0x22,
                0x44, 0x01, 0x80, 0x22, 0x44, 0x01, 0x80, 0x22,
                0x66, 0x01, 0x80, 0x66, 0x33, 0x01, 0x80, 0xCC,
                0x19, 0x81, 0x81, 0x98, 0x0C, 0xC1, 0x83, 0x30,
                0x07, 0xe1, 0x87, 0xe0, 0x03, 0x3f, 0xfc, 0xc0,
                0x03, 0x31, 0x8c, 0xc0, 0x03, 0x33, 0xcc, 0xc0,
                0x06, 0x64, 0x26, 0x60, 0x0c, 0xcc, 0x33, 0x30,
                0x18, 0xcc, 0x33, 0x18, 0x10, 0xc4, 0x23, 0x08,
                0x10, 0x63, 0xC6, 0x08, 0x10, 0x30, 0x0c, 0x08,
                0x10, 0x18, 0x18, 0x08, 0x10, 0x00, 0x00, 0x08};
             glPolygonStipple(fly);
             glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else setColors(color);

        shaderProgram->setAttributeArray("vertex", vertices.constData());
        shaderProgram->enableAttributeArray("vertex");

        shaderProgram->setAttributeArray("color", colors.constData());
        shaderProgram->enableAttributeArray("color");

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }

}

void Cube::drawFace(QVector3D p1, QVector3D p2, QVector3D p3, QVector3D p4,
                    int segmentsX, int segmentsY)
{
    QVector<QVector3D> l1 = pointControl(p1,p2,segmentsY);
    QVector<QVector3D> l2 = pointControl(p2,p3,segmentsX);
    QVector<QVector3D> l3 = pointControl(p4,p3,segmentsY);
    QVector<QVector3D> l4 = pointControl(p1,p4,segmentsX);

    QVector<QVector3D> lastLine;

    lastLine.push_back(p1);
    for(int i=0;i<l4.size();i++) lastLine.push_back(l4[i]);
    lastLine.push_back(p4);

    QVector<QVector3D> line;

    for(int i=0;i<l1.size();i++){
        line.clear();
        line.push_back(l1[i]);

        QVector<QVector3D> medium = pointControl(l1[i], l3[i], segmentsX);
        for (int j=0;j<medium.size() ;j++ ) {
            line.push_back(medium[j]);
        }

        line.push_back(l3[i]);



        addTriangles(lastLine, line);

        lastLine.clear();
        for (int j=0;j<line.size() ;j++ ) {
            lastLine.push_back(line[j]);
        }
    }

    QVector<QVector3D> finalLine;

    finalLine.push_back(p2);
    for(int i=0;i<l2.size();i++) finalLine.push_back(l2[i]);
    finalLine.push_back(p3);

    addTriangles(lastLine, finalLine);
}

void Cube::addTriangles(QVector<QVector3D> line1, QVector<QVector3D> line2)
{

    for (int i = 0; i < line1.size() - 1; i++)
    {
        vertices.push_back(line1[i]);
        vertices.push_back(line1[i + 1]);
        vertices.push_back(line2[i + 1]);
        vertices.push_back(line2[i]);
        vertices.push_back(line2[i + 1]);
        vertices.push_back(line1[i]);
    }
}

QVector<QVector3D> Cube::pointControl(QVector3D initialPoint, QVector3D finalPoint,
                                      int segments)
{
    QVector<QVector3D> points;
    QVector3D point = QVector3D(initialPoint.x(), initialPoint.y(), initialPoint.z());
    int stepSegments = segments;

    for (int i = 2; i <= segments; i++)
    {
        point.setX((finalPoint.x() - point.x()) / stepSegments + point.x());
        point.setY((finalPoint.y() - point.y()) / stepSegments + point.y());
        point.setZ((finalPoint.z() - point.z()) / stepSegments + point.z());

        stepSegments--;
        points.append(point);
    }

    return points;
}
