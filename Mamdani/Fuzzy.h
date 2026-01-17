#ifndef FUZZY_H
#define FUZZY_H

#include "GeometricShapes.h"

// Struktur für die Membershipgrade der Eingangsseite
struct InputMembershipDegrees {
    float smallLeft;
    float bigLeft;
    float normal;
    float smallRight;
    float bigRight;
};

// Struktur für die Output-Membershipgrade (left, normal, right)
struct OutputMembershipDegrees {
    float left;
    float normal;
    float right;
};

class Fuzzy {
public:
    Fuzzy();

    // 1. Fuzzifizierung / Inferenz
    InputMembershipDegrees inference(int smallLeft,
                                     int bigLeft,
                                     int smallRight,
                                     int bigRight);

    // 2. Regelbasis
    OutputMembershipDegrees rules(const InputMembershipDegrees &m);

    // 3. Defuzzifikation: gibt PWM-Signal (1300...1700) zurück
    float defuzzify(const OutputMembershipDegrees &o);

};

#endif // FUZZY_H
