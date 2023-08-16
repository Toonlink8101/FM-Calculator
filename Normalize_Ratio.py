def NormalizeRatio(raw_ratio:list) -> list:
    mult_by_2 = raw_ratio[0] <= 7 and raw_ratio[1] <= 7
    mult_by_3 = raw_ratio[0] <= 5 and raw_ratio[1] <= 5
    mult_by_4_or_5 = raw_ratio[0] <= 3 and raw_ratio[1] <= 3
    mult_by_6_or_7 = raw_ratio[0] <= 2 and raw_ratio[1] <= 2
    mult_by_8 = raw_ratio[0] <= 1 and raw_ratio[1] <= 1

    # # can mult by 16
    # if raw_ratio[0] < 1 and raw_ratio[1] < 1:
    #     return [raw_ratio[0] * 16, raw_ratio[1] * 16]
    # # can mult by 8
    # elif raw_ratio[0] <= 1 and raw_ratio[1] <= 1:
    #     return [raw_ratio[0] * 8, raw_ratio[1] * 8]
    # # can mult by 4
    # elif raw_ratio[0] <= 3 and raw_ratio[1] <= 3:
    #     return [raw_ratio[0] * 4, raw_ratio[1] * 4]
    
    num = 0

    return