from NoteValueConverter import Note2Value
from pyperclip import paste as Get_Clipboard
from Add_Delay import Add_Delay

"""
   Reads clipboard data

   When true, double_column will interpret the data as two columns, 
   alternating left and right column entries for each entry

   Returns a list of ints for the note values, with the first list item as the header
"""
def Parse_Column(double_column:bool = False) -> list:
    split_data = Get_Clipboard().split()
    
    header = ""

    for i in range(6):
        header += split_data[i] + '\r\n'
    
    parsed_data = [header]

    if double_column:
        for row in range(6, len(split_data)):
            split_row = split_data[row].split('|')
            parsed_data.append(Note2Value(split_row[0]))
            parsed_data.append(Note2Value(split_row[1]))
    else:
        for row in range(6, len(split_data)):
            parsed_data.append(Note2Value(split_data[row]))

    return parsed_data

"""
org.tildearrow.furnace - Pattern Data (159)
0
...........|...........|
...........|...........|
...........|...........|
...........|...........|
...........|...........|
...........|...........|
...........|...........|
...........|...........|
"""

#testing
print(Get_Clipboard())
print(Parse_Column())
print(Add_Delay(Parse_Column(), 2))