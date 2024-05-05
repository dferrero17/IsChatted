MOBY_DICK_PATH = "moby_dick.txt"

def cleanse_moby_dick():
    with open(MOBY_DICK_PATH, "r", encoding='utf-8-sig') as file:
        lines = file.readlines()

    filtered_lines = [line for line in lines if "CHAPTER" not in line]

    with open(MOBY_DICK_PATH, "w") as file:
        file.writelines(filtered_lines)
