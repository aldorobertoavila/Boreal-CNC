def read_gcode(path: str):
    for row in open(path, "r+"):
        yield row


def remove_comment(command: str):
    if ';' in command:
        return command[:command.index(';')]
    return command


def main():
    gcode = read_gcode('../../data/gcode/output.ngc')
    commands = {}

    for row in gcode:
        strip = row.strip()
        if not strip.startswith(';'):
            split = remove_comment(strip).split()
            if split:
                command = split[0]
                if command in commands:
                    commands[command] += 1
                else:
                    commands[command] = 1
    print(commands)


if __name__ == "__main__":
    main()
