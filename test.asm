          .ORIG 0x3000
         AND R0, R0, #0  ; Clear R0 (set R0 to 0)
         ADD R0, R0, #5  ; R0 = 5
 AC      AND R1, R1, #-16  ; Clear R1 (set R1 to 0)
         ADD R1, R1, #7  ; R1 = 7
	 BRz AC
         .STRINGZ "abcdefghijklmnopqrstuvAZeaA1391wxyz"
         HALT            ; End program execution
         .END
