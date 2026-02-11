import fuzzy
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

fuzzy = fuzzy.Fuzzy()
output = []
sR = []
bR = []

for i in range(0,4095, 50):
    for j in range(0,4095, 50):
        sR.append(i)
        bR.append(j)
        membershipdegrees = fuzzy.inference(i,j,0,0)
        outputmembershipdegrees = fuzzy.rules(membershipdegrees)
        output.append(fuzzy.defuzzify(outputmembershipdegrees))

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')


ax.scatter(sR, bR, output, s=10)
ax.scatter([350],[4050], [1455.73], s=10, color='red')

ax.set_xlabel('sR')
ax.set_ylabel('bR')
ax.set_zlabel('pwm')

plt.show()


for value in output:
    if 1440 < value < 1460:
        index = output.index(value)
        if sR[index] < 1000:
            print(value)
            print('output: ', output[index])
            print('sR: ', sR[index])
            print('bR: ', bR[index])

print(fuzzy.compute_pwm((0,0,1000,3200)))


# print(sR)
# print(bR)
# print(output)


