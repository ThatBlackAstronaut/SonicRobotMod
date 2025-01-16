from PIL import Image
import os

def trim_transparent_bottom(image):
    """
    Trims transparent space at the bottom of the image.
    """
    bbox = image.getbbox()
    if bbox:
        # Crop to the bounding box
        cropped = image.crop((0, 0, image.width, bbox[3]))
        return cropped
    return image

def process_images(folder_path):
    """
    Processes all images in the folder to ensure they are the same size.
    - Trims transparent space at the bottom.
    - Expands the canvas size to match the largest image in the folder.
    - Makes all images square without resizing existing pixels.
    """
    images = []
    max_size = 0

    # Load images and determine the maximum size
    for filename in os.listdir(folder_path):
        if filename.lower().endswith(('png', 'jpg', 'jpeg')) and "jump" not in filename.lower():
            img_path = os.path.join(folder_path, filename)
            image = Image.open(img_path).convert("RGBA")
            trimmed = trim_transparent_bottom(image)
            images.append((filename, trimmed))
            max_size = max(max_size, trimmed.width, trimmed.height)

    # Ensure the max size is square
    max_size = max(max_size, max_size)

    # Process images to match the maximum size
    for filename, image in images:
        if image.width == max_size and image.height == max_size:
            print(f"{filename} is already the correct size, skipping.")
            continue

        # Create a new blank image with the maximum size
        new_image = Image.new("RGBA", (max_size, max_size), (0, 0, 0, 0))
        x_offset = (max_size - image.width) // 2
        y_offset = max_size - image.height
        new_image.paste(image, (x_offset, y_offset))

        # Save the adjusted image
        save_path = os.path.join(folder_path, filename)
        new_image.save(save_path)
        print(f"Processed {filename} to size {max_size}x{max_size}.")

    # Log skipped images
    for filename in os.listdir(folder_path):
        if "jump" in filename.lower():
            print(f"Skipped processing {filename} as it contains 'jump'.")

if __name__ == "__main__":
    folder_path = "sheet"  # Change this to the path of your "sheet" folder
    if os.path.exists(folder_path):
        process_images(folder_path)
        print("All images processed successfully.")
    else:
        print(f"The folder {folder_path} does not exist.")
