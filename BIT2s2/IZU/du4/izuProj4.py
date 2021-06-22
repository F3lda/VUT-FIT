###
# CODE SOURCE: FIT Discord -> MrakDun-desu (18. 4. 2021) [EDITED]
###

import numpy

data = [
    [ 0,-1,-2], 
    [-3,-1,-3], 
    [ 1,-3, 2], 
    [-2,-2, 2],
    [ 1, 2,-4],
    [ 0,-4, 3],
    [ 1, 0,-3],
    [-3, 0, 0],
    [-2, 2,-4],
    [-2, 4, 3],
    [ 3,-2, 4],
    [ 2,-5,-4]
]

# set your values here
midpoints = [
    [4, -1, 4],[0, -3, 1],[-3, -1, -3]
]

print("Body:")
print("C0={")
for i in range(len(data)):
    print("  " + str(data[i]))
print("}")

print()

print("Stredove body (zadani c. 171):")
for i in range(len(midpoints)):
    print("P" + str(i+1) + "=" + str(midpoints[i]))

print()

number_of_dimensions = 3

groups = []
former_groups = []

for i in range(number_of_dimensions):
    groups.append([])
    former_groups.append([])

iterations = 0

print("------------------------------")

while True:
    #print("Iteracia", iterations)
    
    print("------------------------------")
    print("Shluky:")
    distancess = []
    for i in range(len(data)):
        #print("Bod %d:" % (i+1))
        distances = numpy.array(numpy.zeros(number_of_dimensions))
        for j in range(len(midpoints)):
            distances[j] = numpy.linalg.norm(numpy.array(data[i]) - numpy.array(midpoints[j]))
            #print("  Vzdialenost od ciela %d: %f" % (j+1, distances[j]))

        if distances[0] <= distances[1] and distances[0] <= distances[2]:
            groups[0].append([i, data[i]])
        elif distances[1] <= distances[0] and distances[1] <= distances[2]:
            groups[1].append([i, data[i]])
        elif distances[2] <= distances[0] and distances[2] <= distances[1]:
            groups[2].append([i, data[i]])
        distancess.append(distances)
    
    for i in range(len(groups)):
        print("C%d={" % (i+1))
        for j in range(len(groups[i])):
            #print("  Bod %d:" % (groups[i][j][0] + 1), end="")
            print(" ", groups[i][j][1], end="")
            print(" => vzdalenosti od stredovych bodu (P1, P2, P3):", distancess[groups[i][j][0]].tolist())
            #print(type(distancess[groups[i][j][0]]))
            #print(type(groups[i][j][1]))
        print("}")

    if groups == former_groups:
        print("------------------------------")
        print("------------------------------")
        print()
        print("Shluky se ustalily!")
        print()
        print("Vysledne stredove body:")
        for i in range(len(midpoints)):
            #print("%d:" % (i+1), midpoints[i])
            print("P" + str(i+1) + "=" + str(midpoints[i]))
        break
    else:
        former_groups = groups

    for i in range(len(midpoints)): # set each midpoint
        positions = [.0, .0, .0]
        for j in range(len(groups[i])): # with each midpoint, check it's group
            for k in range(number_of_dimensions): # check each dimension
                positions[k] += groups[i][j][1][k]
        for j in range(number_of_dimensions):
            try:
                midpoints[i][j] = positions[j] / len(groups[i])
            except ZeroDivisionError:
                pass
    
    print()
    print("Stredove body:")
    for i in range(len(midpoints)):
        #print("%d:" % (i+1), midpoints[i])
        print("P" + str(i+1) + "=" + str(midpoints[i]))
    
    groups = []
    for i in range(number_of_dimensions):
        groups.append([])
    
    iterations += 1

