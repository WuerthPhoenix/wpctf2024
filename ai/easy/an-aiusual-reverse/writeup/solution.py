# NOTE: This script is meant to be used as a one-shot solution to the challenge.
# For more information about the specific steps of the solution, please consult the writeup.ipynb notebook.

# load the model
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '1'

import tensorflow as tf
import numpy as np

# Load the model
model = tf.keras.models.load_model("../train/model.keras")

# Print useful infos about the model

print("\n\n=======================================")
print("Model useful information")
print("=======================================")

print("**Model summary**")
print(model.summary())

print("**Model config**")
print(model.get_config())

print("**Model optimizer specs**")
with open("../train/optimizer_config.json", "r") as f:
    print(f.read())

# define the number of epochs and the learning rate
epochs = 10

learning_rate = 0.001

# Define function to rescale the image
def rescale(img):
    return tf.keras.layers.Rescaling(scale=1./255)(img)


# Define the loss function
loss_object = tf.keras.losses.CategoricalCrossentropy()

# Define the model inversion step
def model_inversion_step(img, label):
    # Create a variable to hold the image
    image = tf.Variable(rescale(img))
    # Create the optimizer based on the specified learning rate
    optimizer = tf.keras.optimizers.Adam(learning_rate=learning_rate)
    with tf.GradientTape() as tape:
        tape.watch(image)
        # Get the prediction of the model
        pred = model(image, training=False)
        # Calculate the loss
        loss = loss_object(label, pred)
        print(f"Loss: {loss}")
        # Get the gradients of the loss w.r.t to the input image.
        gradient = tape.gradient(loss, image)
        # Update the image with the gradients.
        optimizer.apply_gradients(zip([gradient], [image]))

    return image.numpy()

# Define the starting image to be made up by random values between 0 and 1
reconstructed_image = np.random.random((1, 300, 300, 1))
reconstructed_image = tf.convert_to_tensor(reconstructed_image)

# Define the target label (i.e. the first class, as mentioned in the description of the challenge)
target_label = 0
target_label = np.zeros(2)
target_label[0] = 1

print("\n\n=======================================")
print("Reconstruct the image")
print("=======================================")

# Perform the model inversion
for i in range(0, epochs):
    print("=======================================")
    print(f"Epoch {i + 1}")
    reconstructed_image = model_inversion_step(reconstructed_image, np.array([target_label]))


print("\n\n=======================================")
print("Final stats")
print("=======================================")

# Calculate and print the final loss
pred = model(reconstructed_image, training=False)
predicted_class = np.argmax(pred)
loss = loss_object(np.array([target_label]), pred)
print(f"Final loss: {loss}")

print("\n\n=======================================")
print("Show and save the final image")
print("=======================================")

# Show and save the reconstructed image
image = tf.keras.preprocessing.image.array_to_img(reconstructed_image[0])
image.show()
image.save("flag.png")