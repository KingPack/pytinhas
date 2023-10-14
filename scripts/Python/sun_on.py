import requests
import time


activated = False


def activate_sun_relay():
    try:
        response = requests.get("http://192.168.1.8/sun-on")
        if response.status_code == 200:
            print("Sun relay activated successfully!")
            global activated
            activated = True
        else:
            print(f"""Failed to activate sun relay.
                      Status code: {response.status_code}""")

    except Exception as e:
        print(f"Error making HTTP request: {e}")


if __name__ == "__main__":
    while True:
        current_time = time.strftime("%H:%M")

        if current_time >= "08:00":
            if not activated:
                activate_sun_relay()
                time.sleep(10)
            else:
                print("The function has already been activated.")
                time.sleep(10)
        else:
            if activated:
                print("The function has already been activated.")
                time.sleep(10)
            else:
                print("The function has already been activated.")
                time.sleep(12)
