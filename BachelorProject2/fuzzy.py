from dataclasses import dataclass
from typing import Tuple


# ------------------- Datenklassen -------------------

@dataclass
class InputMembershipDegrees:
    smallLeft: float = 0.0
    bigLeft: float = 0.0
    normal: float = 0.0
    smallRight: float = 0.0
    bigRight: float = 0.0


@dataclass
class OutputMembershipDegrees:
    smallLeft: float = 0.0
    normal: float = 0.0
    smallRight: float = 0.0
    # Membershipfunktionen erweitert
    bigLeft: float = 0.0
    bigRight: float = 0.0


# ------------------- Geometrie-Helfer (wie in C++) -------------------

TRIANGLE_LEFT = "left"    # Höhe links, fällt nach rechts auf 0
TRIANGLE_RIGHT = "right"  # Höhe rechts, steigt von 0 links nach rechts auf Höhe


class RectangleShape:
    def __init__(self, x0: float, x1: float, h: float):
        self.x0 = float(x0)
        self.x1 = float(x1)
        self.h = float(h)

    def getArea(self) -> float:
        w = max(0.0, self.x1 - self.x0)
        return w * self.h

    def getCenterOfGravity(self) -> float:
        return 0.5 * (self.x0 + self.x1)


class Trapezoid:
    """
    Trapez mit linearer Oberkante:
    Höhe h1 bei x1, Höhe h2 bei x2.
    """
    def __init__(self, x1: float, x2: float, h1: float, h2: float):
        self.x1 = float(x1)
        self.x2 = float(x2)
        self.h1 = float(h1)
        self.h2 = float(h2)

    def getArea(self) -> float:
        b = max(0.0, self.x2 - self.x1)
        return b * (self.h1 + self.h2) * 0.5

    def getCenterOfGravity(self) -> float:
        b = self.x2 - self.x1
        if b <= 0.0:
            return self.x1
        denom = (self.h1 + self.h2)
        if denom <= 0.0:
            return 0.5 * (self.x1 + self.x2)

        # Schwerpunkt eines linear verjüngten Trapezes (baryzentrisch):
        # gemessen ab x1 (wo Höhe h1 sitzt)
        # xbar_rel = b * (2*h1 + h2) / (3*(h1 + h2))
        xbar_rel = b * (2.0 * self.h1 + self.h2) / (3.0 * denom)
        return self.x1 + xbar_rel


class Triangle:
    """
    Rechtwinkliges Dreieck über [x0, x1] mit Höhe h.
    TRIANGLE_LEFT: Höhe bei x0, 0 bei x1.
    TRIANGLE_RIGHT: 0 bei x0, Höhe bei x1.
    """
    def __init__(self, x0: float, x1: float, h: float, orientation: str):
        self.x0 = float(x0)
        self.x1 = float(x1)
        self.h = float(h)
        self.orientation = orientation

    def getArea(self) -> float:
        b = max(0.0, self.x1 - self.x0)
        return 0.5 * b * self.h

    def getCenterOfGravity(self) -> float:
        b = self.x1 - self.x0
        if b <= 0.0:
            return self.x0
        if self.orientation == TRIANGLE_LEFT:
            # Eckpunkte: (x0,0), (x1,0), (x0,h) -> xbar = x0 + b/3
            return self.x0 + b / 3.0
        else:
            # Eckpunkte: (x0,0), (x1,0), (x1,h) -> xbar = x0 + 2b/3
            return self.x0 + 2.0 * b / 3.0


# ------------------- Fuzzy (Python-Port deines C++ Codes) -------------------

class Fuzzy:
    def __init__(self):
        pass

    def inference(self, smallLeft: int, bigLeft: int, smallRight: int, bigRight: int) -> InputMembershipDegrees:
        m = InputMembershipDegrees()

        def _clip(v: int) -> int:
            return v if v >= 100 else 0 # TODO: Werte clippen

        muSmall = 0.0
        muBig = 0.0
        muNormal = 0.0

        # smallLeft = _clip(smallLeft) # TODO: Hier dann die geklippten werte
        # bigLeft = _clip(bigLeft)
        # smallRight = _clip(smallRight)
        # bigRight = _clip(bigRight)

        leftDeviation = int(smallLeft + bigLeft)
        rightDeviation = int(smallRight + bigRight)

        # deviation + Richtung bestimmen (wie C++)
        if leftDeviation < 400 and rightDeviation < 400:
            deviation = 0
            directionNormal = True
            directionLeft = False
        elif leftDeviation > rightDeviation:
            s = smallLeft / 4095
            b = bigLeft / 4095
            deviation = 0.5 * s + 0.5 * b
            # TODO: Hier eine angepasste Formel hin, damit BIG mehr gwichtet wird
            directionLeft = True
            directionNormal = False
        else: # rightDeviation > leftDeviation: # TODO: auch hier anpassen
            s = smallRight / 4095
            b = bigRight / 4095
            deviation = 0.5 * s + 0.5 * b
            # TODO: Hier eine angepasste Formel hin, damit BIG mehr gwichtet wird
            directionLeft = False
            directionNormal = False
        # else:
        #     deviation = 0
        #     directionNormal = True # TODO: Hier auch Normal einfügen
        #     directionLeft = False

        # # clamp
        # if deviation < 0.0:
        #     deviation = 0.0
        # if deviation > 1.0:
        #     deviation = 1.0
        if deviation == 0: # TODO: einfügen
            moNormal = 1
        elif deviation < 0.5:  # Absteigende Kante der Normal Membershipfunktion
            muNormal = -2 * deviation + 1
            muSmall = 1 - muNormal  # Aufsteigende Kante von Small
        elif deviation == 0.5:
            muSmall = 1
        else:
            muSmall = -2 * deviation + 2  # Absteigende Kante von Small
            muBig = 1 - muSmall  # Aufsteigende Kante von Big

        if directionLeft:
            m.smallLeft = muSmall
            m.bigLeft = muBig
            m.normal = muNormal
            m.smallRight = 0.0
            m.bigRight = 0.0
        elif directionNormal:
            m.normal = 1.0
        else:
            m.smallLeft = 0.0
            m.bigLeft = 0.0
            m.normal = muNormal
            m.smallRight = muSmall
            m.bigRight = muBig

        return m

    def rules(self, m: InputMembershipDegrees) -> OutputMembershipDegrees:
        o = OutputMembershipDegrees()

        if  (m.bigLeft > 0.0) : # TODO: Regeln für mehr membershipfunktionen
            o.bigRight = m.bigLeft
            pass
        if (m.smallLeft > 0.0) :
            o.smallRight = m.smallLeft
            pass
        if (m.smallRight > 0.0) :
            o.smallLeft = m.smallRight
            pass
        if (m.bigRight > 0.0) :
            o.bigLeft = m.bigRight
            pass
        if (m.normal > 0.0) :
            o.normal = m.normal
            pass


        # if (m.smallLeft > 0.0) or (m.bigLeft > 0.0):
        #     # nach rechts lenken
        #     o.right = m.bigLeft if (m.bigLeft > m.smallLeft) else m.smallLeft
        #
        # if (m.smallRight > 0.0) or (m.bigRight > 0.0):
        #     # nach links lenken
        #     o.left = m.smallRight if (m.smallRight > m.bigRight) else m.bigRight
        #
        # if m.normal > 0.0:
        #     o.normal = m.normal

        return o

    def defuzzify(self, o: OutputMembershipDegrees) -> float:
        steering_pwm = 1570.0  # Default geradeaus (Mitte geändert)

        smalLeft = float(o.smallLeft)
        smallRight = float(o.smallRight)
        normal = float(o.normal)
        # für erweiterte membershipdegrees
        bigLeft = float(o.bigLeft)
        bigRight = float(o.bigRight)

        offset = 1370.0   # niedrigster Wert Angepasst
        maxvalue = 1770.0 # höchster Wert
        center = 1570.0   # Mitte geändert

        if bigLeft > 0.0 and smalLeft > 0.0:
            if bigLeft < smalLeft:
                x1 = bigLeft * 100.0 + 1370
                x2 = smalLeft * 100.0 + 1370
                x3 = smalLeft * -100.0 + 1570 #TODO: Die Funktionen sind falsch

                shape1 = RectangleShape(offset, x1, bigLeft)
                shape2 = Trapezoid(x1, x2, bigLeft, smalLeft)
                shape3 = RectangleShape(x2, x3, smalLeft)
                shape4 = Triangle(x3, 1570, smalLeft, TRIANGLE_LEFT) # geprüft, könnte stimmen
            else:
                x1 = bigLeft * -100.0 + 1470
                x2 = smalLeft * -100.0 + 1470
                x3 = smalLeft * -100.0 + 1570

                shape1 = RectangleShape(offset, x1, bigLeft)
                shape2 = Trapezoid(x1, x2, bigLeft, smalLeft)
                shape3 = RectangleShape(x2, x3, smalLeft)
                shape4 = Triangle(x3, 1570, smalLeft, TRIANGLE_LEFT)

            area1, cog1 = shape1.getArea(), shape1.getCenterOfGravity()
            area2, cog2 = shape2.getArea(), shape2.getCenterOfGravity()
            area3, cog3 = shape3.getArea(), shape3.getCenterOfGravity()
            area4, cog4 = shape4.getArea(), shape4.getCenterOfGravity()

            sumArea = area1 + area2 + area3 + area4
            if sumArea > 0.0:
                steering_pwm = (area1*cog1 + area2*cog2 + area3*cog3 + area4*cog4) / sumArea

        elif bigRight > 0.0 and smallRight > 0.0:
            if bigRight < smallRight:
                x1 = smallRight * 100.0 + 1570
                x2 = smallRight * -100.0 + 1770
                x3 = bigRight * -100.0 + 1770

                shape1 = Triangle(1570, x1, smallRight, TRIANGLE_RIGHT)
                shape2 = RectangleShape(x1, x2, smallRight)
                shape3 = Trapezoid(x2, x3, bigRight, smallRight)
                shape4 = RectangleShape(x3, 1770, bigRight)
            else:
                x1 = smallRight * 100.0 + 1570
                x2 = smallRight * 100.0 + 1670
                x3 = bigRight * 100.0 + 1670

                shape1 = Triangle(1570, x1, smallRight, TRIANGLE_RIGHT)
                shape2 = RectangleShape(x1, x2, smallRight)
                shape3 = Trapezoid(x2, x3, smallRight, bigRight)
                shape4 = RectangleShape(x3, 1770, bigRight)

            area1, cog1 = shape1.getArea(), shape1.getCenterOfGravity()
            area2, cog2 = shape2.getArea(), shape2.getCenterOfGravity()
            area3, cog3 = shape3.getArea(), shape3.getCenterOfGravity()
            area4, cog4 = shape4.getArea(), shape4.getCenterOfGravity()

            sumArea = area1 + area2 + area3 + area4
            if sumArea > 0.0:
                steering_pwm = (area1*cog1 + area2*cog2 + area3*cog3 + area4*cog4) / sumArea

        elif bigLeft > 0.0:
            x1 = bigLeft * -100.0 + 1470

            shape1 = RectangleShape(1370, x1, bigLeft)
            shape2 = Triangle(x1, 1470, bigLeft, TRIANGLE_LEFT)

            area1, cog1 = shape1.getArea(), shape1.getCenterOfGravity()
            area2, cog2 = shape2.getArea(), shape2.getCenterOfGravity()

            sumArea = area1 + area2
            if sumArea > 0.0:
                steering_pwm = (area1*cog1 + area2*cog2) / sumArea

        elif bigRight > 0.0:
            x1 = bigRight * 100.0 + 1670

            shape1 = Triangle(1670, x1, bigRight, TRIANGLE_RIGHT)
            shape2 = RectangleShape(x1, 1770, bigRight)

            area1, cog1 = shape1.getArea(), shape1.getCenterOfGravity()
            area2, cog2 = shape2.getArea(), shape2.getCenterOfGravity()

            sumArea = area1 + area2
            if sumArea > 0.0:
                steering_pwm = (area1*cog1 + area2*cog2) / sumArea

        # Neue Formen für mehr membershipfunktionen:

        # TODO: Einfügen

        elif normal > 0.0 and (smalLeft > 0.0 or smallRight > 0.0):
            if normal < smalLeft:
                l = smalLeft
                r = normal
                x0 = 1370
                x1 = smalLeft * 100.0 + 1370
                x2 = smalLeft * -100.0 + 1570
                x3 = normal * -100.0 + 1570
                x4 = normal * -100.0 + 1670
                xn = 1670
            elif normal < smallRight:
                l = normal
                r = smallRight
                x0 = 1470
                x1 = normal * 100.0 + 1470
                x2 = normal * 100.0 + 1570
                x3 = smallRight * 100.0 + 1570
                x4 = smallRight * -100.0 + 1770
                xn = 1770
            elif (smalLeft > 0) and (smalLeft < normal):
                l = smalLeft
                r = normal
                x0 = 1370
                x1 = smalLeft * 100.0 + 1370
                x2 = smalLeft * 100.0 + 1470
                x3 = normal * 100.0 + 1470
                x4 = normal * -100.0 + 1670
                xn = 1670
            else:
                l = normal
                r = smallRight
                x0 = 1470
                x1 = normal * 100.0 + 1470
                x2 = normal * -100.0 + 1670
                x3 = smallRight * -100.0 + 1670
                x4 = smallRight * -100.0 + 1770
                xn = 1770



            shape1 = Triangle(x0, x1, l, TRIANGLE_RIGHT)
            shape2 = RectangleShape(x1, x2, l)
            shape3 = Trapezoid(x2, x3, l, r)
            shape4 = RectangleShape(x3,x4, r)
            shape5 = Triangle(x4,xn, r, TRIANGLE_LEFT)

            area1, cog1 = shape1.getArea(), shape1.getCenterOfGravity()
            area2, cog2 = shape2.getArea(), shape2.getCenterOfGravity()
            area3, cog3 = shape3.getArea(), shape3.getCenterOfGravity()
            area4, cog4 = shape4.getArea(), shape4.getCenterOfGravity()
            area5, cog5 = shape5.getArea(), shape5.getCenterOfGravity()

            sumArea = area1 + area2 + area3 + area4 + area5

            if sumArea > 0.0:
                steering_pwm = (area1*cog1 + area2*cog2 + area3*cog3 + area4*cog4 + area5*cog5) / sumArea

        elif smalLeft > 0.0:
            steering_pwm = 1470

        elif smallRight > 0.0:
            steering_pwm = 1670

        else:
            steering_pwm = center

        return steering_pwm

    # convenience: alles in einem Schritt
    def compute_pwm(self, sensorValues: Tuple[int, int, int, int]) -> float:
        m = self.inference(*sensorValues)
        o = self.rules(m)
        return self.defuzzify(o)
