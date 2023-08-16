from collections import namedtuple

# input: semitones between notes
# output: (lower_multiple, higher_multiple, semitones_from_lower_note)
difference2ratio = {
    # first octave
    0:(8, 8, 0), 1:(14, 15, -10), 2:(8, 9, 0), 3:(10, 12, -4), 4:(8, 10, 0), 5:(6, 8, 5),
    6:(10, 14, -4), 7:(8, 12, 0), 8:(5, 8, 8), 9:(6, 10, 5), 10:(5, 9, 8), 11:(8, 15, 0),
    
    # second octave
    12:(4, 8, 12), 13:(7, 15, 2), 14:(4, 9, 12), 15:(5, 12, 8), 16:(4, 10, 12),17:(3, 8, 17),
    18:(5, 14, 8), 19:(4, 12, 12), 21:(3, 10, 17), 22:(4, 14, 12), 23:(4, 15, 12),

    # third octave   
    24:(2, 8, 24), 26:(2, 9, 24), 27:(3, 14, 27), 28:(2, 10, 24), 31:(2, 12, 24),34:(2, 14, 24), 35:(2, 15, 24), 
    
    # fouth octave
    36:(1, 8, 36), 38:(1, 9, 36), 40:(1, 10, 36), 43:(1, 12, 36), 46:(1, 14, 36), 47:(1, 15, 36), 
    
    # fifth octave
    48:(0, 8, 48), 50:(0, 9, 48), 52:(0, 10, 48), 55:(0, 12, 48), 58:(0, 14, 48), 59:(0, 15, 48),
}

# input: operator multiple
# output: tuning hex code
multiple2tuning = {
    0:0x80,
    1:0x80,
    2:0x80,
    3:0x80,
    4:0x86,
    5:0x94,
    6:0x81,
    7:0xAD,
    8:0x84,
    9:0x80,
    10:0x96,
    11:0xC6,
    12:0x81,
    13:0xD6,
    14:0xAD,
    15:0x96,
}

"""
   Has the format ( left_multiple, right_multiple, base_note_value, detune )
"""
row_data = namedtuple("row_data", ["left_multiple", "right_multiple", "base_note_value", "detune"])

"""
    Returns tuple with the format ( left_multiple, right_multiple, base_note_value, detune )
"""
def Get_Interval_Ratio_Tuning(left_note:int, right_note:int) -> row_data:
    # ratio = ( left_multiple, right_multiple, base_note_value )
    ratio = Get_Interval_Ratio(left_note, right_note)
    # tuning = hex value
    tuning = Get_Interval_Tuning(ratio[0], ratio[1])
    
    return row_data(ratio[0], ratio[1], ratio[2], tuning)

"""
    Returns tuple with the format ( left_multiple, right_multiple, base_note_value )
"""
def Get_Interval_Ratio(left_note:int, right_note:int) -> tuple:
    # handle non-notes
    if left_note >= 120:
        if right_note >= 120:
            return (8, 8, 120)
        else:
            return (0, 8, right_note)
    elif right_note >= 120:
        return (8, 0, left_note)

    difference = abs(left_note - right_note)

    lower_note_value = min(left_note, right_note)

    # if possible, get multiplier ratio for note difference
    if difference in difference2ratio:
        return_value = difference2ratio[difference]
    else:
        print(f"Invalid interval of {difference} semitones")
        return (0, 0, 0)
    
    # calculates base_note_value and stores it in the last index
    return_value[2] += lower_note_value

    if left_note > right_note:
        return (return_value[1], return_value[0], return_value[2])

    return return_value

"""
    Returns average detune value for the given multiples
"""
def Get_Interval_Tuning(left_multiple:int, right_multiple:int) -> int:
    # use this for output: https://stackoverflow.com/questions/14678132/python-hexadecimal
    
    return (multiple2tuning[left_multiple] + multiple2tuning[right_multiple]) // 2

# OUTDATED
# def Is_Modulation_Compatible(left_multiple:int, right_multiple:int, left_modulator:int, right_modulator:int) -> bool:
#     return left_multiple % left_modulator == 0 and right_multiple % right_modulator == 0

"""
    Returns tuple with the format ( left_multiple, right_multiple, base_note_value, detune )
    
    Overflowed - Also checks for modulation compatibility
"""
def Get_Interval_Ratio_Tuning(left_note:int, right_note:int, left_modulator:int, right_modulator:int) -> tuple:
    # ratio = ( left_multiple, right_multiple, base_note_value )
    ratio = Get_Interval_Ratio(left_note, right_note)

    # tuning = hex value
    tuning = Get_Interval_Tuning(ratio[0], ratio[1])

    # Check modulation compatibility
    if not Is_Mod_Compatible(ratio[0], ratio[1], left_modulator, right_modulator):
        print(print(f"Incompatible multiplier/modulator combo of {ratio[0]} and {ratio[1]}"))
    
    return (ratio[0], ratio[1], ratio[2], tuning)

def Is_Mod_Compatible(left_multiple:int, right_multiple:int, left_modulator:int, right_modulator:int) -> bool:
    if left_multiple % left_modulator == 0 and right_multiple % right_modulator == 0:
        return True
    while left_multiple % 2 == 0 and right_multiple % 2 == 0:
        left_multiple /= 2
        right_multiple /= 2
        if(left_multiple * left_modulator <= 15 and right_multiple * right_modulator <= 15):
            # True, but only with a change in multipliers and base note
            return False
    return False


    

#testing
print(Is_Mod_Compatible(eval(input("left_multiple: ")), eval(input("right_multiple: ")), eval(input("left_modulator: ")), eval(input("right_modulator: "))))