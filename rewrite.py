from openai import OpenAI
import os
from dotenv import load_dotenv

load_dotenv()

OPENAI_API_KEY = os.getenv('OPENAI_API_KEY')
client = OpenAI(api_key=OPENAI_API_KEY)

def rewrite(human_text, model):
    response = client.chat.completions.create(
    model=model,
    messages=[
        {
        "role": "user",
        "content": f'Rewrite the following text, maintaining roughly the same number of characters:{human_text}'
        }
    ],
    temperature=1,
    max_tokens=256,
    top_p=1,
    frequency_penalty=0,
    presence_penalty=0
    )
    return response.choices[0].message.content


