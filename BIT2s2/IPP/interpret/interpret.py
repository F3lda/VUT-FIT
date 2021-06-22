##
# @file interpret.py
# 
# @brief VUT FIT IPP projekt 2020/2021
# @date 2021-04-10
# @author F3lda
# @update 2021-04-20
##

import re
import sys
import copy
import argparse
import xml.etree.ElementTree as ET


class Argument:
    def __init__(self, arg_num, arg_type):
        self.arg_num = arg_num
        self.arg_type = arg_type
        self.arg_value = None
    
    def add_value(self, value):
        self.arg_value = value
    
    def get_data(self):
        return [self.arg_num, self.arg_type, self.arg_value]


class InstructionsIterator:
    def __init__(self, instructions):
        self._instructions = instructions
        self._index = 0
        
    def __next__(self):
        if (self._instructions != None):
            self._index += 1
            result = self._instructions.get_instruction(self._index)
            if (result != None):
                return result
        raise StopIteration


class Instruction:
    # class variable
    instances = []
    
    def __init__(self, ins_num, ins_name):
        self.ins_num = ins_num
        self.ins_name = ins_name
        self.ins_args = []
        Instruction.instances.append(self)
    
    # class method
    @classmethod
    def get_instruction(cls, ins_num):
        for inst in cls.instances: 
            if inst.ins_num == ins_num:
                return inst
        return None
        # list of alll instances #return [inst for inst in cls.instances if inst.ins_num == ins_num]
    
    def add_argument(self, arg):
        self.ins_args.append(arg)
    
    def get_data(self):
        return [self.ins_num, self.ins_name]
    
    def get_arg_count(self):
        return len(self.ins_args)
    
    def get_args(self):
        return self.ins_args
    
    def get_arg_data(self, arg_num):
        for arg in self.ins_args:
            if (arg.get_data()[0] == arg_num):
                return arg.get_data()
        return None
    
    def __iter__(self):
       return InstructionsIterator(self)


class SymTable:
    def __init__(self):
        self.symtable = {} # dict
    
    def add_symbol(self, sym_name):
        if (not sym_name in self.symtable):
            self.symtable[sym_name] = None
            return True
        return False
    
    def set_symbol(self, sym_name, sym_type, sym_value):
        if (sym_name in self.symtable):
            self.symtable[sym_name] = [sym_type, sym_value]
            return True
        return False
    
    def get_symbol(self, sym_name):
        if (sym_name in self.symtable):
            return self.symtable[sym_name]
        return False
    
    def del_symbol(self, sym_name):
        if (sym_name in self.symtable):
            del self.symtable[sym_name]
            return True
        return False
    
    def clear(self):
        self.symtable.clear()


class SymTabLabels(SymTable):
    def set_symbol(self, sym_name, sym_instr_number):
        if (sym_name in self.symtable):
            self.symtable[sym_name] = ['LABEL', sym_instr_number]
            return True
        return False
    
    def get_symbol(self, sym_name):
        if (sym_name in self.symtable):
            return self.symtable[sym_name][1]
        return False


class Stack:
    def __init__(self):
        self.stack = [] # list
    
    def push(self, value):
        self.stack.insert(0, value)
    
    def pop(self):
        if (self.stack != []):
            return self.stack.pop(0)
        return None
    
    def peek(self):
        if (self.stack != []):
            return self.stack[0]
        return None


def printerr(str):
    print(str, file=sys.stderr)


def intt(strr):
    try:
        return int(strr)
    except ValueError:
        return 0


def sum_count_check(summ, countt):
    right_sum = 0
    for i in range(1,countt+1):
        right_sum += i
    return (right_sum == summ)


def xml_check_attributes(attrs_list, attrs_req, attrs_opt):
    attrs = copy.deepcopy(attrs_list)
    for attr in attrs_list:
        if (attr in attrs_req):
            attrs.remove(attr)
            attrs_req.remove(attr)
        elif (attr in attrs_opt):
            attrs.remove(attr)
            attrs_opt.remove(attr)
        else:
            return False
    if (attrs == [] and attrs_req == []):
        return True
    return False



def is_type(arg):
    return (arg == 'bool' or arg == 'int' or arg == 'string')

def is_label(arg):
    return (re.search('^([a-zA-Z_\-$&%*!?])([a-zA-Z0-9_\-$&%*!?]*)$', arg) != None and re.search('^([a-zA-Z_\-$&%*!?])([a-zA-Z0-9_\-$&%*!?]*)$', arg).group() == arg)

def is_var(arg):
    if (re.findall('\s+', arg) == []):
        parts = re.split('@', arg)
        if (len(parts) == 2):
            if (parts[0] == 'LF' or parts[0] == 'TF' or parts[0] == 'GF'):
                return is_label(parts[1])
    return False

def is_constant(typ,arg):
    if (typ == 'string'):
        return True # check in parser
    elif (re.findall('\s+', arg) == []):
        if(typ == 'nil'):
            # nil@nil
            return (arg == 'nil')
        elif (typ == 'bool'):
            # bool@true or bool@false
            return (arg == 'true' or arg == 'false')
        elif (typ == 'int'):
            # int@<number> (int@35)
            return (re.search('^([0-9-]+)$', arg) != None and re.search('^([0-9-]+)$', arg).group() == arg)
    return False

def is_symbol(typ,arg):
    return (is_constant(typ,arg) or is_var(arg))



### INTERPRET FUNCTIONS

labelSymTable = SymTabLabels()
globalSymTable = SymTable()
localSymTabStack = Stack()
tempSymTable = None
callStack = Stack()
dataStack = Stack()

def interpret_symbol_get_value_and_type(symbol_type, symbol_value):
    exit_code = 0
    
    ### IS VARIABLE
    if (symbol_type == 'var'):
    
        # get memory type
        parts = re.split('@', symbol_value)
        if (parts[0] == 'GF'):
            
            # get variable value
            if (globalSymTable.get_symbol(parts[1]) != False and globalSymTable.get_symbol(parts[1]) != None):
                
                return [0, globalSymTable.get_symbol(parts[1])[0], globalSymTable.get_symbol(parts[1])[1]]
            
            # value not set
            elif (globalSymTable.get_symbol(parts[1]) == None):
                # 56 - běhová chyba interpretace – chybějící hodnota (v proměnné, na datovém zásobníku nebov zásobníku volání)
                printerr("ERROR - interpret_symbol_get_value_and_type: call stack is empty!")
                exit_code = 56
            
            else:
                # 54 - běhová chyba interpretace – přístup k neexistující proměnné (rámec existuje)
                printerr("ERROR - interpret_symbol_get_value_and_type: var doesn't exist!")
                exit_code = 54
        
        
        elif (parts[0] == 'LF'):
            
            # check if local frame exists
            if (localSymTabStack.peek() != None):
                localFrame = localSymTabStack.peek()
                if (localFrame.get_symbol(parts[1]) != False and localFrame.get_symbol(parts[1]) != None):
                    
                    return [0, localFrame.get_symbol(parts[1])[0], localFrame.get_symbol(parts[1])[1]]
                
                # value not set
                elif (localFrame.get_symbol(parts[1]) == None):
                    # 56 - běhová chyba interpretace – chybějící hodnota (v proměnné, na datovém zásobníku nebov zásobníku volání)
                    printerr("ERROR - interpret_symbol_get_value_and_type: call stack is empty!")
                    exit_code = 56
                
                else:
                    # 54 - běhová chyba interpretace – přístup k neexistující proměnné (rámec existuje)
                    printerr("ERROR - interpret_symbol_get_value_and_type: var doesn't exist!")
                    exit_code = 54
                
            else:
                # 55 - běhová chyba interpretace – rámec neexistuje (např. čtení z prázdného zásobníku rámců)
                printerr("ERROR - interpret_symbol_get_value_and_type: LF stack is empty!")
                exit_code = 55
        
        
        elif (parts[0] == 'TF'):
        
            # check if temporary symtable exists
            if (tempSymTable != None):
                if (tempSymTable.get_symbol(parts[1]) != False and tempSymTable.get_symbol(parts[1]) != None):
                    
                    return [0, tempSymTable.get_symbol(parts[1])[0], tempSymTable.get_symbol(parts[1])[1]]
                
                # value not set
                elif (tempSymTable.get_symbol(parts[1]) == None):
                    # 56 - běhová chyba interpretace – chybějící hodnota (v proměnné, na datovém zásobníku nebov zásobníku volání)
                    printerr("ERROR - interpret_symbol_get_value_and_type: call stack is empty!")
                    exit_code = 56
                
                else:
                    # 54 - běhová chyba interpretace – přístup k neexistující proměnné (rámec existuje)
                    printerr("ERROR - interpret_symbol_get_value_and_type: var doesn't exist!")
                    exit_code = 54
                
            else:
                # 55 - běhová chyba interpretace – rámec neexistuje (např. čtení z prázdného zásobníku rámců)
                printerr("ERROR - interpret_symbol_get_value_and_type: TF doesn't exist!")
                exit_code = 55
    
    ### IS CONSTANT
    else:
    
        return [0, symbol_type, symbol_value]
    
    return [exit_code]


def interpret_var_set_value(var, typ, value):
    exit_code = 0
    
    # get memory type
    parts = re.split('@', var)
    if (parts[0] == 'GF'):
        
        # set variable value
        if (not globalSymTable.set_symbol(parts[1], typ, value)):
            # 54 - běhová chyba interpretace – přístup k neexistující proměnné (rámec existuje)
            printerr("ERROR - MOVE: var doesn't exist!")
            exit_code = 54
    
    
    elif (parts[0] == 'LF'):
        
        # check if local frame exists
        if (localSymTabStack.peek() != None):
            localFrame = localSymTabStack.peek()
            if (not localFrame.set_symbol(parts[1], typ, value)):
                # 54 - běhová chyba interpretace – přístup k neexistující proměnné (rámec existuje)
                printerr("ERROR - MOVE: var doesn't exist!")
                exit_code = 54
            
        else:
            # 55 - běhová chyba interpretace – rámec neexistuje (např. čtení z prázdného zásobníku rámců)
            printerr("ERROR - MOVE: LF stack is empty!")
            exit_code = 55
    
    
    elif (parts[0] == 'TF'):
    
        # check if temporary symtable exists
        if (tempSymTable != None):
            if (not tempSymTable.set_symbol(parts[1], typ, value)):
                # 54 - běhová chyba interpretace – přístup k neexistující proměnné (rámec existuje)
                printerr("ERROR - MOVE: var doesn't exist!")
                exit_code = 54
            
        else:
            # 55 - běhová chyba interpretace – rámec neexistuje (např. čtení z prázdného zásobníku rámců)
            printerr("ERROR - MOVE: TF doesn't exist!")
            exit_code = 55
    
    return exit_code


def interpret():
    exit_code = 0
    
    global labelSymTable
    global globalSymTable
    global localSymTabStack
    global tempSymTable
    global callStack
    global dataStack
    
    
    
    ### FILL LABELS SYMTABLE
    currentInstructionNumber = 1
    
    for instr in Instruction.get_instruction(1):

        instr_num_name = instr.get_data()
        
        ### LABEL #########################
        if (instr_num_name[1] == 'LABEL'):
            
            # check arg count and types
            if (instr.get_arg_count() == 1 and is_label(instr.get_arg_data(1)[2])):
                
                if (labelSymTable.add_symbol(instr.get_arg_data(1)[2])):
                    
                    labelSymTable.set_symbol(instr.get_arg_data(1)[2], currentInstructionNumber)
                
                ### no error if label exists and have the same instruction number
                elif (int(labelSymTable.get_symbol(instr.get_arg_data(1)[2])) != currentInstructionNumber):
                    # 52 - chyba při sémantických kontrolách vstupního kódu v IPPcode21
                    printerr("ERROR - LABEL: label already exists!")
                    exit_code = 52
                
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - LABEL: invalid arguments or wrong count!")
                exit_code = 32
        
        currentInstructionNumber += 1
    
    
    
    
    
    ### INTERPRET CODE
    currentInstructionNumber = 1
    
    while (Instruction.get_instruction(currentInstructionNumber) != None and exit_code == 0):
        
        instr = Instruction.get_instruction(currentInstructionNumber)
        instr_num_name = instr.get_data()
        
        
        #print(instr_num_name[1])
        
        
        ### MOVE #########################
        if (instr_num_name[1] == 'MOVE'):
            
            # check arg count and types
            if (instr.get_arg_count() == 2 and is_var(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])):
                
                exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                if (exit_code != 0):
                    printerr("ERROR - MOVE: var set value")
                
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - MOVE: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### CREATEFRAME #########################
        elif (instr_num_name[1] == 'CREATEFRAME'):
            
            # check arg count and types
            if (instr.get_arg_count() == 0):
                
                if (tempSymTable == None):
                    tempSymTable = SymTable()
                else:
                    tempSymTable.clear()
            
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - CREATEFRAME: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### PUSHFRAME #########################
        elif (instr_num_name[1] == 'PUSHFRAME'):
            
            # check arg count and types
            if (instr.get_arg_count() == 0):
            
                # check if temporary symtable exists
                if (tempSymTable != None):
                    localSymTabStack.push(tempSymTable)
                    tempSymTable = None
                    
                else:
                    # 55 - běhová chyba interpretace – rámec neexistuje (např. čtení z prázdného zásobníku rámců)
                    printerr("ERROR - PUSHFRAME: TF doesn't exist!")
                    exit_code = 55
            
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - PUSHFRAME: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### POPFRAME ######################### 
        elif (instr_num_name[1] == 'POPFRAME'):
            
            # check arg count and types
            if (instr.get_arg_count() == 0):
            
                # check if local frame exists
                if (localSymTabStack.peek() != None):
                    tempSymTable = localSymTabStack.pop()
                    
                else:
                    # 55 - běhová chyba interpretace – rámec neexistuje (např. čtení z prázdného zásobníku rámců)
                    printerr("ERROR - POPFRAME: LF stack is empty!")
                    exit_code = 55
            
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - POPFRAME: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### DEFVAR #########################
        elif (instr_num_name[1] == 'DEFVAR'):
            
            # check arg count and types
            if (instr.get_arg_count() == 1 and is_var(instr.get_arg_data(1)[2])):
                
                # get memory type
                parts = re.split('@', instr.get_arg_data(1)[2])
                if (parts[0] == 'GF'):
                    
                    # add and check var to global symtable
                    if (not globalSymTable.add_symbol(parts[1])):
                        # 52 - chyba při sémantických kontrolách vstupního kódu v IPPcode21
                        printerr("ERROR - DEFVAR: var already exists!")
                        exit_code = 52
                
                
                if (parts[0] == 'LF'):
                    
                    # check if local frame exists
                    if (localSymTabStack.peek() != None):
                        localFrame = localSymTabStack.peek()
                        if (not localFrame.add_symbol(parts[1])):
                            # 54 - běhová chyba interpretace – přístup k neexistující proměnné (rámec existuje)
                            printerr("ERROR - DEFVAR: var doesn't exist!")
                            exit_code = 54
                        
                    else:
                        # 55 - běhová chyba interpretace – rámec neexistuje (např. čtení z prázdného zásobníku rámců)
                        printerr("ERROR - DEFVAR: LF stack is empty!")
                        exit_code = 55
                
                
                if (parts[0] == 'TF'):
                
                    # check if temporary symtable exists
                    if (tempSymTable != None):
                        if (not tempSymTable.add_symbol(parts[1])):
                            # 54 - běhová chyba interpretace – přístup k neexistující proměnné (rámec existuje)
                            printerr("ERROR - DEFVAR: var doesn't exist!")
                            exit_code = 54
                        
                    else:
                        # 55 - běhová chyba interpretace – rámec neexistuje (např. čtení z prázdného zásobníku rámců)
                        printerr("ERROR - DEFVAR: TF doesn't exist!")
                        exit_code = 55
                
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - DEFVAR: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### CALL #########################
        elif (instr_num_name[1] == 'CALL'):
            
            # check arg count and types
            if (instr.get_arg_count() == 1 and is_label(instr.get_arg_data(1)[2])):
                
                # push to call stack
                callStack.push(currentInstructionNumber+1)
                # jump to label
                if (labelSymTable.get_symbol(instr.get_arg_data(1)[2]) != False and labelSymTable.get_symbol(instr.get_arg_data(1)[2]) != None):
                    
                    currentInstructionNumber = int(labelSymTable.get_symbol(instr.get_arg_data(1)[2]))
                
                else:
                    # 52 - chyba při sémantických kontrolách vstupního kódu v IPPcode21
                    printerr("ERROR - CALL: label doesn't exist!")
                    exit_code = 52
            
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - CALL: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### RETURN #########################
        elif (instr_num_name[1] == 'RETURN'):
            
            # check arg count and types
            if (instr.get_arg_count() == 0):
                
                # check stack
                if (callStack.peek() != None):
                    # jump
                    currentInstructionNumber = int(callStack.pop())
                
                else:
                    # 56 - běhová chyba interpretace – chybějící hodnota (v proměnné, na datovém zásobníku nebov zásobníku volání)
                    printerr("ERROR - RETURN: call stack is empty!")
                    exit_code = 56
                
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - RETURN: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### PUSHS #########################
        elif (instr_num_name[1] == 'PUSHS'):
            
            # check arg count and types
            if (instr.get_arg_count() == 1 and is_symbol(instr.get_arg_data(1)[1], instr.get_arg_data(1)[2])):
                
                symbol = interpret_symbol_get_value_and_type(instr.get_arg_data(1)[1], instr.get_arg_data(1)[2])
                if (symbol[0] == 0):
                    dataStack.push([symbol[1], symbol[2]])
                else:
                    printerr("ERROR - PUSHS: symbol error")
                    exit_code = symbol[0]
            
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - PUSHS: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### POPS #########################
        elif (instr_num_name[1] == 'POPS'):
            
            # check arg count and types
            if (instr.get_arg_count() == 1 and is_var(instr.get_arg_data(1)[2])):
                
                data = dataStack.pop()
                if (data != None):
                    
                    result = interpret_var_set_value(instr.get_arg_data(1)[2], data[0], data[1])
                    if (result != 0):
                        printerr("ERROR - POPS: var/value error")
                        exit_code = result
                    
                else:
                    # 56 - běhová chyba interpretace – chybějící hodnota (v proměnné, na datovém zásobníku nebov zásobníku volání)
                    printerr("ERROR - POPS: call stack is empty!")
                    exit_code = 56
            
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - POPS: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### ADD/SUB/MUL/IDIV #########################
        elif (instr_num_name[1] == 'ADD' or instr_num_name[1] == 'SUB' or instr_num_name[1] == 'MUL' or instr_num_name[1] == 'IDIV'):
            
            # check arg count and types
            if (instr.get_arg_count() == 3 and is_var(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2]) and is_symbol(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])):
                
                symbol1 = interpret_symbol_get_value_and_type(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                symbol2 = interpret_symbol_get_value_and_type(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])
                if (symbol1[0] != 0):
                    printerr("ERROR - ADD/SUB/MUL/IDIV: symbol1 error")
                    exit_code = symbol1[0]
                elif (symbol2[0] != 0):
                    printerr("ERROR - ADD/SUB/MUL/IDIV: symbol2 error")
                    exit_code = symbol2[0]
                elif (symbol1[1] == 'int' and symbol2[1] == 'int'):
                    
                    result = None
                    
                    if (instr_num_name[1] == 'ADD'):
                        result = int(symbol1[2])+int(symbol2[2])
                    elif(instr_num_name[1] == 'SUB'):
                        result = int(symbol1[2])-int(symbol2[2])
                    elif(instr_num_name[1] == 'MUL'):
                        result = int(symbol1[2])*int(symbol2[2])
                    elif(instr_num_name[1] == 'IDIV'):
                        if (int(symbol2[2]) != 0):
                            result = int(symbol1[2])//int(symbol2[2])
                        else:
                            # 57 - běhová chyba interpretace – špatná hodnota operandu (např. dělení nulou, špatná návra-tová hodnota instrukce EXIT)
                            printerr("ERROR - ADD/SUB/MUL/IDIV: division by zero")
                            exit_code = 57
                    
                    if (exit_code == 0):
                        exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], 'int', result)
                        if (exit_code != 0):
                            printerr("ERROR - ADD/SUB/MUL/IDIV: var set value")
                    
                else:
                    # 53 - běhová chyba interpretace – špatné typy operandů
                    printerr("ERROR - ADD/SUB/MUL/IDIV: invalid arguments types!")
                    exit_code = 53

            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - ADD/SUB/MUL/IDIV: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### LT/GT/EQ #########################
        elif (instr_num_name[1] == 'LT' or instr_num_name[1] == 'GT' or instr_num_name[1] == 'EQ'):
            
            # check arg count and types
            if (instr.get_arg_count() == 3 and is_var(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2]) and is_symbol(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])):
                
                result = None
                
                symbol1 = interpret_symbol_get_value_and_type(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                symbol2 = interpret_symbol_get_value_and_type(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])
                if (symbol1[0] != 0):
                    printerr("ERROR - LT/GT/EQ: symbol1 error")
                    exit_code = symbol1[0]
                elif (symbol2[0] != 0):
                    printerr("ERROR - LT/GT/EQ: symbol2 error")
                    exit_code = symbol2[0]
                elif (symbol1[1] == symbol2[1] and symbol1[1] != 'nil'):
                    
                    if (instr_num_name[1] == 'LT'):
                        
                        if (symbol1[1] == 'bool'):
                            if (symbol1[2] == 'false' and symbol2[2] == 'true'):
                                result = True
                            else:
                                result = False
                        
                        elif (symbol1[1] == 'int'):
                            result = int(symbol1[2]) < int(symbol2[2])
                        
                        elif (symbol1[1] == 'string'):
                            result = str(symbol1[2]) < str(symbol2[2])
                        
                        
                    elif(instr_num_name[1] == 'GT'):
                        
                        if (symbol1[1] == 'bool'):
                            if (symbol1[2] == 'true' and symbol2[2] == 'false'):
                                result = True
                            else:
                                result = False
                        
                        elif (symbol1[1] == 'int'):
                            result = int(symbol1[2]) > int(symbol2[2])
                        
                        elif (symbol1[1] == 'string'):
                            result = str(symbol1[2]) > str(symbol2[2])
                        
                        
                    elif(instr_num_name[1] == 'EQ'):
                        
                        if (symbol1[1] == 'bool'):
                            result = (symbol1[2] == symbol2[2])
                            
                        elif (symbol1[1] == 'int'):
                            result = int(symbol1[2]) == int(symbol2[2])
                        
                        elif (symbol1[1] == 'string'):
                            result = str(symbol1[2]) == str(symbol2[2])
                    
                    
                elif (symbol1[1] == 'nil' or symbol2[1] == 'nil'):
                    
                    if ((symbol1[1] == 'nil' and symbol2[1] == 'string' and symbol2[2] == "") or
                        (symbol2[1] == 'nil' and symbol1[1] == 'string' and symbol1[2] == "") or
                        (symbol1[1] == 'nil' and symbol2[1] == 'int' and int(symbol2[2]) == 0) or 
                        (symbol2[1] == 'nil' and symbol1[1] == 'int' and int(symbol1[2]) == 0) or
                        (symbol1[1] == 'nil' and symbol2[1] == 'bool' and symbol2[2] == 'false') or 
                        (symbol2[1] == 'nil' and symbol1[1] == 'bool' and symbol1[2] == 'false')):
                        
                        result = True
                    
                    elif (symbol1[1] != symbol2[1]):
                        
                        result = False
                    
                    else:
                        # 53 - běhová chyba interpretace – špatné typy operandů
                        printerr("ERROR - LT/GT/EQ: both arguments are nil!")
                        exit_code = 53
                    
                else:
                    # 53 - běhová chyba interpretace – špatné typy operandů
                    printerr("ERROR - LT/GT/EQ: invalid arguments types!")
                    exit_code = 53
                
                
                if (exit_code == 0):
                    if (result == True):
                        result = 'true'
                    else:
                        result = 'false'
                    
                    
                    exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], 'bool', result)
                    if (exit_code != 0):
                        printerr("ERROR - LT/GT/EQ: var set value")
                
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - LT/GT/EQ: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### AND/OR #########################
        elif (instr_num_name[1] == 'AND' or instr_num_name[1] == 'OR'):
        
            # check arg count and types
            if (instr.get_arg_count() == 3 and is_var(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2]) and is_symbol(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])):
                
                symbol1 = interpret_symbol_get_value_and_type(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                symbol2 = interpret_symbol_get_value_and_type(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])
                if (symbol1[0] != 0):
                    printerr("ERROR - AND/OR: symbol1 error")
                    exit_code = symbol1[0]
                elif (symbol2[0] != 0):
                    printerr("ERROR - AND/OR: symbol2 error")
                    exit_code = symbol2[0]
                elif (symbol1[1] == 'bool' and symbol2[1] == 'bool'):
                    
                    result = None
                    
                    if (instr_num_name[1] == 'AND'):
                        
                        result = (symbol1[2] == 'true' and symbol2[2] == 'true')
                        
                    elif (instr_num_name[1] == 'OR'):
                        
                        result = (symbol1[2] == 'true' or symbol2[2] == 'true')
                    
                    if (result == True):
                        result = 'true'
                    else:
                        result = 'false'
                    
                    
                    exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], 'bool', result)
                    if (exit_code != 0):
                        printerr("ERROR - AND/OR: var set value")
                    
                else:
                    # 53 - běhová chyba interpretace – špatné typy operandů
                    printerr("ERROR - AND/OR: invalid arguments types!")
                    exit_code = 53

            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - AND/OR: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### NOT #########################
        elif (instr_num_name[1] == 'NOT'):
            
            # check arg count and types
            if (instr.get_arg_count() == 2 and is_var(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])):
                
                symbol = interpret_symbol_get_value_and_type(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                if (symbol[0] != 0):
                    printerr("ERROR - NOT: symbol1 error")
                    exit_code = symbol[0]
                elif (symbol[1] == 'bool'):
                    
                    result = None
                    
                    if (symbol[2] == 'true'):
                        result = 'false'
                    else:
                        result = 'true'
                    
                    
                    if (exit_code == 0):
                        exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], 'bool', result)
                        if (exit_code != 0):
                            printerr("ERROR - NOT: var set value")
                    
                else:
                    # 53 - běhová chyba interpretace – špatné typy operandů
                    printerr("ERROR - NOT: invalid arguments types!")
                    exit_code = 53

            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - NOT: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### INT2CHAR #########################
        elif (instr_num_name[1] == 'INT2CHAR'):
            
            # check arg count and types
            if (instr.get_arg_count() == 2 and is_var(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])):
                
                symbol = interpret_symbol_get_value_and_type(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                if (symbol1[0] == 0):
                    
                    if (symbol[1] == 'int'):
                        
                        try:
                            string = chr(int(symbol[2]))
                            
                            exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], 'string', string)
                            if (exit_code != 0):
                                printerr("ERROR - INT2CHAR: var set value")
                            
                            
                        except ValueError:
                            # 58 - běhová chyba interpretace – chybná práce s řetězcem.
                            printerr("ERROR - INT2CHAR: invalid value")
                            exit_code = 58
                        
                    else:
                        # 53 - běhová chyba interpretace – špatné typy operandů
                        printerr("ERROR - INT2CHAR: argument is not int!")
                        exit_code = 53
                    
                else:
                    printerr("ERROR - INT2CHAR: symbol1 error")
                    exit_code = symbol1[0]
                
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - INT2CHAR: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### STRI2INT #########################
        elif (instr_num_name[1] == 'STRI2INT'):
            
            # check arg count and types
            if (instr.get_arg_count() == 3 and is_var(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2]) and is_symbol(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])):
                
                symbol1 = interpret_symbol_get_value_and_type(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                symbol2 = interpret_symbol_get_value_and_type(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])
                if (symbol1[0] != 0):
                    printerr("ERROR - STRI2INT: symbol1 error")
                    exit_code = symbol1[0]
                elif (symbol2[0] != 0):
                    printerr("ERROR - STRI2INT: symbol2 error")
                    exit_code = symbol2[0]
                elif (symbol1[1] == 'string' and symbol2[1] == 'int'):
                    string = symbol1[2]
                    position = int(symbol2[2])
                    
                    if (position >= 0 and position < len(string)):
                        
                        exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], 'string', str(string[position]))
                        if (exit_code != 0):
                            printerr("ERROR - STRI2INT: var set value")
                    
                    else:
                        # 58 - běhová chyba interpretace – chybná práce s řetězcem
                        printerr("ERROR - STRI2INT: index out of range!")
                        exit_code = 58
                    
                else:
                    # 53 - běhová chyba interpretace – špatné typy operandů
                    printerr("ERROR - STRI2INT: invalid arguments types!")
                    exit_code = 53

            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - STRI2INT: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### READ #########################
        elif (instr_num_name[1] == 'READ'):
            
            # check arg count and types
            if (instr.get_arg_count() == 2 and is_var(instr.get_arg_data(1)[2]) and is_type(instr.get_arg_data(2)[2])):
                
                typ = instr.get_arg_data(2)[2]
                read = input()
                value = None
                
                try:
                    if (typ == 'bool'):
                        
                        if (read.upper() == 'TRUE'):
                            value = "true"
                        else:
                            value = "false"
                    
                    elif (typ == 'int'):
                    
                        value = int(read)
                    
                    elif (typ == 'string'):
                        
                        value = str(read)
                    
                except Exception as e:
                    typ = 'nil'
                    value = 'nil'
                
                exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], typ, value)
                if (exit_code != 0):
                    printerr("ERROR - READ: set var")
            
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - READ: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### WRITE #########################
        elif (instr_num_name[1] == 'WRITE'):
            
            # check arg count and types
            if (instr.get_arg_count() == 1 and is_symbol(instr.get_arg_data(1)[1], instr.get_arg_data(1)[2])):
                
                data = interpret_symbol_get_value_and_type(instr.get_arg_data(1)[1], instr.get_arg_data(1)[2])
                if (data[0] == 0):
                
                    if (data[1] == 'nil'):
                        print("", end='')
                        
                    elif (data[1] == 'bool'):
                        if (data[2] == 'true'):
                            print("true", end='')
                        else:
                            print("false", end='')
                        
                    elif (data[1] == 'int' or data[1] == 'string'):
                        print(data[2], end='')
                    
                else:
                    printerr("ERROR - WRITE: get value and type from symbol")
                    exit_code = data[0]
                

            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - WRITE: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### CONCAT #########################
        elif (instr_num_name[1] == 'CONCAT'):
            
            # check arg count and types
            if (instr.get_arg_count() == 3 and is_var(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2]) and is_symbol(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])):
                
                symbol1 = interpret_symbol_get_value_and_type(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                symbol2 = interpret_symbol_get_value_and_type(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])
                if (symbol1[0] != 0):
                    printerr("ERROR - CONCAT: symbol1 error")
                    exit_code = symbol1[0]
                elif (symbol2[0] != 0):
                    printerr("ERROR - CONCAT: symbol2 error")
                    exit_code = symbol2[0]
                elif (symbol1[1] == 'string' and symbol2[1] == 'string'):
                    
                    exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], 'string', str(symbol1[2]+symbol2[2]))
                    if (exit_code != 0):
                        printerr("ERROR - CONCAT: var set value")
                    
                else:
                    # 53 - běhová chyba interpretace – špatné typy operandů
                    printerr("ERROR - CONCAT: arguments types are not strings!")
                    exit_code = 53
                
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - CONCAT: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### STRLEN #########################
        elif (instr_num_name[1] == 'STRLEN'):
            
            # check arg count and types
            if (instr.get_arg_count() == 2 and is_var(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])):
                
                symbol = interpret_symbol_get_value_and_type(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                
                if (symbol[0] != 0):
                    printerr("ERROR - STRLEN: symbol error")
                    exit_code = symbol[0]
                    
                else:
                    exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], 'int', len(symbol[2])) ### len(symbol[2])-(symbol[2].count('\\')*3)
                    if (exit_code != 0):
                        printerr("ERROR - STRLEN: var set value")
                
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - STRLEN: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### GETCHAR #########################
        elif (instr_num_name[1] == 'GETCHAR'):
            
            # check arg count and types
            if (instr.get_arg_count() == 3 and is_var(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2]) and is_symbol(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])):
                
                symbol1 = interpret_symbol_get_value_and_type(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                symbol2 = interpret_symbol_get_value_and_type(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])
                if (symbol1[0] != 0):
                    printerr("ERROR - GETCHAR: symbol1 error")
                    exit_code = symbol1[0]
                elif (symbol2[0] != 0):
                    printerr("ERROR - GETCHAR: symbol2 error")
                    exit_code = symbol2[0]
                elif (symbol1[1] == 'string' and symbol2[1] == 'int'):
                    #pos = int(symbol2[2])
                    #string = list(symbol1[2])
                    #str_len = len(string)-(string.count('\\')*3)
                    #length = 0
                    #skip = 0
                    #for char in string:
                    #    if (length == pos):
                    #        break
                    #    
                    #    if (skip != 0):
                    #        skip -= 1
                    #        pos += 1
                    #    
                    #    if (char == '\\'):
                    #        skip = 3
                    #    
                    #    length += 1
                    
                    #if (len(string) > length and int(symbol2[2]) > 0 and int(symbol2[2]) < str_len):
                    #    res = str(string[length])
                    #    if (res == '\\'):
                    #        res += string[length+1]+string[length+2]+string[length+3]
                    #    
                    #    print(res)
                    
                    if (len(symbol1[2]) > int(symbol2[2]) and -1 < int(symbol2[2])):
                        
                        exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], 'string', str(symbol1[2][int(symbol2[2])]))
                        if (exit_code != 0):
                            printerr("ERROR - GETCHAR: var set value")
                    
                    else:
                        # 58 - běhová chyba interpretace – chybná práce s řetězcem
                        printerr("ERROR - GETCHAR: index out of range!")
                        exit_code = 58
                    
                else:
                    # 53 - běhová chyba interpretace – špatné typy operandů
                    printerr("ERROR - GETCHAR: arguments types are invalid!")
                    exit_code = 53
                
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - GETCHAR: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### SETCHAR #########################
        elif (instr_num_name[1] == 'SETCHAR'):
            
            # check arg count and types
            if (instr.get_arg_count() == 3 and is_var(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2]) and is_symbol(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])):
                
                symbol1 = interpret_symbol_get_value_and_type(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                symbol2 = interpret_symbol_get_value_and_type(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])
                var = interpret_symbol_get_value_and_type(instr.get_arg_data(1)[1], instr.get_arg_data(1)[2])
                if (symbol1[0] != 0):
                    printerr("ERROR - SETCHAR: symbol1 error")
                    exit_code = symbol1[0]
                elif (symbol2[0] != 0):
                    printerr("ERROR - SETCHAR: symbol2 error")
                    exit_code = symbol2[0]
                elif (var[0] != 0):
                    printerr("ERROR - SETCHAR: var error")
                    exit_code = var[0]
                elif (symbol1[1] == 'int' and symbol2[1] == 'string' and var[1] == 'string'):
                    
                    if (symbol2[2] != "" and int(symbol1[2]) >= 0 and int(symbol1[2]) < len(var[2])):
                        
                        lst = list(var[2])
                        lst[int(symbol1[2])] = symbol2[2]
                        var[2] = "".join(lst)
                        
                        exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], 'string', str(var[2]))
                        if (exit_code != 0):
                            printerr("ERROR - SETCHAR: var set value")
                        
                    else:
                        # 58 - běhová chyba interpretace – chybná práce s řetězcem
                        printerr("ERROR - SETCHAR: index out of range!")
                        exit_code = 58
                    
                else:
                    # 53 - běhová chyba interpretace – špatné typy operandů
                    printerr("ERROR - SETCHAR: arguments types are invalid!")
                    exit_code = 53
                
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - SETCHAR: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### TYPE #########################
        elif (instr_num_name[1] == 'TYPE'):
            
            # check arg count and types
            if (instr.get_arg_count() == 2 and is_var(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])):
                
                symbol = interpret_symbol_get_value_and_type(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                
                if (symbol[0] == 0 or symbol[0] == 56):
                    
                    value = ""
                    if (symbol[0] != 56):
                        value = symbol[1]
                    
                    exit_code = interpret_var_set_value(instr.get_arg_data(1)[2], 'string', value)
                    if (exit_code != 0):
                        printerr("ERROR - TYPE: var set value")
                
                else:
                    printerr("ERROR - TYPE: symbol error")
                    exit_code = symbol[0]
                
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - TYPE: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### JUMP #########################
        elif (instr_num_name[1] == 'JUMP'):
            
            # check arg count and types
            if (instr.get_arg_count() == 1 and is_label(instr.get_arg_data(1)[2])):
                
                if (labelSymTable.get_symbol(instr.get_arg_data(1)[2]) != False and labelSymTable.get_symbol(instr.get_arg_data(1)[2]) != None):
                    
                    currentInstructionNumber = int(labelSymTable.get_symbol(instr.get_arg_data(1)[2]))
                
                else:
                    # 52 - chyba při sémantických kontrolách vstupního kódu v IPPcode21
                    printerr("ERROR - JUMP: label doesn't exist!")
                    exit_code = 52
                
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - JUMP: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### JUMPIFEQ/JUMPIFNEQ #########################
        elif (instr_num_name[1] == 'JUMPIFEQ' or instr_num_name[1] == 'JUMPIFNEQ'):
            
            # check arg count and types
            if (instr.get_arg_count() == 3 and is_label(instr.get_arg_data(1)[2]) and is_symbol(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2]) and is_symbol(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])):
                
                symbol1 = interpret_symbol_get_value_and_type(instr.get_arg_data(2)[1], instr.get_arg_data(2)[2])
                symbol2 = interpret_symbol_get_value_and_type(instr.get_arg_data(3)[1], instr.get_arg_data(3)[2])
                if (symbol1[0] != 0):
                    printerr("ERROR - JUMPIFEQ/JUMPIFNEQ: symbol1 error")
                    exit_code = symbol1[0]
                elif (symbol2[0] != 0):
                    printerr("ERROR - JUMPIFEQ/JUMPIFNEQ: symbol2 error")
                    exit_code = symbol2[0]
                elif (symbol1[1] == symbol2[1] or symbol1[1] == 'nil' or symbol2[1] == 'nil'):
                    
                    result = None
                    
                    if (symbol1[1] == symbol2[1] and symbol1[1] != 'nil'):
                        
                        if (symbol1[1] == 'bool'):
                            result = (symbol1[2] == symbol2[2])
                            
                        elif (symbol1[1] == 'int'):
                            result = int(symbol1[2]) == int(symbol2[2])
                        
                        elif (symbol1[1] == 'string'):
                            result = str(symbol1[2]) == str(symbol2[2])
                            
                    elif (symbol1[1] == 'nil' or symbol2[1] == 'nil'):
                    
                        if ((symbol1[1] == 'nil' and symbol2[1] == 'string' and symbol2[2] == "") or
                            (symbol2[1] == 'nil' and symbol1[1] == 'string' and symbol1[2] == "") or
                            (symbol1[1] == 'nil' and symbol2[1] == 'int' and int(symbol2[2]) == 0) or 
                            (symbol2[1] == 'nil' and symbol1[1] == 'int' and int(symbol1[2]) == 0) or
                            (symbol1[1] == 'nil' and symbol2[1] == 'bool' and symbol2[2] == 'false') or 
                            (symbol2[1] == 'nil' and symbol1[1] == 'bool' and symbol1[2] == 'false')):
                            
                            result = True
                        
                        elif (symbol1[1] != symbol2[1]):
                            
                            result = False
                        
                        else:
                            # 53 - běhová chyba interpretace – špatné typy operandů
                            printerr("ERROR - LT/GT/EQ: both arguments are nil!")
                            exit_code = 53
                    
                    
                    if (exit_code == 0):
                    
                        if (instr_num_name[1] == 'JUMPIFNEQ'):
                            result = not result
                        
                        
                        if (result):
                            currentInstructionNumber = int(labelSymTable.get_symbol(instr.get_arg_data(1)[2]))
                
                else:
                    # 53 - běhová chyba interpretace – špatné typy operandů
                    printerr("ERROR - JUMPIFEQ/JUMPIFNEQ: arguments types are not equal!")
                    exit_code = 53

            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - JUMPIFEQ/JUMPIFNEQ: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### EXIT #########################
        elif (instr_num_name[1] == 'EXIT'):
            
            # check arg count and types
            if (instr.get_arg_count() == 1 and is_symbol(instr.get_arg_data(1)[1], instr.get_arg_data(1)[2])):
                
                symbol = interpret_symbol_get_value_and_type(instr.get_arg_data(1)[1], instr.get_arg_data(1)[2])
                if (symbol[0] == 0):
                    if (symbol[1] == 'int'):
                        
                        if (int(symbol[2]) >= 0 and int(symbol[2]) < 50):
                        
                            exit_code = int(symbol[2])
                            break
                            
                        else:
                            # 57 - běhová chyba interpretace – špatná hodnota operandu (např. dělení nulou, špatná návra-tová hodnota instrukce EXIT)
                            printerr("ERROR - EXIT: invalid value")
                            exit_code = 57
                        
                    else:
                        # 53 - běhová chyba interpretace – špatné typy operandů
                        printerr("ERROR - EXIT: argument is not int!")
                        exit_code = 53
                    
                else:
                    printerr("ERROR - EXIT: symbol error")
                    exit_code = symbol[0]
            
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - EXIT: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### DPRINT #########################
        elif (instr_num_name[1] == 'DPRINT'):
            
            # check arg count and types
            if (instr.get_arg_count() == 1 and is_symbol(instr.get_arg_data(1)[1], instr.get_arg_data(1)[2])):
                
                symbol = interpret_symbol_get_value_and_type(instr.get_arg_data(1)[1], instr.get_arg_data(1)[2])
                if (symbol[0] == 0):
                    
                    printerr(symbol[2])
                    
                else:
                    printerr("ERROR - DPRINT: symbol error")
                    exit_code = symbol[0]
            
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - DPRINT: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### BREAK #########################
        elif (instr_num_name[1] == 'BREAK'):
            
            # check arg count and types
            if (instr.get_arg_count() == 0):
            
                printerr("Current instruction number: "+str(currentInstructionNumber))
            
            else:
                # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                printerr("ERROR - BREAK: invalid arguments or wrong count!")
                exit_code = 32
        
        
        ### UNKNOWN INSTRUCTION #########################
        elif (instr_num_name[1] != 'LABEL'):
            # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci,instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chybatextových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselnýnebo řetězcový literál, neznámý operační kód apod.).
            printerr("ERROR - UNKNOWN INSTRUCTION!")
            printerr("Instruction: "+instr_num_name[1])
            exit_code = 32
        
        # next instruction
        currentInstructionNumber += 1
    
    ### END OF LOOP #########################
    
    if (exit_code != 0):
        printerr("Last instruction: "+str(currentInstructionNumber-1))
    
    
    #print(globalSymTable.get_symbol("counter"))
    ##print(localSymTabStack.peek().get_symbol("counter"))
    #print(tempSymTable.get_symbol("counter"))
    #print(labelSymTable.get_symbol("end"))
    
    
    return exit_code


def parsexml(source_stream, input_stream):
    exit_code = 0
    
    XMLparserDepth = 0
    XMLparser = ET.XMLPullParser(['start', 'end'])
    
    program_tag_count = 0
    arg_tag_max = 0
    arg_tag_sum = 0
    instruction_tag_max = 0
    instruction_tag_sum = 0
    instruction_tag_count = 0
    
    current_instruction = None
    current_argument = None
    
    # read SOURCE CODE stream
    for line in source_stream:
        #print(line)
        # feed parser
        XMLparser.feed(line)
        # parse elements
        try:
            for event, elem in XMLparser.read_events():
                #print(elem.tag,'=',elem.text)
                ##### EVENT START
                if (event == 'start'):
                    XMLparserDepth += 1
                    
                    # check xml structure
                    check_result = 1
                    ### CHECK START TAGS XML STUCTURE
                    
                    
                    #""" DEPTH 1 """
                    if (XMLparserDepth == 1):
                        
                        ## tag PROGRAM
                        if (elem.tag == 'program'):
                            program_tag_count += 1
                            
                            # check attributes - required and optional
                            if (xml_check_attributes(list(elem.attrib.keys()), ['language'], ['name', 'description'])):
                                
                                # check attributes values
                                if (elem.attrib['language'] != 'IPPcode21'): ###""" check if language attribute value is equal to IPPcode21 """
                                    check_result = 0
                                
                            # unexpected or missing attributes
                            else:
                                check_result = 0
                        # unexpected tag
                        else:
                            check_result = 0
                    
                    
                    #""" DEPTH 2 """
                    elif (XMLparserDepth == 2):
                    
                        ## tag INSTRUCTION
                        if (elem.tag == 'instruction'):
                            
                            # clear instruction help vars
                            arg_tag_max = 0
                            arg_tag_sum = 0
                            
                            # check attributes - required and optional
                            if (xml_check_attributes(list(elem.attrib.keys()), ['order', 'opcode'], [])):
                                
                                # check attributes values
                                if (elem.attrib['opcode'] != elem.attrib['opcode'].upper()): ###""" check if opcode attribute value is uppercase """
                                    check_result = 0
                                
                                ###""" instructions numbers sumcheck """
                                instruction_tag_count += 1
                                instruction_tag_sum += intt(elem.attrib['order'])
                                if (instruction_tag_max < intt(elem.attrib['order'])):
                                    instruction_tag_max = intt(elem.attrib['order'])
                                
                                # create new instruction
                                current_instruction = Instruction(intt(elem.attrib['order']), elem.attrib['opcode'])
                                
                            # unexpected or missing attributes
                            else:
                                check_result = 0
                        # unexpected tag
                        else:
                            check_result = 0
                    
                    
                    #""" DEPTH 3 """
                    elif (XMLparserDepth == 3):
                        ## tag ARG1
                        if (elem.tag == 'arg1'):
                            if (arg_tag_max < 1):
                                arg_tag_max = 1
                            arg_tag_sum += 1
                            
                            # check attributes - required and optional
                            if (xml_check_attributes(list(elem.attrib.keys()), ['type'], [])):
                            
                                current_argument = Argument(1,elem.attrib['type'])
                                current_instruction.add_argument(current_argument)
                            
                            # unexpected or missing attributes
                            else:
                                check_result = 0
                        
                        
                        ## tag ARG2
                        elif (elem.tag == 'arg2'):
                            if (arg_tag_max < 2):
                                arg_tag_max = 2
                            arg_tag_sum += 2
                            
                            # check attributes - required and optional
                            if (xml_check_attributes(list(elem.attrib.keys()), ['type'], [])):
                            
                                current_argument = Argument(2,elem.attrib['type'])
                                current_instruction.add_argument(current_argument)
                            
                            # unexpected or missing attributes
                            else:
                                check_result = 0
                        
                        
                        ## tag ARG3
                        elif (elem.tag == 'arg3'):
                            if (arg_tag_max < 3):
                                arg_tag_max = 3
                            arg_tag_sum += 3
                            
                            # check attributes - required and optional
                            if (xml_check_attributes(list(elem.attrib.keys()), ['type'], [])):
                            
                                current_argument = Argument(3,elem.attrib['type'])
                                current_instruction.add_argument(current_argument)
                            
                            # unexpected or missing attributes
                            else:
                                check_result = 0
                        
                        
                        # unexpected tag
                        else:
                            check_result = 0
                    
                    
                    #""" DEPTH > 3 """
                    else:
                        check_result = 0
                    
                    
                    #""" ERROR CHECK """
                    if (check_result == 0):
                        # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                        printerr("ERROR - XML Parser invalid structure!")
                        if (exit_code == 0):
                            exit_code = 32
                        break
                    
                    
                ##### EVENT END
                if (event == 'end'):
                    # check xml structure
                    check_result = 1
                    ### CHECK END TAGS XML STUCTURE
                    
                    
                    #""" DEPTH 1 """
                    if (XMLparserDepth == 1):
                        
                        # check if no inner text
                        if (elem.text != None and elem.text.strip() != ""):
                            check_result = 0
                    
                    
                    #""" DEPTH 2 """
                    elif (XMLparserDepth == 2):
                        
                        ## tag INSTRUCTION
                        if (elem.tag == 'instruction'):
                        
                            ###""" args numbers sumcheck """
                            if not ((arg_tag_max == 0 and arg_tag_sum == 0) or (arg_tag_max == 1 and arg_tag_sum == 1) or (arg_tag_max == 2 and arg_tag_sum == 3) or (arg_tag_max == 3 and arg_tag_sum == 6)):
                                check_result = 0
                            
                        # unexpected tag
                        else:
                            check_result = 0
                        
                        
                        # check if no inner text
                        if (elem.text != None and elem.text.strip() != ""):
                            check_result = 0
                    
                    
                    #""" DEPTH 3 """
                    elif (XMLparserDepth == 3):
                        
                        ## tag ARGx
                        if (elem.tag == 'arg1' or elem.tag == 'arg2' or elem.tag == 'arg3'):
                            if (elem.text != None):
                                if (current_argument.get_data()[1] == 'string'):
                                    # check is_string
                                    if (re.findall('((\\\(?!([0-9]{3})))|([#\s]))', elem.text) != []):
                                        # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                                        printerr("ERROR - XML Parser invalid structure - string syntax error!")
                                        if (exit_code == 0):
                                            exit_code = 32
                                    
                                    else:
                                        current_string = elem.text
                                        new_string = ""
                                        
                                        if (current_string.find('\\') == -1):
                                            new_string = current_string
                                        else:
                                            while(current_string.find('\\') != -1):
                                                pos = current_string.find('\\')
                                                new_string += current_string[:pos]
                                                
                                                seq_string = int(current_string[pos+1:pos+4])
                                                new_string += str(chr(seq_string))
                                                
                                                current_string = current_string[pos+4:]
                                        
                                        current_argument.add_value(new_string)
                                        
                                else:
                                    current_argument.add_value(elem.text)
                                
                            else:
                                current_argument.add_value("")
                    
                    
                    #""" DEPTH > 3 """
                    else:
                        check_result = 0
                    
                    
                    #""" ERROR CHECK """
                    if (check_result == 0):
                        # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
                        printerr("ERROR - XML Parser invalid structure!")
                        if (exit_code == 0):
                            exit_code = 32
                        break
                    
                    XMLparserDepth -= 1
            
            
            if (exit_code != 0):
                break
            
            
        except ET.ParseError as err:
            # 31 - chybný XML formát ve vstupním souboru (soubor není tzv. dobře formátovaný, angl.well-formed
            printerr("ERROR - XML Parser invalid input!")
            if (exit_code == 0):
                exit_code = 31
            break
    
    ## Close parser
    try:
        XMLparser.close()
    except ET.ParseError as err:
        # 31 - chybný XML formát ve vstupním souboru (soubor není tzv. dobře formátovaný, angl.well-formed
        printerr("ERROR - XML Parser close() invalid input!")
        printerr(err)
        if (exit_code == 0):
            exit_code = 31
    
    
    ###""" Check PROGRAM tag count AND instructions order numbers sum check """
    if (program_tag_count != 1 or not (sum_count_check(instruction_tag_sum, instruction_tag_count) and instruction_tag_count == instruction_tag_max)):
        # 32 - neočekávaná struktura XML (např. element pro argument mimo element pro instrukci, instrukce s duplicitním pořadím nebo záporným pořadím) či lexikální nebo syntaktická chyba textových elementů a atributů ve vstupním XML souboru (např. chybný lexém pro číselný nebo řetězcový literál, neznámý operační kód apod.).
        printerr("ERROR - XML Parser invalid structure - instructions are incorrectly numbered OR PROGRAM tag count is not one!")
        if (exit_code == 0):
            exit_code = 32
    
    return exit_code


def main(argv):
    ### GET ARGS
    ap = argparse.ArgumentParser()
    ap.add_argument("--source", nargs=1, help="vstupní soubor s XML reprezentací zdrojového kódu dle definice ze sekce")
    ap.add_argument("--input", nargs=1, help="soubor se vstupy pro samotnou interpretaci zadaného zdrojového kódu")
    args = ap.parse_known_args()
    
    
    ## Parse args
    # required
    arg = vars(args[0])
    arg_source = arg["source"]
    if (arg_source != None):
        arg_source = arg_source[0]
    arg_input = arg["input"]
    if (arg_input != None):
        arg_input = arg_input[0]
    # unknown
    if (len(args[1]) > 0):
        # 10  - chybějící parametr skriptu (je-li třeba) nebo použití zakázané kombinace parametrů
        printerr("ERROR - Unknown args passed!")
        exit(10)
    
    
    ## Prepare SOURCE CODE and INPUT STREAM
    source_stream = None
    input_stream = None
    if (arg_source != None and arg_input != None):
        source_stream = open(arg_source, 'r')
        input_stream = open(arg_input, 'r')
        pass
    elif (arg_source != None):
        source_stream = open(arg_source, 'r')
        input_stream = sys.stdin
    elif (arg_input != None):
        source_stream = sys.stdin
        input_stream = open(arg_input, 'r')
    else:
        # 10  - chybějící parametr skriptu (je-li třeba) nebo použití zakázané kombinace parametrů
        printerr("ERROR - Missing args!")
        exit(10)
    
    
    
    ### EXIT CODE FOR CLEANUP
    exit_code = 0
    
    
    
    ### PARSE XML
    exit_code = parsexml(source_stream, input_stream)
    
    
    
    ### INTERPRET
    if (exit_code == 0):
        exit_code = interpret()
    
    
    
    ### CLEANUP
    source_stream.close()
    input_stream.close()
    exit(exit_code)



if __name__ == "__main__":
    try:
        main(sys.argv)
    except KeyboardInterrupt:
        #loop = False;
        printerr('\n\nKeyboard exception received. Exiting.')
        exit(99)
    except Exception as e:
        printerr("Fatal ERROR: ", e)
        exit(99)
