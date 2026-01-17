#include "GeometricShapes.h"

// ------- Triangle -------

Triangle::Triangle(float start, float end, float mu, TriangleSide side)
    : start(start), end(end), width(end-start), mu(mu), side(side) //direkte Inititalisierung ohne Leerobjekt
{
}

float Triangle::getArea() const {
    return (width * mu) / 2.0f; //umgeändert in width statt (end - start)
}

float Triangle::getCenterOfGravity() const {
    if (side == TRIANGLE_LEFT) {
        // hohe Seite links: Schwerpunkt bei 1/3 der Basis
        return (width / 3.0f) + start;
    } else {
        // hohe Seite rechts: Schwerpunkt bei 2/3 der Basis
        return (width * 2.0f / 3.0f) + start;
    }
}

// ------- RectangleShape -------

RectangleShape::RectangleShape(float start, float end, float mu)
    : start(start), end(end), mu(mu), width(end-start)
{
}

float RectangleShape::getArea() const {
    return width * mu;
}

float RectangleShape::getCenterOfGravity() const {
    return (width / 2.0f) + start;
}

// ------- Trapezoid -------

Trapezoid::Trapezoid(float start, float end, float mu1, float mu2)
    : start(start), end(end), mu1(mu1), mu2(mu2), width(end-start)
{
}

float Trapezoid::getArea() const {
    return ((mu1 + mu2) / 2.0f) * width;
}

float Trapezoid::getCenterOfGravity() const { //TODO: hier nochmal überprüfen ob links ansteigend und rechts ansteigend korrekt umgesetzt sind
    float denom = (mu1 + mu2);
    if (denom == 0.0f) {
        // Degenerierter Fall: kein Trapez -> Schwerpunkt einfach in der Mitte
        return start + width / 2.0f;
    }
    else if (mu1 <= mu2) { //Fallunterscheidung vermutlich gar nicht gebraucht,wenn der größere Wert immer als mu2 angelegt wird.
    // steigt nach rechts
    return start + width/3 * (mu1 + 2*mu2) / denom;
    } 
    else {
    // steigt nach links
    return start + width/3 * (2*mu1 + mu2) / denom;
    }
}
