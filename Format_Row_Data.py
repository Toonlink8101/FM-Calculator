from FM_Interval_Calculator import row_data
from NoteValueConverter import Value2Note

"""
    Formats data into a single string
"""
def Format_Row_Data(data:row_data, inst:str="..", decay:str="") -> str:
    left_multiple = row_data[0]
    right_multiple = row_data[1]
    base_note = row_data[2]
    detune = row_data[3]

    r = Value2Note(base_note)
    r += inst + ".."
    r += "161" + chr(left_multiple)
    r += "162" + chr(left_multiple)
    r += "163" + chr(right_multiple)
    r += "164" + chr(right_multiple)
    r += "E5" + format(detune, "02X")
    r += decay
    r += "|\r\n"

    return r

def Format_Row_Simple(base_note=120, inst:str="..") -> str:
    r = Value2Note(base_note)
    r += inst + ".."
    r += "...."
    r += "...."
    r += "...."
    r += "...."
    r += "...."
    r += "...."
    r += "|\r\n"

    return r

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