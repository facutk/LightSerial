stream = "000011111000001111100000"
reading_prev = "0"
count = 0
transition_count = 0
transition_found = False
transition_threshold = 2

#state_counting
# event_same -> state_counting
# event_transition -> state_transitioning

#state_transitioning
# event_same -> state_transitioning
# event_transition -> state_counting

for reading in stream:
    transition = False

    if transition_found:
        if reading == reading_prev:
            transition_count += 1
            if transition_count >= transition_threshold:
                transition = True
                transition_found = False
                count += transition_count
                transition_count = 0
        else:
            transition_found = False
            count += transition_count
            transition_count = 0
    else:
        if reading == reading_prev:
            count += 1
        else:
            transition_found = True
            transition_count = 0
        
    if transition:
        object = '0'
        if reading == '0':
            object = '1'
        print object, count
        count = 1

    reading_prev = reading
