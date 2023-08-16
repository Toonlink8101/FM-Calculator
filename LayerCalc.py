import NoteValueConverter
from FM_Interval_Calculator import Get_Interval_Ratio_Tuning
from Parse_Tracker_Data import Parse_Column
from Add_Delay import Add_Delay, Interleave
from Normalize_Ratio import NormalizeRatio

"""
    Important notice!
    
    For this program to work it expects that the instrument supplied has two duplicates. 
    The first must have an opmask so that only the first two operators are triggered.
    The second does the opposite, triggering only the second two operators.
    This is allows the two subinstruments to be triggered asynchronously.

    The program will either assume the values for these instruments or 
    prompt the user for their value.
"""

def Prompt_User():
    ready = False
    
    print("\nEnsure that you have copied the desired column(s) from Furnace")
    col_num = input("How many columns do you have copied? (1 or 2; -1 to exit): ")

    if(col_num[0] == '1'):
        answer = input("Add delay? (y or n): ")

        if(answer[0] == 'y'):
            delay = eval(input("How much delay? "))

            Add_Delay(Parse_Column(), delay)
            # put data where it needs to go
            
            ready = True
        elif(answer[0] == 'n'):
            left_col_data = Parse_Column()
            
            input("Copy another column and hit enter")
            right_col_data = Parse_Column()

            # interleave column data
            Interleave(left_col_data, right_col_data)
            # put data where it needs to go

            ready = True
        else:
            print("Invalid option. Restarting prompt")
    elif(col_num[0] == '2'):
        Parse_Column(True)
        # put data where it needs to go

        ready = True
    elif(col_num[0:2] == list('-1')):
        exit()
    else:
        print("Invalid option. Restarting prompt")
    
    if ready:
        answer = input("Special multipliers (not all 8's)? (y or n): ")
        if answer[0] == 'y':
            ratio = []
            # get multipliers
            while ratio[0] <= 0 and ratio[0] > 15:
                ratio[0] = eval(input("Input the first multipier: "))
            while ratio[1] <= 0 and ratio[1] > 15:
                ratio[1] = eval(input("Input the second multipier: "))
            # set any 0 to 0.5
            for i in range(len(ratio)):
                if ratio[i] == 0:
                    ratio[i] = 0

            # figure out how to "normalize" ratios
            # ratio = NormalizeRatio(ratio)

            # run the thing
            print("not yet implemented")
        else:
            "run the thing"
        

    Prompt_User()

    return

# OUTDATED
# # Generates data for a single row. Ignores legato, the bar '|', and new lines
# def Generate_Row_Data(left_note:int, right_note:int, left_modulator:int, right_modulator:int) -> str:
#     # handle non-notes
#     # non-notes must be handles with instrument (opmask)
    
#     raw_row_data = FM_Interval_Calculator.Get_Interval_Ratio_Tuning(left_note, right_note, left_modulator, right_modulator)

#     formated_row_data = f"""
#     {NoteValueConverter.Value2Note(raw_row_data[2])}....
#     161{raw_row_data[0]}
#     162{raw_row_data[0]}
#     163{raw_row_data[1]}
#     164{raw_row_data[1]}
#     E5{format(raw_row_data[3], '02X')}
#     """

#     return formated_row_data

def Process_columns_into_one(parsed_data:list) -> list:
    # pop header from data
    mixed_column = parsed_data.pop(0)

    last_left_note = 120
    last_right_note = 120

    passed_left_value = 120
    passed_right_value = 120

    set_release_to_decay:bool = False
    
    #start for loop to iterate through parsed_data
    for i in range(0, len(parsed_data), 2):
        row_string = ""
        current_inst = 0
        # check note values for non-notes
        # if any non-notes, handle triggering and note cuts
        if parsed_data[i] >= 120 or parsed_data[i+1] >= 120:
            # if both are the same, output what they are
            if(parsed_data[i] == parsed_data[i+1]):
                passed_left_value = parsed_data[i]
                passed_right_value = parsed_data[i]
            # if only one non-note
            else:
                # if blank
                if parsed_data[i] == 120:
                    "somethign"
                    # try to hold out the most recent note
                    passed_left_value = last_left_note
                    # also set release to match decay value (effect 52xy)
                    set_release_to_decay = True
                elif parsed_data[i+1] == 120:
                    "something"
                    # try to hold out the most recent note
                    passed_right_value = last_right_note
                    # also set release to match decay value (effect 52xy)
                    set_release_to_decay = True

                # if one note cut/release, it is handled by instrument (its opmask and release)
                if parsed_data[i] == 121 or parsed_data[i+1] == 121 or parsed_data[i] == 122 or parsed_data[i+1] == 122:
                    if parsed_data[i] == 121 or parsed_data[i] == 122:
                        current_inst = 1
                    elif parsed_data[i+1] == 121 or parsed_data[i+1] == 122:
                        current_inst = 2
                else:
                    current_inst = 0

                # if macro relase, do it only if there is also another non-note
                if (parsed_data[i] == 123 and parsed_data[i+1] >= 120) or (parsed_data[i+1] == 123 and parsed_data[i] >= 120):
                    "Just output a macro release"

        # if only notes, just do things as normal
        else:
            # get row data
            raw_row_data = Get_Interval_Ratio_Tuning(passed_left_value, passed_right_value)
            
            # update last notes played
            last_left_note = parsed_data[i]
            last_right_note = parsed_data[i+1]

        # raw_row_data = Get_Interval_Ratio_Tuning(passed_left_value, passed_right_value)

        mixed_column += row_string
            
    # output tracker data for one column
    return