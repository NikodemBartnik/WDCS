(G-CODE GENERATED BY FLATCAM v8.994 - www.flatcam.org - Version Date: 2020/11/7)

(Name: drill_1_16.xln_1_cnc)
(Type: G-code from Geometry)
(Units: MM)

(Created on Wednesday, 08 June 2022 at 22:37)

(This preprocessor is the default preprocessor used by FlatCAM.)
(It is made to work with MACH3 compatible motion controllers.)


(TOOLS DIAMETER: )
(Tool: 1 -> Dia: 1.016)

(FEEDRATE Z: )
(Tool: 1 -> Feedrate: 300)

(FEEDRATE RAPIDS: )
(Tool: 1 -> Feedrate Rapids: 1500)

(Z_CUT: )
(Tool: 1 -> Z_Cut: -1.5)

(Tools Offset: )
(Tool: 1 -> Offset Z: 0.0)

(Z_MOVE: )
(Tool: 1 -> Z_Move: 2)

(Z Toolchange: 15 mm)
(X,Y Toolchange: 0.0000, 0.0000 mm)
(Z Start: None mm)
(Z End: 0.5 mm)
(X,Y End: None mm)
(Steps per circle: 64)
(Preprocessor Excellon: default)

(X range:    6.0960 ...   29.2100  mm)
(Y range:    2.4130 ...   16.3830  mm)

(Spindle Speed: 0 RPM)
G21
G90
G94

G01 F300.00

G01 F300.00
M03 S1000
G00 X14.2240 Y15.8750
G01 Z-1.5000
G01 Z0
G00 Z2.0000
G00 X16.7640 Y15.8750
G01 Z-1.5000
G01 Z0
G00 Z2.0000
G00 X19.3040 Y15.8750
G01 Z-1.5000
G01 Z0
G00 Z2.0000
G00 X25.2053 Y9.4974
G01 Z-1.5000
G01 Z0
G00 Z2.0000
G00 X28.7020 Y3.0480
G01 Z-1.5000
G01 Z0
G00 Z2.0000
G00 X6.6040 Y2.9210
G01 Z-1.5000
G01 Z0
G00 Z2.0000
G00 X9.1970 Y9.4949
G01 Z-1.5000
G01 Z0
G00 Z2.0000
M05
G00 Z0.50


