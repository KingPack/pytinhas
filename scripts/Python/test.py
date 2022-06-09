

JSON_1 = "1:05:00:04:59,2:05:00:04:59,3:05:00:04:59,4:05:00:04:59,5:05:00:04:59,6:05:00:04:59,7:05:00:04:59"
JSON_2 = "1:closed,2:closed,3:closed,4:closed,5:closed,6:closed,7:closed"

def test_func(json_data:int) -> dict:
    list_data = []
    json_data = str(json_data)

    if json_data:
        json_data = json_data.split(",")
        for data in json_data:
            data = data.split(":")
            if len(data) == 2:
                json_day = {
                    'day': data[0],
                    'entryHour': data[1],
                    'exitHour': data[1]
                }
                list_data.append(json_day)
            
            elif len(data) == 5:
                entry_hour = '{}:{}'.format(data[1], data[2])
                exit_hour = '{}:{}'.format(data[3], data[4])

                json_day = {
                    'day': data,
                    'entryHour': entry_hour,
                    'exitHour': exit_hour
                }
                list_data.append(json_day)

    return list_data

func_1 = test_func(JSON_1)
func_2 = test_func(JSON_2)    