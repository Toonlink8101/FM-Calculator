letterToValue = {
    "c":0,
    "d":2,
    "e":4,
    "f":5,
    "g":7,
    "a":10,
    "b":11,
    '.':120,
    'o':121,
    '=':122,
    'r':123
}
valueToPrefix = {
    0:"C-",
    1:"C#",
    2:"D-",
    3:"D#",
    4:"E-",
    5:"F-",
    6:"F#",
    7:"G-",
    8:"G#",
    9:"A-",
    10:"A#",
    11:"B-",
    120:"...",
    121:"OFF",
    122:"===",
    123:"REL"
}

"""
    Encodes notes as integers
    The integer is the number of semitones from C-0
    Negative numbers are notes in negative octaves

    Exceptions:
    Anything that isn't a note (a blank, a release command, etc.) is 
    encoded as 120 or above. This is because B-9, the highest note 
    supported by Furnace, is 119.
"""
def Note2Value(note_name:str) -> int:
    note_value = 0

    letter = note_name[0].lower()

    if letter in letterToValue:
        note_value += letterToValue[letter]
        
        # if not-a-note, return value
        if note_value >= 120:
            return note_value
    else:
        # if unusual, return blank
        note_value = 120
        return note_value

    sign = note_name[1]
    if sign == '#' or sign == '+':
        note_value += 1
    
    octave = ord(note_name[2])
    octave_offset = (octave - ord('0')) * 12

    if(letter == note_name[0]):
        octave_offset *= -1

    note_value += octave_offset

    return note_value

"""
    Decodes note values into a string of three characters
"""
def Value2Note(note_value:int) -> str:
    # handle non-notes
    if note_value >= 120:
        if note_value in valueToPrefix:
            return valueToPrefix[note_value]
        else:
            return "ERR"

    is_suboctave = note_value < 0

    # calculate octave value
    if is_suboctave:
        note_value *= -1
    octave = note_value // 12 + is_suboctave

    # remove octave offset
    note_value %= 12

    #flip values if suboctave
    if is_suboctave:
        note_value -= 12
        note_value *= -1

    # get note letter and sign
    note_name = valueToPrefix[note_value]

    # if suboctave, adjust sign to match
    if is_suboctave:
        note_name[0] = note_name[0].lower()
        if note_name[1] == '-':
            note_name[1] = '_'
        elif note_name[1] == '#':
            note_name[1] = '+'

    # append octave value
    note_name += chr(ord('0') + octave)

    return note_name