FUNC LABEL 0
	MOV STK 5 VAL 90
	MOV STK 6 VAL 80
	MOV STK 3 VAL 60
	REF STK 3 STK 5
	REF STK 6 STK 3
	REF PTR 3 PTR 6
	PRINT PTR 3
	RET