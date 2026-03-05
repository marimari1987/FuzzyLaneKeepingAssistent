#ifndef FUZZY_H
#define FUZZY_H

// Struktur für die Richtungsauswertung
struct Directions {
    int small;
    int big;
};

// Struktur für die Membershipgrade der Eingangsseite
struct InputMembershipDegrees {
    float outerLine;
    float innerLine;
};

// Struktur für die Output-Membershipgrade (left, normal, right)
struct OutputMembershipDegrees {
    float rule1;
    float rule2;
    float rule3;
    float rule4;
};

class Fuzzy {
    private:
        int LANE;
        int LINE;
        int CENTER;
        bool left_dir;
public:
    Fuzzy();

    // 1. Richtung bestimmen
    Directions setDirection(int smallLeft,
                            int bigLeft,
                            int smallRight,
                            int bigRight);

    // 1. Fuzzifizierung / Inferenz
    InputMembershipDegrees inference(const Directions &dir);

    // 2. Regelbasis
    OutputMembershipDegrees rules(const InputMembershipDegrees &m);

    // 3. Defuzzifikation: gibt PWM-Signal (1300...1700) zurück
    float defuzzify(const OutputMembershipDegrees &o);

};

#endif // FUZZY_H
