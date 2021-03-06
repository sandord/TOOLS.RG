**************************************************************************************
*	R_FADE_S.S
*
*	interrupt based fade rout
*
*	[c] 2001 Reservoir Gods
**************************************************************************************

**************************************************************************************
;	EXPORTS / IMPORTS
**************************************************************************************

	export	FontGrab_GetpFontBase


**************************************************************************************
;	STRUCTS
**************************************************************************************



**************************************************************************************
	TEXT
**************************************************************************************

*------------------------------------------------------------------------------------*
* FUNCTION : FontGrab_GetpFontBase
* ACTION   : gets pointer to font base
* CREATION : 11.04.01 PNK
*------------------------------------------------------------------------------------*

FontGrab_GetpFontBase:
	movem.l	d0-d7/a1-a6,-(a7)	; save registers


	dc.w	$a000				; 2	; init line a
	lea		-456(a0),a0			; pfont

	movem.l	(a7)+,d0-d7/a1-a6	; restore regs

	rts
