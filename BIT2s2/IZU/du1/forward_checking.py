##
# @file forward_checking.py
# 
# @brief VUT FIT IZU - forward checking - 4x4 sudoku
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

lists = [[2, 3],        [1, 3],         [],          [1, 2],
         
         [],            [3, 4],         [2, 3],      [2, 4],
         
         [3, 4],        [],             [1, 3],      [1, 4],
         
         [2, 4],        [1, 4],         [1, 2],      []]

status = [deepcopy(lists)]

values = []

poss = []



def FORWARD_CHECKING():
    while True:
        # 0 - MOST CONSTRAINED VARIABLE
        list_index = 0
        list_size = 4
        for index in range(0,len(table)):
            if len(status[-1][index]) < list_size and table[index] == 0:
                list_index = index
                list_size = len(status[-1][index])
        print("list index: "+str(list_index))
        print("list size: "+str(list_size))


        # 1 - FORWARD CHECKING ALGORITHM
        value = status[-1][list_index].pop(0)
        table[list_index] = value
        print("value: "+str(value))
        values.append(value)
        poss.append(chr(ord('A')+list_index))


        # 2
        cell_checked = 0
        for cell in table:
            if cell == 0:
                break
            cell_checked += 1
        if cell_checked == len(table):
            #print("DONE.")
            return True
            

        # 3
        status.append(deepcopy(status[-1]))# copy last list
        """
        print("------------")
        for lst in status:
            count = 0
            for cell in lst:
                print(str(cell),end='\t\t')
                count += 1
                if(count == 4):
                    count = 0
                    print()
            print()
        print("------------")
        """
        # row
        row = (list_index//ROW_SIZE)
        print("row: "+str(row)+" ("+str(row*ROW_SIZE)+")")
        row = row*ROW_SIZE
        for cell in range(row,row+ROW_SIZE):# from first element
            if value in status[-1][cell]:
                status[-1][cell].remove(value)
            print(status[-1][cell])
        # column
        column = ((list_index%ROW_SIZE))
        if column == -1: column = column + ROW_SIZE
        if list_index == 0: column = 0
        print("column: "+str(column))
        for cell in range(column,len(table),ROW_SIZE):
            if value in status[-1][cell]:
                status[-1][cell].remove(value)
            print(status[-1][cell])


        """
        print("------------")
        for lst in status:
            count = 0
            for cell in lst:
                print(str(cell),end='\t\t')
                count += 1
                if(count == 4):
                    count = 0
                    print()
            print()
        print("------------")
        """
        # 4
        print("---")
        breakk = 0
        for lst_index in range(0,len(table)):#list_index
            if table[lst_index] == 0 and not status[-1][lst_index]:
                print("BREAK")
                print(table[lst_index])
                print(status[-1][lst_index])
                print(lst_index)
                status.pop(-1)
                table[list_index] = 0
                breakk = 1
                break
                #goto 7
        #return
        
        if(breakk == 0):
        
            # 5 a 6
            if(FORWARD_CHECKING()):
                return True

        
        # 7
        if(not status[-1][list_index]):
            print("-------------")
            print("FALSE")
            print(list_index)
            return False
    
    pass





#CALL
result = FORWARD_CHECKING()
print()
print()
print("---------------------------------------------")
print("FORWARD CHECKING RESULT: "+str(result))


print()
print("STEPS:")
print("A\tB\t4\tD")
print("1\tF\tG\tH")
print("I\t2\tK\tL")
print("M\tN\tO\t3")
print()
print(str(poss[0]), end=' ')
print(str(values[0]), end=' ')
print(lists)
count = 1
for lst in status:
    if len(values) > count:
        print(str(poss[count]), end=' ')
        print(str(values[count]), end=' ')
    print(lst)
    count += 1
count = 0


print()
print("SUDOKU RESULT:")
for cell in table:
    print(str(cell), end='\t')
    count += 1
    if(count == 4):
        count = 0
        print()
