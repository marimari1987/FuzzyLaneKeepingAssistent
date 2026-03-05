#include "Fuzzy.h"
#include <Arduino.h>


// ------------------- Konstruktor -------------------

Fuzzy::Fuzzy() {
    LANE = 500;
    LINE = 3000;
    CENTER = 1570;
    left_dir = false;
}

// ------------------- inference -------------------

Directions Fuzzy::setDirection(int smallLeft, 
                    int bigLeft,
                    int smallRight,
                    int bigRight)
{
   Directions d;
   float l = (smallLeft + bigLeft) * 0.5;
   float r = (smallRight + bigRight) * 0.5;

   if (l > r) left_dir = true; 
   else left_dir = false;

   if (left_dir) {
    d.small = smallLeft;
    d.big = bigLeft;
   }
   else{
    d.small = smallRight;
    d.big = bigRight;
   }

   return d;
}

InputMembershipDegrees Fuzzy::inference(const Directions &d)
{
    InputMembershipDegrees m;
    m.outerLine  = 0.0f;
    m.innerLine    = 0.0f;

    // Auswertung innerer Sensor
    if (d.small <= LANE) m.innerLine = 0;
    else if (LANE < d.small && d.small < LINE){
        m.innerLine = float(d.small - LANE) / float(LINE - LANE);
    }
    else {
        m.innerLine = 1;
    }

    // Auswertung äußerer Sensor
    if (d.big <= LANE) m.outerLine = 0;
    else if (LANE < d.big && d.big < LINE){
        m.outerLine = float(d.big - LANE) / float(LINE - LANE);
    }
    else{
        m.outerLine = 1;
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
    o.rule1   = 0.0f;
    o.rule2 = 0.0f;
    o.rule3 = 0.0f;
    o.rule4  = 0.0f;

    // Regel 1 : keiner der Sensoren berührt die Linie
    o.rule1 = (1.0 - m.outerLine) * (1.0 - m.innerLine);
    // Regel 2: Nur der äußere Sensor berührt die Linie
    o.rule2 = (1.0 - m.outerLine) * m.innerLine;
    // Regel 3: beide Sensoren berühren die Linie
    o.rule3 = m.outerLine * m.innerLine;
    // Regel 4: nur der innere Sensor berührt die Linie
    o.rule4 = m.outerLine * (1.0 - m.innerLine);


    return o;
}

// ------------------- defuzzify -------------------

float Fuzzy::defuzzify(const OutputMembershipDegrees &o) { //"&o" übergibt das Objekt keine Kopie
    float steering_pwm = CENTER;

    float noSteering = 0.0;
    float lightSteering = 60.0;
    float mediumSteering = 160.0;
    float maxSteering = 200.0;

    float z = (o.rule1 * noSteering + o.rule2 * lightSteering + o.rule3 * mediumSteering + o.rule4 * maxSteering)/(o.rule1+o.rule2+o.rule3+o.rule4);

    if (left_dir){
        steering_pwm = CENTER + z;
    }
    else {
        steering_pwm = CENTER - z;
    }

    return steering_pwm;
}
