| Issues found |
| --- |
| SetTransitionState doesn't get called in TransitionResourceEnd |
| We don't need material for vertices with colors -> For now dummy material used |
| In GeometryGenerator::ComputeVertices find takes too much time |

| Ideas to implement |
| --- |
| Allow simulation to set/reset initial camera position and respond to keyboard/mouse |
| `auto v = static_cast<RenderItemHandle*>(&ri)->GetValue();` -> `auto v = GetValue(ri)` use kind of traits |

