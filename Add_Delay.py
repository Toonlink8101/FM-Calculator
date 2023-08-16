"""
    Takes a parsed data list (a header str followed by ints) and a delay amout

    Outputs a new data list adding a second channel that is a delayed version of the first
"""
def Add_Delay(parsed_data:list, delay_amount:int = 1) -> list:
    # pop header from parsed data into delayed data
    delayed_data = [parsed_data.pop(0)]

    echo_buffer = []

    for i in range(delay_amount):
        # add a blanks at the end to account for delay
        parsed_data.append(120)

        # add dry notes to echo buffer
        echo_buffer.append(parsed_data[i])

        # add dry notes to output 
        delayed_data.append(parsed_data[i])

        # add silence to output for echo
        delayed_data.append(120)

    for i in range(delay_amount, len(parsed_data)):
        # add dry note to output
        delayed_data.append(parsed_data[i])

        # add dry note to echo buffer
        echo_buffer.append(parsed_data[i])

        # pop the start of echo buffer to output
        delayed_data.append(echo_buffer.pop(0))

    return delayed_data

"""
    Interleaves two lists of data
"""
def Interleave(left_data:list, right_data:list) -> list:
    if len(left_data) != len(right_data):
        print("Invalid input: data lengths do not match")
        return

    # get header
    mixed_data = [left_data[0]]

    for i in range(1, len(left_data)):
        mixed_data.append(left_data[i])
        mixed_data.append(right_data[i])

    return mixed_data