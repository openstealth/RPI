# RPI - Rendering Pipeline Integration

RPI is a sample project that demonstrates the way how to incorporate new rendering functionality into existing rendering pipeline without refactoring UE source code

### Features
- Custom fog component & render proxy
- Custom fog shader & render pass 
- Custom Weather actor 
- Time of Day control
- WGS84 coordinate system

### Usage

### To control fog appearence

### In editor:
- Choose Weather actor in folder REQUIRED
- Choose Details tab
- Change Fog Start & Height

### In PIE mode:
- Press F + Numpad Up Arrow (8) to increase the fog height
- Press F + Numpad Down Arrow (2) to decrease the fog height
- Press F + Numpad Right Arrow (6) to increase the fog start distance
- Press F + Numpad Left Arrow (4) to decrease the fog start distance

### To change Time of day

### In editor:
- Add World Settings actor via Window Menu
- Find & change Time of Day 

### In PIE mode:
- Press M to decrease time
- Press CTRL + M to increase time 
  

 