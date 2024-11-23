import os
import random
import uuid

from contextlib import asynccontextmanager

from fastapi import FastAPI, HTTPException, Request
from fastapi.responses import JSONResponse
from fastapi_cache import FastAPICache
from fastapi_cache.backends.inmemory import InMemoryBackend

from pydantic import BaseModel

from slowapi.errors import RateLimitExceeded
from slowapi import Limiter, _rate_limit_exceeded_handler
from slowapi.util import get_remote_address

from PIL import Image

import base64

import numpy as np

from ai_edge_litert.interpreter import Interpreter

IMAGE_FOLDER = '/app/images'
MODEL_PATH = '/app/model/model.tflite'
CACHE_TIMEOUT = 10
IMAGE_SiZE = 64

class ClassifyImageRequest(BaseModel):
    image: str
    id: str

@asynccontextmanager
async def startup(app: FastAPI):
    # Initialize the in memory cache
    FastAPICache.init(InMemoryBackend(), prefix="fastapi-cache")

    yield


def generate_session_id():
    return str(uuid.uuid4())


def select_image():
    images = os.listdir(IMAGE_FOLDER)
    return random.choice(images)


limiter = Limiter(key_func=get_remote_address, strategy="fixed-window")
app = FastAPI(lifespan=startup)
app.state.limiter = limiter
app.add_exception_handler(RateLimitExceeded, _rate_limit_exceeded_handler)


@app.exception_handler(Exception)
async def global_exception_handler(request: Request, exc: Exception):
    return JSONResponse(
        status_code=500,
        content={"Error while handling the request. Error:": str(exc)},
    )

@app.get("/get_image")
@limiter.limit("1/minute")
async def get(request: Request):
    # Generate a session ID and select a random image
    session_id = generate_session_id()
    selected_image = select_image()

    # Cache the selected image
    await FastAPICache.get_backend().set(session_id, selected_image, expire=CACHE_TIMEOUT)

    # Encode the image in base64
    with open(os.path.join(IMAGE_FOLDER, selected_image), "rb") as image_file:
        encoded_image = base64.b64encode(image_file.read())

        # Return the session ID and the encoded image
        return JSONResponse(content={"id": session_id, "image": encoded_image.decode("utf-8")})


@app.post("/classify_image")
async def evaluate(classify_request: ClassifyImageRequest):
    cached_image = await FastAPICache.get_backend().get(classify_request.id)

    if cached_image is None:
        raise HTTPException(status_code=400, detail="The ID sent does not correspond to any session currently active")

    with Image.open(os.path.join(IMAGE_FOLDER, cached_image)) as image_file:
        image = image_file.resize((IMAGE_SiZE, IMAGE_SiZE))

    image = np.array(image).astype(np.float32)

    modified_image = base64.b64decode(classify_request.image)
    modified_image = np.frombuffer(modified_image, dtype=np.float32)
    modified_image = modified_image.reshape((IMAGE_SiZE, IMAGE_SiZE, 3))

    changed_positions = np.where(image != modified_image)

    x_changed = len(np.unique(changed_positions[0]))
    y_changed = len(np.unique(changed_positions[1]))

    if x_changed > 1 or y_changed > 1:
        raise HTTPException(status_code=400, detail=f"Genetic modified animals are not allowed in this office. Only one-pixel modifications are tolerated (found {max(x_changed, y_changed)} changed pixels)")

    # Load the TFLite model, allocate the tensors and get the input and output details
    interpreter = Interpreter(model_path=MODEL_PATH)
    interpreter.allocate_tensors()
    interpreter_input_details = interpreter.get_input_details()
    interpreter_output_details = interpreter.get_output_details()

    # app.state.interpreter.allocate_tensors()
    interpreter.set_tensor(interpreter_input_details[0]['index'], [modified_image])
    interpreter.invoke()
    output_data = interpreter.get_tensor(interpreter_output_details[0]['index'])

    if output_data[0][0] > 0.5:
        raise HTTPException(status_code=418, detail={"probability_of_being_a_cat": str(output_data[0][0]), "message": "Cats are not allowed in the office"})
    else:
        return {"detail": {"probability_of_being_a_cat": str(output_data[0][0]), "message": os.environ.get("FLAG")}}
