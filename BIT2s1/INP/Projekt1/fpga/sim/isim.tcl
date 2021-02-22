proc isim_script {} {

   add_divider "Signals of the Vigenere Interface"
   add_wave_label "" "CLK" /testbench/clk
   add_wave_label "" "RST" /testbench/rst
   add_wave_label "-radix ascii" "DATA" /testbench/tb_data
   add_wave_label "-radix ascii" "KEY" /testbench/tb_key
   add_wave_label "-radix ascii" "CODE" /testbench/tb_code

   add_divider "Vigenere Inner Signals"
   add_wave_label "" "state" /testbench/uut/state
   # sem doplnte vase vnitrni signaly. chcete-li v diagramu zobrazit desitkove
   # cislo, vlozte do prvnich uvozovek: -radix dec
	add_wave_label "-radix unsigned" "POSUV" /testbench/uut/posuvVelikost
	add_wave_label "" "TYP" /testbench/uut/posuvTyp
	add_wave_label "-radix ascii" "NAHORU" /testbench/uut/dataPosunutaNahoru
	add_wave_label "-radix ascii" "DOLU" /testbench/uut/dataPosunutaDolu
	
	add_wave_label "-radix ascii" "STAV" /testbench/uut/stavTypPosuvu
	add_wave_label "-radix ascii" "DALSI_STAV" /testbench/uut/dalsiStavTypPosuvu

   run 8 ns
}
