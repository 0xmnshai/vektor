import vektor_ui

# State
checkbox_val = True
slider_val = 0.5

def draw():
    global checkbox_val, slider_val

    # Create a Python-driven window
    if vektor_ui.begin("Properties (Python)", 1, 0): # Name, Open, Flags
        vektor_ui.text("This window is defined in Python!")
        vektor_ui.separator()
        
        if vektor_ui.button("Print to Console", 100, 20):
            print("Button Clicked from Python!")
            
        vektor_ui.separator()
        
        # Checkbox
        checkbox_val = vektor_ui.checkbox("Enable Feature", checkbox_val)
        
        # Slider
        slider_val = vektor_ui.slider_float("Value", slider_val, 0.0, 1.0)
        
        vektor_ui.text(f"Current Value: {slider_val:.2f}")

    vektor_ui.end()

    # Another window
    if vektor_ui.begin("Outliner (Python)"):
        vektor_ui.text("Collection 1")
        vektor_ui.text("  Cube")
        vektor_ui.text("  Camera")
        vektor_ui.text("  Light")
    vektor_ui.end()
