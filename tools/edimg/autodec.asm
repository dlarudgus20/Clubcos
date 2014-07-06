[BITS 32]
GLOBAL _autodecomp
[SECTION .text]
_autodecomp:
			PUSHFD	
			PUSHAD	
			CLD	
			mov	EDI,dword     [SS:ESP+48]
			mov	ESI,dword     [SS:ESP+44]
			cmp	EDI,17
			jb	.skip
			add	EDI,ESI
			mov	ECX,14
.LL0000:			
			mov	AL,[DS:ESI+ECX+1]
CMP AL,[DS:.sign+ECX]
			jne	.skip
			dec	ECX
			jge	.LL0000
.LL0001:			
			mov	AL,[DS:ESI]
			add	ESI,16
			cmp	AL,83H
			je	.pass
			cmp	AL,85H
			je	.pass
			cmp	AL,89H
			jne	.err
.pass:				
			call	near     getnum_s7s
			XCHG	EAX,EBP
			je	.decomped
			NEG	ESI
			LEA	ECX,[EDI+ESI]
			NEG	ESI
			mov	EDI,dword     [SS:ESP+40]
			LEA	EBX,[ECX+EBP]
			cmp	EBX,EDI
			ja	.err
			add	EDI,dword     [SS:ESP+44]
			PUSH	EDI
			sub	EDI,ECX
			PUSH	EDI
REP MOVSB
			POP	ESI
			POP	EDX
			mov	EDI,dword     [SS:ESP+44]
			call	near     getnum_s7s
			mov	CH,[DS:EDI]
			TEST	AL,01H
			jne	.LL0002
			cmp	CH,89H
			jne	.err
			dec	ESI
			jmp	.LL0003
.LL0002:			
			TEST	AL,20H
			jne	.err
			PUSH	EAX
			mov	CL,AL
			shr	CL,1
			xor	EAX,EAX
			and	CL,0fH
			inc	EAX
			add	CL,8
			SHL	EAX,CL
			CMP	EAX,EBP
			POP	EAX
			jae	.LL0004
			cmp	CL,8
			jne	.err
.LL0004:			
			TEST	AL,40H
			je	.LL0006
			call	near     getnum_s7s
.LL0006:			
.LL0003:			
			PUSH	EBP
			cmp	CH,89H
			jne	.LL0008
			call	near     lzrestore_tek5
			jmp	.LL0009
.LL0008:			
			cmp	CH,85H
			jne	.LL000A
			call	near     lzrestore_stk2
			jmp	.LL000B
.LL000A:			
			call	near     lzrestore_stk1
.LL000B:			
.LL0009:			
			TEST	AL,AL
			POP	EBP
			jne	.LL000C
.decomped:			
			mov	dword     [SS:ESP+28],EBP
			POPAD	
			POPFD	
			RETN	
.LL000C:			
.err:				
			POPAD	
			POPFD	
			or	EAX,-1
			RETN	
.skip:				
			POPAD	
			POPFD	
			mov	EAX,dword     [SS:ESP+12]
			RETN	
.sign:				
			DB	0ffH,0ffH,0ffH,01H,00H,00H,00H
			DB	4fH,53H,41H,53H,4bH,43H,4dH,50H
.local			equ	0
				
getnum_s7s:
			xor	EAX,EAX
.LL0000:			
			sal	EAX,8
			LODSB	
			shr	EAX,1
			jnc	.LL0000
.LL0001:			
			RETN	
.local			equ	0
				
lzrestore_tek5:
			LODSB	
			mov	AH,AL
			and	AL,0fH
			cmp	AL,01H
			je	.tek5_z0
			cmp	AL,05H
			je	.tek5_z1
			cmp	AL,09H
			je	.tek5_z2
.err:				
			xor	EAX,EAX
			inc	EAX
			RETN	
.tek5_z0:			
			shr	EAX,12
			mov	CL,9*5
			and	EAX,0fH
			je	.LL0000
			cmp	AL,3
			jae	.err
MOV AL,[CS:.prm_table-1+EAX]
			jmp	.LL0001
.LL0000:			
			LODSB	
.LL0001:			
			DIV	CL
			ROR	EAX,8
			mov	CL,9
			xor	AH,AH
			DIV	CL
			mov	CL,AL
			add	CL,AH
			mov	EBX,768
			SHL	EBX,CL
			add	EBX,1846
			mov	ECX,ESP
.LL0002:			
			PUSH	1024
			dec	EBX
			jne	.LL0002
.LL0003:			
			PUSH	EAX
			PUSH	-1
			PUSH	EDI
			PUSH	ESI
			PUSH	EDI
			PUSH	EBP
			mov	EDI,ECX
			mov	CL,AL
			mov	EBX,256
			add	CL,AH
			SHL	EBX,CL
			add	EBX,1846
			xor	EDX,EDX
			inc	EDX
			mov	CL,AL
			SHL	EDX,CL
			dec	EDX
			PUSH	EDX
			mov	AL,8
			sub	AL,AH
			PUSH	EAX
			ROL	EAX,8
			xor	EDX,EDX
			inc	EDX
			mov	CL,AL
			SHL	EDX,CL
			dec	EDX
			PUSH	EDX
			PUSH	0
			PUSH	1
			PUSH	-4
			PUSH	-3
			PUSH	-2
			PUSH	-1
			mov	EBP,ESP
			PUSH	EDI
			mov	CL,4
.LL0004:			
			sal	EAX,8
			LODSB	
			dec	CL
			jne	.LL0004
.LL0005:			
			mov	dword     [SS:EBP+44],ESI
			mov	dword     [SS:EBP+56],EAX
			mov	dword     [SS:EBP+1846*4+60],EBX
			call	near     decodemain
			POP	EAX
			XCHG	EAX,ESP
			xor	EAX,EAX
			RETN	
.prm_table:			
			DB	5dH,00H
.tek5_z1:			
.tek5_z2:			
			mov	BL,AL
			shr	EAX,12
			mov	CL,9*5
			and	EAX,0fH
			je	.LL0006
			cmp	AL,2
			jae	.err
MOV AL,[CS:.prm_table1-1+EAX]
			jmp	.LL0007
.LL0006:			
			LODSB	
.LL0007:			
			DIV	CL
			mov	BH,AL
			mov	AL,AH
			mov	CL,9
			xor	AH,AH
			DIV	CL
			mov	CL,BH
			add	CL,AH
			mov	EDX,768
			SHL	EDX,CL
			add	EDX,2064
			mov	ECX,ESP
.LL0008:			
			PUSH	8000H
			dec	EDX
			jne	.LL0008
.LL0009:			
			sub	ESP,128
			inc	EDX
			PUSH	EDI
			PUSH	EDI
			mov	EDI,ECX
			PUSH	EBP
			mov	CL,BH
			SHL	EDX,CL
			dec	EDX
			PUSH	EDX
			mov	EDX,00780008H
			mov	DH,AH
			sub	DL,AH
			PUSH	EDX
			or	EDX,-1
			mov	CL,AL
			SHL	EDX,CL
			NOT	EDX
			PUSH	EDX
			PUSH	0
			PUSH	1
			mov	DL,16
.LL000A:			
			PUSH	8
			dec	DL
			jne	.LL000A
.LL000B:			
			sub	ESP,1024
			PUSH	-4
			PUSH	-3
			PUSH	-2
			PUSH	-1
			mov	EDX,256
			mov	CL,AH
			add	CL,BH
			SHL	EDX,CL
			add	EDX,320+2064
			PUSH	EDX
			mov	BH,-1
			cmp	BL,09H
			jne	.LL000C
			LODSB	
			mov	BH,AL
.LL000C:			
			mov	CL,4
.LL000E:			
			sal	EAX,8
			LODSB	
			dec	CL
			jne	.LL000E
.LL000F:			
			PUSH	EAX
			PUSH	-1
			PUSH	ESI
			mov	EBP,ESP
			PUSH	EDI
			mov	EDI,32
.LL0010:			
			mov	dword     [SS:EBP+EDI+20],1
			mov	dword     [SS:EBP+EDI+24],16*1024
			cmp	EDI,160
			jae	.LL0012
			mov	dword     [SS:EBP+EDI+20],7fffffffH
			mov	dword     [SS:EBP+EDI+24],80000000H
.LL0012:			
			cmp	EDI,800
			jb	.LL0014
			mov	dword     [SS:EBP+EDI+24],8*1024
.LL0014:			
			mov	byte     [SS:EBP+EDI],5
			mov	byte     [SS:EBP+EDI+1],5
			mov	byte     [SS:EBP+EDI+2],0
			add	EDI,32
			cmp	EDI,1056
			jb	.LL0010
.LL0011:			
			mov	byte     [SS:EBP+64+1],3
			mov	byte     [SS:EBP+96],9
			mov	byte     [SS:EBP+96+1],2
			TEST	BH,40H
			je	.LL0016
			mov	byte     [SS:EBP+128],0
			mov	byte     [SS:EBP+128+1],1
			mov	dword     [SS:EBP+1536],0ffffH
.LL0016:			
			mov	byte     [SS:EBP+736],0
			mov	byte     [SS:EBP+736+1],1
			mov	dword     [SS:EBP+1540],0ffffH
			mov	AL,18
			mov	byte     [SS:EBP+1096],AL
			mov	byte     [SS:EBP+1100],AL
			mov	byte     [SS:EBP+1104],AL
			cmp	BL,05H
			jne	.LL0018
			mov	dword     [SS:EBP+736+20],7fffffffH
			mov	dword     [SS:EBP+736+24],80000000H
			jmp	.LL0019
.LL0018:			
			mov	EDI,32+1024
			mov	AL,8
.LL001A:			
			mov	byte     [SS:EBP+EDI],AL
			add	EDI,4
			inc	EAX
			cmp	AL,22
			jb	.LL001A
.LL001B:			
			mov	CL,BH
			mov	AL,7fH
			and	CL,03H
			SHR	AL,CL
			and	AL,78H
			mov	byte     [SS:EBP+1134],AL
			TEST	BH,04H
			jne	.LL001C
			dec	byte     [SS:EBP+1060]
.LL001C:			
			TEST	BH,08H
			jne	.LL001E
			dec	byte     [SS:EBP+1076]
.LL001E:			
			TEST	BH,10H
			jne	.LL0020
			dec	byte     [SS:EBP+1092]
.LL0020:			
			TEST	BH,20H
			jne	.LL0022
			dec	byte     [SS:EBP+1100]
.LL0022:			
.LL0019:			
			mov	EDI,32+1024
.LL0024:			
			mov	EAX,[SS:EBP+EDI]
			inc	EAX
			shl	EAX,5
			mov	[SS:EBP+EDI],EAX
			add	EDI,4
			cmp	EDI,1120
			jb	.LL0024
.LL0025:			
			mov	EDI,32
.LL0026:			
			call	near     setbm
			add	EDI,32
			cmp	EDI,1056
			jb	.LL0026
.LL0027:			
			call	near     decodemain1
			POP	EAX
			XCHG	EAX,ESP
			xor	EAX,EAX
			RETN	
.prm_table1:			
			DB	00H
.local			equ	0
				
rdd_direct:
.LL0000:			
			cmp	EDI,01000000H
			jb	.shift
.shift1:			
			shr	EDI,1
			CMP	EAX,EDI
			jb	.LL0002
			sub	EAX,EDI
.LL0002:			
			RCL	EDX,1
			dec	ECX
			jne	.LL0000
.LL0001:			
			RETN	
.shift:			
			shl	EAX,8
			shl	EDI,8
			LODSB	
			jmp	.shift1
.local			equ	0
				
rdd_bittree:
			mov	EDX,1
.local			equ	0
				
rdd_bittree0:
.LL0000:			
			PUSH	EBX
			add	EBX,EDX
			PUSH	EDX
			PUSH	ECX
			PUSH	EBX
			mov	EDX,dword     [SS:EBP+EBX*4+60]
			cmp	EDI,01000000H
			jb	.shift
.shift1:			
			mov	EBX,EDI
			mov	ECX,EDX
			shr	EBX,11
			IMUL	EBX,EDX
			cmp	EAX,EBX
			jae	.LL0002
			mov	EDI,EBX
			dec	EDX
			xor	EBX,EBX
			sub	ECX,0800H+1
.LL0002:			
			sub	EAX,EBX
			sub	EDI,EBX
			sar	ECX,5
			POP	EBX
			sub	EDX,ECX
			POP	ECX
			mov	dword     [SS:EBP+EBX*4+60],EDX
			POP	EDX
			POP	EBX
			RCL	EDX,1
			dec	CL
			jne	.LL0000
.LL0001:			
			RETN	
.shift:			
			shl	EAX,8
			shl	EDI,8
			LODSB	
			jmp	.shift1
.local			equ	0
				
getlen:
			PUSH	EDX
			call	near     rdd_bit
			jc	.LL0000
			POP	EDX
			mov	CL,3
			LEA	EBX,[EBX+EDX*8+2]
			call	near     rdd_bittree
			sub	EDX,8
			RETN	
.LL0000:			
			inc	EBX
			call	near     rdd_bit
			POP	EDX
			jc	.LL0002
			mov	CL,3
			LEA	EBX,[EBX+EDX*8+129]
			jmp	rdd_bittree
.LL0002:			
			add	EBX,258-1
			mov	CL,8
			call	near     rdd_bittree
			sub	EDX,248+256
			jbe	.LL0004
			mov	ECX,EDX
			mov	EDX,1
			call	near     rdd_direct
			LEA	ECX,[EDX-1]
			mov	EDX,1
			call	near     rdd_direct
			dec	EDX
.LL0004:			
			add	EDX,16+248
			RETN	
.local			equ	0
				
rdd_bit:
			PUSH	EBX
			mov	EDX,dword     [SS:EBP+EBX*4+60]
			cmp	EDI,01000000H
			jb	.shift
.shift1:			
			mov	EBX,EDI
			mov	ECX,EDX
			shr	EBX,11
			IMUL	EBX,EDX
			cmp	EAX,EBX
			jb	.LL0000
			sub	EAX,EBX
			sub	EDI,EBX
			sar	ECX,5
			POP	EBX
			sub	EDX,ECX
			mov	dword     [SS:EBP+EBX*4+60],EDX
			RETN	
.LL0000:			
			dec	EDX
			sub	ECX,0800H+1
			mov	EDI,EBX
			sar	ECX,5
			POP	EBX
			sub	EDX,ECX
			mov	dword     [SS:EBP+EBX*4+60],EDX
			RETN	
.shift:			
.LL0002:			
			shl	EAX,8
			shl	EDI,8
			LODSB	
			cmp	EDI,01000000H
			jb	.LL0002
.LL0003:			
			jmp	.shift1
.local			equ	0
				
decodemain:
			mov	ESI,dword     [SS:EBP+44]
			or	EDI,-1
			mov	EAX,dword     [SS:EBP+56]
			xor	EBX,EBX
			call	near     rdd_bit
			jc	.err
			mov	EBX,1846
			mov	CL,8
			call	near     rdd_bittree
			mov	EBX,dword     [SS:EBP+40]
			mov	[DS:EBX],DL
			inc	EBX
			mov	dword     [SS:EBP+40],EBX
			mov	EDX,dword     [SS:EBP+16]
			cmp	EDX,dword     [SS:EBP+36]
			jae	.LL0000
.loophead:			
			mov	EBX,dword     [SS:EBP+20]
			and	EDX,dword     [SS:EBP+24]
			sal	EBX,4
			PUSH	EDX
			add	EBX,EDX
			call	near     rdd_bit
			jc	.LL0002
			POP	EDX
			mov	EBX,dword     [SS:EBP+20]
			PUSH	EDI
			mov	EDX,dword     [SS:EBP+16]
			mov	EDI,dword     [SS:EBP+40]
MOV BL,[DS:.state_table+EBX]
			and	EDX,dword     [SS:EBP+32]
			mov	byte     [SS:EBP+20],BL
			mov	CL,byte     [SS:EBP+29]
			movzx	EBX,byte     [DS:EDI-1]
			SHL	EDX,CL
			mov	CL,byte     [SS:EBP+28]
			SHR	EBX,CL
			add	EBX,EDX
			shl	EBX,8
			POP	EDI
			add	EBX,1846
			mov	CL,8
			call	near     rdd_bittree
			mov	EBX,dword     [SS:EBP+40]
			mov	[DS:EBX],DL
			mov	EDX,dword     [SS:EBP+16]
			inc	EBX
			inc	EDX
			mov	dword     [SS:EBP+40],EBX
			mov	dword     [SS:EBP+16],EDX
			cmp	EDX,dword     [SS:EBP+36]
			jb	.loophead
			jmp	.fin
.LL0002:			
.lzphase:			
			mov	EBX,dword     [SS:EBP+20]
			add	EBX,192
			call	near     rdd_bit
			jnc	.LL0004
			add	EBX,204-192
			call	near     rdd_bit
			jc	.LL0006
			mov	EBX,dword     [SS:EBP+20]
			POP	EDX
			sal	EBX,4
			PUSH	EDX
			LEA	EBX,[EBX+EDX+240]
			call	near     rdd_bit
			jc	.LL0008
			POP	EDX
			mov	BL,byte     [SS:EBP+20]
			PUSH	EDI
			CMP	BL,7
			mov	EDI,dword     [SS:EBP+40]
			mov	EDX,dword     [SS:EBP+0]
			SBB	BL,BL
			PUSH	EAX
			and	BL,-2
			mov	AL,[DS:EDI+EDX]
			mov	EDX,dword     [SS:EBP+16]
			add	BL,11
			STOSB	
			mov	byte     [SS:EBP+20],BL
			inc	EDX
			mov	dword     [SS:EBP+40],EDI
			POP	EAX
			mov	dword     [SS:EBP+16],EDX
			POP	EDI
			cmp	EDX,dword     [SS:EBP+36]
			jb	.lzdone
			jmp	.fin
.LL0008:			
			jmp	.LL0007
.LL0006:			
			add	EBX,216-204
			call	near     rdd_bit
			mov	EDX,dword     [SS:EBP+4]
			jnc	.LL000A
			add	EBX,228-216
			call	near     rdd_bit
			mov	EDX,dword     [SS:EBP+8]
			jnc	.LL000C
			mov	EBX,EDX
			mov	EDX,dword     [SS:EBP+12]
			mov	dword     [SS:EBP+12],EBX
.LL000C:			
			mov	EBX,dword     [SS:EBP+4]
			mov	dword     [SS:EBP+8],EBX
.LL000A:			
			mov	EBX,dword     [SS:EBP+0]
			mov	dword     [SS:EBP+0],EDX
			mov	dword     [SS:EBP+4],EBX
.LL0007:			
			mov	BL,byte     [SS:EBP+20]
			POP	EDX
			CMP	BL,7
			SBB	BL,BL
			and	BL,-3
			add	BL,11
			mov	byte     [SS:EBP+20],BL
			mov	EBX,1332
			call	near     getlen
			mov	ECX,EDX
			mov	EDX,dword     [SS:EBP+0]
			jmp	.LL0005
.LL0004:			
			mov	EDX,dword     [SS:EBP+8]
			mov	EBX,dword     [SS:EBP+4]
			mov	dword     [SS:EBP+12],EDX
			mov	EDX,dword     [SS:EBP+0]
			mov	dword     [SS:EBP+8],EBX
			mov	dword     [SS:EBP+4],EDX
			mov	BL,byte     [SS:EBP+20]
			POP	EDX
			CMP	BL,7
			SBB	BL,BL
			and	BL,-3
			add	BL,10
			mov	byte     [SS:EBP+20],BL
			mov	EBX,818
			call	near     getlen
			mov	EBX,EDX
			PUSH	EDX
			cmp	EBX,3
			jb	.LL000E
			mov	EBX,3
.LL000E:			
			sal	EBX,6
			mov	CL,6
			add	EBX,432
			call	near     rdd_bittree
			and	EDX,3fH
			cmp	EDX,4
			jb	.LL0010
			mov	ECX,EDX
			mov	EBX,EDX
			shr	ECX,1
			mov	EDX,1
			dec	ECX
			RCL	EDX,1
			cmp	EBX,14
			jae	.LL0012
			PUSH	EDX
			SHL	EDX,CL
			NOT	EBX
			PUSH	ECX
			LEA	EBX,[EBX+EDX+688]
			call	near     rdd_bittree
			mov	EBX,EDX
			POP	ECX
			jmp	.LL0013
.LL0012:			
			sub	ECX,4
			call	near     rdd_direct
			mov	CL,4
			mov	EBX,802
			PUSH	EDX
			call	near     rdd_bittree
			mov	EBX,EDX
			mov	CL,4
.LL0013:			
			POP	EDX
.LL0014:			
			shr	EBX,1
			RCL	EDX,1
			dec	ECX
			jne	.LL0014
.LL0015:			
.LL0010:			
			NOT	EDX
			POP	ECX
			mov	dword     [SS:EBP+0],EDX
.LL0005:			
			PUSH	EDI
			add	ECX,2
			PUSH	ESI
			mov	EDI,dword     [SS:EBP+40]
			mov	ESI,dword     [SS:EBP+36]
			sub	ESI,dword     [SS:EBP+16]
			cmp	ECX,ESI
			jb	.LL0016
			mov	ECX,ESI
.LL0016:			
			LEA	ESI,[EDI+EDX]
			add	dword     [SS:EBP+16],ECX
			cmp	ESI,dword     [SS:EBP+48]
			jb	.err
rep movsb
			mov	dword     [SS:EBP+40],EDI
			POP	ESI
			mov	EDX,dword     [SS:EBP+16]
			POP	EDI
			cmp	EDX,dword     [SS:EBP+36]
			jae	.fin
.lzdone:			
			mov	EBX,dword     [SS:EBP+20]
			and	EDX,dword     [SS:EBP+24]
			sal	EBX,4
			PUSH	EDX
			add	EBX,EDX
			call	near     rdd_bit
			jc	.lzphase
			POP	EDX
			mov	EBX,dword     [SS:EBP+20]
			PUSH	EDI
MOV BL,[DS:.state_table+EBX]
			mov	EDX,dword     [SS:EBP+16]
			mov	byte     [SS:EBP+20],BL
			mov	EDI,dword     [SS:EBP+40]
			and	EDX,dword     [SS:EBP+32]
			mov	CL,byte     [SS:EBP+29]
			movzx	EBX,byte     [DS:EDI-1]
			SHL	EDX,CL
			mov	CL,byte     [SS:EBP+28]
			SHR	EBX,CL
			add	EBX,EDX
			mov	EDX,dword     [SS:EBP+0]
			shl	EBX,8
			mov	CL,[DS:EDI+EDX]
			POP	EDI
.lit1:				
			PUSH	EBX
			MOV	EDX,1
			add	EBX,EBX
			add	EBX,dword     [SS:EBP+1846*4+60]
.LL0018:			
			add	ECX,ECX
			PUSH	EBX
			add	EBX,EDX
			PUSH	EDX
			PUSH	ECX
			and	ECX,100H
			add	EBX,ECX
			PUSH	EBX
			mov	EDX,dword     [SS:EBP+EBX*4+60]
			cmp	EDI,01000000H
			jb	.lit1load0
.lit1load1:			
			mov	EBX,EDI
			mov	ECX,EDX
			shr	EBX,11
			IMUL	EBX,EDX
			cmp	EAX,EBX
			jae	.LL001A
			mov	EDI,EBX
			dec	EDX
			xor	EBX,EBX
			sub	ECX,0800H+1
.LL001A:			
			sub	EAX,EBX
			sub	EDI,EBX
			sar	ECX,5
			POP	EBX
			sub	EDX,ECX
			POP	ECX
			mov	dword     [SS:EBP+EBX*4+60],EDX
			POP	EDX
			POP	EBX
			RCL	EDX,1
			cmp	EDX,100H
			jae	.finlit1
			mov	DH,DL
			xor	DH,CH
			TEST	DH,1
			MOV	DH,0
			je	.LL0018
.LL0019:			
			mov	EBX,EDX
			xor	CL,CL
.LL001C:			
			add	EBX,EBX
			inc	CL
			cmp	EBX,100H
			jb	.LL001C
.LL001D:			
			POP	EBX
			add	EBX,1846
			call	near     rdd_bittree0
			jmp	.lit11
.LL0000:			
.lit1load0:			
			shl	EAX,8
			shl	EDI,8
			LODSB	
			jmp	.lit1load1
.finlit1:			
			POP	EBX
			jmp	.lit11
.lit11:			
			mov	EBX,dword     [SS:EBP+40]
			mov	[DS:EBX],DL
			mov	EDX,dword     [SS:EBP+16]
			inc	EBX
			inc	EDX
			mov	dword     [SS:EBP+40],EBX
			mov	dword     [SS:EBP+16],EDX
			cmp	EDX,dword     [SS:EBP+36]
			jb	.loophead
.fin:				
			RETN	
.err:				
			xor	EAX,EAX
			mov	ESP,[SS:EBP-4]
			inc	EAX
			RETN	
.state_table:			
			DB	0,0,0,0,1,2,3,4,5,6,4,5
.local			equ	0
				
rdd_direct1b:
			mov	EDX,1
.local			equ	0
				
rdd_direct1:
			mov	EDI,dword     [SS:EBP+4]
			mov	EAX,dword     [SS:EBP+8]
.LL0000:			
			cmp	EDI,01000000H
			jb	.shift
.shift1:			
			shr	EDI,1
			CMP	EAX,EDI
			jb	.LL0002
			sub	EAX,EDI
.LL0002:			
			RCL	EDX,1
			dec	ECX
			jne	.LL0000
.LL0001:			
			mov	dword     [SS:EBP+4],EDI
			mov	dword     [SS:EBP+8],EAX
			RETN	
.shift:			
			shl	EAX,8
			shl	EDI,8
			LODSB	
			cmp	EDI,01000000H
			jae	.shift1
			jmp	.shift
.local			equ	0
				
setbm:
			mov	CL,byte     [SS:EBP+EDI]
			or	EAX,-1
			SHL	EAX,CL
			mov	CL,byte     [SS:EBP+EDI+1]
			mov	EDX,EAX
			mov	dword     [SS:EBP+EDI+12],EAX
			SHL	EDX,CL
			mov	ECX,EAX
			NEG	EAX
			and	ECX,0ffffH
			or	EAX,EDX
			NOT	EDX
			and	EAX,ECX
			and	EDX,ECX
			mov	dword     [SS:EBP+EDI+8],EAX
			mov	dword     [SS:EBP+EDI+4],EDX
			sub	EAX,EDX
			mov	dword     [SS:EBP+EDI+28],EAX
			RETN	
.local			equ	0
				
rdd_probtree1:
			mov	EDX,1
.local			equ	0
				
rdd_probtree:
.LL0000:			
			PUSH	EBX
			add	EBX,EDX
			PUSH	EBX
			PUSH	EDX
			dec	dword     [SS:EBP+EDI+20]
			mov	EDX,[SS:EBP+EBX*4]
			jle	.bm_timeout0
.bm_timeout1:			
			mov	EBX,EDX
			sub	EDX,dword     [SS:EBP+EDI+4]
			and	EBX,dword     [SS:EBP+EDI+12]
			mov	EAX,dword     [SS:EBP+4]
			cmp	EDX,dword     [SS:EBP+EDI+28]
			ja	.fixprob0
.fixprob2:			
			cmp	EAX,01000000H
			jb	.shift
.shift1:			
			MUL	EBX
			mov	CL,byte     [SS:EBP+EDI+1]
			SHRD	EAX,EDX,16
			mov	EDX,EBX
			cmp	dword     [SS:EBP+8],EAX
			jae	.LL0002
			mov	dword     [SS:EBP+4],EAX
			add	EBX,dword     [SS:EBP+EDI+12]
			sub	EDX,10000H+1
			xor	EAX,EAX
.LL0002:			
			SAR	EDX,CL
			sub	dword     [SS:EBP+8],EAX
			and	EDX,dword     [SS:EBP+EDI+12]
			sub	dword     [SS:EBP+4],EAX
			sub	EBX,EDX
			POP	EDX
			POP	EAX
			RCL	EDX,1
			mov	[SS:EBP+EAX*4],EBX
			mov	AL,CH
			dec	CH
			shr	AL,4
			POP	EBX
			and	AL,CH
			CMP	AL,1
			SBB	EAX,EAX
			and	EAX,32
			add	EDI,EAX
			TEST	CH,07H
			jne	.LL0000
.LL0001:			
			TEST	EDX,EDX
			RETN	
.shift:			
			mov	EDX,EAX
			mov	EAX,dword     [SS:EBP+8]
.LL0004:			
			sal	EDX,8
			sal	EAX,8
			LODSB	
			cmp	EDX,01000000H
			jb	.LL0004
.LL0005:			
			mov	dword     [SS:EBP+8],EAX
			mov	dword     [SS:EBP+4],EDX
			mov	EAX,EDX
			jmp	.shift1
.fixprob0:			
			mov	EBX,dword     [SS:EBP+EDI+4]
			test	EDX,EDX
			jl	.LL0006
			add	EBX,dword     [SS:EBP+EDI+28]
.LL0006:			
			jmp	.fixprob2
.bm_timeout0:			
			mov	EBX,dword     [SS:EBP+EDI+24]
			test	EBX,EBX
			jl	.LL0008
			PUSH	EDX
			mov	dword     [SS:EBP+EDI+20],EBX
			PUSH	ECX
			mov	EBX,320+64
			PUSH	EDI
			xor	EDX,EDX
			mov	EDI,32+96
			mov	CH,71H
			call	near     rdd_probtree
			je	.err
			POP	EBX
			mov	EDI,EBX
			sub	EBX,32
			PUSH	EDI
			sar	EBX,4
			xor	EDX,EDX
			or	BL,byte     [SS:EBP+EDI+2]
			mov	EDI,32+32
			add	EBX,320+0
			mov	CH,71H
			call	near     rdd_probtree
			POP	EDI
			mov	byte     [SS:EBP+EDI+2],DL
			jne	.LL000A
			PUSH	EDI
			mov	EBX,320+1600-1
			mov	EDI,32+64
			mov	CH,74H
			call	near     rdd_probtree1
			POP	EDI
			and	DL,0fH
			mov	byte     [SS:EBP+EDI],DL
			cmp	DL,15
			je	.err
			PUSH	EDI
			mov	EBX,320+1616-1
			mov	EDI,32+64
			mov	CH,74H
			call	near     rdd_probtree1
			POP	EDI
			and	DL,0fH
			je	.err
			mov	byte     [SS:EBP+EDI+1],DL
			add	DL,byte     [SS:EBP+EDI]
			cmp	DL,15
			ja	.err
			call	near     setbm
.LL000A:			
			POP	ECX
			POP	EDX
			jmp	.bm_timeout1
.LL0008:			
			mov	dword     [SS:EBP+EDI+20],7fffffffH
			jmp	.bm_timeout1
.err:				
			xor	EAX,EAX
			mov	ESP,[SS:EBP-4]
			inc	EAX
			RETN	
.local			equ	0
				
rdd_probtree1_f:
			mov	EDX,1
.local			equ	0
				
rdd_probtree_f:
			PUSH	ESI
			PUSH	EBX
			mov	EAX,dword     [SS:EBP+EDI+28]
			mov	EBX,dword     [SS:EBP+EDI+4]
			PUSH	EAX
			PUSH	EBX
			movzx	EBX,CH
			mov	EAX,dword     [SS:EBP+EDI+20]
			mov	CL,byte     [SS:EBP+EDI+1]
			sub	EAX,EBX
			mov	ESI,dword     [SS:EBP+EDI+12]
			jle	.slow
			mov	dword     [SS:EBP+EDI+20],EAX
			mov	EDI,dword     [SS:EBP+8]
.LL0000:			
			mov	EBX,[SS:ESP+8]
			PUSH	EDX
			add	EBX,EDX
			mov	EAX,dword     [SS:EBP+4]
			PUSH	EBX
			mov	EDX,[SS:EBP+EBX*4]
			mov	EBX,EDX
			sub	EDX,[SS:ESP+8]
			and	EBX,ESI
			cmp	EDX,[SS:ESP+12]
			ja	.fixprob0
.fixprob2:			
			cmp	EAX,01000000H
			jb	.shift
.shift1:			
			MUL	EBX
			SHRD	EAX,EDX,16
			mov	EDX,EBX
			cmp	EDI,EAX
			jae	.LL0002
			mov	dword     [SS:EBP+4],EAX
			sub	EDX,10000H+1
			xor	EAX,EAX
			add	EBX,ESI
.LL0002:			
			SAR	EDX,CL
			sub	EDI,EAX
			and	EDX,ESI
			sub	dword     [SS:EBP+4],EAX
			POP	EAX
			sub	EBX,EDX
			POP	EDX
			mov	[SS:EBP+EAX*4],EBX
			RCL	EDX,1
			dec	CH
			jne	.LL0000
.LL0001:			
			mov	dword     [SS:EBP+8],EDI
			POP	EBX
			POP	ESI
			POP	EBX
			POP	ESI
			TEST	EDX,EDX
			RETN	
.shift:			
			PUSH	ESI
			mov	EDX,EAX
			mov	ESI,[SS:ESP+24]
			mov	EAX,EDI
.LL0004:			
			sal	EDX,8
			sal	EAX,8
			LODSB	
			cmp	EDX,01000000H
			jb	.LL0004
.LL0005:			
			mov	[SS:ESP+24],ESI
			mov	EDI,EAX
			mov	dword     [SS:EBP+4],EDX
			POP	ESI
			mov	EAX,EDX
			jmp	.shift1
.fixprob0:			
			mov	EBX,[SS:ESP+8]
			test	EDX,EDX
			jl	.fixprob2
			add	EBX,[SS:ESP+12]
			jmp	.fixprob2
.slow:				
			POP	EBX
			POP	ESI
			POP	EBX
			POP	ESI
			or	CH,70H
JMP rdd_probtree
.local			equ	0
				
getlen1:
			shl	EDX,3
			mov	EDI,dword     [SS:EBP+12+1056]
			PUSH	EDX
			mov	CH,01H
			xor	EDX,EDX
			call	near     rdd_probtree_f
			je	.LL0000
			POP	EDX
			mov	EDI,dword     [SS:EBP+16+1056]
			mov	CH,03H
			add	EBX,EDX
			call	near     rdd_probtree1_f
			sub	EDX,8
			RETN	
.LL0000:			
			mov	EDI,dword     [SS:EBP+12+1056]
			sub	EBX,-128
			mov	CH,01H
			xor	EDX,EDX
			call	near     rdd_probtree_f
			POP	EDX
			je	.LL0002
			mov	EDI,dword     [SS:EBP+20+1056]
			mov	CH,03H
			add	EBX,EDX
			jmp	rdd_probtree1_f
.LL0002:			
			mov	EDI,dword     [SS:EBP+24+1056]
			sub	EBX,-128
			mov	CH,08H
			call	near     rdd_probtree1_f
			sub	EDX,248+256
			jbe	.LL0004
			cmp	EDX,6
			jae	.LL0006
			mov	EDI,dword     [SS:EBP+28+1056]
			xor	EBX,EBX
			mov	CL,DL
			inc	EBX
			mov	CH,DL
			SHL	EBX,CL
			add	EBX,320+66-2
			call	near     rdd_probtree1_f
			jmp	.LL0007
.LL0006:			
			mov	ECX,EDX
			call	near     rdd_direct1b
.LL0007:			
			LEA	ECX,[EDX-1]
			call	near     rdd_direct1b
			dec	EDX
.LL0004:			
			add	EDX,16+248
			RETN	
.local			equ	0
				
decodemain1:
			mov	ESI,dword     [SS:EBP]
			mov	EDI,dword     [SS:EBP+0+1056]
			mov	EBX,320+1632
			mov	CH,01H
			xor	EDX,EDX
			call	near     rdd_probtree_f
			je	.err
			mov	EDI,32+768
			mov	EBX,320+2064
			mov	CH,byte     [SS:EBP+1134]
			call	near     rdd_probtree1
			mov	EDI,dword     [SS:EBP+1144]
			XCHG	EAX,EDX
			STOSB	
			mov	EDX,dword     [SS:EBP+1120]
			mov	dword     [SS:EBP+1144],EDI
			cmp	EDX,dword     [SS:EBP+1140]
			jae	.fin
.mainloop:			
			mov	EBX,dword     [SS:EBP+1124]
			mov	EDI,dword     [SS:EBP+4+1056]
			and	EDX,dword     [SS:EBP+1128]
			shl	EBX,4
			jne	.LL0000
			mov	EDI,dword     [SS:EBP+0+1056]
.LL0000:			
			PUSH	EDX
			LEA	EBX,[EBX+EDX+320+1632]
			mov	CH,01H
			xor	EDX,EDX
			call	near     rdd_probtree_f
			je	.LL0002
			POP	EDX
			mov	EBX,dword     [SS:EBP+1124]
			mov	EDX,dword     [SS:EBP+1120]
			mov	EDI,dword     [SS:EBP+1144]
MOV BL,[DS:.state_table+EBX]
			and	EDX,dword     [SS:EBP+1136]
			mov	byte     [SS:EBP+1124],BL
			mov	CL,byte     [SS:EBP+1133]
			movzx	EBX,byte     [DS:EDI-1]
			SHL	EDX,CL
			PUSH	EDI
			mov	CL,byte     [SS:EBP+1132]
			SHR	EBX,CL
			add	EBX,EDX
			mov	EDI,32+768
			shl	EBX,8
			mov	CH,byte     [SS:EBP+1134]
			add	EBX,320+2064
			cmp	CH,78H
			jne	.LL0004
			mov	CH,08H
			call	near     rdd_probtree1_f
			jmp	.LL0005
.LL0004:			
			cmp	CH,38H
			jne	.LL0006
			mov	CH,04H
			call	near     rdd_probtree1_f
			mov	CH,04H
			mov	EDI,32+800
			call	near     rdd_probtree_f
			jmp	.LL0007
.LL0006:			
			call	near     rdd_probtree1
.LL0007:			
.LL0005:			
.lit11:			
			POP	EDI
			mov	AL,DL
			mov	EDX,dword     [SS:EBP+1120]
			STOSB	
			inc	EDX
			mov	dword     [SS:EBP+1144],EDI
			mov	dword     [SS:EBP+1120],EDX
			cmp	EDX,dword     [SS:EBP+1140]
			jb	.mainloop
			jmp	.fin
.LL0002:			
.lzphase:			
			mov	EBX,dword     [SS:EBP+1124]
			mov	EDI,dword     [SS:EBP+52+1056]
			add	EBX,320+2016
			xor	EDX,EDX
			PUSH	EDI
			mov	CH,01H
			call	near     rdd_probtree_f
			POP	EDI
			jne	.LL0008
			add	EBX,12
			PUSH	EDI
			mov	CH,01H
			call	near     rdd_probtree_f
			POP	EDI
			je	.LL000A
			mov	EBX,dword     [SS:EBP+1124]
			POP	EDX
			shl	EBX,4
			PUSH	EDX
			LEA	EBX,[EBX+EDX+2144]
			xor	EDX,EDX
			PUSH	EDI
			mov	CH,01H
			call	near     rdd_probtree_f
			POP	EDI
			mov	ECX,dword     [SS:EBP+0+16]
			jne	.LL000C
			mov	AL,byte     [SS:EBP+1124]
			POP	EDX
			CMP	AL,7
			mov	EDX,dword     [SS:EBP+0+16]
			SBB	AL,AL
			and	AL,-2
			add	AL,11
			mov	EDX,ECX
			mov	byte     [SS:EBP+1124],AL
			or	ECX,-1
			jmp	.rep0l1skip
.LL000C:			
			jmp	.LL000B
.LL000A:			
			add	EBX,12
			xor	EDX,EDX
			PUSH	EDI
			mov	CH,01H
			call	near     rdd_probtree_f
			mov	ECX,dword     [SS:EBP+4+16]
			POP	EDI
			jne	.LL000E
			add	EBX,12
			mov	CH,01H
			call	near     rdd_probtree_f
			mov	ECX,dword     [SS:EBP+8+16]
			jne	.LL0010
			mov	EDI,32+704
			mov	EBX,320+65
			mov	CH,01H
			call	near     rdd_probtree_f
			mov	EAX,dword     [SS:EBP+8+16]
			mov	ECX,dword     [SS:EBP+12+16]
			mov	dword     [SS:EBP+12+16],EAX
.LL0010:			
			mov	EAX,dword     [SS:EBP+4+16]
			mov	dword     [SS:EBP+8+16],EAX
.LL000E:			
			mov	EDX,dword     [SS:EBP+0+16]
			mov	dword     [SS:EBP+0+16],ECX
			mov	dword     [SS:EBP+4+16],EDX
.LL000B:			
			mov	AL,byte     [SS:EBP+1124]
			CMP	AL,7
			POP	EDX
			SBB	AL,AL
			and	AL,-3
			PUSH	ECX
			add	AL,11
			mov	EBX,320+512
			mov	byte     [SS:EBP+1124],AL
			call	near     getlen1
			mov	ECX,EDX
			POP	EDX
			jmp	.LL0009
.LL0008:			
			mov	EAX,dword     [SS:EBP+8+16]
			mov	EDX,dword     [SS:EBP+4+16]
			mov	EBX,dword     [SS:EBP+0+16]
			mov	dword     [SS:EBP+12+16],EAX
			mov	dword     [SS:EBP+8+16],EDX
			mov	AL,byte     [SS:EBP+1124]
			mov	dword     [SS:EBP+4+16],EBX
			CMP	AL,7
			POP	EDX
			SBB	AL,AL
			and	AL,-3
			add	AL,10
			mov	EBX,320+1024
			mov	byte     [SS:EBP+1124],AL
			call	near     getlen1
			mov	EBX,EDX
			PUSH	EDX
			mov	EDI,dword     [SS:EBP+32+1056]
			cmp	EBX,3
			jb	.LL0012
			mov	EBX,3
			mov	EDI,dword     [SS:EBP+36+1056]
.LL0012:			
			shl	EBX,6
			mov	CH,06H
			add	EBX,320+256
			call	near     rdd_probtree1_f
			and	EDX,3fH
			cmp	EDX,4
			jl	.LL0014
			mov	ECX,EDX
			mov	EBX,EDX
			shr	ECX,1
			mov	EDX,1
			dec	ECX
			RCL	EDX,1
			cmp	EBX,14
			jae	.LL0016
			PUSH	EDX
			SHL	EDX,CL
			NOT	EBX
			PUSH	ECX
			mov	EDI,dword     [SS:EBP+40+1056]
			mov	CH,CL
			LEA	EBX,[EBX+EDX+448]
			cmp	CL,4
			jl	.LL0018
			mov	EDI,dword     [SS:EBP+44+1056]
.LL0018:			
			call	near     rdd_probtree1_f
			POP	ECX
			jmp	.LL0017
.LL0016:			
			sub	ECX,6
			jbe	.LL001A
			call	near     rdd_direct1
.LL001A:			
			mov	EDI,dword     [SS:EBP+48+1056]
			mov	CH,06H
			mov	EBX,320+1536
			PUSH	EDX
			call	near     rdd_probtree1_f
			mov	CL,6
.LL0017:			
			mov	EBX,EDX
			POP	EDX
.LL001C:			
			shr	EBX,1
			RCL	EDX,1
			dec	CL
			jne	.LL001C
.LL001D:			
.LL0014:			
			NOT	EDX
			POP	ECX
			mov	dword     [SS:EBP+0+16],EDX
.LL0009:			
.rep0l1skip:			
			PUSH	EDI
			add	ECX,2
			PUSH	ESI
			mov	EDI,dword     [SS:EBP+1144]
			mov	ESI,dword     [SS:EBP+1140]
			sub	ESI,dword     [SS:EBP+1120]
			cmp	ECX,ESI
			jb	.LL001E
			mov	ECX,ESI
.LL001E:			
			LEA	ESI,[EDI+EDX]
			add	dword     [SS:EBP+1120],ECX
			cmp	ESI,dword     [SS:EBP+1148]
			jb	.err
rep movsb
			mov	dword     [SS:EBP+1144],EDI
			POP	ESI
			mov	EDX,dword     [SS:EBP+1120]
			POP	EDI
			cmp	EDX,dword     [SS:EBP+1140]
			jae	.fin
.lzdone:			
			mov	EBX,dword     [SS:EBP+1124]
			mov	EDI,dword     [SS:EBP+4+1056]
			and	EDX,dword     [SS:EBP+1128]
			shl	EBX,4
			jne	.LL0020
			mov	EDI,dword     [SS:EBP+0+1056]
.LL0020:			
			PUSH	EDX
			LEA	EBX,[EBX+EDX+320+1632]
			mov	CH,01H
			xor	EDX,EDX
			call	near     rdd_probtree_f
			je	.lzphase
			POP	EDX
			mov	EBX,dword     [SS:EBP+1124]
			mov	EDX,dword     [SS:EBP+1120]
			mov	EDI,dword     [SS:EBP+1144]
MOV BL,[DS:.state_table+EBX]
			and	EDX,dword     [SS:EBP+1136]
			mov	byte     [SS:EBP+1124],BL
			mov	CL,byte     [SS:EBP+1133]
			movzx	EBX,byte     [DS:EDI-1]
			SHL	EDX,CL
			PUSH	EDI
			mov	CL,byte     [SS:EBP+1132]
			SHR	EBX,CL
			add	EBX,EDX
			mov	EDX,dword     [SS:EBP+0+16]
			shl	EBX,8
			mov	CL,[DS:EDI+EDX]
			PUSH	EBX
			xor	EDX,EDX
			add	EBX,EBX
			inc	EDX
			add	EBX,dword     [SS:EBP+12]
			mov	AL,byte     [SS:EBP+1134]
			mov	EDI,32+768
.LL0022:			
			add	ECX,ECX
			PUSH	EBX
			PUSH	ECX
			PUSH	EDI
			PUSH	EAX
			and	ECX,100H
			mov	EDI,dword     [SS:EBP+8+1056]
			add	EBX,ECX
			mov	CH,01H
			call	near     rdd_probtree_f
			POP	EAX
			POP	EDI
			mov	CL,AL
			dec	EAX
			shr	CL,4
			TEST	AL,CL
			jne	.LL0024
			add	EDI,32
.LL0024:			
			POP	ECX
			mov	AH,DL
			POP	EBX
			xor	AH,CH
			cmp	EDX,100H
			jge	.finlit1
			TEST	AH,1
			je	.LL0022
.LL0023:			
			mov	CH,AL
			POP	EBX
			add	EBX,320+2064
			cmp	AL,70H
			jle	.LL0026
			and	CH,0fH
			call	near     rdd_probtree_f
			jmp	.lit11
.LL0026:			
			call	near     rdd_probtree
			jmp	.lit11
.finlit1:			
			POP	EBX
			jmp	.lit11
.err:				
			xor	EAX,EAX
			mov	ESP,[SS:EBP-4]
			inc	EAX
.fin:				
			RETN	
.state_table:			
			DB	0,0,0,0,1,2,3,4,5,6,4,5
.local			equ	0
				
lzrestore_stk2:
			inc	ESI
			add	EBP,EDI
			xor	DH,DH
			PUSH	EDI
			PUSH	EBP
			PUSH	3
			PUSH	2
			PUSH	1
			PUSH	0
			mov	EBP,ESP
.LL0000:			
			xor	ECX,ECX
.LL0002:			
			inc	ECX
			cmp	ECX,17
			jae	.long_by0
			test	DH,DH
			jne	.LL0004
			mov	DH,8
			mov	DL,[DS:ESI]
			inc	ESI
.LL0004:			
			dec	DH
			shr	DL,1
			jnc	.LL0002
.LL0003:			
.long_by1:			
REP MOVSB
			cmp	EDI,[SS:EBP+16]
			jae	.LL0001
			xor	ECX,ECX
.LL0006:			
			inc	ECX
			cmp	ECX,17
			jae	.long_lz0
			test	DH,DH
			jne	.LL0008
			mov	DH,8
			mov	DL,[DS:ESI]
			inc	ESI
.LL0008:			
			dec	DH
			shr	DL,1
			jnc	.LL0006
.LL0007:			
.long_lz1:			
			mov	EBX,ECX
			PUSH	EDX
.LL000A:			
			movzx	EDX,byte     [DS:ESI]
			inc	ESI
			PUSH	EDX
			and	EDX,0fH
			shr	EDX,1
			jc	.LL000C
			movzx	EAX,byte     [DS:ESI]
			inc	ESI
			shr	EAX,1
			jnc	.long_ds0
.long_ds1:			
			LEA	EDX,[EDX+EAX*8+8]
.LL000C:			
			PUSH	EDI
			mov	ECX,[SS:EBP+8]
			mov	EAX,[SS:EBP+4]
			mov	EDI,[SS:EBP+0]
			sub	EDX,6
			jb	.repdis_hit
.repdis_hit3:			
			mov	[SS:EBP+12],ECX
.repdis_hit2:			
			mov	[SS:EBP+8],EAX
.repdis_hit1:			
			mov	[SS:EBP+4],EDI
.repdis_hit0:			
			POP	EDI
			mov	[SS:EBP+0],EDX
			POP	ECX
			NOT	EDX
			shr	ECX,4
			je	.long_cp0
			inc	ECX
.long_cp1:			
			PUSH	ESI
			LEA	EAX,[EDI+ECX]
			LEA	ESI,[EDI+EDX]
			cmp	EAX,[SS:EBP+16]
			ja	.fix_ECX
.fix_ECX1:			
			cmp	ESI,[SS:EBP+20]
			jb	.err
REP MOVSB
			POP	ESI
			dec	EBX
			jne	.LL000A
.LL000B:			
			POP	EDX
			cmp	EDI,[SS:EBP+16]
			jb	.LL0000
.LL0001:			
.fin:				
			add	ESP,24
			xor	EAX,EAX
			RETN	
.long_by0:			
			call	near     getnum_s7s
			add	ECX,EAX
			jmp	.long_by1
.long_lz0:			
			call	near     getnum_s7s
			add	ECX,EAX
			jmp	.long_lz1
.long_ds0:			
			PUSH	EDX
			dec	ESI
			call	near     getnum_s7
			POP	EDX
			jmp	.long_ds1
.long_cp0:			
			movzx	ECX,byte     [DS:ESI]
			inc	ESI
			add	ECX,17*2
			shr	ECX,1
			jc	.long_cp1
			PUSH	EDX
			dec	ESI
			call	near     getnum_s7
			POP	EDX
			LEA	ECX,[EAX+17]
			jmp	.long_cp1
.repdis_hit:			
			cmp	DL,-6
			je	.repdis0_hit
			cmp	DL,-5
			je	.repdis1_hit
			cmp	DL,-4
			je	.repdis2_hit
			cmp	DL,-2
			je	.rel0
			cmp	DL,-1
			je	.rel1
			mov	EDX,[SS:EBP+12]
			jmp	.repdis_hit3
.repdis0_hit:			
			mov	EDX,[SS:EBP+0]
			jmp	.repdis_hit0
.repdis1_hit:			
			mov	EDX,[SS:EBP+4]
			jmp	.repdis_hit1
.repdis2_hit:			
			mov	EDX,[SS:EBP+8]
			jmp	.repdis_hit2
.rel0:				
			mov	[SS:EBP+12],ECX
			mov	[SS:EBP+8],EAX
			call	near     getnum_s7
			LEA	EDX,[EDI+EAX+1]
			jmp	.repdis_hit1
.rel1:				
			mov	[SS:EBP+12],ECX
			mov	[SS:EBP+8],EAX
			call	near     getnum_s7
			NEG	EAX
			LEA	EDX,[EDI+EAX-1]
			jmp	.repdis_hit1
.fix_ECX:			
			mov	ECX,[SS:EBP+16]
			sub	ECX,EDI
			jmp	.fix_ECX1
.err:				
			add	ESP,28
			xor	EAX,EAX
			inc	EAX
			RETN	
.local			equ	0
				
getnum_s7:
			xor	EAX,EAX
			LODSB	
			shr	EAX,1
			jc	.LL0000
			xor	EDX,EDX
			xor	ECX,ECX
			inc	EDX
.LL0002:			
			sal	EDX,7
			sal	EAX,8
			add	ECX,EDX
			LODSB	
			shr	EAX,1
			jnc	.LL0002
.LL0003:			
			add	EAX,ECX
.LL0000:			
			RETN	
.local			equ	0
				
lzrestore_stk1:
			inc	ESI
			add	EBP,EDI
			xor	ECX,ECX
			PUSH	EDI
.LL0000:			
			mov	CL,byte     [DS:ESI]
			inc	ESI
			mov	EBX,ECX
			and	ECX,0fH
			je	.getlong_by
.getlong_by0:			
			shr	EBX,4
			je	.getlong_lz
.getlong_lz0:			
REP MOVSB
			cmp	EDI,EBP
			jae	.LL0001
.LL0002:			
			movzx	EDX,byte     [DS:ESI]
			inc	ESI
			mov	ECX,EDX
			and	EDX,0fH
			shr	EDX,1
			jc	.LL0004
.LL0006:			
			sal	EDX,8
			mov	DL,[DS:ESI]
			inc	ESI
			shr	EDX,1
			jnc	.LL0006
.LL0007:			
.LL0004:			
			sar	ECX,4
			je	.long_cp
.long_cp0:			
			inc	ECX
			NOT	EDX
			PUSH	ESI
			LEA	EAX,[EDI+ECX]
			LEA	ESI,[EDI+EDX]
			cmp	EAX,EBP
			ja	.fix_ECX
.fix_ECX1:			
			cmp	ESI,[SS:ESP+4]
			jb	.err
REP MOVSB
			POP	ESI
			dec	EBX
			jne	.LL0002
.LL0003:			
			cmp	EDI,EBP
			jb	.LL0000
.LL0001:			
.fin:				
			POP	ESI
			xor	EAX,EAX
			RETN	
.getlong_lz:			
			call	near     getnum_s7s
			XCHG	EAX,EBX
			jmp	.getlong_lz0
.getlong_by:			
			call	near     getnum_s7s
			XCHG	EAX,ECX
			jmp	.getlong_by0
.long_cp:			
			call	near     getnum_s7s
			XCHG	EAX,ECX
			jmp	.long_cp0
.fix_ECX:			
			mov	ECX,EBP
			sub	ECX,EDI
			jmp	.fix_ECX1
.err:				
			POP	ESI
			xor	EAX,EAX
			POP	ESI
			inc	EAX
			RETN	
.local			equ	0

