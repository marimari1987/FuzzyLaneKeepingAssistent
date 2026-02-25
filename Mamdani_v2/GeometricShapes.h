#ifndef GEOMETRIC_SHAPES_H
#define GEOMETRIC_SHAPES_H

// Kleine Hilfs-Enumeration statt String "left"/"right"
enum TriangleSide {
    TRIANGLE_LEFT,
    TRIANGLE_RIGHT
};

class Triangle {
public:
    Triangle(float start, float end, float mu, TriangleSide side);

    float getArea() const;
    float getCenterOfGravity() const;

private:
    float start;
    float end;
    float width; //zusätzlich eingefügt für bessere lesbarkeit im Code
    float mu;
    TriangleSide side;
};

class RectangleShape {
public:
    RectangleShape(float start, float end, float mu);

    float getArea() const;
    float getCenterOfGravity() const;

private:
    float start;
    float end;
    float mu;
    float width;
};

class Trapezoid {
public:
    Trapezoid(float start, float end, float mu1, float mu2);

    float getArea() const;
    float getCenterOfGravity() const;

private:
    float start;
    float end;
    float mu1;
    float mu2;
    float width;
};

#endif // GEOMETRIC_SHAPES_H
