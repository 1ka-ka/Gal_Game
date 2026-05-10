from PIL import Image, ImageDraw, ImageFont
import os

# Base directories
project_dir = r"e:\trea\视觉系小说\VisualNovel"
resources_dir = os.path.join(project_dir, "Resources")
characters_dir = os.path.join(resources_dir, "Characters")
backgrounds_dir = os.path.join(resources_dir, "Backgrounds")

# Create directories
for d in [resources_dir, characters_dir, backgrounds_dir]:
    os.makedirs(d, exist_ok=True)

# Color palettes for characters (each character gets a unique color scheme)
char_palettes = {
    "protagonist": {
        "normal": ("#3498db", "#2c3e50", "#ffffff"),
        "smile": ("#3498db", "#2c3e50", "#ffffff"),
        "surprised": ("#3498db", "#2c3e50", "#ffffff"),
        "thinking": ("#3498db", "#2c3e50", "#ffffff"),
        "nervous": ("#3498db", "#2c3e50", "#ffffff"),
        "determined": ("#3498db", "#2c3e50", "#ffffff"),
        "curious": ("#3498db", "#2c3e50", "#ffffff"),
        "calm": ("#3498db", "#2c3e50", "#ffffff"),
    },
    "lin_yue": {
        "normal": ("#e74c3c", "#c0392b", "#ffffff"),
        "smile": ("#e74c3c", "#c0392b", "#ffffff"),
        "surprised": ("#e74c3c", "#c0392b", "#ffffff"),
        "thinking": ("#e74c3c", "#c0392b", "#ffffff"),
        "serious": ("#e74c3c", "#c0392b", "#ffffff"),
    },
    "su_yao": {
        "normal": ("#f39c12", "#d68910", "#ffffff"),
        "smile": ("#f39c12", "#d68910", "#ffffff"),
        "surprised": ("#f39c12", "#d68910", "#ffffff"),
        "calm": ("#f39c12", "#d68910", "#ffffff"),
    },
    "zhang_hao": {
        "normal": ("#27ae60", "#1e8449", "#ffffff"),
        "smile": ("#27ae60", "#1e8449", "#ffffff"),
    },
    "xia_yin": {
        "normal": ("#8e44ad", "#6c3483", "#ffffff"),
        "serious": ("#8e44ad", "#6c3483", "#ffffff"),
        "calm": ("#8e44ad", "#6c3483", "#ffffff"),
    },
}

# Background palettes
bg_palettes = {
    "bg_room_morning": (
        (255, 248, 220),
        (255, 235, 205),
        (255, 222, 173),
    ),
    "bg_street_morning": (
        (135, 206, 235),
        (176, 226, 247),
        (200, 230, 250),
    ),
    "bg_school_gate": (
        (189, 189, 189),
        (158, 158, 158),
        (211, 211, 211),
    ),
    "bg_bus": (
        (70, 130, 180),
        (100, 149, 237),
        (135, 170, 245),
    ),
    "bg_seaside_road": (
        (0, 191, 255),
        (173, 216, 230),
        (224, 255, 255),
    ),
    "bg_classroom": (
        (255, 250, 240),
        (245, 245, 220),
        (255, 239, 213),
    ),
    "bg_school_corridor": (
        (211, 211, 211),
        (192, 192, 192),
        (220, 220, 220),
    ),
    "bg_rooftop": (
        (135, 206, 250),
        (176, 224, 230),
        (240, 248, 255),
    ),
    "bg_library": (
        (139, 69, 19),
        (160, 82, 45),
        (210, 180, 140),
    ),
    "bg_music_room": (
        (128, 0, 128),
        (147, 112, 219),
        (186, 85, 211),
    ),
    "bg_student_council": (
        (255, 215, 0),
        (255, 223, 0),
        (255, 235, 0),
    ),
    "bg_basketball_court": (
        (205, 92, 92),
        (233, 150, 122),
        (255, 160, 122),
    ),
}

# Emotion indicators (simple symbols)
emotion_symbols = {
    "normal": "",
    "smile": "☺",
    "surprised": "!",
    "thinking": "?",
    "nervous": "...",
    "determined": "!",
    "curious": "?",
    "serious": "!",
    "calm": "~",
}

def create_character_image(char_id, expression, color_tuple):
    """Create a character portrait image"""
    width, height = 300, 500
    img = Image.new('RGB', (width, height), color=color_tuple[0])
    draw = ImageDraw.Draw(img)
    
    # Draw character body outline
    draw.ellipse([50, 30, 250, 230], fill=color_tuple[1], outline="white", width=3)
    
    # Draw expression symbol
    symbol = emotion_symbols.get(expression, "")
    if symbol:
        try:
            font = ImageFont.truetype("arial.ttf", 80)
        except:
            font = ImageFont.load_default()
        draw.text((120, 100), symbol, fill="white", font=font)
    
    # Draw body shape
    draw.rectangle([80, 220, 220, 480], fill=color_tuple[1], outline="white", width=3)
    
    # Draw character name at bottom
    try:
        font = ImageFont.truetype("arial.ttf", 16)
    except:
        font = ImageFont.load_default()
    
    name = char_id.replace('_', ' ').title()
    text = f"{name}"
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    draw.text(((width - text_width) / 2, 400), text, fill="white", font=font)
    
    # Draw expression name
    expr_text = f"[{expression}]"
    bbox = draw.textbbox((0, 0), expr_text, font=font)
    expr_width = bbox[2] - bbox[0]
    draw.text(((width - expr_width) / 2, 425), expr_text, fill="white", font=font)
    
    # Save
    filename = f"{char_id}_{expression}.png"
    filepath = os.path.join(characters_dir, filename)
    img.save(filepath, 'PNG')
    print(f"Created: {filepath}")

def create_background_image(bg_id, color_tuple):
    """Create a background image"""
    width, height = 1280, 720
    img = Image.new('RGB', (width, height))
    draw = ImageDraw.Draw(img)
    
    # Create gradient
    for y in range(height):
        r = int(color_tuple[0][0] + (color_tuple[1][0] - color_tuple[0][0]) * y / height)
        g = int(color_tuple[0][1] + (color_tuple[1][1] - color_tuple[0][1]) * y / height)
        b = int(color_tuple[0][2] + (color_tuple[1][2] - color_tuple[0][2]) * y / height)
        draw.line([(0, y), (width, y)], fill=(r, g, b))
    
    # Add some decorative elements
    try:
        font = ImageFont.truetype("arial.ttf", 48)
    except:
        font = ImageFont.load_default()
    
    text = bg_id.replace('_', ' ').title()
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    draw.text(((width - text_width) / 2, 300), text, fill=(255, 255, 255), font=font)
    
    # Save
    filename = f"{bg_id}.png"
    filepath = os.path.join(backgrounds_dir, filename)
    img.save(filepath, 'PNG')
    print(f"Created: {filepath}")

# Generate all character images
print("=== Generating Character Images ===")
for char_id, expressions in char_palettes.items():
    for expression, colors in expressions.items():
        create_character_image(char_id, expression, colors)

# Generate all background images
print("\n=== Generating Background Images ===")
for bg_id, colors in bg_palettes.items():
    create_background_image(bg_id, colors)

print("\n=== All images generated successfully! ===")
