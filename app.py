import customtkinter as ctk
import os
import subprocess
from tkinter import filedialog

ctk.set_appearance_mode("light")


def process_inputs():
    """
    Process inputs from the GUI and execute a command to perform a task.

    :return: None
    """
    try:
        # Disable the button to prevent multiple submissions
        process_button.configure(state=ctk.DISABLED)

        # Update the status label to indicate processing
        status_label.configure(text="Processing... Please wait.")
        root.update_idletasks()

        # Collect parameters from the GUI
        filters = []
        if exclude_numbers.get(): filters.append("n")
        if exclude_symbols.get(): filters.append("s")
        if exclude_spaces.get(): filters.append("e")
        if exclude_case.get(): filters.append("c")
        filters_str = ''.join(filters)

        model_values = {"GPT 4": "gpt-4", "GPT 3.5": "gpt-3.5", "Both": "merge"}
        model = model_values[model_var.get()]
        alpha = alpha_slider.get()
        k = k_entry.get()
        size_limit = sizeLimit_entry.get()

        input_dir = './input_files/'
        if not os.path.exists(input_dir):
            os.makedirs(input_dir)

        results_dir = "./results/"
        if not os.path.exists(results_dir):
            os.makedirs(results_dir)

        with open(f"./{input_dir}/input_file.txt", "w") as file:
            file.write(input_text.get("1.0", "end-1c"))

        with open(f"./{results_dir}/results_file.txt", "w") as file:
            pass

        model_path_gpt = f"./models/{model}/gpt_files/"
        model_path_human = f"./models/{model}/human_files/"
        results_file_path = "./results_file.txt"
        gpt_human = gpt_human_var.get()

        # Prepare the command
        command = [
            "./run_interface.sh",
            "-g", model_path_gpt,
            "-h", model_path_human,
            "-t", input_dir,
        ]
        if filters_str != "":
            command.append("-f")
            command.append(filters_str)
        if k != "":
            command.append("-k")
            command.append(k)
        command += [
            "-a", str(alpha),
        ]
        if str(size_limit) != "":
            command.append("-s")
            command.append(size_limit)
        command += [
            "-o", results_file_path,
            "-b", gpt_human
        ]

        # Execute the command
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        _ = process.communicate()

        if process.returncode == 0:
            # Read the results from the specified results file
            with open(results_dir + results_file_path, "r") as file:
                results_content = file.read()

            # Parse the results
            results_list = results_content.split(',')
            # is_gpt = results_list[3]
            is_human = results_list[5]
            bit_ratio = results_list[6]
            execution_time = results_list[8]

            # Display the results in the results_frame
            result_text.configure(state=ctk.NORMAL)
            result_text.delete(1.0, ctk.END)
            if int(is_human):
                result_text.insert(ctk.END, "Human")
            else:
                result_text.insert(ctk.END, "GPT")
            bit_ratio_text.delete(1.0, ctk.END)
            result_text.configure(state=ctk.DISABLED)

            bit_ratio_text.configure(state=ctk.NORMAL)
            bit_ratio_text.delete(1.0, ctk.END)
            bit_ratio_text.insert(ctk.END, f"{bit_ratio}")
            bit_ratio_text.configure(state=ctk.DISABLED)

            execution_time_text.configure(state=ctk.NORMAL)
            execution_time_text.delete(1.0, ctk.END)
            execution_time_text.insert(ctk.END, f"{execution_time}")
            execution_time_text.configure(state=ctk.DISABLED)

        status_label.configure(text="Concluded. Program's Ready")

    except Exception as e:
        print("Error", str(e))
        status_label.configure(text="Error verifying. Program's Ready")
        result_text.configure(state=ctk.NORMAL)
        result_text.delete(1.0, ctk.END)
        result_text.insert(ctk.END, "")
        bit_ratio_text.delete(1.0, ctk.END)
        result_text.configure(state=ctk.DISABLED)

        bit_ratio_text.configure(state=ctk.NORMAL)
        bit_ratio_text.delete(1.0, ctk.END)
        bit_ratio_text.insert(ctk.END, "")
        bit_ratio_text.configure(state=ctk.DISABLED)

        execution_time_text.configure(state=ctk.NORMAL)
        execution_time_text.delete(1.0, ctk.END)
        execution_time_text.insert(ctk.END, "")
        execution_time_text.configure(state=ctk.DISABLED)

    finally:
        # Re-enable the button and update tasks regardless of the outcome
        process_button.configure(state=ctk.NORMAL)

        root.update_idletasks()


def toggle_theme():
    """
    Toggles the theme based on the switch state.

    :return: None
    """
    # Directly toggle the theme based on the switch state
    if theme_switch.get() == 1:
        ctk.set_appearance_mode("dark")
    else:
        ctk.set_appearance_mode("light")


def open_file_dialog():
    """
    This function opens a file dialog and updates the `input_text` with the content of the selected file.

    :return: None
    """
    # This function opens a file dialog and updates the input_text with the content of the selected file
    file_path = filedialog.askopenfilename(title="Select a file",
                                           filetypes=(("Text files", "*.txt"), ("All files", "*.*")))
    if file_path:
        try:
            with open(file_path, 'r') as file:
                file_content = file.read()
                input_text.delete("1.0", ctk.END)
                input_text.insert("1.0", file_content)
        except Exception as e:
            print("Error", f"Failed to read the file: {str(e)}")
        finally:
            root.update_idletasks()


root = ctk.CTk()
root.title("GPT Detector")
root.geometry("1000x550")

# Configure column weights
root.grid_columnconfigure(0, weight=1)
root.grid_columnconfigure(1, weight=1)
root.grid_columnconfigure(2, weight=1)
root.grid_rowconfigure(0, weight=1)

# Input frame
input_frame = ctk.CTkFrame(root, corner_radius=10)
input_frame.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")

# Filters with checkboxes
filters_frame = ctk.CTkFrame(input_frame, corner_radius=10)
filters_frame.pack(padx=10, pady=5, fill="x")
ctk.CTkLabel(filters_frame, text="Filters:").pack(side="top", fill="x", padx=10, pady=5)
exclude_numbers = ctk.CTkCheckBox(filters_frame, text="Exclude numbers")
exclude_numbers.pack(side="top", fill="x", padx=10, pady=5)
exclude_symbols = ctk.CTkCheckBox(filters_frame, text="Exclude symbols")
exclude_symbols.pack(side="top", fill="x", padx=10, pady=10)
exclude_spaces = ctk.CTkCheckBox(filters_frame, text="Exclude spaces")
exclude_spaces.pack(side="top", fill="x", padx=10, pady=5)
exclude_case = ctk.CTkCheckBox(filters_frame, text="Exclude case sensitiveness")
exclude_case.pack(side="top", fill="x", padx=10, pady=10)

# Model selection using dropdown
model_frame = ctk.CTkFrame(input_frame, corner_radius=10)
model_frame.pack(padx=10, pady=5, fill="x")
ctk.CTkLabel(model_frame, text="Model to be used:").pack(side="top", fill="x", padx=10, pady=5)
model_var = ctk.StringVar()
model_options = ["GPT 3.5", "GPT 4", "Both"]
model_dropdown = ctk.CTkOptionMenu(model_frame, variable=model_var, values=model_options)
model_dropdown.pack(side="top", fill="x", padx=10, pady=5)
model_var.set("GPT 3.5")

# Numeric inputs for alpha, k, and sizeLimit
alpha_frame = ctk.CTkFrame(input_frame, corner_radius=10)
alpha_frame.pack(padx=10, pady=5, fill="x")
ctk.CTkLabel(alpha_frame, text="Alpha (0 to 1):").pack(side="top", fill="x", padx=10, pady=5)
alpha_slider = ctk.CTkSlider(alpha_frame, from_=0, to=1, number_of_steps=100)
alpha_slider.pack(side="top", fill="x", padx=10, pady=5)

# Launch Frame
k_sizeLimit_frame = ctk.CTkFrame(input_frame, corner_radius=10)
k_sizeLimit_frame.pack(padx=10, pady=5, fill="both", expand=True)

k_entry = ctk.CTkEntry(k_sizeLimit_frame, placeholder_text="k (integer)")
k_entry.pack(side="top", fill="x", padx=10, pady=5, expand=True)

sizeLimit_entry = ctk.CTkEntry(k_sizeLimit_frame, placeholder_text="sizeLimit (integer)")
sizeLimit_entry.pack(side="top", fill="x", padx=10, pady=5, expand=True)

# Process button frame
process_frame = ctk.CTkFrame(root, corner_radius=10)
process_frame.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")

# Launch Frame
input_text_frame = ctk.CTkFrame(process_frame, corner_radius=10)
input_text_frame.pack(padx=10, pady=5, fill="both")

# Input text for processing
input_text = ctk.CTkTextbox(input_text_frame)
input_text.pack(fill="both", expand=True, padx=10, pady=10)

# File selection button
file_select_button = ctk.CTkButton(input_text_frame, text="Select File to Verify", command=open_file_dialog)
file_select_button.pack(pady=10, padx=10, fill="both")

# GPT/Human selection using dropdown
gpt_human_frame = ctk.CTkFrame(process_frame, corner_radius=10)
gpt_human_frame.pack(padx=10, pady=5, fill="both")
ctk.CTkLabel(gpt_human_frame, text="Input Source:").pack(side="top", fill="x", padx=10, pady=5)
gpt_human_var = ctk.StringVar()
gpt_human_options = ["Human", "GPT"]
gpt_human_dropdown = ctk.CTkOptionMenu(gpt_human_frame, variable=gpt_human_var, values=gpt_human_options)
gpt_human_dropdown.pack(side="top", fill="x", padx=10, pady=5)
gpt_human_var.set("Human")

# Launch Frame
launch_frame = ctk.CTkFrame(process_frame, corner_radius=10)
launch_frame.pack(padx=10, pady=5, fill="both")

# Process (Was Chatted?) Button
process_button = ctk.CTkButton(launch_frame, text="Was Chatted?", command=process_inputs, height=28)
process_button.pack(pady=10, padx=10, fill="both", expand=True)

# Status Frame
status_frame = ctk.CTkFrame(launch_frame, corner_radius=10)
status_frame.pack(padx=10, pady=5, fill="both")

# Status Lable Text
status_label = ctk.CTkLabel(status_frame, text="Program's Ready", height=28)
status_label.pack(pady=5, padx=10)

# Results frame
results_frame = ctk.CTkFrame(root, corner_radius=10)
results_frame.grid(row=0, column=2, padx=10, pady=10, sticky="nsew")

# Frame for Result
result_frame = ctk.CTkFrame(results_frame)
result_frame.pack(side="top", fill="both", expand=True, padx=10, pady=10)
result_label = ctk.CTkLabel(result_frame, text="Result")
result_label.pack(side="top", padx=10, pady=(10, 5))
result_text = ctk.CTkTextbox(result_frame, height=10, width=50, state=ctk.DISABLED)
result_text.pack(side="top", fill="both", padx=10, pady=(0, 10))

# Frame for Bit Ratio
bit_ratio_frame = ctk.CTkFrame(results_frame)
bit_ratio_frame.pack(side="top", fill="both", expand=True, padx=10, pady=0)
bit_ratio_label = ctk.CTkLabel(bit_ratio_frame, text="Bit Ratio GPT/Human")
bit_ratio_label.pack(side="top", padx=10, pady=(10, 5))
bit_ratio_text = ctk.CTkTextbox(bit_ratio_frame, height=2, width=50, state=ctk.DISABLED)
bit_ratio_text.pack(side="top", fill="both", padx=10, pady=(0, 10))

# Frame for Execution Time
execution_time_frame = ctk.CTkFrame(results_frame)
execution_time_frame.pack(side="top", fill="both", expand=True, padx=10, pady=10)
execution_time_label = ctk.CTkLabel(execution_time_frame, text="Execution Time")
execution_time_label.pack(side="top", padx=10, pady=(10, 5))
execution_time_text = ctk.CTkTextbox(execution_time_frame, height=2, width=50, state=ctk.DISABLED)
execution_time_text.pack(side="top", fill="both", padx=10, pady=(0, 10))

# Theme toggle frame at the bottom center
theme_frame = ctk.CTkFrame(root, corner_radius=10)
theme_frame.grid(row=1, column=0, columnspan=3, padx=10, pady=(0, 10), sticky="ws")

# Theme toggle switch
theme_switch = ctk.CTkSwitch(theme_frame, text="Dark Mode", command=toggle_theme)
if ctk.get_appearance_mode() == "dark":
    theme_switch.select()  # This ensures the switch is in the correct position if starting in dark mode
else:
    theme_switch.deselect()
theme_switch.pack(pady=10, padx=10, side="left")

root.mainloop()
