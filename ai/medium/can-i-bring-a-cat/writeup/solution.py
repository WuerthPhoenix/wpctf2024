import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '1'

import numpy as np
from PIL import Image
import requests
from io import BytesIO
import base64

from scipy.optimize import differential_evolution
from scipy.optimize import OptimizeResult

# Define the target URL. This is the URL of the server that we want to attack
target_url = "http://localhost:8000"
get_image_url = target_url + "/get_image"
submit_image_url = target_url + "/classify_image"
image_size = 64
flag = ""

def alter_image(image, pixel):
    pixel = pixel.astype(int)
    altered_image = np.copy(image)
    # Change the pixel value at the given location
    altered_image[pixel[0]][pixel[1]][0] = pixel[2]
    altered_image[pixel[0]][pixel[1]][1] = pixel[3]
    altered_image[pixel[0]][pixel[1]][2] = pixel[4]

    return altered_image


def probability_of_being_a_cat(image, image_id):
    global flag
    img_str = base64.b64encode(image[0])
    response = requests.post(submit_image_url, json={'image': img_str.decode('UTF-8'), 'id': image_id})

    if response.status_code == 200:
        print(response.json())
        flag = response.json()["detail"]["message"]

    try:
        probability = float(response.json()["detail"]["probability_of_being_a_cat"])
    except:
        raise Exception("Error in response: " + str(response.json()))

    return probability


def predict_fn(altered_pixel, args):
    # Alter the image by the given amount
    image = alter_image(args[0], altered_pixel)
    return probability_of_being_a_cat([image], id)


def callback_fn(intermediate_result: OptimizeResult):
    return intermediate_result["fun"] < 0.5


# make request to get the image
response = requests.get(get_image_url)

response_json = response.json()

# load the image and resize it
id = response_json["id"]
image = Image.open(BytesIO(base64.b64decode(response_json["image"])))
image = image.resize((image_size, image_size))
image = np.array([image]).astype(np.float32)


# Generate the bounds for the pixel that we need to alter
bounds = [(0,image_size), (0,image_size), (0,255), (0,255), (0,255)]

maxiter=10

# Run the differential evolution algorithm
attack_result = differential_evolution(
    predict_fn, bounds, args=([image]), popsize=8, tol=0, atol=0, maxiter=maxiter, callback=callback_fn
)
print(attack_result)
print(flag)

# Alter the image with the pixel found by the differential evolution algorithm
altered_image = alter_image(image[0], attack_result.x)
altered_image = Image.fromarray(altered_image.astype(np.uint8))
# Display the image
altered_image.show()
