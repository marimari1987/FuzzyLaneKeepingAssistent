#include "Fuzzy.h"
#include <Arduino.h>


// ------------------- Konstruktor -------------------

Fuzzy::Fuzzy() {
    // aktuell nichts zu tun
}

// ------------------- inference -------------------


InputMembershipDegrees Fuzzy::inference(int smallLeft,
                                        int bigLeft,
                                        int smallRight,
                                        int bigRight)
{
    InputMembershipDegrees m;
    m.smallLeft  = 0.0f;
    m.bigLeft    = 0.0f;
    m.normal     = 0.0f;
    m.smallRight = 0.0f;
    m.bigRight   = 0.0f;

    float muSmall  = 0.0f;
    float muBig    = 0.0f;
    float muNormal = 0.0f;

    int leftDeviation  = smallLeft  + bigLeft;
    int rightDeviation = smallRight + bigRight;

    // Grad der Deviation von 0% (Normal) bis 100% (Big) berechnen:
    float deviation;
    bool directionLeft;
    bool directionNormal; // hinzugefügt

    if ((leftDeviation < 400) && (rightDeviation < 400)){
        deviation = 0;
    }else {
        if (leftDeviation > rightDeviation) {
        float s = smallLeft/4095.0f;
        float b = bigLeft / 4095.0f;
        deviation = 0.5*s+0.5*b;
        directionLeft = true;
        directionNormal = false;
        } else {
        float s = smallRight / 4095.0f;
        float b = bigRight /4095.0f;
        deviation = 0.5*s+0.5*b;
        directionLeft = false;
        directionNormal=false;
        }
    }



    // Begrenzung, nur falls irgendwas schief geht
    if (deviation < 0.0f) deviation = 0.0f;
    if (deviation > 1.0f) deviation = 1.0f;

    // inference berechnen mit drei generellen Membershipfunktionen
    // (gespiegelt für left und right)
    if (deviation == 0.0f){
        muNormal = 1.0f;
    }
    else if (deviation < 0.5f) {
        // Absteigende Kante der Normal-Membershipfunktion
        muNormal = -2.0f * deviation + 1.0f;
        muSmall  = 1.0f - muNormal;  // Aufsteigende Kante von Small
        muBig    = 0.0f;
    } else {
        // Absteigende Kante von Small
        muSmall = -2.0f * deviation + 2;
        muBig   = 1.0f - muSmall;    // Aufsteigende Kante von Big
        muNormal = 0.0f;
    }

    if (directionLeft) {
        m.smallLeft  = muSmall;
        m.bigLeft    = muBig;
        m.normal     = muNormal;
        m.smallRight = 0.0f;
        m.bigRight   = 0.0f;
    } else {
        m.smallLeft  = 0.0f;
        m.bigLeft    = 0.0f;
        m.normal     = muNormal;
        m.smallRight = muSmall;
        m.bigRight   = muBig;
    }
    Serial.print("deviation: ");
    Serial.println(deviation);
    return m;
}

// ------------------- rules -------------------
//
// Entspricht der Python-Methode:
// def rules(self, membershipdegrees):
//   if smallLeft or bigLeft -> right
//   if smallRight or bigRight -> left
//   if normal -> normal
//

OutputMembershipDegrees Fuzzy::rules(const InputMembershipDegrees &m) {
    OutputMembershipDegrees o;
    o.bigLeft   = 0.0f;
    o.smallLeft = 0.0f;
    o.normal = 0.0f;
    o.bigRight  = 0.0f;
    o.smallRight = 0.0f;

    // Kleine Hilfsfunktion -> im Code direkt mit > 0.0f
    if (m.smallLeft > 0.0f ) {
        // nach rechts lenken
        o.smallRight = m.smallLeft;
    }
    if (m.bigLeft > 0.0f){
        o.bigRight = m.bigLeft;
    }
    if (m.smallRight > 0.0){
        o.smallLeft = m.smallRight;
    }
    if ( m.bigRight > 0.0f) {
        // nach links lenken
        o.bigLeft = m.bigRight;
    }
    if (m.normal > 0.0f) {
        o.normal = m.normal;
    }

    return o;
}

// ------------------- defuzzify -------------------

float Fuzzy::defuzzify(const OutputMembershipDegrees &o) { //"&o" übergibt das Objekt keine Kopie
    float steering_pwm = 1570.0f; // Default: geradeaus Mitte geändert

    // Zur Lesbarkeit: lokale Kopien
    float smallLeft  = o.smallLeft;
    float bigLeft = o.bigLeft;
    float smallRight = o.smallRight;
    float bigRight = o.bigRight;
    float normal = o.normal;
    float offset = 1230.0f; // nierigster Wert angepasst
    float maxvalue = 1770.0f; // höchster Wert angepasst

    if (bigLeft > 0.0f && smallLeft > 0.0f) {
        float x1, x2, x3;
        float area1, area2, area3, area4;
        float cog1, cog2, cog3, cog4;

        if (bigLeft < smallLeft) {
            // relevante Schnittpunkte berechnen
            x1 = bigLeft * 100.0f + 1370;
            x2 = smallLeft * 100.0f + 1370;
            x3 = smallLeft * -100.0f + 1570; 

            // Einzelne Flächen und Schwerpunkte berechnen
            RectangleShape shape1(1370, x1, bigLeft);
            area1 = shape1.getArea();
            cog1  = shape1.getCenterOfGravity();

            Trapezoid shape2(x1, x2, bigLeft, smallLeft);
            area2 = shape2.getArea();
            cog2  = shape2.getCenterOfGravity();

            RectangleShape shape3(x2, x3, smallLeft);
            area3 = shape3.getArea();
            cog3  = shape3.getCenterOfGravity();

            Triangle shape4(x3, 1570, smallLeft, TRIANGLE_LEFT);
            area4 = shape4.getArea();
            cog4  = shape4.getCenterOfGravity();
        } else {
            x1 = bigLeft * -100.0f + 1470;
            x2 = smallLeft * -100.0f + 1470;
            x3 = smallLeft * -100.0f + 1570;

            RectangleShape shape1(1370, x1, bigLeft);
            area1 = shape1.getArea();
            cog1  = shape1.getCenterOfGravity();

            Trapezoid shape2(x1, x2, bigLeft, smallLeft);
            area2 = shape2.getArea();
            cog2  = shape2.getCenterOfGravity();

            RectangleShape shape3(x2, x3, smallLeft);
            area3 = shape3.getArea();
            cog3  = shape3.getCenterOfGravity();

            Triangle shape4(x3, 1570, smallLeft, TRIANGLE_LEFT);
            area4 = shape4.getArea();
            cog4  = shape4.getCenterOfGravity();
        }

        float sumArea = area1 + area2 + area3 + area4;
        if (sumArea > 0.0f) {
            steering_pwm = (area1 * cog1 + area2 * cog2 + area3 * cog3 + area4 * cog4) / sumArea;
        }
    }
    else if (bigRight > 0.0f && smallRight > 0.0f) {
        float x1, x2, x3;
        float area1, area2, area3, area4;
        float cog1, cog2, cog3, cog4;

        if (bigRight < smallRight) {
            x1 = smallRight * 100.0f + 1570;
            x2 = smallRight * -100.0f + 1770;
            x3 = bigRight * -100.0f + 1770;

            Triangle shape1(1570, x1, smallRight, TRIANGLE_RIGHT);
            area1 = shape1.getArea();
            cog1  = shape1.getCenterOfGravity();

            RectangleShape shape2(x1, x2, smallRight);
            area2 = shape2.getArea();
            cog2  = shape2.getCenterOfGravity();

            Trapezoid shape3(x2, x3, bigRight, smallRight);
            area3 = shape3.getArea();
            cog3  = shape3.getCenterOfGravity();

            RectangleShape shape4(x3, 1770, bigRight);
            area4 = shape4.getArea();
            cog4  = shape4.getCenterOfGravity();
        } else {
            x1 = smallRight * 100.0f + 1570;
            x2 = smallRight * 100.0f + 1670;
            x3 = bigRight * 100.0f + 1670;

            Triangle shape1(1570, x1, smallRight, TRIANGLE_RIGHT);
            area1 = shape1.getArea();
            cog1  = shape1.getCenterOfGravity();

            RectangleShape shape2(x1, x2, smallRight);
            area2 = shape2.getArea();
            cog2  = shape2.getCenterOfGravity();

            Trapezoid shape3(x2, x3, smallRight, bigRight);
            area3 = shape3.getArea();
            cog3  = shape3.getCenterOfGravity();

            RectangleShape shape4(x3, 1770, bigRight);
            area4 = shape4.getArea();
            cog4  = shape4.getCenterOfGravity();
        }

        float sumArea = area1 + area2 + area3 + area4;
        if (sumArea > 0.0f) {
            steering_pwm = (area1 * cog1 + area2 * cog2 + area3 * cog3 + area4 * cog4) / sumArea;
        }
    }
    else if (bigLeft > 0.0f) {
        float x1 = bigLeft * -100.0f + 1470;

        RectangleShape shape1(1370, x1, bigLeft);
        float area1 = shape1.getArea();
        float cog1  = shape1.getCenterOfGravity();

        Triangle shape2(x1, 1470, bigLeft, TRIANGLE_LEFT);
        float area2 = shape2.getArea();
        float cog2  = shape2.getCenterOfGravity();

        float sumArea = area1 + area2;
        if (sumArea > 0.0f) {
            steering_pwm = (area1 * cog1 + area2 * cog2) / sumArea;
        }
    }
    else if (bigRight > 0.0f) {
        float x1 =  bigRight * 100.0f + 1670;

        Triangle shape1(1670, x1, bigRight, TRIANGLE_RIGHT); //Mitte geändert
        float area1 = shape1.getArea();
        float cog1  = shape1.getCenterOfGravity();

        RectangleShape shape2(x1, 1770, bigRight);
        float area2 = shape2.getArea();
        float cog2  = shape2.getCenterOfGravity();

        float sumArea = area1 + area2;
        if (sumArea > 0.0f) {
            steering_pwm = (area1 * cog1 + area2 * cog2) / sumArea;
        }
    }
    else if (normal > 0.0f && (smallLeft > 0.0 || smallRight > 0.0f)){
        float l, r, x0, x1 , x2, x3, x4, xn;
        if (normal < smallLeft){
            l = smallLeft;
            r = normal;
            x0 = 1370;
            x1 = smallLeft * 100.0f + 1370;
            x2 = smallLeft * -100.0f + 1570;
            x3 = normal * -100.0f + 1570;
            x4 = normal * -100.0f + 1670;
            xn = 1670;
        }
        else if (normal < smallRight){
            l = normal;
            r = smallRight;
            x0 = 1470;
            x1 = normal * 100.0f + 1470;
            x2 = normal * 100.0f + 1570;
            x3 = smallRight * 100.0f + 1570;
            x4 = smallRight * -100.0f + 1770;
            xn = 1770;
        }
        else if ((smallLeft > 0) and (smallLeft < normal)){
            l = smallLeft;
            r = normal;
            x0 = 1370;
            x1 = smallLeft * 100.0f + 1370;
            x2 = smallLeft * 100.0f + 1470;
            x3 = normal * 100.0f + 1470;
            x4 = normal * -100.0f + 1670;
            xn = 1670;
        }
        else {
            l = normal;
            r = smallRight;
            x0 = 1470;
            x1 = normal * 100.0f + 1470;
            x2 = normal * -100.0f + 1670;
            x3 = smallRight * -100.0f + 1670;
            x4 = smallRight * -100.0f + 1770;
            xn = 1770;
        }

        Triangle shape1(x0, x1, l, TRIANGLE_RIGHT);
        float area1 = shape1.getArea();
        float cog1  = shape1.getCenterOfGravity();

        RectangleShape shape2(x1, x2, l);
        float area2 = shape2.getArea();
        float cog2  = shape2.getCenterOfGravity();

        Trapezoid shape3(x2, x3, l, r);
        float area3 = shape3.getArea();
        float cog3 = shape3.getCenterOfGravity();

        RectangleShape shape4(x3, x4, r);
        float area4 = shape4.getArea();
        float cog4 = shape4.getCenterOfGravity();

        Triangle shape5(x4, xn, r, TRIANGLE_LEFT);
        float area5 = shape5.getArea();
        float cog5 = shape5.getCenterOfGravity();

        float sumArea = area1 + area2 + area3 + area4 + area5;
        if (sumArea > 0.0f) {
            steering_pwm = (area1 * cog1 + area2 * cog2 + area3 * cog3 + area4*cog4 + area5 * cog5) / sumArea;
    }
    else if (smallLeft >0.0f) {
        steering_pwm = 1470; 
    }
    else if (smallRight > 0.0){
        steering_pwm = 1670;
    }
    else {
        steering_pwm = 1570.0f; // angepasste Mitte
    }
    }
    return steering_pwm;
}
