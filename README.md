# FiveInArow
This is a five_in_a_row project with ai(kuon) .</br>
The ai include kuon(クオン) and hitagi(ひたぎ):</br>
Each ai is written by C and compiled into dll (cdecl) individually .</br>
The android version is in [FiveInArow_Xrelated](https://github.com/YuriSizuku/FiveInArow_Xrelated).</br>

### Intruduction: 
* kuon:
	* クオン can solve the problem by gametree and α-β pruning.
	* クオン can be stochastic in some way.
 	* クオン is compiled by c.

* hitagi:
	* The project is compiled on entire c with extern dependence fann.
	* ひたぎ is based on ann network and reinformance learning(bp-td).
	* ひたぎ can be trained to be more intelligent.
	* ひたぎ can evaluate the chessboard in a dynamic way depends on trainning.
	* ひたぎ involved クオンsuch as gametree and α-β pruning.

### Structures:
>FiveInArow_ai\src  
>>Ai_hitagi.h  
>>Ai_kuon.h  
>>console_io.h  
>>stdFunc.h  
>>stdValue.h     //define data structures and macros  
>>Ai_hitagi.cpp     //ひたぎ，td-bp machine learning  
>>Ai_kuon.cpp     //クオン，static evaluation and game trees related  
>>console_io.cpp     //console show,save and load  
>>stdFunc.cpp     //playing chess functions  
>>WinConsoleMain.cpp     //game event callbacks
>  
>\FiveInArow_ai\externlib     //fann etc.  
>\FiveInArow_ai\FiveInArow_console     //win32console project  
>\FiveInArow_ai\FiveInArow_hitagi_dll     //ひたぎdll project  
>\FiveInArow_ai\FiveInArow_kuon_dll     //クオンdll project  
