from openai import OpenAI

from dotenv import load_dotenv

import os

load_dotenv()

client = OpenAI()

def convert_ir_to_mips(file_path, output_path):
    # Read the file content
    with open(file_path, 'r') as file:
        ir_code = file.read()

    # Define the prompt
    prompt = f"{ir_code}"

    # Send the request to OpenAI
    response = client.chat.completions.create(
        model="gpt-4",
        messages=[
            {"role": "system", "content": "You are a code translator. Convert the IR code to MIPS Assembly code. Combine both the data and main section into one file. Add an exit to the end of the program. Return only the code."},
           {"role": "user", "content": prompt}
        ]
    )

    # Extract and return the response text
    #    response_text = response.choices[0].message.content
    converted_code = response.choices[0].message.content
    print(converted_code)

    output_filename = "generatedMIPS.asm"  # The name of the output file
    full_output_path = os.path.join(output_path, output_filename)
    with open(full_output_path, 'w') as output_file:
        output_file.write(converted_code)

# Example usage
# Note: You need to provide your own OpenAI API key and have the 'IRC.ir' file in the same directory
# openai_api_key = "your-api-key-here"
# converted_code = convert_ir_to_mips("IRC.ir", openai_api_key)
# print(converted_code)

convert_ir_to_mips('build/output/irc/IRC.ir', 'build/output/asm/')