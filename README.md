# DML
Dan's Markup Language: YAML-inspired minimalistic, human-readable markup

Supported syntax as of Feb 3, 2019 (subject to change):

    test scene
    !1:entity
      name: string = "Timmy"  # This is entity #1's name
      age: int32 = 42
      weight: float32 = 123.000000:0x42f60000
      height: string = "5ft 6in"
      city: string = "San Diego"
    !2:entity
      name: string = "Bobby"  # This is a "another" guy's name
      age: int32 = 24
      weight1: float32 = 321.000000
      weight2: float32 = 321.000000:0x43a08000
      height: string = "2.86 meters"
      city: string = "New York"
      numbers: int32[] = {
        92492, 27745, 37710, 348, 28714, 68388, 18949, 4572, 568654, 654568, 213115
      }
      position: vec3 = {
        1.001,
        2.002,
        3.003,
      }
      char_array_fixed: char[7] = { 't', 'e', 's', 't', '1', '2', '3' }
      char_array_dyn: char[] = {
        't', 'h', 'i', 's',
        'i', 's',
        'a',
        't', 'e', 's', 't',
        'a', 'r', 'r', 'a', 'y',
      }
      position2: float32[3] = { 0.001, 0.002, 0.003 }
