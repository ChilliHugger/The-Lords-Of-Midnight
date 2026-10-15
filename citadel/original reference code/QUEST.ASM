;-------------------------------------------------------------------------
;			LORDS OF MIDNIGHT III
;
;			    THE CITADEL
;-------------------------------------------------------------------------

;-------------------------------------------------------------------------
; (C) James Shaw, March 1st 1995.
;-------------------------------------------------------------------------

	MODEL HUGE
	IDEAL
	JUMPS
	P386

	%NOINCL
	%NOSYMS
	%NOLIST

	INCLUDE	'EQUATES.ASM'
	INCLUDE	'STRUCTUR.ASM'
	INCLUDE 'REGION.EQU'
	INCLUDE 'QUEST.EQU'
	INCLUDE 'CHARPROP.EQU'

	%LIST

;-------------------------------------------------------------------------

	SEGMENT	DATA_SEG PARA PUBLIC USE32 'LOMDATA'

EXTRN	realm_data:BYTE			;this data is saved in the citadels file
EXTRN	region_capitals:BYTE
EXTRN	seize_character:BYTE

EXTRN	game_mins:WORD
EXTRN	watch_flag:WORD
EXTRN	map_regions:WORD
EXTRN	zmap_buffer:WORD
EXTRN	Boroth_dead:WORD
EXTRN	game_day_year:WORD

EXTRN	list_list:DWORD
EXTRN	cast_list:DWORD
EXTRN	citad0025:DWORD
EXTRN	charac0153:DWORD
EXTRN	charac0136:DWORD
EXTRN	charac0105:DWORD
EXTRN	charac0023:DWORD
EXTRN	charac0015:DWORD
EXTRN	charac0001:DWORD
EXTRN	place_list:DWORD
EXTRN	kingdom_adj:DWORD
EXTRN	citadel_list:DWORD
EXTRN	kingdom_list:DWORD
EXTRN	character_list:DWORD
EXTRN	character_addr:DWORD
EXTRN	controlled_addr:DWORD


Boroth		=	charac0015

purpose_list	dd	defend_homeland_rout
		dd	dark_fey_rout
		dd	random_wander_nothing
		dd	be_hostage_nothing

reaction_list	dd	return_home_rout
		dd	stand_firm_rout
		dd	take_back_stronghold_rout
		dd	attack_enemy_rout
		dd	help_neighbour_rout
		dd	retreat_rout
		dd	gather_strength_rout
		dd	counter_threat_rout
		dd	lend_service_rout

list_posn	dd	0

PUBLIC list_buffer,list_count
list_buffer_size	=	160	;big enough for all the blokes
list_count	dw	0
list_buffer	dd	list_buffer_size dup (-1)

PUBLIC temp_count,temp_buffer
temp_buffer_size	=	160	;BIGGER THAN REQUIRED. ;big enough for all the blokes in 1 realm
temp_count	dw	0
temp_buffer	dd	temp_buffer_size dup (-1)

PUBLIC last_result
last_result	dd	-1

last_game_day	dw	-1

attacker	dd	0
	EVEN

	ENDS

;-------------------------------------------------------------------------

	SEGMENT	_TEXT PARA PUBLIC USE32 'CODE'
	ASSUME	CS:_TEXT,DS:DATA_SEG,SS:NOTHING

;-------------------------------------------------------------------------

EXTRN	modrandn:NEAR
EXTRN	quest_clear:NEAR
EXTRN	init_journey:NEAR
EXTRN	map_position:NEAR
EXTRN	dark_fey_rout:NEAR
EXTRN	restock_citadels:NEAR
EXTRN	get_leader_pointer:NEAR
EXTRN	calc_combat_energy:NEAR

;-------------------------------------------------------------------------
PUBLIC reevaluate_quests
PROC reevaluate_quests NEAR

;	cmp	[game_mins],60
;	jae	@@x

;--- only goes off once a day JS 5/4/95 ---
	mov	ax,[game_day_year]
	cmp	ax,[last_game_day]
	je	@@x
	mov	[last_game_day],ax
;------------------------------------------

	call	restock_citadels

	call	setup_character_positions
	call	evaluate_realm_war_states

	mov	ebp,OFFSET character_list
@@1:
	mov	esi,[ebp]
	add	ebp,4
	or	esi,esi
	je	@@x

	add	esi,offset character_list

	movzx	eax,[WORD obj_type+esi]
;	mov	ebx,quest_flags
;	bt	ebx,eax
;	jnc	@@1				;only questers
	cmp	eax,typ_character
	jne	@@1

	test	[WORD esi+obj_flags2],no_process_bit
	jne	@@1
	cmp	esi,offset seize_character
	je	@@1

	cmp	esi,[controlled_addr]
	jne	@@2

    	test	[watch_flag],-1
	je	@@1
@@2:
	cmp	[WORD esi+obj_quest],_qrest
	jne	@@1

	test	[WORD esi+obj_char_flags],rescue_bit
	jne	@@1

	cmp	esi,offset Boroth
	je	@@1

	test	[Boroth_dead],-1
	je	@@c

	cmp	[BYTE esi+obj_start_realm],Marish
	je	@@1

@@c:
	movsx	eax,[WORD esi+obj_purpose]
	or	eax,eax
	js	@@1

	call	[eax*4+purpose_list]

	jmp	@@1
@@x:
	ret
ENDP
;-------------------------------------------------------------------------
PUBLIC defend_homeland_rout
DEFINE_PURPOSE defend_homeland_rout
	pushad
	xor	ebx,ebx
@@1:
	movsx	eax,[WORD obj_reaction+esi]
	or	eax,eax
	js	@@x

	inc	ebx
	cmp	ebx,14	;max 13 iterations
	jae	@@x

	push	ebx
	call	[reaction_list+eax*4]
	pop	ebx
	jc	@@1
@@x:
	popad
	ret
END_PURPOSE

;-------------------------------------------------------------------------
DEFINE_REACTION return_home_rout

;--- Take Back Stronghold
	HOMELAND LS_SUBJUGATED
	BIFF 1
		GO_STATE TAKE_BACK_STRONGHOLD
		BRA END

;--- Stand Firm
LAB 1
	AT_STRONGHOLD SELF
	BIFF 2
		GO_STATE STAND_FIRM
		BRA END
LAB 2
	CLEAR_AVAILABLE

	ARE_QUESTING
	NOTT
	BIFF END

	SET_QUEST QGOTO OWN_STRONGHOLD		;will always succeed
LAB END
END_REACTION

;-------------------------------------------------------------------------
DEFINE_REACTION stand_firm_rout

		IF	0
;--- Lend Service
	FELLOW_LORD_RECRUITING
	BIFF 1
	COWARDLY
	NOTT
	BIFF 1
	HOMELAND LS_UNDER_ATTACK
	NOTT
	BIFF 1
		GO_STATE LEND_SERVICE
		BRA END

		ENDIF

;--- Gather Strength
LAB 1
	HOMELAND LS_IN_PERIL
	BIFF 2
	FAR_SIGHTED
	BIFF 2
	COWARDLY
	NOTT
	BIFF 2
	SOMEONE_TO_RECRUIT
	BIFF 2
		GO_STATE GATHER_STRENGTH
		BRA END

;--- Attack Enemy
LAB 2
	HOMELAND LS_IN_PERIL
	BIFF 3
		BRAVE
		BIFF COND2
		CAUTIOUS
		NOTT
		BIFF COND2
			GO_STATE ATTACK_ENEMY
			BRA END
LAB COND2
		RECKLESS
		BIFF 3
		COWARDLY
		NOTT
		BIFF 3
			GO_STATE ATTACK_ENEMY
			BRA END

;--- Help Neighbour
LAB 3
	HOMELAND LS_IN_DANGER
	BIFF 4
	COWARDLY
	NOTT
	BIFF 4
	SELFISH
	NOTT
	BIFF 4

;		GEN_LIST LC_HELP_NEIGHBOUR	;can be expressed in JIM!

		GEN_LIST LC_NEARLANDS
		SUB_LIST SC_GTE_UNDER_ATTACK	;== SC_LTE_IN_PERIL!
		IS_EMPTY_LIST
		BIFT 4

		CONV_LIST CV_CIT_2_BLOKE
		SUB_LIST SC_NOT_IN_HOMELAND
		SUB_LIST SC_NOT_DEFENDING_HOMELAND
		SUB_LIST SC_RETREATING
		IS_EMPTY_LIST
		BIFT 4

			PICK_FROM_LIST
			SET_QUEST QJOIN LAST_PARAMETER 	;Kludge : set up the quest before we go in
							;usually this will be reevaluated in next routine
			GO_STATE HELP_NEIGHBOUR
			BRA END

;--- Counter Threat
LAB 4
	HOMELAND LS_TROUBLED
	BIFF 5
	ANY_NEARLAND LS_THREATENED
	BIFF 5
	COWARDLY
	NOTT
	BIFF 5
	SELFISH
	NOTT
	BIFF 5
		GO_STATE COUNTER_THREAT
		BRA END

;--- Retreat
LAB 5
	HOMELAND LS_UNDER_ATTACK
	BIFF 6
	COWARDLY
	BIFF COND3
	GET_ATTACKER
	BATTLE_ODDS 3 4
	BIFF COND3
		GO_STATE RETREAT
		BRA END
LAB COND3
	BRAVE
	NOTT
	BIFF 6
	GET_ATTACKER
	BATTLE_ODDS 3 1
	BIFF 6
		GO_STATE RETREAT
		BRA END

LAB 6
	HOMELAND LS_UNDER_ATTACK
	BIFF N1
	      	CLEAR_AVAILABLE
		BRA 7
LAB N1
     		SET_AVAILABLE
LAB 7

;--- Wait
	SET_QUEST QREST -1		;will always succeed
LAB END
END_REACTION

;-------------------------------------------------------------------------
DEFINE_REACTION take_back_stronghold_rout

;--- Stand Firm
LAB 1
	HOMELAND LS_SUBJUGATED
	NOTT
	BIFF 2
    		GO_STATE STAND_FIRM
		BRA END

;--- Retreat
	IN_HOMELAND SELF
	BIFF 1
	COWARDLY
	BIFF COND1
	GET_OWN_CIT_RULER
	BATTLE_ODDS 1 2
	BIFF COND1
		GO_STATE RETREAT
		BRA END
LAB COND1
	BRAVE
	NOTT
	GET_OWN_CIT_RULER
	BATTLE_ODDS 2 1
	BIFF 1
		GO_STATE RETREAT
		BRA END

;---
LAB 2
	CLEAR_AVAILABLE

	ARE_QUESTING
	NOTT
	BIFF END

		SET_QUEST QSEIZE OWN_STRONGHOLD		;will always succeed

LAB END	
END_REACTION

;-------------------------------------------------------------------------
DEFINE_REACTION attack_enemy_rout
		IF	0
;--- Lend Service
	FELLOW_LORD_RECRUITING
	BIFF 1
	COWARDLY
	NOTT
	BIFF 1
		GO_STATE LEND_SERVICE
		BRA END

		ENDIF

;--- Return Home
LAB 1
	IN_HOMELAND SELF
	NOTT
	BIFF COND2
	HOMELAND LS_UNDER_ATTACK
	BIFF COND2
		GO_STATE RETURN_HOME
		BRA END
LAB COND2
	IN_HOMELAND ENEMY
	NOTT
	BIFF COND3
	IN_NEARLAND ENEMY
	NOTT
	BIFF COND3
		GO_STATE RETURN_HOME
		BRA END
LAB COND3
	IN_HOMELAND SELF
	NOTT
	BIFF CONDN
	IN_NEARLAND SELF
	BIFF CONDN
		GO_STATE RETURN_HOME
		BRA END

LAB CONDN
	DEAD QUEST_SUBJECT
	BIFF 2
		GO_STATE RETURN_HOME
		BRA END

LAB 2
	SET_AVAILABLE

	ARE_QUESTING
	NOTT
	BIFF END

;--- Attack
	IN_SAMELAND ENEMY
	BIFF COND4
		SET_QUEST QKILL LAST_PARAMETER
		BRA END
LAB COND4
	IN_HOMELAND ENEMY
	BIFF COND5
		SET_QUEST QKILL LAST_PARAMETER
		BRA END
LAB COND5
	IN_ADJACENT_LAND ENEMY
	BIFF COND6
		SET_QUEST QKILL LAST_PARAMETER
		BRA END
LAB COND6
	IN_NEARLAND ENEMY
	BIFF 3
     		SET_QUEST QKILL LAST_PARAMETER
		BRA END

LAB 3
		SET_QUEST QREST -1		;default

LAB END
END_REACTION

;-------------------------------------------------------------------------
DEFINE_REACTION help_neighbour_rout
	HOMELAND LS_UNDER_ATTACK
	BIFT GO_HOME

	DEAD QUEST_SUBJECT
	BIFT GO_HOME

	GEN_LIST LC_QUEST_SUBJECT
	SUB_LIST SC_NOT_DEFENDING_HOMELAND
	IS_EMPTY_LIST
	BIFT GO_HOME

	GEN_LIST LC_QUEST_SUBJECT
	SUB_LIST SC_RETREATING
	IS_EMPTY_LIST
	BIFT GO_HOME

	HOMELAND LS_IN_PERIL
	BIFF 2
	GEN_LIST LC_QUEST_SUBJECT
	CONV_LIST CV_BLOKE_2_CIT
	SUB_LIST SC_NOT_IN_DANGER
	IS_EMPTY_LIST
	BIFF GO_HOME
	BRA 2

LAB GO_HOME
		GO_STATE RETURN_HOME
		BRA END
LAB 2
	CLEAR_AVAILABLE

	ARE_QUESTING
	NOTT
	BIFF END

	GEN_LIST LC_QUEST_SUBJECT
	IS_EMPTY_LIST
	BIFT 3
		PICK_FROM_LIST
		SET_QUEST QJOIN LAST_PARAMETER
		BRA END
LAB 3
     		SET_QUEST QREST -1
		BRA END
LAB END

END_REACTION

;-------------------------------------------------------------------------
DEFINE_REACTION retreat_rout

		IF	0
;--- Lend Service
	FELLOW_LORD_RECRUITING
	BIFF 2
	COWARDLY
	NOTT
	BIFF 2
		GO_STATE LEND_SERVICE
		BRA END

		ENDIF

;--- Return Home
LAB 2
	STATE MY_HOMELAND		;state returned in eax
	IS_LT LS_UNDER_ATTACK
	BIFF 3
		GO_STATE RETURN_HOME
		BRA END

;--- Goto
LAB 3
	SET_AVAILABLE

	ARE_QUESTING
	NOTT
	BIFF END

	GEN_LIST LC_NEARLANDS
	SUB_LIST SC_GTE_UNDER_ATTACK
	IS_EMPTY_LIST
	BIFT COND2
		PICK_FROM_LIST
		SET_QUEST QGOTO LAST_PARAMETER
		BRA END
LAB COND2
	GEN_LIST LC_HOME_REALM_LANDS
	SUB_LIST SC_GTE_UNDER_ATTACK
	IS_EMPTY_LIST
	BIFT COND3
		PICK_FROM_LIST
		SET_QUEST QGOTO LAST_PARAMETER
		BRA END
LAB COND3
	GEN_LIST LC_LANDS_IN_NEIGHBOUR_REALMS_TO_ME
	SUB_LIST SC_NOT_AT_PEACE
	IS_EMPTY_LIST
	BIFT COND4
		PICK_FROM_LIST
		SET_QUEST QGOTO LAST_PARAMETER
		BRA END
LAB COND4
	GEN_LIST LC_ALL_LANDS
	SUB_LIST SC_NOT_AT_PEACE
	IS_EMPTY_LIST
	BIFT 4
		PICK_FROM_LIST
		SET_QUEST QGOTO LAST_PARAMETER
		BRA END

LAB 4
		SET_QUEST QREST -1

LAB END	
END_REACTION

;-------------------------------------------------------------------------
DEFINE_REACTION gather_strength_rout

;--- Return Home
	HOMELAND LS_UNDER_ATTACK
	BIFT GO_HOME
	SOMEONE_TO_RECRUIT
	BIFF GO_HOME
	BRA 1
LAB GO_HOME
		GO_STATE RETURN_HOME
		BRA END

;--- Attack Enemy
LAB 1
	IN_SAMELAND ENEMY
	BIFF 2
		GO_STATE ATTACK_ENEMY
		BRA END

;--- Recruit
LAB 2
	CLEAR_AVAILABLE

	ARE_QUESTING
	NOTT
	BIFF 3

	GEN_LIST LC_NEIGHBOURS
	SUB_LIST SC_COWARDLY
	SUB_LIST SC_NOT_AVAILABLE
	SUB_LIST SC_NOT_IN_NEARLAND
	IS_EMPTY_LIST
	BIFT 3
     		PICK_FROM_LIST
		SET_QUEST QRECRUIT LAST_PARAMETER
		BRA END

LAB 3
		SET_QUEST QREST -1
LAB END
END_REACTION

;-------------------------------------------------------------------------
DEFINE_REACTION counter_threat_rout

;--- Return Home
	STATE MY_HOMELAND	;state returned in eax
	IS_GT LS_TROUBLED
	BIFF 1
     		GO_STATE RETURN_HOME
		BRA END

;--- Stay or Goto
LAB 1
	CLEAR_AVAILABLE

	GEN_LIST LC_NEARLANDS
	SUB_LIST SC_NOT_THREATENED
	IS_EMPTY_LIST
	NOTT			;;this is a Kludge to get carry 
	BIFF 2			;;clear before exiting
	AT_STRONGHOLD LIST
	BIFF COND2
		SET_QUEST QREST -1
		BRA END
LAB COND2
	
		ARE_QUESTING
		NOTT
		BIFF END

		PICK_FROM_LIST
		SET_QUEST QGOTO LAST_PARAMETER
		BRA END

LAB 2
	FALSE			;;This ensures the Kludge above
				;;is wholly unnecessary
LAB END
END_REACTION

;-------------------------------------------------------------------------
DEFINE_REACTION lend_service_rout
	HOMELAND LS_UNDER_ATTACK
	BIFF COND2
	 	GO_STATE RETURN_HOME
		BRA END
LAB COND2
	DEAD LEADER
	BIFF 2
      		GO_STATE RETURN_HOME
		BRA END
LAB 2
	CLEAR_AVAILABLE
       	FALSE			;;This is a Kludge similar to
				;;that above.
LAB END
END_REACTION

;-------------------------------------------------------------------------
; Proper functions
;-------------------------------------------------------------------------

;fill in character struture with correct map position values
PROC setup_character_positions NEAR
	mov	edi,offset charac0001
	mov	ecx,num_characters+24
@@l:
	mov	eax,[obj_x+edi]
	mov	ebx,[obj_y+edi]
	shr	eax,16+9
	shr	ebx,16+9
	sub	ebx,127
	neg	ebx
	shl	ebx,7
	add	ebx,eax
	mov	dx,[map_regions+ebx*2]
	mov	[edi+obj_curr_land],dh
	and	dl,0fh
	mov	[edi+obj_curr_realm],dl

	add	edi,character_size
	loop	@@l
	ret
ENDP

;fill in realm data with correct values
PUBLIC evaluate_realm_war_states
PROC evaluate_realm_war_states NEAR
	pushad
	call	evaluate_realms
	call	evaluate_lands
	popad
	ret
ENDP

PROC evaluate_realms NEAR
;-----------------
;make a pseudo character to set
;up list generation parameters

	mov	esi,offset charac0001
	mov	edi,offset zmap_buffer
	mov	ecx,character_size
	rep	movsb

;-----------------
	mov	ecx,16
	xor	eax,eax
	mov	edi,offset realm_data
	mov	esi,offset zmap_buffer	;temp char pointer!
@@l:

	cmp	ax,14		;Great Ocean
	je	@@no_process

;-----------------
	mov	[esi+obj_start_realm],al		;fudge realm#
	movzx	edx,[BYTE region_capitals+eax]
	mov	edx,[citadel_list+edx*4]
	add	edx,offset citadel_list
	mov	dl,[edx+obj_port]
	mov	[esi+obj_start_land],dl

;-----------------
;evaluate realm_war_state
	mov	[BYTE edi+realm_war_state],RW_SUBJUGATED
	pushad
	GEN_LIST LC_HOME_REALM_LANDS
	popad

	push	ecx
	movzx	ecx,[WORD list_count]
	mov	edx,offset list_buffer
@@l3:
	mov	ebp,[edx]
	call	is_subjugated
	jnc	@@nsubj
	add	edx,4
	loop	@@l3

	pop	ecx
	jmp	@@e

@@nsubj:
	pop	ecx

	mov	[BYTE edi+realm_war_state],RW_AT_PEACE
	pushad
	call	realmland_check
	popad
	IS_EMPTY_LIST
	BIFT END

	mov	[BYTE edi+realm_war_state],RW_UNDER_ATTACK
	test	[WORD realm_enemies+edi],-1
	jne	@@e
	
	mov	[BYTE edi+realm_war_state],RW_THREATENED
	
LAB END
@@e:
;-----------------
;evaluate realm_state
	mov	[BYTE realm_state+edi],RS_INTERREGNUM
	movzx	ebx,[WORD edi+realm_owner]
	mov	ebx,[character_list+ebx*4]
	test	[WORD ebx+character_list+obj_char_flags],dead_bit
	jne	@@f

	mov	[BYTE realm_state+edi],RS_BLOOD_FEUD
	movzx	ebx,[WORD realm_hostage+edi]
	mov	ebx,[character_list+ebx*4]
	test	[WORD ebx+character_list+obj_char_flags],dead_bit
	jne	@@f
	
	mov	[BYTE realm_state+edi],RS_AT_RANSOM
	test	[WORD ebx+character_list+obj_char_flags],rescue_bit
	jne	@@f

	mov	[BYTE realm_state+edi],RS_UNFETTERED
@@f:	
;-----------------
@@no_process:
	add	edi,realm_SIZEOF
	inc	eax
	loop	@@l
;-----------------
	ret
ENDP

;ebp - land ptr (is land subjugated)
PROC is_subjugated NEAR
	push	ebx
	movzx	ebx,[WORD obj_land_owner+ebp]
	cmp	[obj_land_ruler+ebp],bx
	je	@@not
	mov	ebx,[character_list+ebx*4]
	add	ebx,offset character_list
	xchg	ebx,ebp
	call	is_enemy
	pop	ebx
	ret
@@not:
	pop	ebx
	clc
	ret
ENDP

PROC evaluate_lands NEAR
;-----------------
;make a pseudo character to set
;up list generation parameters

	mov	esi,offset charac0001
	mov	edi,offset zmap_buffer
	mov	ecx,character_size
	rep	movsb

;-----------------
	mov	ecx,130-25	;130 - 25 citadels
	mov	eax,25			;Immiel cit list #
	mov	edi,offset citad0025	;Immiel
	mov	esi,offset zmap_buffer
@@l:
;-----------------
	mov	[esi+obj_start_land],al	;fudge realm#
	mov	dl,[obj_land_realm+edi]
	mov	[esi+obj_start_realm],dl

;-----------------
;evaluate citadel state

	mov	[BYTE obj_land_war_state+edi],LS_SUBJUGATED
	mov	ebp,eax
	mov	ebp,[citadel_list+ebp*4]
	add	ebp,offset citadel_list
	call	is_subjugated
	jc	@@e

	movzx	ebx,[BYTE edi+obj_land_realm]
	region_mul	ebx
	add	ebx,offset realm_data

;----------------
	cmp	[BYTE realm_war_state+ebx],RW_UNDER_ATTACK
	jne	@@not_under_attack

	mov	[BYTE obj_land_war_state+edi],LS_UNDER_ATTACK
	pushad
	IN_HOMELAND ENEMY
	popad
	BIFT END

	mov	[BYTE obj_land_war_state+edi],LS_IN_PERIL
	pushad
	IN_NEARLAND ENEMY
	popad
	BIFT END

	mov	[BYTE obj_land_war_state+edi],LS_IN_DANGER
	jmp	@@e

;----------------
@@not_under_attack:
	movzx	ebx,[BYTE edi+obj_land_realm]
	region_mul	ebx
	add	ebx,offset realm_data
	cmp	[BYTE realm_war_state+ebx],RW_THREATENED
	jne	@@not_troubled

	mov	[BYTE obj_land_war_state+edi],LS_MENACED
	pushad
	IN_HOMELAND ENEMY
	popad
	BIFT END

	mov	[BYTE obj_land_war_state+edi],LS_THREATENED
	pushad
	IN_NEARLAND ENEMY
	popad
	BIFT END

	mov	[BYTE obj_land_war_state+edi],LS_TROUBLED
	jmp	@@e

@@not_troubled:
	mov	[BYTE obj_land_war_state+edi],LS_AT_PEACE
	
LAB END
@@e:
;-----------------
	add	edi,citadel_size
	inc	eax
	loop	@@l	
;-----------------

	ret
ENDP

PROC get_map_position NEAR
;	mov	eax,[obj_x+esi]
;	mov	ebx,[obj_y+esi]
;	shr	eax,16
;	shr	ebx,16
;	call	map_position
;use the stored versions
	movzx	ecx,[BYTE esi+obj_curr_land]
	movzx	eax,[BYTE esi+obj_curr_realm]
	ret
ENDP

;IN  edx - citadel ptr
;OUT ebx - realm data ptr
PROC get_orig_cit_owner_ptr NEAR
	movzx	ebx,[BYTE obj_land_realm+edx]
	region_mul	ebx
	add	ebx,offset realm_data
	ret
ENDP
;-------------------------------------------------------------------------
; Macro callable functions
;-------------------------------------------------------------------------
;eax - who : SELF
PROC check_at_stronghold NEAR

	cmp	eax,SELF
	jne	@@1

	stc
	ret

	test	[WORD esi+obj_flags],inside_bit
	je	@@x

	mov	eax,[obj_x+esi]
	mov	ebx,[obj_y+esi]
	shr	eax,16
	shr	ebx,16

	movzx	ecx,[BYTE obj_start_land+esi]
	mov	ecx,[citadel_list+ecx*4]
	mov	edx,[citadel_list+ecx+obj_y]
	mov	ecx,[citadel_list+ecx+obj_x]
	shr	ecx,16
	shr	edx,16

	cmp	eax,ecx
	jne	@@x

	cmp	ebx,edx
	jne	@@x

	stc
	ret
@@x:
	clc
	ret
@@1:
	cmp	eax,LIST
@@err2:
	jne	@@err2
;search a land list for me being at citadel
;don't change list!

	movzx	ecx,[WORD list_count]
	jecxz	@@err
	mov	ebp,offset list_buffer
@@l:
	mov	edx,[ebp]
	mov	al,[edx+obj_port]
	cmp	al,[esi+obj_curr_land]
	je	@@yy
	add	ebp,4
	loop	@@l
	clc
	ret
@@yy:
	stc
	ret

@@err:
	jmp	@@err
ENDP

;eax - ratio1
;edx - ratio2
;edi - attacker
;esi - me
;check battle odds between me and home.
PROC check_battle_odds NEAR
	call	calc_combat_energy
	push	esi
	mov	esi,edi
	call	calc_combat_energy
	pop	esi

	mov	ecx,[esi+obj_strength]	;A
	mov	ebx,[edi+obj_strength]	;B

	imul	ecx,edx			;A*odds B
	imul	ebx,eax			;B*odds A

	cmp	ebx,ecx			;if odds B > odds A
	ja	@@toohigh			;odds are too high
	stc
	ret

@@toohigh:
	clc
	ret
ENDP

;return ruler of esi's citadel in edi
PROC get_cit_ruler NEAR
	movzx	edi,[BYTE esi+obj_start_land]
	mov	edi,[citadel_list+edi*4]
	movzx	edi,[WORD edi+obj_land_ruler]
	mov	edi,[cast_list+edi*4]
	ret
ENDP

;eax - who : ENEMY, SELF, NEIGHBOUR, LEADER
PROC is_dead NEAR
	cmp	eax,LEADER
	jne	@@1

	call	get_leader_pointer
	test	[WORD edi+obj_char_flags],dead_bit
	ZtoC
	cmc
	ret
@@1:
	cmp	eax,QUEST_SUBJECT
	jne	@@2

	call	fetch_quest_subject
	cmp	edi,-1
	je	@@f
	test	[WORD edi+obj_char_flags],dead_bit
	ZtoC
	cmc
	ret
@@f:
	clc
	ret
@@2:
	cmp	eax,NEIGHBOUR
@@x:
	jmp	@@x
	ret
ENDP

;check if fellow lord out recruiting (me?)
PROC check_fellow_lord_quest NEAR
	clc
	ret
ENDP

;eax - type
;edx - where
PUBLIC setup_quest_data
PROC setup_quest_data NEAR
	call	quest_clear
	call	[cs:quest_setup_routs+eax*4]
	clc
	ret
ENDP

quest_setup_routs	dd	setup_recruit
			dd	setup_join
			dd	setup_kill
			dd	-1
			dd	-1
			dd	setup_goto
			dd	-1
			dd	setup_seize
			dd	-1
			dd	-1
			dd	-1
			dd	setup_rest
			dd	-1
			dd	-1

PROC setup_recruit NEAR
	mov	edi,[last_result]
	cmp	edi,-1
@@cc:
	je	@@cc
	mov	[WORD esi+obj_quest],_qrecruit
	call	bung_quest_info_bloke
	ret
ENDP

PROC setup_join NEAR
	mov	edi,[last_result]
	cmp	edi,-1
@@cc:
	je	@@cc
	mov	[WORD esi+obj_quest],_qjoin
	call	bung_quest_info_bloke
	ret
ENDP

PUBLIC setup_kill
PROC setup_kill NEAR
	mov	edi,[last_result]
	cmp	edi,-1
@@cc:
	je	@@cc
	mov	[WORD esi+obj_quest],_qkill
	call	bung_quest_info_bloke
	ret
ENDP

PROC setup_seize NEAR
    	movzx	edi,[BYTE esi+obj_start_land]
	mov	edi,[edi*4+citadel_list]
	add	edi,offset citadel_list
	mov	[WORD esi+obj_quest],_qseize
	call	bung_quest_info_place
	ret
ENDP

PROC setup_rest NEAR
	mov	[WORD obj_quest+esi],_qrest
	ret
ENDP

PROC setup_goto NEAR
	mov	edi,[last_result]

	cmp	edx,LAST_PARAMETER
	je	@@c

    	movzx	edi,[BYTE esi+obj_start_land]
	mov	edi,[edi*4+citadel_list]
	add	edi,offset citadel_list
@@c:
	mov	[WORD esi+obj_quest],_qgoto
	cmp	edi,-1
@@cc:
	je	@@cc
	call	bung_quest_info_place
	ret
ENDP

PROC bung_quest_info_bloke NEAR
	mov	[WORD esi+obj_qdat1],_cast
	mov	[WORD esi+obj_guide_type],0
	call	bung_general
	ret
ENDP

PROC bung_quest_info_place NEAR
	mov	[WORD esi+obj_qdat1],_location
	call	bung_general

		IF	0
	mov	eax,[obj_x+edi]
	mov	ebx,[obj_y+edi]
	shr	eax,16
	shr	ebx,16
	mov	[character_addr],esi
	call	init_journey
		ENDIF

	mov	[WORD esi+obj_guide_type],0
	ret
ENDP

PROC bung_general NEAR
	mov	ax,[edi+obj_num]
	mov	[WORD esi+obj_qdat2],ax
	mov	[WORD esi+obj_qdat3],0
	mov	[WORD esi+obj_qdat4],0
	mov	[BYTE esi+obj_qtype1],qt_char
	mov	[BYTE esi+obj_qtype2],qt_not_defined
	mov	[WORD esi+obj_qstate],0

	or	[WORD esi+obj_flags2],autoq_bit		;automatic quest

	ret
ENDP

;-------------------------------------------------------------------------
PROC fetch_quest_subject NEAR
	movzx	edx,[WORD esi+obj_quest]
	and	edx,07fffh
	cmp	edx,_qrest
	je	@@x

    	movzx	eax,[WORD obj_qdat1+esi]
	movzx	ebx,[WORD obj_qdat2+esi]
	cmp	[BYTE obj_qtype1+esi],qt_char
	je	@@c
    	movzx	eax,[WORD obj_qdat3+esi]
	movzx	ebx,[WORD obj_qdat4+esi]
	cmp	[BYTE obj_qtype2+esi],qt_char
	jne	@@x
@@c:
	mov	edi,[list_list+eax*4]
	mov	edi,[edi+ebx*4]
	ret
@@x:
	mov	edi,-1
	ret
ENDP

;-------------------------------------------------------------------------

;in own homeland (who) ?
;eax - who (SELF or ENEMY)
PROC homeland_check NEAR
	cmp	eax,SELF
	jne	@@1

	call	get_map_position
	cmp	[esi+obj_start_land],cl
	ZtoC
	ret
@@1:
	cmp	eax,ENEMY
	jne	@@err

	mov	edx,offset home_scan
	call	scan_blokes		;cf set if any found & [last_result] is valid
	mov	edx,[last_result]
	mov	[attacker],edx
	ret

@@err:
	jmp	@@err
ENDP

;in own nearland (who)
;eax - who (SELF or ENEMY)
PROC nearland_check NEAR
	cmp	eax,SELF
	jne	@@1

	GEN_LIST LC_NEARLANDS
	call	get_map_position
	mov	eax,ecx
	movzx	ecx,[list_count]
	jecxz	@@n
	mov	ebp,offset list_buffer
@@l:
	mov	edx,[ebp]
	cmp	[edx+obj_port],al	;does cit region = my region?
	je	@@y
	add	ebp,4
	loop	@@l
@@n:
	clc
	ret
@@y:
	mov	[last_result],edx	;setup parameter
	stc
	ret
@@1:
	cmp	eax,ENEMY
	jne	@@err

	GEN_LIST LC_NEARLANDS
	mov	edx,offset enemy_scan
	call	scan_blokes
	jnc	@@x
	PICK_FROM_LIST
@@x:
	ret
@@err:
	jmp	@@err
ENDP

;in own home realm land (who)
PUBLIC realmland_check
PROC realmland_check NEAR
	GEN_LIST LC_HOME_REALM_LANDS
	IS_EMPTY_LIST
	BIFT END
	mov	edx,offset enemy_scan
	call	scan_blokes
	jnc	@@x
	PICK_FROM_LIST
LAB END
@@x:
	ret
@@err:
	jmp	@@err
ENDP

;in own nearland and adjacent to me (who)
;eax - who to check for (ENEMY only)
PROC adjacent_check NEAR
	cmp	eax,ENEMY
	jne	@@err
	GEN_LIST LC_ADJACENT_NEIGHBOURS
	IS_EMPTY_LIST
	BIFT NONE
	mov	edx,offset enemy_scan
	call	scan_blokes
	jnc	@@x
    	PICK_FROM_LIST
@@x:
	ret

LAB NONE
	FALSE
	ret
@@err:
	jmp	@@err
ENDP

;eax - who (ENEMY only)
PROC sameland_check NEAR
	cmp	eax,ENEMY
	jne	@@x

	movzx	eax,[BYTE esi+obj_curr_land]
	mov	eax,[citadel_list+eax*4]
	add	eax,offset citadel_list
	mov	[list_buffer],eax
	mov	[DWORD list_buffer+4],-1
	mov	[list_count],1
	mov	edx,offset enemy_scan
	call	scan_blokes
	ret
@@x:
	jmp	@@x
ENDP

;-------------------------------------------------------------------------
;edx = check routine
PROC scan_blokes NEAR
	pushad
	mov	ebp,offset charac0001
	mov	edi,offset temp_buffer
	mov	ecx,num_characters+24
@@l:
	test	[WORD ebp+obj_flags2],no_process_bit
	jne	@@c
	cmp	ebp,offset seize_character
	je	@@c
	call	edx
	jnc	@@c
    	mov	[edi],ebp
	add	edi,4
@@c:
	add	ebp,character_size
	loop	@@l

	mov	[DWORD edi],-1

	sub	edi,offset temp_buffer
	shr	edi,2
;	je	@@x

	mov	[temp_count],di

	call	move_temp_to_list

	test	[temp_count],-1
	je	@@x

	mov	ebx,edi
	call	modrandn
	mov	edi,[temp_buffer+eax*4]

	mov	[last_result],edi
	popad
	stc
	ret
@@x:
	mov	[last_result],0
	popad
	clc
	ret
ENDP

;ebp - char ptr
;return with carry set if bloke ebp in land in list
PROC enemy_scan NEAR
	pushad
	mov	esi,ebp
	call	get_map_position
	mov	eax,ecx			;al - this blokes land

    	movzx	ecx,[list_count]
	jecxz	@@no_entries

	mov	edi,offset list_buffer
@@l:
	mov	ebx,[edi]
	cmp	[ebx+obj_port],al	;is bloke in any land in list
	jne	@@1			;no

	call	is_enemy		;bloke ebp is enemy of land ebx?
	jnc	@@1

	popad
	ret

@@1:
	add	edi,4			;next list entry
	loop	@@l

@@no_entries:
	popad
	clc
	ret
ENDP	

;ebp - bloke ptr
;is in homeland ?
PROC home_scan NEAR
	push	ecx esi
	mov	esi,ebp
	call	get_map_position
	pop	esi
	cmp	[esi+obj_start_land],cl
	pop	ecx
	jne	@@x

	call	are_enemies	;(esi & ebp)
	jnc	@@x
	ret
@@x:
	clc
	ret
ENDP
	IF	0
>;esi - c1
>;ebp - c2
>;is bloke1 enemy of bloke2
>PUBLIC are_enemies
>PROC are_enemies NEAR
>	push	eax ebx
>	movzx	eax,[BYTE esi+obj_start_realm]
>	movzx	ebx,[BYTE ebp+obj_start_realm]
>	cmp	eax,ebx
>	je	@@xc
>
>	region_mul	ebx
>	movzx	ebx,[WORD realm_enemies+realm_data+ebx]
>	bt	ebx,eax		;sets carry
>	pop	ebx eax
>	ret
>@@xc:
>	pop	ebx eax
>	clc
>	ret
>ENDP
>
>;ebp - bloke ptr
>;ebx - land (citadel ptr)
>;is bloke enemy of land
>PUBLIC is_enemy
>PROC is_enemy NEAR
>	push	eax ebx
>	movzx	eax,[BYTE obj_start_realm+ebp]
>	movzx	ebx,[BYTE obj_land_realm+ebx]
>	cmp	eax,ebx
>	je	@@xc
>
>	region_mul	ebx
>	movzx	ebx,[WORD realm_data+realm_enemies+ebx]
>	bt	ebx,eax		;sets carry
>	pop	ebx eax
>	ret
>@@xc:
>	pop	ebx eax
>	clc
>	ret
>ENDP
	ELSE
;esi - c1
;ebp - c2
;is bloke1 enemy of bloke2
PUBLIC are_enemies
PROC are_enemies NEAR
	push	eax ebx
;	movzx	eax,[BYTE esi+obj_start_realm]
;	movzx	ebx,[BYTE ebp+obj_start_realm]

	movzx	eax,[BYTE esi+obj_allegiance]
	movzx	ebx,[BYTE ebp+obj_allegiance]

	cmp	eax,ebx
	je	@@xc

	region_mul	ebx
	movzx	ebx,[WORD realm_enemies+realm_data+ebx]
	bt	ebx,eax		;sets carry if enemies
	pop	ebx eax
	ret
@@xc:
	pop	ebx eax
	clc
	ret
ENDP

;ebp - bloke ptr
;ebx - land (citadel ptr)
;is bloke enemy of land
PUBLIC is_enemy
PROC is_enemy NEAR
	push	eax ebx

;	movzx	eax,[BYTE obj_start_realm+ebp]
;	movzx	ebx,[BYTE obj_land_realm+ebx]

	movzx	eax,[BYTE ebp+obj_allegiance]
	movzx	ebx,[WORD ebx+obj_land_ruler]
	mov	ebx,[cast_list+ebx*4]
	movzx	ebx,[BYTE ebx+obj_allegiance]
	cmp	eax,ebx
	je	@@xc

	region_mul	ebx
	movzx	ebx,[WORD realm_data+realm_enemies+ebx]
	bt	ebx,eax		;sets carry
	pop	ebx eax
	ret
@@xc:
	pop	ebx eax
	clc
	ret
ENDP
	ENDIF
;-------------------------------------------------------------------------
;IN
;esi - c1
;edi - c2
;OUT
;carry denotes truth
PUBLIC are_friends
PROC are_friends NEAR
	push	eax ebx
	movzx	eax,[BYTE esi+obj_allegiance]
	movzx	ebx,[BYTE edi+obj_allegiance]

	cmp	eax,ebx
	jne	@@xc

	pop	ebx eax
     	stc
	ret
@@xc:
	pop	ebx eax
	clc
	ret
ENDP

;-------------------------------------------------------------------------
;IN
;esi - bloke ptr
;edi - land (citadel ptr)
;is bloke friend of land
PUBLIC is_friendly
PROC is_friendly NEAR
	push	eax ebx

	movzx	eax,[BYTE esi+obj_allegiance]
	movzx	ebx,[WORD edi+obj_land_ruler]
	mov	ebx,[cast_list+ebx*4]
	movzx	ebx,[BYTE ebx+obj_allegiance]
	cmp	eax,ebx
	jne	@@xc

	pop	ebx eax
	stc
	ret
@@xc:	
	pop	ebx eax
	clc
	ret
ENDP



;-------------------------------------------------------------------------

;IN  [listcount] - # entries
;OUT eax - random entry selected
PROC pick_list_entry NEAR
	movzx	ebx,[WORD list_count]
	test	ebx,-1
	je	@@p
	call	modrandn
	mov	eax,[list_buffer+eax*4]
	mov	[last_result],eax
	stc
	ret
@@p:
	jmp	@@p
ENDP

PROC check_purpose NEAR
	cmp	eax,NEIGHBOUR
	jne	@@err
	cmp	[WORD obj_purpose+esi],dx
	ZtoC
	ret
@@err:
	jmp	@@err
ENDP

;not used
PROC check_reaction NEAR
	clc
	ret
ENDP

;eax - what state to check for
;this is just a check condition
;not a selection condition
;ie. ANY_NEARLAND THREATENED
PROC check_nearland_state NEAR
	push	eax
	GEN_LIST LC_NEARLANDS
	IS_EMPTY_LIST
	pop	eax
	BIFT END
	mov	ebp,offset list_buffer
	movzx	ecx,[WORD list_count]
@@l:
	mov	ebx,[ebp]			;ebx - citadel pointer
	cmp	[ebx+obj_land_war_state],al	;check state
	je	@@found

	add	ebp,4				;next entry
    	loop	@@l
LAB END
	clc
	ret

@@found:
	stc
	ret
ENDP

;copy temp list to main list
PROC move_temp_to_list NEAR
	push	esi edi
	movzx	ecx,[temp_count]
	mov	[list_count],cx
	jecxz	@@x
	mov	esi,offset temp_buffer
	mov	edi,offset list_buffer
	rep	movsd
@@x:
	pop	edi esi
	ret
ENDP

PROC move_list_to_temp NEAR
	push	esi edi
	movzx	ecx,[list_count]
	mov	[temp_count],cx
	jecxz	@@x
	mov	esi,offset list_buffer
	mov	edi,offset temp_buffer
	rep	movsd
@@x:
	pop	edi esi
	ret
ENDP

;-------------------------------------------------------------------------
; Jump routines / tables for list generation
;-------------------------------------------------------------------------

PROC conv_list_rout NEAR
	call	[cs:conv_tab_jumps+eax*4]
	ret
ENDP

conv_tab_jumps	dd	land_to_man
		dd	man_to_land

PROC land_to_man NEAR
	movzx	ecx,[WORD list_count]
	jecxz	@@x
	mov	ebp,offset list_buffer
@@l:
	mov	edi,[ebp]
	movzx	eax,[BYTE edi+obj_land_owner]
	mov	eax,[character_list+eax*4]
	add	eax,offset character_list
	mov	[ebp],eax
	add	ebp,4
	loop	@@l
@@x:
	ret
ENDP

PROC man_to_land NEAR
	movzx	ecx,[list_count]
	jecxz	@@x
	mov	ebp,offset list_buffer
@@l:
	mov	edi,[ebp]
	movzx	eax,[BYTE edi+obj_start_land]
	mov	eax,[citadel_list+eax*4]
	add	eax,offset citadel_list
	mov	[ebp],eax
	add	ebp,4
	loop	@@l
@@x:
	ret
ENDP

;eax - condition #
PROC generate_list NEAR
	pushad
	call	[cs:condition_jumps+eax*4]
	popad
	stc
	ret
ENDP

;-------------------------------------------------------------------------
;eax - condition #
PROC subject_list NEAR
	pushad
	call	[cs:subtraction_jumps+eax*4]
	popad
	stc
	ret
ENDP

condition_jumps		dd	lc_neighbours_rout
			dd	lc_nearlands_rout
			dd	lc_home_realm_rout
			dd	lc_all_lands_rout
			dd	lc_adjacent_lands_rout
			dd	lc_realm_lands_rout
 			dd	lc_quest_subject_rout
			dd	lc_this_kingdom_rout
;			dd	lc_return_home_rout
;			dd	lc_help_neighbour_rout
	   		dd	bastard1
	   		dd	bastard2

subtraction_jumps	dd	sc_gte_under_attack_rout
			dd	sc_cowardly_rout
			dd	sc_not_in_nearland_rout
			dd	sc_not_threatened_rout
			dd	sc_not_at_peace_rout
			dd	sc_not_available_rout
			dd	sc_not_in_homeland_rout
			dd	sc_not_standing_firm_rout
			dd	sc_retreating_rout
	   		dd	sc_not_in_danger_rout
	   		dd	bastard3
	   		dd	bastard4

;-------------------------------------------------------------------------

;ebx - citadel (land) #
PROC calc_cit_ptr NEAR
	mov	eax,[citadel_list+ebx*4]
	add	eax,offset citadel_list
	ret
ENDP

;eax - citadel ptr
PROC calc_cit_owner NEAR
	movzx	eax,[WORD obj_land_owner+eax]
	mov	eax,[character_list+eax*4]
	add	eax,offset character_list
	ret
ENDP

;list of neighbours near homeland
PROC lc_neighbours_rout NEAR
	movzx	eax,[BYTE obj_start_land+esi]
	mov	ebp,offset list_buffer
	mov	edx,[place_list+eax*4]
	movzx	ecx,[BYTE edx]
	test	ecx,-1
	jbe	@@x
	inc	edx
@@l:
	movzx	ebx,[BYTE edx]

	cmp	ebx,25
	jb	@@n
	cmp	ebx,Corlon
	je	@@n
	cmp	ebx,Elormane
	je	@@n

	inc	edx
	call	calc_cit_ptr
	call	calc_cit_owner
	mov	[ebp],eax
	add	ebp,4
@@n:
	loop	@@l

	mov	[DWORD ebp],-1
	sub	ebp,offset list_buffer
	shr	ebp,2
	mov	[list_count],bp

@@x:
	stc
	ret
ENDP

;list of nearlands
PROC lc_nearlands_rout NEAR
	movzx	eax,[BYTE obj_start_land+esi]
	mov	ebp,offset list_buffer
	mov	edx,[place_list+eax*4]
	movzx	ecx,[BYTE edx]
	test	ecx,-1
	jbe	@@x
	inc	edx
@@l:
	movzx	ebx,[BYTE edx]

	cmp	ebx,25
	jb	@@n
	cmp	ebx,Corlon
	je	@@n
	cmp	ebx,Elormane
	je	@@n

	inc	edx
	call	calc_cit_ptr
	mov	[ebp],eax
	add	ebp,4
@@n:
	loop	@@l

	mov	[DWORD ebp],-1
	sub	ebp,offset list_buffer
	shr	ebp,2
	mov	[list_count],bp
@@x:
	stc
	ret
ENDP

;gen list of ALL lands in home realm
PROC lc_home_realm_rout NEAR
	movzx	eax,[BYTE obj_start_realm+esi]
	mov	edx,[kingdom_list+eax*4]
	movzx	ecx,[BYTE edx]
	test	ecx,-1
	jbe	@@x
	inc	edx
	mov	edi,offset list_buffer
@@m:
	movzx	ebx,[BYTE edx]

	cmp	ebx,25
	jb	@@n
	cmp	ebx,Corlon
	je	@@n
	cmp	ebx,Elormane
	je	@@n

	call	calc_cit_ptr
	mov	[edi],eax
	add	edi,4
@@n:
	inc	edx
	loop	@@m

	mov	[DWORD edi],-1
	sub	edi,offset list_buffer
	shr	edi,2
	mov	[list_count],di
	stc
	ret
@@x:
	clc
	ret
ENDP

;gen list of lands in neighbouring realms
PROC lc_realm_lands_rout NEAR
	call	get_map_position
	mov	ebp,offset list_buffer
	mov	edx,[kingdom_adj+eax*4]

	movzx	ecx,[BYTE edx]
	test	ecx,-1
	jbe	@@xx
	inc	edx
@@l:
	push	ecx

	movzx	edi,[BYTE edx]
	cmp	edi,GreatOcean
	je	@@x

	cmp	edi,MistsOblivion
	je	@@x

	inc	edx
	mov	edi,[kingdom_list+edi*4]
	movzx	ecx,[BYTE edi]
	test	ecx,-1
	jbe	@@x
	inc 	edi
@@m:
	movzx	ebx,[BYTE edi]
	inc	edi

	cmp	ebx,25
	jb	@@n
	cmp	ebx,Corlon
	je	@@n
	cmp	ebx,Elormane
	je	@@n

	call	calc_cit_ptr		;ebx = cit#
	mov	[ebp],eax		;eax = cit_ptr
	add	ebp,4
@@n:
	loop	@@m
@@x:
	pop	ecx
	loop	@@l

@@xx:
	mov	[DWORD ebp],-1
	sub	ebp,offset list_buffer
	shr	ebp,2
	mov	[list_count],bp

	stc
	ret
ENDP

;all the lands on the map
PROC lc_all_lands_rout NEAR
	mov	ebp,offset list_buffer
	mov	ebx,25
	mov	ecx,130-25
@@l:
	cmp	ebx,Corlon
	je	@@n
	cmp	ebx,Elormane
	je	@@n

	call	calc_cit_ptr
	mov	[ebp],eax
	add	ebp,4
@@n:
	inc	ebx
	loop	@@l

	mov	[DWORD ebp],-1
	sub	ebp,offset list_buffer
	shr	ebp,2
	mov	[list_count],bp

	stc
	ret
ENDP

;subject of current quest
PROC lc_quest_subject_rout NEAR
	mov	[list_count],1
	call	fetch_quest_subject
	mov	[list_buffer],edi
	mov	[DWORD list_buffer+4],-1
	cmp	edi,-1
	jne	@@c
    	mov	[list_count],0
@@c:
	stc
	ret
ENDP

PROC lc_this_kingdom_rout NEAR
	movzx	eax,[BYTE obj_curr_realm+esi]
	mov	eax,[kingdom_list+eax*4]
	movzx	ecx,[BYTE eax]
	mov	ebp,offset list_buffer
	test	ecx,-1
	jbe	@@x
	inc	eax
@@l:
	movzx	ebx,[BYTE eax]
	cmp	ebp,25
	jb	@@n
	cmp	ebp,Corlon
	je	@@n
	cmp	ebp,Elormane
	je	@@n

	call	calc_cit_ptr
	mov	[ebp],eax
	add	ebp,4
@@n:
	inc	eax
	loop	@@l
@@x:
	mov	[DWORD ebp],-1
	sub	ebp,offset list_buffer
	shr	ebp,2
	mov	[list_count],bp
	stc
	ret
ENDP

	IF	0
;massive difficult condition from STAND_FIRM->HELP_NEIGHBOUR
PROC lc_help_neighbour_rout NEAR
	GEN_LIST LC_NEARLANDS
	SUB_LIST SC_GTE_UNDER_ATTACK	;== SC_LTE_IN_PERIL!
	IS_EMPTY_LIST
	BIFT END

	CONV_LIST CV_CIT_2_BLOKE
	SUB_LIST SC_NOT_IN_HOMELAND
	SUB_LIST SC_NOT_STANDING_FIRM
	SUB_LIST SC_RETREATING
	IS_EMPTY_LIST
	BIFT END

	PICK_FROM_LIST	
	TRUE
	BRA EXIT

LAB END
	FALSE
LAB EXIT
	ret
ENDP
	ENDIF

	IF	0
;massive difficult condition from HELP_NEIGHBOUR->RETURN_HOME
PROC lc_return_home_rout NEAR
	GEN_LIST LC_NEIGHBOURS
	SUB_LIST SC_STANDING_FIRM
	SUB_LIST SC_NOT_RETREATING
	SUB_LIST SC_NOT_UNDER_ATTACK
	SUB_LIST SC_ANOTHER
	clc
	ret
ENDP
	ENDIF

;generate list of adjancent neighbours
PROC lc_adjacent_lands_rout NEAR
	GEN_LIST LC_NEARLANDS
	call	get_map_position
	mov	edx,[place_list+ecx*4]
	mov	ebp,offset list_buffer
	mov	edi,offset temp_buffer
	movzx	ecx,[WORD list_count]
	jecxz	@@x
@@l:
	mov	eax,[ebp]
	call	is_in_place_list
	jnc	@@2
    	mov	[edi],eax
	add	edi,4
@@2:
	add	ebp,4
	loop	@@l

	mov	[DWORD edi],-1
	sub	edi,offset temp_buffer
	shr	edi,2
	je	@@x

	mov	[temp_count],di

	call	move_temp_to_list

	stc
	ret
@@x:
	clc
	ret
ENDP

;eax - citadel pointer
;edx - place list pointer
PROC is_in_place_list NEAR
	pushad
	mov	ebp,eax			;ebp - citadel to test for

	movzx	ecx,[BYTE edx]
	test	ecx,-1
	jbe	@@x
	inc	edx
@@l:
	movzx	ebx,[BYTE edx]

	cmp	ebx,25
	jb	@@2			;water!
	cmp	ebx,Corlon
	je	@@2			;Corlon
	cmp	ebx,Elormane
	je	@@2

	call	calc_cit_ptr		;calc citadel pointer of land in place list
	cmp	eax,ebp
	jne	@@2
	popad				;found it!
    	stc
	ret
@@2:
	inc	edx			;next place list entry
	loop	@@l
@@x:
	popad				;not found
	clc
	ret
ENDP

;-------------------------------------------------------------------------

;subtract from list if war state >= UNDER_ATTACK
PROC sc_gte_under_attack_rout NEAR
	mov	ebp,offset list_buffer
	mov	edi,ebp
	movzx	ecx,[WORD list_count]
	jecxz	@@x
@@l:
	mov	eax,[ebp]
	cmp	[BYTE eax+obj_land_war_state],LS_UNDER_ATTACK
	jge	@@n
    	mov	[edi],eax
	add	edi,4
@@n:
	add	ebp,4
	loop	@@l

@@x:
	mov	[DWORD edi],-1
	sub	edi,offset list_buffer
	shr	edi,2
	mov	[list_count],di

	stc
	ret
ENDP

;subtract cowardly people from list
PROC sc_cowardly_rout NEAR
	push	esi
	mov	ebp,offset list_buffer
	mov	edi,ebp
	movzx	ecx,[WORD list_count]
	jecxz	@@x
@@l:
	mov	esi,[ebp]
	COWARDLY
	BIFT N
    	mov	[edi],esi
	add	edi,4
LAB N
	add	ebp,4
	loop	@@l

@@x:
	mov	[DWORD edi],-1
	sub	edi,offset list_buffer
	shr	edi,2
	mov	[list_count],di

	pop	esi
	stc
	ret
ENDP

PROC sc_not_in_nearland_rout NEAR
;gen temp list of nearlands
	movzx	eax,[BYTE obj_start_land+esi]
	mov	ebp,offset temp_buffer
	mov	edx,[place_list+eax*4]
	movzx	ecx,[BYTE edx]
	test	ecx,-1
	jbe	@@err
	inc	edx
@@l:
	movzx	ebx,[BYTE edx]
	inc	edx
	call	calc_cit_ptr
	mov	[ebp],eax
	add	ebp,4
	loop	@@l

	mov	[DWORD ebp],-1
	sub	ebp,offset temp_buffer
	shr	ebp,2
	mov	[temp_count],bp

;now for each player in list
;if he is in a temp_list land
;entry then keep it else fuck off

	mov	ebp,offset list_buffer
	mov	edi,ebp
	movzx	ecx,[WORD list_count]
	jecxz	@@x
@@l2:
	mov	eax,[ebp]
	call	check_in_temp_list
	jnc	@@n
    	mov	[edi],eax
	add	edi,4
@@n:
	add	ebp,4
	loop	@@l2

@@x:
	mov	[DWORD edi],-1
	sub	edi,offset list_buffer
	shr	edi,2
	mov	[list_count],di

	stc
	ret
@@err:
	jmp	@@err
ENDP

;eax - man to check whether in temp_list land
PROC check_in_temp_list NEAR
	pushad
	mov	esi,eax
	call	get_map_position
	mov	eax,ecx			;eax - person land
	movzx	ecx,[WORD temp_count]
	mov	ebp,offset temp_buffer
@@l:
	mov	ebx,[ebp]
	cmp	[obj_port+ebx],al	;citadel place = player land
	je	@@found
	add	ebp,4
	loop	@@l

	popad
	clc
	ret
@@found:
	popad
	stc
	ret
ENDP

PROC sc_not_threatened_rout NEAR
	mov	ebp,offset list_buffer
	mov	edi,ebp
	movzx	ecx,[WORD list_count]
	jecxz	@@x
@@l:
	mov	eax,[ebp]
	cmp	[BYTE eax+obj_land_war_state],LS_THREATENED
	jne	@@n
    	mov	[edi],eax
	add	edi,4
@@n:
	add	ebp,4
	loop	@@l

@@x:
	mov	[DWORD edi],-1
	sub	edi,offset list_buffer
	shr	edi,2
	mov	[list_count],di

	stc
	ret
ENDP

PROC sc_not_at_peace_rout NEAR
	mov	ebp,offset list_buffer
	mov	edi,ebp
	movzx	ecx,[WORD list_count]
	jecxz	@@x
@@l:
	mov	eax,[ebp]
	cmp	[BYTE eax+obj_land_war_state],LS_AT_PEACE
	jne	@@n
    	mov	[edi],eax
	add	edi,4
@@n:
	add	ebp,4
	loop	@@l

@@x:
	mov	[DWORD edi],-1
	sub	edi,offset list_buffer
	shr	edi,2
	mov	[list_count],di

	stc
	ret
ENDP

PROC sc_not_in_homeland_rout NEAR
	mov	ebp,offset list_buffer
	mov	edi,ebp
	movzx	ecx,[WORD list_count]
	jecxz	@@x
@@l:
	mov	eax,[ebp]
	mov	bl,[eax+obj_start_land]
	cmp	[eax+obj_curr_land],bl
	jne	@@c
    	mov	[edi],eax
	add	edi,4
@@c:
	add	ebp,4
	loop	@@l

@@x:
	mov	[DWORD edi],-1
	sub	edi,offset list_buffer
	shr	edi,2
	mov	[list_count],di

	ret
ENDP

;subtract those not defending homeland
PROC sc_not_standing_firm_rout NEAR
	mov	ebp,offset list_buffer
	mov	edi,ebp
	movzx	ecx,[WORD list_count]
	jecxz	@@x
@@l:
	mov	eax,[ebp]
	cmp	[WORD obj_purpose+eax],DEFEND_HOMELAND
	jne	@@c
    	mov	[edi],eax
	add	edi,4
@@c:
	add	ebp,4
	loop	@@l

@@x:
	mov	[DWORD edi],-1
	sub	edi,offset list_buffer
	shr	edi,2
	mov	[list_count],di

	ret
ENDP

PROC sc_not_in_danger_rout NEAR
	mov	ebp,offset list_buffer
	mov	edi,ebp
	movzx	ecx,[WORD list_count]
	jecxz	@@x
@@l:
	mov	eax,[ebp]
	cmp	[BYTE eax+obj_land_war_state],LS_IN_DANGER
	jne	@@c
    	mov	[edi],eax
	add	edi,4
@@c:
	add	ebp,4
	loop	@@l

@@x:
	mov	[DWORD edi],-1
	sub	edi,offset list_buffer
	shr	edi,2
	mov	[list_count],di

	ret
ENDP

;subtract thos from list not available
PROC sc_not_available_rout NEAR
	mov	ebp,offset list_buffer
	mov	edi,ebp
	movzx	ecx,[WORD list_count]
	jecxz	@@x
@@l:
	mov	eax,[ebp]
	test	[WORD eax+obj_flags],available_bit
	je	@@c
    	mov	[edi],eax
	add	edi,4
@@c:
	add	ebp,4
	loop	@@l

@@x:
	mov	[DWORD edi],-1
	sub	edi,offset list_buffer
	shr	edi,2
	mov	[list_count],di

	ret
ENDP

;subtract those retreating
PROC sc_retreating_rout NEAR
	mov	ebp,offset list_buffer
	mov	edi,ebp
	movzx	ecx,[WORD list_count]
	jecxz	@@x
@@l:
	mov	eax,[ebp]
	cmp	[WORD obj_purpose+eax],DEFEND_HOMELAND
	jne	@@c
	cmp	[WORD obj_reaction+eax],RETREAT
	je	@@c
    	mov	[edi],eax
	add	edi,4
@@c:
	add	ebp,4
	loop	@@l

@@x:
	mov	[DWORD edi],-1
	sub	edi,offset list_buffer
	shr	edi,2
	mov	[list_count],di

	ret
ENDP

PROC check_recruitables NEAR
	stc
	ret
ENDP

;-------------------------------------------------------------------------
; External routines that need to do JIM processing
;-------------------------------------------------------------------------

;find any of current kingdom's citadels/castles
;IN
;  esi - character pointer
;OUT
;  lots of registers poo'ed up, but not esi
PUBLIC find_a_region_citadel
PROC find_a_region_citadel NEAR
	call	fill_curr_posn
	GEN_LIST LC_KINGDOM_LANDS
	IS_EMPTY_LIST
	BIFT NO_CITS
    	PICK_FROM_LIST
	jmp	@@got1
LAB NO_CITS
	mov	[last_result],0
@@got1:
	mov	eax,[last_result]
	ret
ENDP

;find any neighbouring kingdoms citadel/castle
;IN
;  esi - character pointer
;OUT
;  lots of registers poo'ed up, but not esi
PUBLIC find_any_neighbouring_citadel
PROC find_any_neighbouring_citadel NEAR
	call	fill_curr_posn

	GEN_LIST LC_LANDS_IN_NEIGHBOUR_REALMS_TO_ME
	IS_EMPTY_LIST
	BIFT NO_CITS
    	PICK_FROM_LIST
	jmp	@@got1
LAB NO_CITS
	mov	[last_result],0
@@got1:
	mov	eax,[last_result]
	ret
ENDP

;fill in curr_land & curr_realm for character esi
PROC fill_curr_posn NEAR
	mov	eax,[obj_x+esi]
	mov	ebx,[obj_y+esi]
	shr	eax,16+9
	shr	ebx,16+9
	sub	ebx,127
	neg	ebx
	shl	ebx,7
	add	ebx,eax
	mov	dx,[map_regions+ebx*2]
	mov	[esi+obj_curr_land],dh
	and	dl,0fh
	mov	[esi+obj_curr_realm],dl
	ret
ENDP

PUBLIC fudge_people
PROC fudge_people NEAR
	pushfd
	pushad
	mov	edi,offset charac0001
	mov	ecx,num_characters+24
@@l:
	mov	[WORD edi+obj_char_flags],0e12bh
	add	edi,character_size
	loop	@@l
	popad
	popfd
	ret
ENDP

PUBLIC set_beddies
PROC set_beddies NEAR
	pushfd
	pushad
	mov	esi,offset charac0001
	mov	ecx,154
@@1:
	mov	[WORD esi+obj_sleep_time],(18*60)
	mov	[WORD esi+obj_wake_time],(6*60)
	add	esi,character_size
	loop	@@1
	popad
	popfd
	ret
ENDP

PROC bastard1 NEAR
@@x:
	jmp	@@x
	ret
ENDP
PROC bastard2 NEAR
@@x:
	jmp	@@x
	ret
ENDP
PROC bastard3 NEAR
@@x:
	jmp	@@x
	ret
ENDP
PROC bastard4 NEAR
@@x:
	jmp	@@x
	ret
ENDP


;IN
;edi - citadel pointer
;OUT
;ebp - owner pointer or -1
;All registers preserved
PUBLIC ruler_at_citadel
PROC ruler_at_citadel NEAR
	push	bx eax
	mov	bx,[obj_land_ruler+edi]
	movzx	eax,[WORD obj_cit_link+edi]
@@l:
	cmp	ax,-1
	je	@@false
	mov	ebp,[cast_list+eax*4]
	cmp	[ebp+obj_num],bx
	je	@@true
	movzx	eax,[WORD obj_next_rlink+ebp]
	jmp	@@l
@@false:
	mov	ebp,-1
	pop	eax bx
	ret
@@true:
	pop	eax bx
	ret
ENDP

PROC random_wander_nothing NEAR
	ret
ENDP

PROC be_hostage_nothing NEAR
	ret
ENDP

;-------------------------------------------------------------------------
	ENDS

;-------------------------------------------------------------------------
	END
