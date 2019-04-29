# DML
Dan's Markup Language: YAML-inspired minimalistic, human-readable markup

### Dependencies
https://github.com/dbechrd/dlb

### Example Usage
My primary goal with this project was to use DML to describe scene data in my 3D game engine. The following is the example file from the data directory of this project `custom.dml`:

    # custom.dml test file
    ta_entity:
      name: "entity_1"
      material: "material_1"
      transform:
        position: { x: 1.0, y: 2.0, z: 3.0 }
        rotation: { x: 1.0, y: 2.0, z: 3.0, w: 4.0 }
        scale:
          x: 1.0
          y: 1.0
          z: 1.0
    ta_material:
      name: "material_1"
    ta_mesh:
      name: "mesh_1"
      path: "mesh_1_path"
    ta_shader:
      name: "shader_1"
      path: "shader_1_path"
    ta_texture:
      name: "texture_1"
      path: "texture_1_path"
    ta_sun_light:
      name: "sun"
      direction: { x: 1.0, y: 2.0, z: 3.0 }
      color:     { r: 1.0, g: 2.0, b: 3.0 }
    ta_point_light:
      name: "point_light_1"
      position: { x: 1.0, y: 2.0, z: 3.0 }
      color:    { r: 1.0, g: 2.0, b: 3.0 }

### Updates
DML has been integrated into my closed source (as of writing) game engine and this repository will most likely not be updated or supported as a standalone product. That said, I think this project is serves as a fairly complete example of how to implement a basic parser in C that can serialize/deserialize C structs to a human-readable file format. The main advantage of the hard-coded schema in C is that it allows keeping the schema up-to-date when the struct's are renamed using Visual Studio's refactoring tools.

### Support
If you have any questions or comments about the codebase, feel free to email me at `my github username at gmail.com`.

### Recommended Improvements
The following is a list of things I'd highly recommend you consider before using this in production:
- Finish adding support for arrays
- Write up a proper specification
- Test suite
- Possibly use arena allocator instead of stretchy buffers for tokens and interned strings
- Everything else I said in the TODO comments
