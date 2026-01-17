#include "Fuzzy.h"

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

    if (leftDeviation > rightDeviation) {
        // range: (0 - 4095)*2 für zwei Sensoren --> durch 8192
        deviation = leftDeviation / 8192.0f;
        directionLeft = true;
    } else {
        deviation = rightDeviation / 8192.0f;
        directionLeft = false;
    }

    // Begrenzung, nur falls irgendwas schief geht
    if (deviation < 0.0f) deviation = 0.0f;
    if (deviation > 1.0f) deviation = 1.0f;

    // inference berechnen mit drei generellen Membershipfunktionen
    // (gespiegelt für left und right)
    if (deviation < 0.5f) {
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
    o.left   = 0.0f;
    o.normal = 0.0f;
    o.right  = 0.0f;

    // Kleine Hilfsfunktion -> im Code direkt mit > 0.0f
    if (m.smallLeft > 0.0f || m.bigLeft > 0.0f) {
        // nach rechts lenken
        o.right = (m.bigLeft > m.smallLeft) ? m.bigLeft : m.smallLeft;
    }
    if (m.smallRight > 0.0f || m.bigRight > 0.0f) {
        // nach links lenken
        o.left = (m.smallRight > m.bigRight) ? m.smallRight : m.bigRight;
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
    float left  = o.left;
    float right = o.right;
    float normal = o.normal;
    float offset = 1230.0f; // nierigster Wert angepasst
    float maxvalue = 1770.0f; // höchster Wert angepasst

    if (left > 0.0f && normal > 0.0f) {
        float x1, x2, x3;
        float area1, area2, area3, area4;
        float cog1, cog2, cog3, cog4;

        if (left < normal) {
            // relevante Schnittpunkte berechnen
            x1 = left * 200.0f + offset;
            x2 = normal * 200.0f + offset;
            x3 = (normal - 2.0f) * -200.0f + offset;

            // Einzelne Flächen und Schwerpunkte berechnen
            RectangleShape shape1(offset, x1, left);
            area1 = shape1.getArea();
            cog1  = shape1.getCenterOfGravity();

            Trapezoid shape2(x1, x2, left, normal);
            area2 = shape2.getArea();
            cog2  = shape2.getCenterOfGravity();

            RectangleShape shape3(x2, x3, normal);
            area3 = shape3.getArea();
            cog3  = shape3.getCenterOfGravity();

            Triangle shape4(x3, maxvalue, normal, TRIANGLE_LEFT);
            area4 = shape4.getArea();
            cog4  = shape4.getCenterOfGravity();
        } else {
            x1 = (left - 1.0f)  * -200.0f + offset;
            x2 = (normal - 1.0f) * -200.0f + offset;
            x3 = (normal - 2.0f) * -200.0f + offset;

            RectangleShape shape1(offset, x1, left);
            area1 = shape1.getArea();
            cog1  = shape1.getCenterOfGravity();

            Trapezoid shape2(x1, x2, left, normal);
            area2 = shape2.getArea();
            cog2  = shape2.getCenterOfGravity();

            RectangleShape shape3(x2, x3, normal);
            area3 = shape3.getArea();
            cog3  = shape3.getCenterOfGravity();

            Triangle shape4(x3, maxvalue, normal, TRIANGLE_LEFT);
            area4 = shape4.getArea();
            cog4  = shape4.getCenterOfGravity();
        }

        float sumArea = area1 + area2 + area3 + area4;
        if (sumArea > 0.0f) {
            steering_pwm = (area1 * cog1 + area2 * cog2 + area3 * cog3 + area4 * cog4) / sumArea;
        }
    }
    else if (right > 0.0f && normal > 0.0f) {
        float x1, x2, x3;
        float area1, area2, area3, area4;
        float cog1, cog2, cog3, cog4;

        if (right < normal) {
            x1 = normal * 200.0f + offset;
            x2 = (normal - 2.0f) * -200.0f + offset;
            x3 = (right - 2.0f) * -200.0f + offset;

            Triangle shape1(offset, x1, normal, TRIANGLE_RIGHT);
            area1 = shape1.getArea();
            cog1  = shape1.getCenterOfGravity();

            RectangleShape shape2(x1, x2, normal);
            area2 = shape2.getArea();
            cog2  = shape2.getCenterOfGravity();

            Trapezoid shape3(x2, x3, right, normal);
            area3 = shape3.getArea();
            cog3  = shape3.getCenterOfGravity();

            RectangleShape shape4(x3, maxvalue, right);
            area4 = shape4.getArea();
            cog4  = shape4.getCenterOfGravity();
        } else {
            x1 = normal * 200.0f + offset;
            x2 = (normal + 1.0f) * 200.0f + offset;
            x3 = (right + 1.0f) * 200.0f + offset;

            Triangle shape1(offset, x1, normal, TRIANGLE_RIGHT);
            area1 = shape1.getArea();
            cog1  = shape1.getCenterOfGravity();

            RectangleShape shape2(x1, x2, normal);
            area2 = shape2.getArea();
            cog2  = shape2.getCenterOfGravity();

            Trapezoid shape3(x2, x3, normal, right);
            area3 = shape3.getArea();
            cog3  = shape3.getCenterOfGravity();

            RectangleShape shape4(x3, maxvalue, right);
            area4 = shape4.getArea();
            cog4  = shape4.getCenterOfGravity();
        }

        float sumArea = area1 + area2 + area3 + area4;
        if (sumArea > 0.0f) {
            steering_pwm = (area1 * cog1 + area2 * cog2 + area3 * cog3 + area4 * cog4) / sumArea;
        }
    }
    else if (left > 0.0f) {
        float x1 = (left - 1.0f) * -200.0f + offset;
        float x2 = 1570.0f; //Mitte geändert

        RectangleShape shape1(offset, x1, left);
        float area1 = shape1.getArea();
        float cog1  = shape1.getCenterOfGravity();

        Triangle shape2(x1, x2, left, TRIANGLE_LEFT);
        float area2 = shape2.getArea();
        float cog2  = shape2.getCenterOfGravity();

        float sumArea = area1 + area2;
        if (sumArea > 0.0f) {
            steering_pwm = (area1 * cog1 + area2 * cog2) / sumArea;
        }
    }
    else if (right > 0.0f) {
        float x1 = (right + 1.0f) * 200.0f + offset;

        Triangle shape1(1570.0f, x1, right, TRIANGLE_RIGHT); //Mitte geändert
        float area1 = shape1.getArea();
        float cog1  = shape1.getCenterOfGravity();

        RectangleShape shape2(x1, maxvalue, right);
        float area2 = shape2.getArea();
        float cog2  = shape2.getCenterOfGravity();

        float sumArea = area1 + area2;
        if (sumArea > 0.0f) {
            steering_pwm = (area1 * cog1 + area2 * cog2) / sumArea;
        }
    }
    else {
        // nur normal (oder alles 0): geradeaus
        steering_pwm = 1570.0f; // angepasste Mitte
    }

    return steering_pwm;
}
