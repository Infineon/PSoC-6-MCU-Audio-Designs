# False paths present for both 8 and 16 bits Graphic LCD Interface configurations 
expr {(`$BitWidth`) == 8 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd8:Lsb\/p_out_0}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_0}]] : \
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Lsb\/p_out_0}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_0}]]} 
   
expr {(`$BitWidth`) == 8 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd8:Lsb\/p_out_1}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_1}]] : \
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Lsb\/p_out_1}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_1}]]} 
   
expr {(`$BitWidth`) == 8 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd8:Lsb\/p_out_2}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_2}]] : \
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Lsb\/p_out_2}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_2}]]} 
   
expr {(`$BitWidth`) == 8 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd8:Lsb\/p_out_3}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_3}]] : \
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Lsb\/p_out_3}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_3}]]} 
   
expr {(`$BitWidth`) == 8 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd8:Lsb\/p_out_4}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_4}]] : \
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Lsb\/p_out_4}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_4}]]} 
   
expr {(`$BitWidth`) == 8 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd8:Lsb\/p_out_5}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_5}]] : \
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Lsb\/p_out_5}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_5}]]} 
   
expr {(`$BitWidth`) == 8 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd8:Lsb\/p_out_6}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_6}]] : \
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Lsb\/p_out_6}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_6}]]} 
 
expr {(`$BitWidth`) == 8 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd8:Lsb\/p_out_7}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_7}]] : \
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Lsb\/p_out_7}] -to [get_pins {\`$CY_FITTER_NAME`:LsbReg\/status_7}]]} 
   
# False paths present only for 16 bits Graphic LCD Interface configuration
expr {(`$BitWidth`) == 16 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Msb\/p_out_0}] -to [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:MsbReg\/status_0}]] : {}}
   
expr {(`$BitWidth`) == 16 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Msb\/p_out_1}] -to [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:MsbReg\/status_1}]] : {}}
   
expr {(`$BitWidth`) == 16 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Msb\/p_out_2}] -to [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:MsbReg\/status_2}]] : {}}

expr {(`$BitWidth`) == 16 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Msb\/p_out_3}] -to [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:MsbReg\/status_3}]] : {}}
   
expr {(`$BitWidth`) == 16 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Msb\/p_out_4}] -to [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:MsbReg\/status_4}]] : {}}
   
expr {(`$BitWidth`) == 16 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Msb\/p_out_5}] -to [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:MsbReg\/status_5}]] : {}}
   
expr {(`$BitWidth`) == 16 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Msb\/p_out_6}] -to [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:MsbReg\/status_6}]] : {}}
   
expr {(`$BitWidth`) == 16 ?
   [set_false_path -from [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:Msb\/p_out_7}] -to [get_pins {\`$CY_FITTER_NAME`:GraphLcd16:MsbReg\/status_7}]] : {}}
