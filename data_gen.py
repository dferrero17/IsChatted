import os
import argparse
from rewrite import rewrite

def ensure_unique_file(directory):
    def decorator(func):
        def wrapper(paragraph, *args, **kwargs):
            count = 1
            filename = f"{directory}/file_{count}.txt"
            while os.path.exists(filename):
                count += 1
                filename = f"{directory}/file_{count}.txt"
            if not os.path.exists(directory):
                os.makedirs(directory)
            return func(paragraph, filename, *args, **kwargs)
        return wrapper
    return decorator

@ensure_unique_file("human_files")
def write_human_text(paragraph, filename):
    with open(filename, "w") as file:
        file.writelines(paragraph)

@ensure_unique_file("gpt_files")
def write_gpt_text(paragraph, filename, model):
    chatted_paragraph = rewrite(paragraph, model)
    with open(filename, "w") as file:
        file.writelines(chatted_paragraph)

def extract_paragraph(datafile, model):
    with open(datafile, "r") as file:
        lines = file.readlines()

    paragraph = []
    for line in lines:
        if line.strip():
            paragraph.append(line)
        else:
            if paragraph:
                process_paragraph(paragraph, model)
                paragraph = []

    if paragraph:
        process_paragraph(paragraph, model)

def process_paragraph(paragraph, model):
    write_human_text(paragraph)
    write_gpt_text(paragraph, model)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("filepath", help="Path to the data file")
    parser.add_argument("-m", "--model", help="Model name", default="gpt-4-turbo")
    args = parser.parse_args()
    extract_paragraph(args.filepath, args.model)

if __name__ == "__main__":
    main()
