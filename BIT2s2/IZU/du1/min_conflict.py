##
# @file min_conflict.py
# 
# @brief VUT FIT IZU - min-conflict - 4x4 sudoku
# @date 2021-03-10
# @author F3lda
# @update 2021-03-13
##

from copy import deepcopy

print("HI")

ROW_SIZE = 4
COL_SIZE = 4
table = [0, 0, 4, 0,
         1, 0, 0, 0,
         0, 2, 0, 0,
         0, 0, 0, 3]

status = [[1, 2, 4, 3,
          1, 4, 1, 2,
          3, 2, 4, 1,
          2, 3, 4, 3]]

conflicts = []

poss = []



#1
status.append(deepcopy(status[-1]))
print(status)



#2
cell_index = 0 # I
J = 1


###LOOP
while True:
    conflict = [0, 0, 0, 0]
    
    print("------------------------")
    print(cell_index)
    print("------------------------")
    
    
    if(table[cell_index] == 0):

        # row
        row = (cell_index//ROW_SIZE)
        print("row: "+str(row)+" ("+str(row*ROW_SIZE)+")")
        row = row*ROW_SIZE
        for cell in range(row,row+ROW_SIZE):# from first element
            if(cell != cell_index):
                print(status[-1][cell])
                conflict[status[-1][cell]-1] += 1

        # column
        column = ((cell_index%ROW_SIZE))
        if column == -1: column = column + ROW_SIZE
        if cell_index == 0: column = 0
        print("column: "+str(column))
        for cell in range(column,len(table),ROW_SIZE):
            if(cell != cell_index):
                print(status[-1][cell])
                conflict[status[-1][cell]-1] += 1
            
        print("conflict: "+str(conflict))
        conflicts.append(conflict)


        #3
        current_value = status[-1][cell_index]# current value
        min_value = min(conflict)
        print("current value: "+str(current_value))
        print("current conflict value: "+str(conflict[current_value-1]))
        print("min conflict: "+str(min_value))
        
        
        conflict_value = 6
        conflict_index = 0

            
        
        if(min_value < conflict[current_value-1]):
            print("MIN")
            count = 0
            while count < 4:
                if(conflict[count] < conflict_value):
                    conflict_value = conflict[count]
                    conflict_index = count
                count += 1
        
        else:
            print("NEXT")
            index = (current_value) % (len(conflict)) # next index

            count = 0
            while count < 4:
                if(conflict[index] < conflict_value):
                    conflict_value = conflict[index]
                    conflict_index = index
                index = (index + 1) % (len(conflict))
                count += 1


        poss.append(conflict_index)
        print("conflict index: "+str(conflict_index))
        print("conflict value: "+str(conflict_value))
        print("new value: "+str(conflict_index+1))
        status[-1][cell_index] = conflict_index+1




        if(current_value != conflict_index+1):
            J = 1
        else:
            J += 1

        print("J: "+str(J))

    else:
        conflicts.append([])
        poss.append(-1)
        J += 1



    #4
    if(J == len(table)):
        print("DONE")
        break
    else:
        cell_index += 1



    #5
    if(cell_index >= len(table)):
        status.append(deepcopy(status[-1]))
        cell_index = 0



    #6
    # back to #2





print()
print()
print("---------------------------------------------")
print("MIN CONFLICT")


print()
print("SUDOKU RESULT:")
# sudoku
count = 0
for cell in status[-1]:
    print(str(cell), end='\t')
    count += 1
    if(count == 4):
        count = 0
        print()


print()
print("TABLES: ")
# tables
count = 0
for lst in status:
    #print(lst)
    for cell in lst:
        print(str(cell), end='\t')
        count += 1
        if(count == 4):
            count = 0
            print()
    print()


print()
print("CONFLICTS: ")
# conflicts
steps = 0
for lst in conflicts:
    if steps % 16 == 0:
        print("STEP: "+str(steps // 16 + 1))
        print("----------------")
    steps += 1
    print((poss[steps-1]+1), end=' ')
    print(lst)
