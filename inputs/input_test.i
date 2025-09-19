[Mesh]
  [fmg]
    type = FileMeshGenerator
    file = "../example_geom/cube_hex.e"
  []
 # [Partitioner]
 #   type = GridPartitioner
 #   grid_computation = 'automatic'
 # []
[]

[Problem]
  type = CompiledSourceTestingProblem 
  strength_type = 1
[]

[Executioner]
  type = Steady
[]
