import matplotlib.pyplot as plt

class Fuzzy:
    def __init__(self):
        self.LANE = 500
        self.LINE = 3000
        self.CENTER = 1570
        self.left_dir = False

    def setDirection(self, sL, bL, sR, bR):
        ''' Takes Sensorvalues small Left, big Left, small Right, big Right. \n
        Returns: small, big (from the side of the bigger deviation) '''
        l = (sL + bL) * 0.5
        r = (sR + bR) * 0.5

        # Richtung bestimmen
        self.left_dir = True if l > r else False

        if self.left_dir:
            s = sL
            b = bL
        else:
            s = sR
            b = bR

        return s, b

    def inference(self, s, b):
        '''  Array [boolean leftDirection, float line membershipdegree outer Sensor,
        float line membershipdegree inner Sensor] '''


        # Zugehörigkeitsgrade Linie bestimmen
        if s <= self.LANE: line_i = 0
        elif self.LANE < s < self.LINE:
            line_i = (s - self.LANE) / (self.LINE - self.LANE)
            pass
        else:
            line_i = 1

        if b <= self.LANE: line_o = 0
        elif self.LANE < b < self.LINE:
            line_o = (b - self.LANE) / (self.LINE - self.LANE)
            pass
        else:
            line_o = 1


        return line_o, line_i

    def ruleSet(self, line_i, line_o):
        # keiner der Sesnoren berührt die Linie:
        w1 = (1-line_i)*(1-line_o)
        # Nur der äußere Sensor berührt die Linie
        # if not line_i and line_o:
        w2 = (1-line_i)*line_o
        # beide Sensoren berühren die Linie
        # if line_i and line_0:
        w3 = line_i*line_o
        # nur der innere Sensor berührt die Linie
        w4 = line_i*(1-line_o)

        return w1, w2, w3, w4

    def calculateOutput(self,w1, w2, w3, w4):
        no_steering = 0
        light_steering = 60
        medium_steering = 160
        max_steering = 200

        z = (w1 * no_steering + w2 * light_steering + w3 * medium_steering + w4 * max_steering)/(w1+w2+w3+w4)

        if self.left_dir:
            output = self.CENTER + z
        else:
            output = self.CENTER - z


        return output

fuzzy = Fuzzy()
s,b = fuzzy.setDirection(0,0,3350,100)
line_o, line_i = fuzzy.inference(s,b)
w1,w2,w3,w4 = fuzzy.ruleSet(line_o, line_i)
print(fuzzy.calculateOutput(w1,w2,w3,w4))

# fuzzy = Fuzzy()
# outputL = []
# outputR = []
# sR = []
# bR = []
# sL = []
# bL = []
#
#
# for i in range(0, 4095, 50):
#     for j in range(0,4095, 50):
#         sL.append(i)
#         bL.append(j)
#
#         s, b = fuzzy.setDirection(i, j, 0, 0)
#         line_o, line_i = fuzzy.inference(s, b)
#         w1,w2, w3, w4 = fuzzy.ruleSet(line_o, line_i)
#         outputR.append(fuzzy.calculateOutput(w1, w2, w3, w4))
#
# for i in range(0, 4095, 50):
#     for j in range(0,4095, 50):
#         sR.append(i)
#         bR.append(j)
#
#         s, b = fuzzy.setDirection(0, 0, i, j)
#         line_o, line_i = fuzzy.inference(s, b)
#         w1,w2, w3, w4 = fuzzy.ruleSet(line_o, line_i)
#         outputL.append(fuzzy.calculateOutput(w1, w2, w3, w4))
#
#
# fig = plt.figure()
# ax = fig.add_subplot(111, projection='3d')
#
# ax.scatter(sL,bL,outputR, s=10, color='red')
# ax.scatter(sR,bR,outputL, s=10, color='blue')
# ax.scatter(3350,100, 1370, s=10, color='green')
#
# ax.set_xlabel('small')
# ax.set_ylabel('big')
# ax.set_zlabel('pwm')
#
# plt.show()
#
#
#
#
