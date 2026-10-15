;-------------------------------------------------------------------------
;			LORDS OF MIDNIGHT III
;
;			    THE CITADEL
;-------------------------------------------------------------------------
; (C) David Ollman & James Shaw 1995.
;-------------------------------------------------------------------------

	MODEL HUGE
	IDEAL
	JUMPS
	P386

	GROUP DGROUP DATA_SEG,BSS_SEG

;-------------------------------------------------------------------------

	INCLUDE	'EQUATES.ASM'
	INCLUDE	'STRUCTUR.ASM'

	INCLUDE	'TEXTDEF.EQU'
	INCLUDE 'REGION.EQU'
	INCLUDE 'QUEST.EQU'

	INCLUDE	'ERRMAC.EQU'
	INCLUDE 'TEXTMESS.EQU'

	INCLUDE 'TRANSMUT.EQU'

	SEGMENT BSS_SEG PARA PUBLIC USE32 'LOMBSS'
	ENDS

	SEGMENT DATA_SEG WORD PUBLIC USE32 'LOMDATA'

;-------------------------------------------------------------------------
EXTRN	citad0027:BYTE

EXTRN	type_list:WORD
EXTRN	watch_flag:WORD
EXTRN	realm_data:WORD
EXTRN	do_barging:WORD
EXTRN	map_regions:WORD
EXTRN	no_messages:WORD
EXTRN	zmap_buffer:WORD
EXTRN	combat_flag:WORD
EXTRN	advance_flag:WORD
EXTRN	quest_result:WORD
EXTRN	seize_semaphore:WORD
EXTRN	quest_no_program:WORD

EXTRN	linkmap:DWORD
EXTRN	list_list:DWORD
EXTRN	cast_list:DWORD
EXTRN	props_list:DWORD
EXTRN	beast_list:DWORD
EXTRN	temp_obj_x:DWORD
EXTRN	temp_obj_y:DWORD
EXTRN	barge_tmp_x:DWORD
EXTRN	barge_tmp_y:DWORD
EXTRN	shelter_num:DWORD
EXTRN	shelter_xco:DWORD
EXTRN	shelter_yco:DWORD
EXTRN	info_params:DWORD
EXTRN	shelter_dist:DWORD
EXTRN	shelter_type:DWORD
EXTRN	shelter_addr:DWORD
EXTRN	scan_routine:DWORD
EXTRN	citadel_list:DWORD
EXTRN	scan_routine:DWORD
EXTRN	proximity_obj:DWORD
EXTRN	proximity_val:DWORD
EXTRN	ignore_object:DWORD
EXTRN	location_list:DWORD
EXTRN	controlled_addr:DWORD
EXTRN	seize_character:DWORD
EXTRN	info_param_block:DWORD

;-------------------------------------------------------------------------

qdt_qdat1	dw	0
qdt_qdat2	dw	0
qdt_qtype1	db	0
	EVEN

quest_cancelled	dw	0

temp_x		dw	0	;JS 19/6/95
temp_y		dw	0

find_status	dw	0

temp_shelt	dd	-1
temp_shelt2	dd	-1

;-------------------------------------------------------------------------
link_offsets	dw	0,0
		dw	0,-1
		dw	1,-1

		dw	-1,0
		dw	-1,-1
		dw	1,0

		dw	-1,1
		dw	0,1
		dw	1,1

;-------------------------------------------------------------------------
quest_type	dd	recruit_quest	;0
		dd	join_quest	;1
		dd	kill_quest	;2
		dd	rescue_quest	;3
		dd	follow_quest	;4
		dd	goto_quest	;5
		dd	guard_quest	;6
		dd	seize_quest	;7
		dd	find_quest	;8
		dd	take_quest	;9
		dd	destroy_quest	;10
		dd	rest_quest	;11

; IF ANY OF THESE QUESTS ARE CHANGED, CHANGE THE LIST
; ON LINE 49 OF action.asm
;

;-------------------------------------------------------------------------
recruit_quest	dd	quest_wake_up_rout
		dd	quest_store_charac_quest_rout
		dd	quest_find_rout
		dd	quest_goto_rout
		dd	quest_reset_maybe_rout
		dd	quest_check_not_dead_rout
;		dd	quest_move_outside_maybe_rout
		dd	quest_move_inside_maybe_rout
		dd	quest_is_about_to_recruit_rout
		dd	quest_delay_1_frame_rout
		dd	quest_delay_1_frame_rout
		dd	quest_recruit_rout
		dd	quest_cancel
	REPT	8
		dd	quest_panic
	ENDM

;-------------------------------------------------------------------------
join_quest	dd	quest_wake_up_rout
		dd	quest_store_charac_quest_rout   ;if you change this then change the auto quest
		dd	quest_find_rout			;number in the army structures & in party.asm
		dd	quest_goto_rout			;4/4/95 : no longer have to do this, change only
		dd	quest_reset_maybe_rout		;join_quest_no in quest.equ
		dd	quest_check_not_dead_rout
		dd	quest_move_inside_maybe_rout
		dd	quest_is_about_to_join_rout
		dd	quest_delay_1_frame_rout
		dd	quest_delay_1_frame_rout
		dd	quest_join_rout
		dd	quest_party_rout		;number of this one
	REPT	8
		dd	quest_panic
	ENDM

;-------------------------------------------------------------------------
kill_quest	dd	quest_wake_up_rout
		dd	quest_store_charac_quest_rout
		dd	quest_find_rout
		dd	quest_goto_rout
		dd	quest_reset_maybe_rout
		dd	quest_check_not_dead_rout
;		dd	quest_move_inside_maybe_rout
		dd	quest_is_about_to_kill_rout
		dd	quest_delay_1_frame_rout
		dd	quest_delay_1_frame_rout
		dd	quest_kill_rout
		dd	quest_cancel
	REPT	8
		dd	quest_panic
	ENDM

;-------------------------------------------------------------------------
rescue_quest	dd	quest_do_nothing
		dd	quest_do_nothing
		dd	quest_cancel
	REPT	8
		dd	quest_panic
	ENDM


;-------------------------------------------------------------------------
; If the follow quest list is changed you must change the
; bodges in :
; party.asm : program_party :
;	mov	[obj_qstate+esi],# of quest_follow_rout (5)
; savedata.asm : character data
; charac0002 & charac0003 have the same bodge
;
follow_quest	dd	quest_wake_up_rout
		dd	quest_store_charac_quest_rout
		dd	quest_find_rout
		dd	quest_skip_maybe_rout
		dd	quest_goto_rout
		dd	quest_reset_maybe_rout
		dd	quest_check_not_dead_rout
		dd	quest_is_about_to_follow_rout
		dd	quest_delay_1_frame_rout
		dd	quest_delay_1_frame_rout
		dd	quest_follow_rout
		dd	quest_cancel		;get here if target dead!
	REPT	8
		dd	quest_panic
	ENDM

;-------------------------------------------------------------------------
goto_quest	dd	quest_goto_init_rout
		dd	quest_move_outside_maybe_rout
		dd	quest_goto_rout
		dd	quest_move_inside_rout
		dd	quest_reset_meet_rout
		dd	quest_cancel
	REPT	8
		dd	quest_panic
	ENDM

;-------------------------------------------------------------------------
;guard_quest	dd	quest_find_rout
;		dd	quest_goto_rout
;		dd	quest_reset_maybe_rout
;		dd	quest_store_target_rout
;		dd	quest_goto_rout
;		dd	quest_guard_rout
;		dd	quest_kill_rout
;		dd	quest_restore_goto_rout
;		dd	quest_restart_rout

;-------------------------------------------------------------------------
guard_quest	dd	quest_wake_up_rout
		dd	quest_init_guard_rout
		dd	quest_store_target_rout
		dd	quest_goto_rout
		dd	quest_guard_rout
		dd	quest_goto_rout
		dd	quest_check_not_dead_guard_rout ;dont add any between this and next without
		dd	quest_kill_rout			;changing checknotdeadguardrout
		dd	quest_restore_goto_rout
		dd	quest_restart_rout
	REPT	8
		dd	quest_panic
	ENDM

;-------------------------------------------------------------------------
seize_quest	dd	quest_wake_up_rout
		dd	quest_goto_rout
		dd	quest_is_about_to_seize_rout
		dd	quest_delay_1_frame_rout
		dd	quest_delay_1_frame_rout
		dd	quest_seize_rout	;this is seize_quest_no in quest.equ
		dd	quest_cancel
	REPT	8
		dd	quest_panic
	ENDM

;-------------------------------------------------------------------------
find_quest	dd	quest_wake_up_rout
		dd	quest_store_charac_quest_rout
		dd	quest_find_rout
;		dd	quest_skip_maybe_rout
		dd	quest_goto_rout
		dd	quest_reset_maybe_rout
;		dd	quest_goto_rout
		dd	quest_is_about_to_pickup_rout
		dd	quest_delay_1_frame_rout
		dd	quest_delay_1_frame_rout
		dd	quest_pickup_rout
		dd	quest_cancel
	REPT	8
		dd	quest_panic
	ENDM

;-------------------------------------------------------------------------
take_quest	dd	quest_wake_up_rout
		dd	quest_init_take_rout
		dd	quest_store_charac_quest_rout
		dd	quest_find_rout
		dd	quest_goto_rout
		dd	quest_reset_maybe_rout
		dd	quest_move_inside_maybe_rout
		dd	quest_is_about_to_take_rout
		dd	quest_delay_1_frame_rout
		dd	quest_delay_1_frame_rout
		dd	quest_decide_rout		;decide whether to skip kill	: These 2 taken out as they are
		dd	quest_kill_obj_holder_rout			;kill person with object	: dealt with in pickup_particular
		dd	quest_pickup_rout		;pickup objects dropped		: JS 25/7/95
		dd	quest_cancel
	REPT	8
		dd	quest_panic
	ENDM

;-------------------------------------------------------------------------
destroy_quest	dd	quest_wake_up_rout
		dd	quest_init_take_rout		;JS 24/7/95
		dd	quest_store_charac_quest_rout
		dd	quest_find_rout
		dd	quest_goto_rout
		dd	quest_reset_maybe_rout
		dd	quest_move_inside_maybe_rout
		dd	quest_is_about_to_destroy_rout
		dd	quest_delay_1_frame_rout
		dd	quest_delay_1_frame_rout
		dd	quest_decide_rout
		dd	quest_kill_obj_holder_rout
		dd	quest_pickup_rout		;pickup objects dropped (will start fight, maybe)
		dd	quest_destroy_object
		dd	quest_cancel
	REPT	8
		dd	quest_panic
	ENDM

rest_quest	dd	quest_do_nothing
	dd	quest_cancel	;THIS IS A FUDGE

	REPT	8
		dd	quest_panic
	ENDM

;-------------------------------------------------------------------------

	ENDS

;-------------------------------------------------------------------------

	SEGMENT _TEXT PARA PUBLIC USE32 'CODE'
	ASSUME CS:_TEXT,DS:DGROUP,SS:NOTHING

;-------------------------------------------------------------------------

EXTRN	modrandn:NEAR
EXTRN	is_enemy:NEAR
EXTRN	set_border:NEAR
EXTRN	c_set_border_:NEAR
EXTRN	pickup_sub:NEAR
EXTRN	link_first:NEAR
EXTRN	link_first:NEAR
EXTRN	is_visible:NEAR
EXTRN	are_friends:NEAR
EXTRN	add_2_party:NEAR
EXTRN	check_links:NEAR
EXTRN	delete_link:NEAR
EXTRN	drop_weapon:NEAR
EXTRN	are_enemies:NEAR
EXTRN	party_member:NEAR
EXTRN	party_shelter:NEAR
EXTRN	link_map_scan:NEAR
EXTRN	start_fighting:NEAR
EXTRN	cancel_journey:NEAR
EXTRN	check_collision:NEAR
EXTRN	barge_emergency:NEAR
EXTRN	in_same_kingdom:NEAR
EXTRN	enter_proximity:NEAR
EXTRN	ruler_at_citadel:NEAR
EXTRN	fight_sim_battle:NEAR
EXTRN	load_combat_head:NEAR
EXTRN	kingdom_neighbour:NEAR
EXTRN	delete_from_party:NEAR
EXTRN	pickup_particular:NEAR
EXTRN	link_into_citadel:NEAR
EXTRN	shelter_proximity:NEAR
EXTRN	move_player_inside:NEAR
EXTRN	regions_neighbours:NEAR
EXTRN	make_info_register:NEAR
EXTRN	do_water_test_both:NEAR
EXTRN	set_emergency_quest:NEAR
EXTRN	delete_from_citadel:NEAR
EXTRN	make_quest_register:NEAR
EXTRN	do_water_test_either:NEAR
EXTRN	check_dark_fey_quest:NEAR
EXTRN	move_player_inside_ok:NEAR
EXTRN	get_region_in_kingdom:NEAR
EXTRN	move_all_party_inside:NEAR
EXTRN	goodie_takes_dark_citadel:NEAR

EXTRN	TR_check_take:NEAR
EXTRN	TR_check_join:NEAR
EXTRN	TR_check_attack:NEAR
EXTRN	TR_check_recruit:NEAR

;-------------------------------------------------------------------------
;
; BASE ROUTINES FOR CONTROLLING QUESTS
;
;-------------------------------------------------------------------------
PUBLIC quest_control
PROC quest_control NEAR

	movzx	eax,[WORD obj_type+esi]
	mov	ebx,quest_flags
	bt	ebx,eax			;object can be on a quest ?
	jnc	@@x			;no

	test	[WORD esi+obj_flags2],really_inside_bit
	je	@@t
	test	[WORD esi+obj_flags],rest_bit
	je	@@1
@@t:
	test	[WORD obj_rstate+esi],-1 ;any resting to be done ?
	jns	@@x			;yes
@@1:
;	movsx	eax,[WORD obj_quest+esi] ;object quest type
	movzx	eax,[WORD obj_quest+esi] ;object quest type

	mov	ebx,eax			;copy quest type

;	and	ebx,ebx			;adjust quest data
;	jns	@@2			;no
	test	ebx,08000h
	je	@@2

;	neg	ebx			;adjust type value
	xor	ebx,08000h
@@2:
	mov	ebx,[quest_type+ebx*4]	;list of quest states
	movzx	ecx,[WORD obj_qstate+esi] ;relevant state index
	call	[DWORD ebx+ecx*4]	;branch to relevant controls
	jc	@@1			;more processing to do
@@x:
	ret

ENDP

PROC quest_delay_1_frame_rout NEAR
	inc	[WORD esi+obj_qstate]
	stc	;don't delay!
	ret
ENDP

PROC quest_is_about_to_recruit_rout NEAR
	call	set_about_to_stuff
	inc	[WORD esi+obj_qstate]
	clc
	ret
ENDP

PROC quest_is_about_to_join_rout NEAR
	call	set_about_to_stuff
	inc	[WORD esi+obj_qstate]
	clc
	ret
ENDP

PROC quest_is_about_to_kill_rout NEAR
	call	set_about_to_stuff
	inc	[WORD esi+obj_qstate]
	clc
	ret
ENDP

PROC quest_is_about_to_follow_rout NEAR
	call	set_about_to_stuff
	inc	[WORD esi+obj_qstate]
	clc
	ret
ENDP

PROC quest_is_about_to_seize_rout NEAR
	call	set_about_to_stuff
	inc	[WORD esi+obj_qstate]
	clc
	ret
ENDP

PROC quest_is_about_to_pickup_rout NEAR
	call	set_about_to_stuff
	inc	[WORD esi+obj_qstate]
	clc
	ret
ENDP

PROC quest_is_about_to_take_rout NEAR
	call	set_about_to_stuff
	inc	[WORD esi+obj_qstate]
	clc
	ret
ENDP

PROC quest_is_about_to_destroy_rout NEAR
	call	set_about_to_stuff
	inc	[WORD esi+obj_qstate]
	clc
	ret
ENDP

PUBLIC set_about_to_stuff
PROC set_about_to_stuff NEAR
	test	[advance_flag],-1
	jne	@@doit

	cmp	esi,[controlled_addr]
	je	@@x
@@doit:
	call	get_quest_target_address

	mov	eax,qr_about_to
	call	make_quest_register
@@x:
	ret
ENDP

;-------------------------------------------------------------------------
PROC quest_destroy_object NEAR
	movsx	eax,[WORD esi+obj_weap_link]
	cmp	eax,-1
	je	@@x		;need this drop out in case the pickup doesn't work JS 25/7/95
	mov	edi,[props_list+eax*4]
	call	drop_weapon
	or	[WORD edi+obj_char_flags],(dead_bit+hidden_bit)
	and	[WORD edi+obj_char_flags],NOT (knowledge_bit+location_bit)
@@x:
	inc	[WORD esi+obj_qstate]
	stc
	ret
ENDP

;-------------------------------------------------------------------------
PROC quest_pickup_rout NEAR
	mov	ax,[esi+obj_quest]
	and	ax,07fffh
	cmp	ax,_qdestroy
	jne	@@p

	movsx	eax,[WORD esi+obj_weap_link]	;if destroying, don't pick it up if we have that weapon
	cmp	eax,-1				;JS 13/7/95 am
	je	@@p

	cmp	ax,[esi+obj_qdat2]
	je	@@n
	
@@p:
	movzx	eax,[WORD esi+obj_qdat2]
	cmp	[BYTE esi+obj_qtype1],qt_char
	je	@@c
	movzx	eax,[WORD esi+obj_qdat4]
@@c:
	mov	edi,[props_list+eax*4]	;generate the object address JS 4/7/95

;	call	pickup_sub
	call	pickup_particular
	jnc	@@log			;no object to take!

;	movsx	edi,[WORD obj_weapon_owner+edi]
;	cmp	edi,-1

	mov	eax,qr_has_found
	call	make_quest_register
	inc	[WORD obj_qstate+esi]	;next quest process
	stc
	ret
@@log:
	message	'arrived at target with no object near'
@@n:
	inc	[WORD obj_qstate+esi]	;next quest process
	stc
	ret
ENDP

;-------------------------------------------------------------------------
PUBLIC get_quest_target_address
PROC get_quest_target_address NEAR	
	push	eax ebx
	movzx	eax,[WORD esi+obj_qdat1]
	movzx	ebx,[WORD esi+obj_qdat2]
	cmp	[BYTE esi+obj_qtype1],qt_char
	je	@@n
	movzx	eax,[WORD esi+obj_qdat3]
	movzx	ebx,[WORD esi+obj_qdat4]
	cmp	[BYTE esi+obj_qtype2],qt_char
	je	@@n
	message	'targ address required for non char.'
@@n:
	mov	eax,[list_list+eax*4]
	mov	edi,[eax+ebx*4]
	pop	ebx eax
	ret
ENDP

;-------------------------------------------------------------------------
; decide whether to kill person (take quest) or just take object
; add 1 to quest no if kill, else 2
PROC quest_decide_rout NEAR

	call	get_quest_target_address

	movsx	edi,[WORD edi+obj_weapon_owner]
	cmp	edi,-1			       	;if weapon has no owner, don't kill!
	je	@@nkill
	
	mov	edi,[cast_list+edi*4]		;owner ptr
	cmp	esi,edi
	je	@@nkill				;it's me!!! (especially for destroy)

	call	TR_check_take

	cmp	[quest_result],tr_take_fight	;need to fight for it
	je	@@kill

	cmp	[quest_result],tr_cancel	;it's all over
	clc
	je	@@x
@@nkill:
	inc	[WORD esi+obj_qstate]		;skip kill bit
@@kill:
	inc	[WORD esi+obj_qstate]
	stc
@@x:
	ret
ENDP

;-------------------------------------------------------------------------
PROC quest_check_not_dead_rout NEAR
	movzx	eax,[WORD obj_qdat1+esi]
	movzx	ebx,[WORD obj_qdat2+esi]
	cmp	[BYTE obj_qtype1+esi],qt_char
	je	@@n
	movzx	eax,[WORD obj_qdat3+esi]
	movzx	ebx,[WORD obj_qdat4+esi]
	cmp	[BYTE obj_qtype2+esi],qt_char
@@f:
	jne	@@f	;no character target
@@n:
	mov	eax,[list_list+eax*4]
	cmp	eax,offset cast_list
@@g:
    	jne	@@g

	mov	eax,[eax+ebx*4]
	test	[WORD eax+obj_char_flags],dead_bit
	je	@@c

	call	quest_cancel
	clc
	ret
@@c:
	inc	[WORD obj_qstate+esi]
	stc
	ret
ENDP

;-------------------------------------------------------------------------
PROC quest_check_not_dead_guard_rout NEAR
	movzx	eax,[WORD obj_qdat1+esi]
	movzx	ebx,[WORD obj_qdat2+esi]
	cmp	[BYTE obj_qtype1+esi],qt_char
	je	@@n
	movzx	eax,[WORD obj_qdat3+esi]
	movzx	ebx,[WORD obj_qdat4+esi]
	cmp	[BYTE obj_qtype2+esi],qt_char
@@f:
	jne	@@f	;no character target
@@n:
	mov	eax,[list_list+eax*4]
	cmp	eax,offset cast_list
@@g:
    	jne	@@g

	mov	eax,[eax+ebx*4]
	test	[WORD eax+obj_char_flags],dead_bit
	je	@@c

	add	[WORD obj_qstate+esi],2	;how to get from here to going back to start pos
	stc
	ret
@@c:
	inc	[WORD obj_qstate+esi]
	stc
	ret
ENDP

;-------------------------------------------------------------------------
; On Entry:	EAX	= Requested quest type
;		ESI	= Pointer to object data
;
; On Exit:	Carry set if more processing necessary
;
;-------------------------------------------------------------------------
PROC quest_store_target_rout NEAR
	call	quest_target
	mov	[esi+obj_qdat3],bx
	mov	[esi+obj_qdat4],cx	;store quest goto position

	mov	[BYTE esi+obj_qtype2],qt_goto

	inc	[WORD obj_qstate+esi]	;next quest process
	stc
	ret
ENDP

;need to copy target object into target coords
;-------------------------------------------------------------------------
PUBLIC quest_goto_init_rout
PROC quest_goto_init_rout NEAR
	cmp	[BYTE obj_qtype1+esi],qt_goto
	je	@@c

    	movzx	eax,[WORD obj_qdat1+esi]
	movzx	ebx,[WORD obj_qdat2+esi]
	mov	eax,[list_list+eax*4]
	mov	eax,[eax+ebx*4]
	mov	ebx,[obj_y+eax]
	mov	eax,[obj_x+eax]
	shr	eax,16
	shr	ebx,16
	mov	[obj_qdat1+esi],ax
	mov	[obj_qdat2+esi],bx
	mov	[BYTE obj_qtype1+esi],qt_goto

	mov	[obj_targ_x+esi],ax
	mov	[obj_targ_y+esi],bx
@@c:
	inc	[WORD obj_qstate+esi]	;next quest process
	stc
	ret
ENDP

;-------------------------------------------------------------------------
PUBLIC quest_move_outside_maybe_rout
PROC quest_move_outside_maybe_rout NEAR
	test	[WORD esi+obj_flags],inside_bit
	je	@@x

    	movzx	eax,[WORD esi+obj_qdat1]
	movzx	ebx,[WORD esi+obj_qdat2]
	cmp	[BYTE esi+obj_qtype1],qt_char
	je	@@c
    	mov	ax,[esi+obj_shelter_x]
	mov	bx,[esi+obj_shelter_y]
	cmp	ax,[esi+obj_qdat1]
	jne	@@mo
	cmp	bx,[esi+obj_qdat2]
	jne	@@mo
	jmp	@@x
@@c:
	mov	eax,[list_list+eax*4]
	mov	edi,[eax+ebx*4]
	test	[WORD edi+obj_flags],inside_bit
	jne	@@mo

    	movsx	eax,[WORD obj_shelter_num+edi]
	test	eax,-1
	js	@@mo

	cmp	ax,[esi+obj_shelter_num]	;same shelter!
	je	@@x
@@mo:
    	call	move_player_inside_ok
@@x:
	inc	[WORD obj_qstate+esi]	;next quest process
	stc
	ret
ENDP

;IN
; esi - pointer to character
; eax - pointer to building
;OUT
; eax - 0 : CANCEL QUEST
;     - 1 : START SEIGE
;     - 2 : ENTER
; building allegiance status may have been reset if carry set
; ie. the character inside will have changed his allegiance
PROC check_move_inside NEAR
	push	ebx ecx edx esi edi ebp
	mov	[quest_no_program],-1

	call	get_quest_target_address

	cmp	[WORD esi+obj_quest],_qkill
	jne	@@n1

	call	TR_check_attack

	mov	eax,MI_enter_building
	mov	bx,[quest_result]
	mov	cx,attack_enter
	bt	cx,bx
	jc	@@x

	mov	eax,MI_start_seige
	mov	bx,[quest_result]
	mov	cx,attack_seize
	bt	cx,bx
	jc	@@x

	mov	eax,MI_cancel_quest
	mov	bx,[quest_result]
	mov	cx,attack_cancel
	bt	cx,bx
	jc	@@x

@@e1:
	jmp	@@e1
@@n1:
	cmp	[WORD esi+obj_quest],_qjoin
	jne	@@n2

	call	TR_check_join

	mov	eax,MI_enter_building
	mov	bx,[quest_result]
	mov	cx,join_enter
	bt	cx,bx
	jc	@@x

	mov	eax,MI_start_seige
	mov	bx,[quest_result]
	mov	cx,join_seize
	bt	cx,bx
	jc	@@x

	mov	eax,MI_cancel_quest
	mov	bx,[quest_result]
	mov	cx,join_cancel
	bt	cx,bx
	jc	@@x
@@e2:
	jmp	@@e2
@@n2:
	cmp	[WORD esi+obj_quest],_qrecruit
	jne	@@n3

	call	TR_check_recruit
	mov	eax,MI_enter_building
	mov	bx,[quest_result]
	mov	cx,recruit_enter
	bt	cx,bx
	jc	@@x

	mov	eax,MI_start_seige
	mov	bx,[quest_result]
	mov	cx,recruit_seize
	bt	cx,bx
	jc	@@x

	mov	eax,MI_cancel_quest
	mov	bx,[quest_result]
	mov	cx,recruit_cancel
	bt	cx,bx
	jc	@@x
@@e3:
	jmp	@@e3
@@n3:
	cmp	[WORD esi+obj_quest],_qtake
	jne	@@n4

	call	TR_check_take
	mov	eax,MI_enter_building
	mov	bx,[quest_result]
	mov	cx,take_enter
	bt	cx,bx
	jc	@@x

	mov	eax,MI_start_seige
	mov	bx,[quest_result]
	mov	cx,take_seize
	bt	cx,bx
	jc	@@x

	mov	eax,MI_cancel_quest
	mov	bx,[quest_result]
	mov	cx,take_cancel
	bt	cx,bx
	jc	@@x
@@e4:
	jmp	@@e4
@@n4:
	mov	ebp,esi
	mov	ebx,eax
	call	is_enemy
@@x:
	mov	[quest_no_program],0

	pop	ebp edi esi edx ecx ebx
	ret
ENDP

;IN
; esi - pointer to bloke
; ebp - pointer to citadel
;OUT
; carry if citadel owner is a bastard
PROC check_not_enemy_owner NEAR
	cmp	[WORD ebp+obj_type],typ_building
	je	@@xc
	pushad
	mov	ebx,ebp
	mov	ebp,esi
	call	is_enemy
	popad
	ret
@@xc:
	clc
	ret
ENDP

;-------------------------------------------------------------------------
PROC quest_move_inside_maybe_rout NEAR
	test	[WORD esi+obj_flags],inside_bit
	jne	@@x

    	movzx	eax,[WORD esi+obj_qdat1]
	movzx	ebx,[WORD esi+obj_qdat2]
	cmp	[BYTE esi+obj_qtype1],qt_char
@@f:
	jne	@@f				;should be!
	mov	eax,[list_list+eax*4]
	mov	edi,[eax+ebx*4]
	test	[WORD edi+obj_flags],inside_bit
	je	@@x

	movsx	eax,[WORD edi+obj_shelter_num]
	test	eax,-1
	js	@@x

	mov	eax,[location_list+eax*4]
	cmp	[WORD eax+obj_type],typ_citadel
	je	@@c
    	cmp	[WORD eax+obj_type],typ_castle
	jne	@@n2
@@c:
	mov	ebp,eax		;preserve cit ptr

	call	check_move_inside
	cmp	eax,MI_cancel_quest
	jne	@@n1

@@cancel:
     	call	quest_cancel
	jmp	@@y
@@n1:
	cmp	eax,MI_start_seige
	jne	@@n2
	call	move_player_inside_ok
	jmp	@@x
@@n2:
	cmp	eax,MI_enter_building
	jne	@@n3
	call	check_not_enemy_owner
	jc	@@cancel
	call	move_player_inside_ok
	jmp	@@x
@@n3:
@@x:
	inc	[WORD obj_qstate+esi]	;next quest process
	stc
	ret
@@y:
	clc
	ret
ENDP

;we've reached the end of the goto bit
;do we want to enter a building
;-------------------------------------------------------------------------
PUBLIC quest_move_inside_rout
PROC quest_move_inside_rout NEAR
	test	[WORD esi+obj_flags2],autoq_bit
	jne	@@x

	cmp	[WORD esi+obj_purpose],DEFEND_HOMELAND
	jne	@@x
	cmp	[WORD esi+obj_reaction],RETURN_HOME
	jne	@@x

	test	[WORD esi+obj_flags],inside_bit		;already inside!
	je	@@c

	or	[WORD esi+obj_flags2],really_inside_bit	;rejigged this JS 11/8/95
    	jmp	@@x
@@c:
	mov	[no_messages],-1
	call	move_player_inside_ok
	mov	[no_messages],0
@@x:
	inc	[WORD esi+obj_qstate]
	stc
	ret
ENDP

;-------------------------------------------------------------------------
PROC quest_reset_meet_rout NEAR
	test	[WORD esi+obj_flags2],meet_autoq_bit
	je	@@x

	call	quest_cancel

	and	[WORD esi+obj_flags2],NOT meet_autoq_bit

	movzx	eax,[BYTE obj_start_land+esi]	;go home
	mov	edi,[citadel_list+eax*4]
	add	edi,offset citadel_list
	mov	bx,[edi+obj_num]
	mov	[WORD esi+obj_qdat1],_location
	mov	[esi+obj_qdat2],bx
	mov	[BYTE esi+obj_qtype1],qt_char
	mov	[WORD esi+obj_quest],_qgoto

	stc
	ret
@@x:
	inc	[WORD esi+obj_qstate]
	stc
	ret
ENDP

;wake up player when assigned a new quest
;-------------------------------------------------------------------------
PROC quest_wake_up_rout NEAR
	test	[WORD esi+obj_flags2],really_inside_bit
	je	@@x
	movsx	eax,[WORD esi+obj_shelter_num]
	test	eax,-1
	js	@@y
    	mov	edi,[location_list+eax*4]
	call	delete_from_citadel
@@y:
	and	[WORD esi+obj_flags2],NOT really_inside_bit
@@x:
	inc	[WORD esi+obj_qstate]
	stc
	ret
ENDP

;
; On Entry:	EAX	= Requested quest type
;		ESI	= Pointer to object data
;
; On Exit:	Carry set if more processing necessary
;
;-------------------------------------------------------------------------
PUBLIC quest_goto_rout
PROC quest_goto_rout NEAR

	call	quest_target		;BX/CX = X/Y target coords

	call	update_quest_target

	cmp	[BYTE obj_start_realm+esi],Marish
	jne	@@not_dark_fey

	cmp	[WORD obj_purpose+esi],DARK_FEY
	jne	@@not_dark_fey

	call	check_dark_fey_quest
	jc	@@x			;update quest stuff

@@not_dark_fey:
	mov	[BYTE obj_int_speed+esi],quest_speed
	mov	[BYTE obj_movemode+esi],goto_pos_mode

	movzx	eax,[WORD obj_quest+esi]
;	test	eax,08000h
;	jne	@@no_check_local

	and	eax,07fffh

	cmp	eax,_qgoto
	je	@@no_check_local

	cmp	eax,_qguard
	je	@@no_check_local

	cmp	eax,_qseize
	je	@@no_check_local

;	test	[WORD esi+obj_flags2],autoq_bit
;	je	@@no_check_local	;only bother with auto questing characters

	call	check_locality
	jc	@@x

@@no_check_local:
	call	reached_target		;target position reached ?
	jnc	@@x			;no

	test	[quest_cancelled],-1
	jne	@@end

	inc	[WORD obj_qstate+esi]	;next quest process
		IF	0
	mov	ax,[WORD obj_rest+esi]
	mov	[WORD obj_rest_count+esi],ax	;stop!
		ENDIF

;	cmp	[BYTE esi+obj_qtype2],qt_goto
;	je	@@pm
;     	cmp	[BYTE esi+obj_qtype2],qt_unknown
;	jne	@@y
;	cmp	[BYTE esi+obj_qtype1],qt_goto
;	jne	@@y
@@pm:

	movzx	eax,[WORD obj_quest+esi]
	and	eax,07fffh
	cmp	eax,_qgoto
	jne	@@y
		;don't need to know unless we're goto'ing
	mov	eax,qr_reached_target
	call	make_quest_register
@@y:
	stc
@@x:
	ret
@@end:
	mov	[quest_cancelled],0
	clc
	ret
ENDP

;-------------------------------------------------------------------------
;IN
;  esi - character pointer
;OUT
;  Carry flag set if route revaluation required
;
;  While we're goto'ing check to see if we go into a neighbouring
;  land to our real target. If we do we need to exit and
;  re-evaluate our route finding status
;
;  All registers preserved
;
PROC check_locality NEAR
	pushad

	cmp	[WORD esi+obj_find_status],fs_region
	je	@@no_restart
	cmp	[WORD esi+obj_find_status],fs_know
	je	@@no_restart		;got 'im in our sights

;-------------------------------
; don't know exactly where we are going (yet!) so
; set up a new goto quest either with fixed location
; or fixed character target

	movzx	ebx,[WORD obj_qdat1+esi]	;list index
	movzx	ecx,[WORD obj_qdat2+esi]	;index number
	cmp	[BYTE obj_qtype1+esi],qt_char
;	je	@@nc2
	je	@@err		;these SHOULD be coordinates!
	cmp	[BYTE obj_qtype1+esi],qt_goto
	jne	@@err

	movzx	ebx,[WORD obj_qdat3+esi]	;list index
	movzx	ecx,[WORD obj_qdat4+esi]	;index number
	cmp	[BYTE obj_qtype2+esi],qt_char
	jne	@@err
@@nc2:
	mov	edi,[list_list+ebx*4]
	mov	edi,[edi+ecx*4]			;edi = address of object to find

	mov	[find_status],fs_know

;	test	[WORD obj_char_flags+edi],knowledge_bit	;do we know about the object?
;	jne	@@know_about_object		;yep

	cmp	[WORD edi+obj_type],typ_object
	jne	@@notobj
	movsx	edx,[WORD obj_weapon_owner+edi]
	cmp	edx,-1
	je	@@notobj
	mov	edx,[props_list+edx*4]
	test	[WORD edx+obj_char_flags],location_bit
	jne	@@know_about_object

@@notobj:
	test	[WORD obj_char_flags+esi],recruit_bit+major_bit
	jne	@@do_alliance_know

	cmp	ebx,_prop			;others know about everything except objects
	jne	@@know_about_object
	jmp	@@dunno

@@do_alliance_know:
	test	[WORD edi+obj_char_flags],location_bit
	jne	@@know_about_object		;we know all about it

;-------------------------------		;nope
; We don't know where the object is
; so try to search for it.
@@dunno:
	mov	[find_status],fs_region

	mov	ecx,[obj_x+edi]			;dest position
	mov	edx,[obj_y+edi]
	not	edx
	shr	ecx,16+9
	shr	edx,16+9
	shl	edx,7+1

	mov	ax,[map_regions+ecx*2+edx]	;dest region

	mov	ecx,[obj_x+esi]			;our position
	mov	edx,[obj_y+esi]
	not	edx
	shr	ecx,16+9
	shr	edx,16+9
	shl	edx,7+1

	mov	bx,[map_regions+ecx*2+edx]

	and	al,0fh	;JS 3/8/95
	and	bl,0fh

	cmp	bh,ah				;in same region?
	jne	@@not_in_same_region		;(region == land)

;-------------------------------
; If in same region or object known about
; then make a b-line for it
@@know_about_object:
	mov	eax,0
  	jmp	@@x

@@not_in_same_region:
;------------------------------
; If not in same region then see if in a
; neighbouring region
;
; IN
; bh : our region
; ah : dest region
	
	mov	[find_status],fs_neighbour

	call	regions_neighbours
	jnc	@@not_in_neighbouring_regions

	jmp	@@x

;---------------------------------
; If in same kingdom then pick a region in it at random
@@not_in_neighbouring_regions:

	mov	[find_status],fs_kingdom

	call	in_same_kingdom
	jnc	@@not_in_same_kingdom
	jmp	@@x

;---------------------------------
; Pick a neighbour kingdom at random and go to a region there
@@not_in_same_kingdom:

	mov	[find_status],fs_dont_know
	jmp	@@x

;---------------------------------
; A full quest is already set
; with a character list pointer
@@we_already_know:
	inc	[WORD obj_qstate+esi]		;next quest process
	stc
	jmp	@@we_already_know		;error! Bad quest status

;---------------------------------
; Phew!
@@x:
	mov	ax,[find_status]
	cmp	[esi+obj_find_status],ax
	jbe	@@no_restart

	;we need to set up a new quest : eax is the new quest subject
	;(provided it is not zero)
	cmp	eax,0
	je	@@c

	;set up the quest here...
	;actually, don't set it up,
	;it will be re-evaluated almost immediately
	;by quest_find_rout
@@c:
	inc	[WORD obj_qstate+esi]	;next quest process
	stc
	jmp	@@z	
@@no_restart:
	clc
@@z:
	popad
	ret

@@err:	mov	eax,03f3f00h			;lello!
	call	set_border
@@err2:
	message	'invalid shelter selected A'
	jmp	@@err2				;character info lost!
ENDP

;-------------------------------------------------------------------------
PUBLIC quest_restore_goto_rout
PROC quest_restore_goto_rout NEAR
	test	[WORD obj_char_flags+esi],dead_bit
	jne	@@set_cancel_quest

;	test	[WORD esi+obj_quest],-1
;	js	@@ok
;     	neg	[WORD esi+obj_quest]
	test	[WORD esi+obj_quest],08000h
	jne	@@ok
	xor	[WORD esi+obj_quest],08000h
@@ok:
	mov	bx,[esi+obj_qdat3]
	mov	[esi+obj_qdat1],bx
	mov	bx,[esi+obj_qdat4]
	mov	[esi+obj_qdat2],bx		;restore quest goto position

	mov	bl,[esi+obj_qtype2]
	mov	[esi+obj_qtype1],bl		;copy type flag

;	mov	[BYTE esi+obj_qtype2],qt_not_defined	;clear so we are goto'ing again!

	inc	[WORD obj_qstate+esi]		;next quest process
	stc
	ret

@@set_cancel_quest:
	call	quest_cancel			;if player dead cancel their quest!
	clc
	ret
ENDP

;-------------------------------------------------------------------------
PUBLIC quest_skip_maybe_rout
PROC quest_skip_maybe_rout NEAR

;	test	[WORD obj_quest+esi],-1
;	jns	@@skip
	test	[WORD obj_quest+esi],08000h
	je	@@skip
	
;------------------------------
; We haven't found a character to go to
; so we will need to do another goto
; quest before continuing
	inc	[WORD obj_qstate+esi]		;next quest process
	jmp	@@x

;------------------------------
; We're now locked onto a character (at last)
; so we can skip the following goto state
;
@@skip:
	add	[WORD obj_qstate+esi],2		;next quest process
@@x:
	stc
	ret
ENDP

;-------------------------------------------------------------------------
PUBLIC quest_reset_maybe_rout
PROC quest_reset_maybe_rout NEAR

;	test	[WORD obj_quest+esi],-1
;	jns	@@continue
	test	[WORD obj_quest+esi],08000h
	je	@@continue

;---------------------------------
; Flipping Warts! Haven't we located him yet
; reset the find quest and start again.
	call	restore_charac_quest_rout	;begin with the first quest
	call	quest_restart_rout		;go back and do it all again

	jmp	@@x
;---------------------------------
; Hooray! We're heading for a blokey
; we can just skip to the next stage
@@continue:
;	mov	ax,[esi+obj_qdat3]
;	mov	bx,[esi+obj_qdat4]
;	mov	[esi+obj_qdat1],ax
;	mov	[esi+obj_qdat2],bx
;	mov	al,[esi+obj_qtype2]
;	mov	[esi+obj_qtype1],ax
;	mov	[esi+obj_qtype2],qt_not_defined
;	mov	[WORD esi+obj_guide_type],0

	inc	[WORD obj_qstate+esi]		;next quest process
@@x:
    	stc
	ret
ENDP

;-------------------------------------------------------------------------
PUBLIC quest_store_charac_quest_rout
PROC quest_store_charac_quest_rout NEAR
;	test	[WORD obj_quest+esi],-1
;	js	@@fuckup
	test	[WORD obj_quest+esi],08000h
	jne	@@fuckup

	cmp	[BYTE esi+obj_qtype1],qt_char
	jne	@@x

	mov	ax,[WORD obj_qdat1+esi]
	mov	[WORD obj_qdat3+esi],ax
	mov	ax,[WORD obj_qdat2+esi]
	mov	[WORD obj_qdat4+esi],ax		;store quest info

	mov	al,[esi+obj_qtype1]
	mov	[esi+obj_qtype2],al		;copy type
@@x:
	inc	[WORD obj_qstate+esi]		;next quest process
	stc
	ret

@@fuckup:
;	message	'bad quest status #1'
;	nop					;error! Bad quest status
;	jmp	@@fuckup
	call	set_emergency_quest
	clc
	ret
ENDP

;-------------------------------------------------------------------------
PUBLIC restore_charac_quest_rout
PROC restore_charac_quest_rout NEAR
	mov	ax,[WORD obj_qdat3+esi]
	mov	[WORD obj_qdat1+esi],ax
	mov	ax,[WORD obj_qdat4+esi]
	mov	[WORD obj_qdat2+esi],ax		;copy quest

	mov	al,[esi+obj_qtype2]
	mov	[esi+obj_qtype1],al		;copy quest type

;	mov	[BYTE esi+obj_qtype2],qt_not_defined

;	inc	[WORD obj_qstate+esi]		;next quest process : NONONONONONO!

;	test	[WORD obj_quest+esi],-1
;	jns	@@ok
;     	neg	[WORD obj_quest+esi]		;ensure obj_quest +ve
	test	[WORD obj_quest+esi],08000h
	je	@@ok
	xor	[WORD obj_quest+esi],08000h
@@ok:
	stc
	ret
ENDP

;-------------------------------------------------------------------------
;change quest for object to quest for person holding object
PROC quest_init_take_rout NEAR

		IF	1

;keep the quest subject as the weapon JS 25/7/95
	inc	[WORD esi+obj_qstate]
	stc
	ret

		ELSE

	cmp	[WORD esi+obj_qdat1],_prop
	jne	@@x

;---------------------------------
;find out who is holding this prop
	movzx	eax,[WORD esi+obj_qdat1]
	movzx	ebx,[WORD esi+obj_qdat2]
	mov	eax,[list_list+eax*4]
	mov	eax,[eax+ebx*4]
	mov	ax,[eax+obj_weapon_owner]

	cmp	ax,-1	;JS 24/7/95 update to use with destroy
	je	@@x

;---------------------------------
;and make them the subject of the quest
	mov	[WORD esi+obj_qdat1],_cast
	mov	[WORD esi+obj_qdat2],ax
	mov	[BYTE esi+obj_qtype1],qt_char
@@x:
	inc	[WORD obj_qstate+esi]
	stc
	ret
		ENDIF
ENDP

;--------------------------------------------------
; This is the basis of all quests
; If entered with obj_quest -ve then a definate
; destination is already specified in obj_qdat#
; If so then the target position will remain
; unchanged.
; If not then the region & kingdom maps along with the
; adjacency & containment tables are used to provide
; a new destination.
; If the object is known about or found then a target is
; provided which will definately reach the desired object
; Otherwise a fixed pseudo-random location is set which may
; or may not provide a sensible route to the object.

; On Entry:	EAX	= Requested quest type
;		ESI	= Pointer to object data
;
; James, September 1994
;
;-------------------------------------------------------------------------
PUBLIC quest_find_rout
PROC quest_find_rout NEAR
;	inc	[WORD obj_qstate+esi]		;next quest process
;	stc
;	ret

;	test	[WORD obj_quest+esi],-1		;if obj_quest is -ve then
;	js	@@we_already_know		;we already have a set target
	test	[WORD obj_quest+esi],08000h
	jne	@@we_already_know		;will lock up (deliberately) if this is true!

	cmp	[WORD esi+obj_find_status],fs_region
	je	@@know_about_object
	cmp	[WORD esi+obj_find_status],fs_know
	je	@@know_about_object		;got 'im in our sights

;-------------------------------
; don't know exactly where we are going (yet!) so
; set up a new goto quest either with fixed location
; or fixed character target

	movzx	ebx,[WORD obj_qdat1+esi]	;list index
	movzx	ecx,[WORD obj_qdat2+esi]	;index number
	cmp	[BYTE esi+obj_qtype1],qt_char
	je	@@ccc
	movzx	ebx,[WORD obj_qdat3+esi]	;list index
	movzx	ecx,[WORD obj_qdat4+esi]	;index number
	cmp	[BYTE esi+obj_qtype2],qt_char
@@cde:
	jne	@@cde
@@ccc:
	mov	edi,[list_list+ebx*4]
	mov	edi,[edi+ecx*4]			;edi = address of object to find

	mov	[WORD esi+obj_find_status],fs_know

;	test	[WORD obj_char_flags+edi],knowledge_bit	;do we know about the object?
;	jne	@@know_about_object		;yep

	cmp	[WORD edi+obj_type],typ_object
	jne	@@notobj
	movsx	edx,[WORD obj_weapon_owner+edi]
	cmp	edx,-1
	je	@@notobj
	mov	edx,[props_list+edx*4]
	test	[WORD edx+obj_char_flags],location_bit
	jne	@@know_about_object

@@notobj:
	test	[WORD obj_char_flags+esi],recruit_bit+major_bit
	jne	@@do_alliance_know

	cmp	ebx,_prop			;others know about everything except objects
	jne	@@know_about_object
	jmp	@@dunno

@@do_alliance_know:
	test	[WORD edi+obj_char_flags],location_bit
	jne	@@know_about_object		;we know all about it

;-------------------------------		;nope
; We don't know where the object is
; so try to search for it.
@@dunno:
	mov	[WORD esi+obj_find_status],fs_region

	mov	ecx,[obj_x+esi]			;our position
	mov	edx,[obj_y+esi]
	not	edx
	shr	ecx,16+9
	shr	edx,16+9
	shl	edx,7+1

	mov	bx,[map_regions+ecx*2+edx]	

@@got_my_position:

	mov	ecx,[obj_x+edi]			;dest position
	mov	edx,[obj_y+edi]
	not	edx
	shr	ecx,16+9
	shr	edx,16+9
	shl	edx,7+1

	mov	ax,[map_regions+ecx*2+edx]	;dest land

	and	al,0fh				;al/bl == kingdom
	and	bl,0fh

	cmp	bh,ah				;in same land?
	jne	@@not_in_same_region		;not in same land

;-------------------------------
; If in same land or object known about
; then make a b-line for it using original quest data
@@know_about_object:
	mov	[WORD esi+obj_find_status],fs_know


	or	[WORD edi+obj_char_flags],knowledge_bit+location_bit	;JS 3/8/95	;we now know where it is
	cmp	[WORD edi+obj_type],typ_object
	jne	@@not
      	movsx	edi,[WORD edi+obj_weapon_owner]
	cmp	edi,-1
	je	@@not
	mov	edi,[cast_list+edi*4]
	or	[WORD edi+obj_char_flags],knowledge_bit+location_bit	;JS 3/8/95	;we now know where weapon owner is
@@not:

						;this gets us out of the looping stage
	call	restore_charac_quest_rout	;reset the main quest
	inc	[WORD obj_qstate+esi]		;next quest process
	stc

  	jmp	@@z

@@not_in_same_region:
;------------------------------
; If not in same region then see if in a
; neighbouring region
;
; IN
; bh : our region
; ah : dest region

; bl : our realm
; al : their realm

;--- JS 3/8/95
	cmp	al,bl
	jne	@@not_both_on_sea
	cmp	al,GreatOcean
	je	@@know_about_object

@@not_both_on_sea:
;---
	mov	[WORD esi+obj_find_status],fs_neighbour

	call	regions_neighbours
	jnc	@@not_in_neighbouring_regions

	;find neighbouring regions citadel/castle and head for it

;--- JS 3/8/95
	cmp	al,GreatOcean
	je	@@know_about_object		;neighbour land is in ocean!

	cmp	ah,25
	jb	@@know_about_object		;neighbour land is water

	cmp	ah,Corlon
	je	@@know_about_object		;neighbour land is Corlon

	cmp	ah,Elormane
	je	@@know_about_object		;neighbour land is Elormane	
;--- 

	movzx	eax,ah
	mov	eax,[citadel_list+eax*4]
	add	eax,offset citadel_list
	jmp	@@x

;---------------------------------
; If in same kingdom then pick a region in it at random
@@not_in_neighbouring_regions:

	mov	[WORD esi+obj_find_status],fs_kingdom

	call	in_same_kingdom
	jnc	@@not_in_same_kingdom

	;find any regions citadel/castle and head for it

	cmp	ah,GreatOcean
	je	@@know_about_object		;Both in the sea, go to object

	movzx	eax,bh
	call	get_region_in_kingdom		;returns a random region in kingdom in eax 
	jnc	@@no_regions			;(not water or Corlon or Elormane).

	mov	eax,[citadel_list+eax*4]
	add	eax,offset citadel_list
	jmp	@@x

@@no_regions:
	;pick a random region!
	mov	ebx,14
	call	modrandn
	call	get_region_in_kingdom		;return non-watery region
	jnc	@@no_regions
	mov	eax,[citadel_list+eax*4]
	add	eax,offset citadel_list
	jmp	@@x

;---------------------------------
; Pick a neighbour kingdom at random and go to a region there
@@not_in_same_kingdom:

	mov	[WORD esi+obj_find_status],fs_dont_know

	movzx	eax,bl
	call	kingdom_neighbour		;eax = which citadel to aim at next
	jnc	@@no_regions			;panic condition

	cmp	ebx,GreatOcean			;if target realm is great ocean, goto object
	je	@@know_about_object

	mov	eax,[citadel_list+eax*4]	;region eax is in range
	add	eax,offset citadel_list
	jmp	@@x
	
;---------------------------------
; A full quest is already set
; with a character list pointer
@@we_already_know:
	inc	[WORD obj_qstate+esi]		;next quest process
	stc
	message	'error #1'
	jmp	@@we_already_know		;error! Bad quest status

;---------------------------------
; Phew!
;we've got here and eax is pointing at the citadel
;we want to aim at next
@@x:
	call	cancel_journey

	push	eax

	mov	ebx,[obj_y+eax]
	mov	eax,[obj_x+eax]
	shr	ebx,16
	shr	eax,16
	mov	[obj_qdat1+esi],ax
	mov	[obj_qdat2+esi],bx
	mov	[BYTE obj_qtype1+esi],qt_goto
	or	[WORD obj_quest+esi],08000h	;JS 26/4/95

	pop	eax

;--- JS 6/8/95 : pretend i've arrived if i'm too close to cit
	call	do_prox_check
	jnc	@@c

	mov	ecx,[obj_x+eax]
	mov	edx,[obj_y+eax]
	not	edx
	shr	ecx,16+9
	shr	edx,16+9
	shl	edx,7+1
	mov	bx,[map_regions+ecx*2+edx]
	jmp	@@got_my_position
@@c:
	inc	[WORD obj_qstate+esi]
	stc
;we got to here and it's already set up etc.
@@z:
	ret
ENDP

;--- JS 6/8/95
PROC do_prox_check NEAR
	pushad
	mov	ax,[esi+obj_qdat1]
	mov	bx,[esi+obj_qdat2]
	push	[WORD esi+obj_targ_x] [WORD esi+obj_targ_y]
	mov	[esi+obj_targ_x],ax
	mov	[esi+obj_targ_y],bx
	call	reached_target
	pop	[WORD esi+obj_targ_y] [WORD esi+obj_targ_x]
	popad
	ret
ENDP

;-------------------------------------------------------------------------
PROC quest_init_guard_rout NEAR
	cmp	[BYTE esi+obj_qtype1],qt_goto
	je	@@x
;-----------------------------------
; Must make any other type of quest into a goto
; type quest for this to make any sense
	movzx	eax,[WORD esi+obj_qdat1]
	movzx	ebx,[WORD esi+obj_qdat2]
	mov	eax,[list_list+eax*4]
	mov	eax,[eax+ebx*4]
	mov	ebx,[eax+obj_y]
	mov	eax,[eax+obj_x]
	shr	eax,16
	shr	ebx,16
	mov	[esi+obj_qdat1],ax
	mov	[esi+obj_qdat2],bx
	mov	[BYTE esi+obj_qtype1],qt_goto
@@x:
	inc	[WORD obj_qstate+esi]
	stc
	ret
ENDP

; On Entry:	EAX	= Requested quest type
;		ESI	= Pointer to object data
;
;-------------------------------------------------------------------------
PUBLIC quest_guard_rout
PROC quest_guard_rout NEAR

	and	[WORD obj_flags+esi],NOT anim_bit	;stop character walking
	mov	[BYTE obj_int_speed+esi],0	;and moving
	mov	[WORD obj_speed+esi],0

	mov	ebx,[obj_x+esi]			;position of this character
	mov	ecx,[obj_y+esi]

	shr	ebx,16+7+2
	shr	ecx,16+7+2

	xor	ebp,ebp

@@link_loop:
	push	ebx ecx

	add	bx,[link_offsets+ebp*4]
	add	cx,[link_offsets+ebp*4+2]	;offset to current link

	cmp	bx,link_size
	ja	@@next_link

	cmp	cx,link_size
	ja	@@next_link			;check is a valid link


	shl	ecx,(7+2)			;y * 128 * sizeof(DWORD)

	mov	edi,[linkmap+ebx*4+ecx]		;list of objects in this square
	and	edi,edi
	je	@@next_link			;none
@@obj_loop:

	;check how far away

;	nop					;already in char's square!

	;check if character

	cmp	esi,edi	  			;not this character! (don't attack self)
	je	@@next_obj

	movzx	eax,[WORD obj_type+edi]

	cmp	eax,typ_character		;character?
	je	@@chk_friendly

;	cmp	eax,typ_citizen			;citizen?
;	je	@@chk_friendly
;
;	cmp	eax,typ_warrior			;warrior?
;	je	@@chk_friendly

	jmp	@@next_obj

@@chk_friendly:
	;check on ground

	cmp	[BYTE obj_movetype+edi],man_type	;check walking
	jne	@@next_obj

	push	edi esi
	mov	esi,edi
	call	check_links			;object linked to another ?
	pop	esi edi
	jc	@@next_obj			;yes

	;check if friendly

;	test	[WORD obj_char_flags+edi],(friendly_bit+dead_bit)
;	jne	@@next_obj

	mov	ebp,edi
	call	are_enemies		;not enemies
	jnc	@@next_obj

	test	[WORD obj_char_flags+edi],dead_bit	;make sure not dead!
	jne	@@next_obj

	test	[WORD obj_flags+edi],inside_bit		;make sure outside
	jne	@@next_obj

	;check not already in combat etc.

	test	[WORD obj_flags+edi],(en_garde_bit+combat_bit)
	jne	@@next_obj

	;check not on a ship/dragon
	cmp	[WORD edi+obj_link_type],-1
	jne	@@next_obj

	;if close & char & non-friendly & not in battle
	;then do combat

	;how do I do this?
	;could tag on to the quest_kill_rout function?
	;fucking brilliant idea!

	movzx	eax,[WORD edi+obj_type]		;setup kill quest data
	movzx	eax,[WORD type_list+eax*2]
	mov	[esi+obj_qdat1],ax

	mov	ax,[edi+obj_num]
	mov	[esi+obj_qdat2],ax

	mov	[BYTE obj_qtype1+esi],qt_char

	pop	ecx ebx

;	test	[WORD obj_quest+esi],-1		;make quest type correct
;	jns	@@ns
;	neg	[WORD obj_quest+esi]
	test	[WORD obj_quest+esi],08000h
	je	@@ns
	xor	[WORD obj_quest+esi],08000h
@@ns:

	inc	[WORD obj_qstate+esi]		;next quest process
	stc
	jmp	@@y

@@next_obj:
	mov	edi,[obj_next_link+edi]
	and	edi,edi
	jne	@@obj_loop

@@next_link:
	pop	ecx ebx
	inc	ebp
	cmp	ebp,9
	jb	@@link_loop

@@x:						;done
	clc					;nothing to do : stay still!
@@y:
	ret
ENDP

;
; On Entry:	EAX	= Requested quest type
;		ESI	= Pointer to object data
;
; On Exit:	Carry set if more processing necessary
;
;-------------------------------------------------------------------------
PUBLIC quest_follow_rout
PROC quest_follow_rout NEAR

;--- added to allow follow quest to cancel if target is dead!
	movzx	ebx,[WORD obj_qdat1+esi]
	movzx	ecx,[WORD obj_qdat2+esi] ;objects quest data
	cmp	[BYTE obj_qtype1+esi],qt_char
	je	@@scx
	movzx	ebx,[WORD obj_qdat3+esi]
	movzx	ecx,[WORD obj_qdat4+esi] ;objects quest data
	cmp	[BYTE obj_qtype2+esi],qt_char
	jne	@@err
@@scx:
	mov	eax,[list_list+ebx*4]	;cast/props/location list
	mov	eax,[eax+ecx*4]		;address of object from list

	test	[WORD eax+obj_char_flags],dead_bit
	je	@@c
	inc	[WORD obj_qstate+esi]
	stc
	ret
@@c:
;--- JS 27/6/95
		call	quest_follow		;EBX/ECX = X/Y target coords
		call	update_quest_target

	sub	bx,[obj_targ_x+esi]
	sub	cx,[obj_targ_y+esi]
	movsx	ebx,bx
	movsx	ecx,cx
	imul	ebx,ebx
	imul	ecx,ecx
	add	ecx,ebx
	cmp	ecx,0500h	;(256*256)
	jb	@@n

;	je	@@n
;	cmp	cx,028h

	and	[WORD esi+obj_char_flags],NOT rest_bit
		mov	[BYTE obj_int_speed+esi],quest_speed
		mov	[BYTE obj_movemode+esi],goto_pos_mode
		clc
		ret
@@n:
	or	[WORD esi+obj_char_flags],rest_bit
	mov	[BYTE obj_int_speed+esi],0
	mov	[BYTE obj_movemode+esi],null_mode
	clc
	ret
@@err:
	message	'follow quest has no char target.'
	inc	[WORD esi+obj_qstate]
	stc
	ret
ENDP

;
; On Entry:	EAX	= Requested quest type
;		ESI	= Pointer to object data
;
; On Exit:	Carry set if more processing necessary
;

;need to swap context if this is a recruit, not a join quest
;-------------------------------------------------------------------------
PUBLIC quest_join_rout
PROC quest_join_rout NEAR
		mov	ebp,esi				;preserve my pointer

		cmp	[WORD esi+obj_link_type],-1	;if i'm riding a dragon / boat
		jne	@@cancelme			;that's it.

		movzx	ebx,[WORD obj_qdat1+esi]
		movzx	ecx,[WORD obj_qdat2+esi] 	;my char's quest data
		cmp	[BYTE obj_qtype1+esi],qt_char
		je	@@nc
		movzx	ebx,[WORD obj_qdat3+esi]
		movzx	ecx,[WORD obj_qdat4+esi]	;my char's quest data
		cmp	[BYTE obj_qtype2+esi],qt_char
@@err:							;lost my quest data
		jne	@@err
@@nc:
		mov	edi,[list_list+ebx*4]		;cast/props/location list
		mov	edi,[edi+ecx*4]			;address of dest char from list

	call	TR_check_join		;check if join is valid
	call	set_quest_record
	jnc	@@xx			;no, cancelled

;--- Check if complicated 'JOIN' necessary ---
@@c:
		cmp	[WORD obj_quest+edi],_qjoin 	;on a 'join' quest ?
		jne	@@1				;no

		movzx	eax,[WORD obj_qdat2+edi] 	;dest char's quest data
		cmp	[BYTE obj_qtype1+edi],qt_char
		je	@@cx
     		movzx	eax,[WORD obj_qdat4+edi]
		cmp	[BYTE obj_qtype2+edi],qt_char
@@err2:							;lost dest quest data
		jne	@@err2
@@cx:
		cmp	ax,[WORD obj_num+esi]		;trying to 'join' each other ?
		je	@@2				;yes

;--- Link character to fellowship ---
@@1:
	cmp	[WORD edi+obj_link_type],-1		;can't join someone on a boat or dragon
	jne	@@cancelthem				;JS 9/5/95

		call	add_2_party			;link new party member
		mov	eax,qr_joined
		call	make_quest_register
		jmp	@@y

;--- Find new leader for joint parties ---
@@2:
		mov	ax,[obj_rank+esi]
		cmp	ax,[obj_rank+edi]		;friend out-ranks ?
		jc	@@2a				;yes
		xchg	esi,edi
@@2a:
		push	esi
		mov	esi,edi
		call	quest_cancel			;cancel the quest of the one being joined
		pop	esi
@@3:
	cmp	[WORD edi+obj_link_type],-1		;can't join someone on a boat or dragon
	jne	@@cancelthem				;JS 9/5/95

		call	add_2_party

		mov	eax,qr_joined
		call	make_quest_register

		cmp	esi,ebp				;esi - joiner : edi - joined
		jne	@@x
							;I am the joiner
		inc	[WORD esi+obj_qstate]
		stc
		ret
@@x:							;I am not the joiner : I am the joined
		mov	[WORD esi+obj_qstate],join_quest_no
@@y:
		mov	esi,ebp
		inc	[WORD esi+obj_qstate]		;Need to go into party (_qunder) mode.
		stc
		ret

@@cancelthem:
	cmp	esi,[controlled_addr]
	jne	@@m
;	mov	ebx,0	;transport type
	call	get_other_transport_type
	call	setup_info_params
	mov	eax,it_couldnt_join
	call	make_info_register
    	jmp	@@cb
@@m:
	mov	eax,qr_couldnt_join
	call	make_quest_register
	jmp	@@cb

@@cancelme:
	cmp	esi,[controlled_addr]
	jne	@@ca
;	mov	ebx,0	;transport type
	call	get_my_transport_type
	call	setup_info_params
	mov	eax,it_no_join
	call	make_info_register
	jmp	@@cb
@@ca:
	call	get_target_address
	mov	eax,qr_no_join
     	call	make_quest_register
@@cb:
	mov	esi,ebp
	call	quest_cancel			;DON'T	JS 9/5/95
	clc
	ret
@@xx:
	stc		;start the new quest immediately JS 27/6/95
	ret
ENDP

;IN
;ebx - 0 if dragon, 1 if boat
;esi - character pointer
PROC setup_info_params NEAR
	call	get_target_address
	lea	edx,[info_param_block]
	movzx	eax,[BYTE esi+obj_port]
	mov	[edx],eax
	movzx	eax,[BYTE edi+obj_port]
	mov	[edx+4],eax
	mov	[DWORD edx+8],ebx
	mov	[info_params],edx
	ret
ENDP

PROC get_other_transport_type NEAR
	mov	ebx,0
	cmp	[WORD edi+obj_link_num],-1
	je	@@x
	movzx	eax,[WORD edi+obj_link_num]
	mov	eax,[beast_list+eax*4]
	cmp	[WORD obj_sprite+eax+beast_list],boat_spr
	jne	@@x
	mov	ebx,1
@@x:
	ret
ENDP

PROC get_my_transport_type NEAR
	push	edi
	mov	edi,esi
	call	get_other_transport_type
	pop	edi
	ret
ENDP

PROC will_partyer_be_recruited NEAR
	stc
	ret
ENDP

;need to swap context if this is a recruit, not a join quest
;-------------------------------------------------------------------------
PUBLIC quest_recruit_rout
PROC quest_recruit_rout NEAR
	cmp	[WORD esi+obj_link_type],-1		;can't recruit someone whilst I am on a boat or dragon
	jne	@@cancelme				;JS 11/5/95

		mov	ebp,esi				;ebp is my character

		movzx	ebx,[WORD obj_qdat1+esi]
		movzx	ecx,[WORD obj_qdat2+esi] 	;my quest data
		cmp	[BYTE obj_qtype1+esi],qt_char
		je	@@nc
		movzx	ebx,[WORD obj_qdat3+esi]
		movzx	ecx,[WORD obj_qdat4+esi] 	;my quest data
		cmp	[BYTE obj_qtype2+esi],qt_char
@@err:	
		jne	@@err				;my quest data is gone
@@nc:
		mov	edi,[list_list+ebx*4]		;cast/props/location list
		mov	edi,[edi+ecx*4]			;address of object from list

	call	TR_check_recruit
	call	set_quest_record
	jnc	@@xx

	cmp	[WORD edi+obj_link_type],-1		;can't recruit someone whilst they are on a boat or dragon
	jne	@@cancelthem				;JS 11/5/95

    		xchg	esi,edi				;recruit is just inverse of join!
							;(with a few caveats)
;--- Check if complicated 'RECRUIT' necessary
;edi - me
;esi - him
		cmp	[WORD esi+obj_quest],_qjoin
		jne	@@bc

     		cmp	[WORD esi+obj_qstate],join_quest_no
		jae	@@is_in_a_party
@@bc:
		cmp	[WORD esi+obj_quest],_qrecruit
		jne	@@1

@@is_complicated:
		movzx	eax,[WORD obj_qdat2+esi] 	;dest char's quest data
		cmp	[BYTE obj_qtype1+esi],qt_char
		je	@@cx2
     		movzx	eax,[WORD obj_qdat4+esi]
		cmp	[BYTE obj_qtype2+esi],qt_char
@@err2:							;no char data in dest character
		jne	@@err2
@@cx2:
		cmp	ax,[WORD obj_num+edi]		;trying to recruit each other?
		je	@@2				;yes

;--- Link character to fellowship ---
@@1:
		call	add_2_party
	IF	1
		call	quest_cancel
		mov	[WORD esi+obj_quest],_qjoin
		mov	[WORD esi+obj_qstate],join_quest_no
		mov	[WORD esi+obj_qdat1],_cast
		mov	ax,[edi+obj_num]
		mov	[esi+obj_qdat2],ax
		mov	[BYTE esi+obj_qtype1],qt_char
		mov	[BYTE esi+obj_qtype2],qt_not_defined
	ENDIF
		xchg	esi,edi
      		mov	eax,qr_recruited
		call	make_quest_register
		xchg	esi,edi
		jmp	@@x				;done

;--- Character to be recruited is already in a party
@@is_in_a_party:
		call	will_partyer_be_recruited
		jnc	@@x

		call	delete_from_party		;remove partyer from party
		jmp	@@1				;now join them

;--- Find new leader for joint parties ---
@@2:
		mov	ax,[obj_rank+esi]
		cmp	ax,[obj_rank+edi]		;friend out-ranks ?
		jc	@@3				;yes
		xchg	esi,edi
@@3:
		call	add_2_party

	IF	1
		call	quest_cancel
		mov	[WORD esi+obj_quest],_qjoin
		mov	[WORD esi+obj_qstate],join_quest_no
		mov	[WORD esi+obj_qdat1],_cast
		mov	ax,[edi+obj_num]
		mov	[esi+obj_qdat2],ax
		mov	[BYTE esi+obj_qtype1],qt_char
		mov	[BYTE esi+obj_qtype2],qt_not_defined
	ENDIF

		xchg	esi,edi
		mov	eax,qr_recruited
		call	make_quest_register
		xchg	esi,edi
		call	quest_cancel			;kill joining character's quest
@@x:
		mov	esi,ebp				;restore my pointer
		inc	[WORD obj_qstate+esi]		;next quest process

		stc
		ret

@@cancelthem:
	cmp	esi,[controlled_addr]
	jne	@@m
;   	mov	ebx,0	;transport type
	call	get_other_transport_type
	call	setup_info_params
	mov	eax,it_couldnt_recruit
	call	make_info_register
	jmp	@@cb
@@m:
	mov	eax,qr_couldnt_recruit
	call	make_quest_register
	jmp	@@cb

@@cancelme:
	cmp	esi,[controlled_addr]
	jne	@@ca
;	mov	ebx,0	;transport type
	call	get_my_transport_type
	call	setup_info_params
	mov	eax,it_no_recruit
	call	make_info_register
	jmp	@@cb
@@ca:
	call	get_target_address
	mov	eax,qr_no_recruit
	call	make_quest_register
@@cb:
		call	quest_cancel
		clc
		ret
@@xx:
	stc	;start new quest immediately JS 27/6/95
	ret
ENDP

;
; On Entry:	EAX	= Requested quest type
;		ESI	= Pointer to object data
;
; On Exit:	Carry set if more processing necessary
;
;-------------------------------------------------------------------------
PUBLIC quest_party_rout
PROC quest_party_rout NEAR
	cmp	eax,_qjoin			;on a join quest ?
	jne	@@1				;no

	movzx	ebx,[WORD obj_qdat1+esi]
	movzx	ecx,[WORD obj_qdat2+esi] 	;objects quest data

	cmp	[BYTE obj_qtype1+esi],qt_char
	je	@@scx
	movzx	ebx,[WORD obj_qdat3+esi]
	movzx	ecx,[WORD obj_qdat4+esi] 	;objects quest data
	cmp	[BYTE obj_qtype2+esi],qt_char
	jne	@@err
	
@@scx:
	mov	edi,[list_list+ebx*4]		;cast/props/location list
	mov	edi,[edi+ecx*4]			;address of object from list

	call	party_member			;is guy i'm following in the same party as me?
	jc	@@1				;yes

;--- Quit current party to join someone else ---
	call	delete_from_party
	mov	[WORD obj_qstate+esi],0		;reset quest procedure
	stc
	ret

;--- Follow colleagues party ---
@@1:
	mov	[BYTE obj_int_speed+esi],quest_speed
	mov	[BYTE obj_movemode+esi],party_mode

;--- Remove party member from collision avoidence ---
	call	reached_target

	test	[quest_cancelled],-1
	jne	@@end

;--- Move party members into leaders building if necessary ---
	call	party_shelter
	clc
	ret
@@end:
	mov	[quest_cancelled],0
	clc
	ret
@@err:
	message	'party character lost target'
	call	quest_cancel
	clc
	ret
ENDP

;
; On Entry:	EAX	= Requested quest type
;		ESI	= Pointer to object data
;
; On Exit:	BX	= Target X coordinate
;		CX	= Target Y coordinate
;
;-------------------------------------------------------------------------
PUBLIC quest_target
PROC quest_target NEAR

		push	edx

		movzx	ebx,[WORD obj_qdat1+esi]
		movzx	ecx,[WORD obj_qdat2+esi] ;objects quest data
		cmp	[BYTE obj_qtype1+esi],qt_goto
		je	@@3

		movzx	ebx,[WORD obj_qdat3+esi]
		movzx	ecx,[WORD obj_qdat4+esi] ;objects quest data
		cmp	[BYTE obj_qtype2+esi],qt_goto
		je	@@3

;		and	eax,eax			;coordinates supplied ?
;		js	@@3
			;yes
;		test	eax,08000h
;		jne	@@3

		
		movzx	ebx,[WORD obj_qdat1+esi]
		movzx	ecx,[WORD obj_qdat2+esi] ;objects quest data
		cmp	[BYTE obj_qtype1+esi],qt_char
		je	@@okkk
		
		movzx	ebx,[WORD obj_qdat3+esi]
		movzx	ecx,[WORD obj_qdat4+esi] ;objects quest data
		cmp	[BYTE obj_qtype2+esi],qt_char
		jne	@@err

@@okkk:
		mov	edx,[list_list+ebx*4]	;cast/props/location list
		mov	edx,[edx+ecx*4]		;address of object from list

;--- Send to end of party if on a join ---

		cmp	eax,_qjoin		;on a join quest ?
		jne	@@2			;no

@@0:		test	[WORD obj_next_party+edx],-1 ;last party member ?
		js	@@1			;yes

		movzx	edx,[WORD obj_next_party+edx]
		mov	edx,[cast_list+edx*4]	;address of objects data

		jmp	SHORT @@0

;--- Person to join in a building ? ---

@@1:
;		movsx	eax,[WORD obj_rstate+edx]

;		mov	[WORD obj_rstate+esi],bld_join

;		cmp	eax,bld_move_2		;enter building ?
;		je	@@2			;yes

;		cmp	eax,bld_rest		;enter building ?
;		je	@@2			;yes

;		mov	[WORD obj_rstate+esi],bld_term

;--- Get coordinates of last person in party ---

@@2:		mov	ebx,[obj_x+edx]
		mov	ecx,[obj_y+edx]		;object coordinates

		shr	ebx,16
		shr	ecx,16			;adjust coordinate scale

@@3:		pop	edx

		ret
@@err:
		jmp	@@err
ENDP

;
; On Entry:	EAX	= Requested quest type
;		ESI	= Pointer to object data
;
; On Exit:	EBX	= Target X coordinate
;		ECX	= Target Y coordinate
;
;-------------------------------------------------------------------------
PROC quest_follow NEAR

		movzx	ebx,[WORD obj_qdat1+esi]
		movzx	ecx,[WORD obj_qdat2+esi] ;objects quest data
		cmp	[BYTE obj_qtype1+esi],qt_char
		je	@@scx
		movzx	ebx,[WORD obj_qdat3+esi]
		movzx	ecx,[WORD obj_qdat4+esi] ;objects quest data
		cmp	[BYTE obj_qtype2+esi],qt_char
		jne	@@err
@@scx:
		mov	eax,[list_list+ebx*4]	;cast/props/location list
		mov	eax,[eax+ecx*4]		;address of object from list

;--- Check target positioning ---

		movsx	ebx,[WORD obj_prev_party+eax]
		movsx	ecx,[WORD obj_next_party+eax]

		mov	edx,ebx
		and	edx,ecx			;target is party member ?
		js	@@2			;no

@@1:		and	ecx,ecx			;last member of party ?
		js	@@2			;yes

		mov	eax,[cast_list+ecx*4]
		movsx	ecx,[WORD obj_next_party+eax]

		jmp	SHORT @@1

;--- Retrieve target coordinates ---

@@2:		mov	ebx,[obj_x+eax]
		mov	ecx,[obj_y+eax]		;object coordinates

		shr	ebx,16
		shr	ecx,16			;adjust coordinate scale

		ret
@@err:
		jmp	@@err
ENDP

	IF	0
;-------------------------------------------------------------------------
;IN  bx:cx - (x,y) of target
;OUT if target has moved the current journey has been cancelled
PUBLIC update_quest_target
PROC update_quest_target NEAR

		sub	bx,[obj_targ_x+esi]
		sub	cx,[obj_targ_y+esi]	;X/Y difference

		mov	ax,bx
		or	ax,cx			;coordinates changed ?
		je	@@x			;no

@@1:		add	[obj_targ_x+esi],bx
		add	[obj_targ_y+esi],cx	;set new target position

		call	cancel_journey		;delete previous route

@@x:		ret

ENDP
	ELSE

;Re-written : if the target is moving (which this is supposed to deal with)
;we don't need to replot every pixel, just every map square, seeing as that
;is the highest resolution we can plot a route to!
;If we don't do this, we're continually replotting routes we don't want/need to.
;JS 5/6/95

;IN  bx:cx - (x,y) of target
;OUT if target has moved the current journey has been cancelled
PUBLIC update_quest_target
PROC update_quest_target NEAR

deres_shift	=	7
	push	bx cx di bp ax

	mov	[temp_x],bx
	mov	[temp_y],cx

	shr	bx,deres_shift		;now at 9 bit res (map route res)
	shr	cx,deres_shift

	mov	di,[obj_targ_x+esi]
	mov	bp,[obj_targ_y+esi]
	shr	di,deres_shift		;now at 9 bit res
	shr	bp,deres_shift

;		sub	bx,[obj_targ_x+esi]
;		sub	cx,[obj_targ_y+esi]	;X/Y difference

	sub	bx,di		;X/Y map coordinate difference
	sub	cx,bp

		mov	ax,bx
		or	ax,cx			;coordinates changed ?
		je	@@x			;no

@@1:
;	shl	bx,deres_shift		;correct back to full size
;	shl	cx,deres_shift		;(we're losing some res here) (too much! JS 19/6/95)

	mov	bx,[temp_x]		;use full res JS 19/6/95
	mov	cx,[temp_y]

	sub	bx,[obj_targ_x+esi]
	sub	cx,[obj_targ_y+esi]	;tart!

	cmp	[WORD obj_targ_x+esi],-1
	jne	@@c
;	mov	eax,3f1f1fh
;	call	c_set_border_
	mov	bx,[temp_x]		;use full res JS 19/6/95
	mov	cx,[temp_y]
    	mov	[obj_targ_x+esi],bx	;check there's a targ to be updated JS 19/6/95
	mov	[obj_targ_y+esi],cx	;else we'll be out by 1
	jmp	@@d
@@c:
		add	[obj_targ_x+esi],bx
		add	[obj_targ_y+esi],cx	;set new target position
@@d:
		call	cancel_journey		;delete previous route

@@x:
	pop	ax bp di cx bx
		ret
;fuck me! it actually worked first time.
ENDP

	ENDIF
;-------------------------------------------------------------------------
PROC quest_do_nothing NEAR
		mov	[BYTE obj_movemode+esi],null_mode

		mov	[BYTE obj_int_speed+esi],0
		mov	[WORD obj_speed+esi],0

		clc
		ret
ENDP

;JS 29/7/95 - this is a major fudge
PUBLIC quest_kill_obj_holder_rout
PROC quest_kill_obj_holder_rout NEAR
	cmp	[WORD esi+obj_qdat1],_prop
	jne	@@no_setup

	call	save_quest_data_temp		;store object things

	call	get_quest_target_address
	movsx	edi,[WORD edi+obj_weapon_owner]
	mov	edi,[cast_list+edi*4]		;owner ptr
	mov	bx,[WORD edi+obj_num]	
	mov	[WORD esi+obj_qdat1],_cast
	mov	[WORD esi+obj_qdat2],bx
	mov	[BYTE esi+obj_qtype1],qt_char

@@no_setup:
	pushad
	call	quest_kill_rout
	popad

	cmp	[WORD esi+obj_quest],_qdestroy
	je	@@c
    	cmp	[WORD esi+obj_quest],_qtake
	jne	@@x				;no longer a take or destroy
@@c:
	test	[WORD esi+obj_flags],combat_bit+en_garde_bit
	jne	@@x
	call	restore_quest_data_temp		;restore object thingies
@@x:
	clc
	ret
ENDP

PROC save_quest_data_temp NEAR
	push	eax
	mov	ax,[WORD esi+obj_qdat1]
	mov	[qdt_qdat1],ax
	mov	ax,[WORD esi+obj_qdat2]
	mov	[qdt_qdat2],ax
	mov	al,[BYTE esi+obj_qtype1]
	mov	[qdt_qtype1],al
	pop	eax
	ret
ENDP

PROC restore_quest_data_temp NEAR
	push	eax
	cmp	[WORD esi+obj_qdat1],_prop
	je	@@x
	mov	ax,[qdt_qdat1]
	mov	[WORD esi+obj_qdat1],ax
	mov	ax,[qdt_qdat2]
	mov	[WORD esi+obj_qdat2],ax
	mov	al,[qdt_qtype1]
	mov	[BYTE esi+obj_qtype1],al
@@x:
	pop	eax
	ret
ENDP

;
; On Entry:	EAX	= Requested quest type
;		ESI	= Pointer to object data
;
; On Exit:	Carry set if more processing necessary
;
;-------------------------------------------------------------------------
PUBLIC quest_kill_rout
PROC quest_kill_rout NEAR

	cmp	[WORD esi+obj_link_type],-1		;can't join someone whilst on a boat or dragon
	jne	@@cancelme				;JS 9/5/95

	movzx	ebx,[WORD obj_qdat1+esi]
	movzx	ecx,[WORD obj_qdat2+esi] ;objects quest data
	cmp	[BYTE obj_qtype1+esi],qt_char
	je	@@scx
	movzx	ebx,[WORD obj_qdat3+esi]
	movzx	ecx,[WORD obj_qdat4+esi] ;objects quest data
	cmp	[BYTE obj_qtype2+esi],qt_char
	jne	@@err
@@scx:
	mov	edi,[list_list+ebx*4]	;cast/props/location list
	mov	edi,[edi+ecx*4]		;address of object from list

	cmp	[WORD edi+obj_link_type],-1		;can't join someone whilst on a boat or dragon
	jne	@@cancelthem				;JS 9/5/95

	test	[WORD obj_flags+esi],(en_garde_bit+combat_bit)	;Combat already started
	jne	@@1

	call	TR_check_attack		;check whether attack should occur.
	call	set_quest_record
	jnc	@@xx			;carry set if continue

;--- the very fact of attacking makes them enemies : make my allied realm an enemy of the attacked persons allied realm
;	movzx	ebx,[WORD obj_opponent+esi]
;	mov	ebx,[cast_list+ebx*4]
	movzx	ebx,[BYTE edi+obj_allegiance]
	movzx	eax,[BYTE esi+obj_allegiance]
	cmp	eax,ebx					;don't make realm enemy of self! JS 5/7/95
	je	@@noen
	region_mul	ebx
	bts	[WORD ebx+realm_data+realm_enemies],ax
@@noen:
	call	is_visible
	jc	@@visible

;--- invisible (instant) fight
	call	fight_sim_battle
	test	[WORD esi+obj_char_flags],dead_bit
	je	@@4	;i'm still alive (and he's dead) (continue)

;--- i'm dead (my quest has been cancelled)

	mov	eax,qr_has_been_killed
	call	make_quest_register

	jmp	@@3	;do no more

;--- visible fight	
@@visible:
	call	start_fighting		;initialise fight procedure

	jmp	SHORT @@2

@@1:	test	[WORD obj_char_flags+edi],dead_bit ;opponent dead ?
	jne	@@4			;yes

@@2:	test	[WORD obj_flags+esi],combat_bit ;fight started ?
	je	@@3			;no

	mov	[BYTE obj_int_speed+esi],0
;	mov	[BYTE obj_movemode+esi],party_mode	;WHY?? JS. 9/5/95
@@3:
	clc
	ret
@@4:
	inc	[WORD obj_qstate+esi]	;next quest process
	and	[WORD obj_flags+esi],NOT (combat_bit+en_garde_bit)

	mov	eax,qr_has_killed
	call	make_quest_register
@@5:
	stc
	ret
@@err:
	jmp	@@err

@@cancelthem:
	cmp	esi,[controlled_addr]
	jne	@@m
;    	mov	ebx,0	;transport type
	call	get_other_transport_type
	call	setup_info_params
	mov	eax,it_couldnt_kill
	call	make_info_register
	jmp	@@cb
@@m:
	mov	eax,qr_couldnt_kill
	call	make_quest_register
	jmp	@@cb

@@cancelme:
	cmp	esi,[controlled_addr]
	jne	@@ca
;	mov	ebx,0
	call	get_my_transport_type
	call	setup_info_params
	mov	eax,it_no_kill
	call	make_info_register
	jmp	@@cb
@@ca:
	call	get_target_address
	mov	eax,qr_no_kill
	call	make_quest_register
@@cb:
	inc	[WORD obj_qstate+esi]	;next quest process
	stc				;can't cancel cos this is used by guard_rout
	ret
@@xx:
	stc	;start new quest immediately JS 27/6/95
	ret
ENDP


;-------------------------------------------------------------------------
PUBLIC quest_seize_rout
PROC quest_seize_rout NEAR
;initialise & carry out a citadel seizure (we are at the citadel)
	cmp	[WORD esi+obj_link_type],-1		;can't join someone whilst on a boat or dragon
	jne	@@cancelme				;JS 9/5/95

	test	[seize_semaphore],-1
	jne	@@do_nothing

	test	[combat_flag],-1	;JS 29/6/95
	jne	@@do_nothing

	test	[WORD esi+obj_flags],seize_bit
	jne	@@itsbeensetup

	test	[WORD esi+obj_flags],inside_bit	;moved here JS 6/8/95
	jne	@@cancel_q		;Simplest way out of this is to abandon everything!!! JS 16/6/95

;--- setup the seige by moving in and setting the flags
;--- and starting the fight

	movzx	eax,[WORD obj_qdat1+esi]
	movzx	ebx,[WORD obj_qdat2+esi]
	cmp	[BYTE obj_qtype1+esi],qt_char
	je	@@c

;this is really an error condition.
	movzx	eax,[WORD obj_qdat3+esi]
	movzx	ebx,[WORD obj_qdat4+esi]
	cmp	[BYTE obj_qtype2+esi],qt_char
@@e1:
	jne	@@e1
@@c:
	mov	eax,[list_list+eax*4]
	mov	ebp,[eax+ebx*4]	     		;citadel address

;--- JS 26/6/95 make enemies due to attack
	movzx	ebx,[BYTE esi+obj_allegiance]	;attacker's realm
	movzx	ecx,[WORD ebp+obj_land_ruler]	;building  ruler
	mov	ecx,[cast_list+ecx*4]
	movzx	ecx,[BYTE ecx+obj_allegiance]	;building allegiance
	cmp	ebx,ecx				;don't make realm enemy of self JS 5/7/95
	je	@@noen
	region_mul	ecx
	bts	[WORD ecx+realm_data+realm_enemies],bx	;make attackers enemies of land realm
@@noen:
;---

;	mov	ax,[esi+obj_num]		;JS 25/5/95
;	cmp	ax,[ebp+obj_land_ruler]
;	je	@@done				;done later on

	cmp	[WORD obj_type+ebp],typ_citadel
	je	@@cc
     	cmp	[WORD obj_type+ebp],typ_castle
@@e2:
	jne	@@e2
@@cc:
	mov	eax,[obj_x+ebp]
	shr	eax,16
	mov	[obj_shelter_x+esi],ax
	mov	eax,[obj_y+ebp]
	shr	eax,16
	mov	[obj_shelter_y+esi],ax
	mov	ax,[obj_dir+ebp]
	mov	[obj_shelter_dir+esi],ax
	mov	ax,[obj_sprite+ebp]
	mov	[obj_shelter_typ+esi],ax
	mov	ax,[obj_num+ebp]
	mov	[obj_shelter_num+esi],ax

	cmp	[WORD esi+obj_type],typ_character
	jne	@@ntc
    	mov	[DWORD esi+obj_xi],028000h	;move to proper interior coords
    	mov	[DWORD esi+obj_yi],028000h	;JS 27/7/95
@@ntc:

	mov	eax,[obj_x+ebp]				;actually move them inside
	mov	ebx,[obj_y+ebp]
	call	delete_link
	mov	[obj_x+esi],eax
	mov	[obj_y+esi],ebx
	call	link_first

	or	[WORD esi+obj_flags2],really_inside_bit
	mov	[WORD esi+obj_rstate],bld_rest		;set correct rest state

	call	move_all_party_inside		;JS 27/7/95

	mov	ax,[esi+obj_num]		;JS 25/5/95
	cmp	ax,[ebp+obj_land_ruler]
	jne	@@not_owner

	or	[WORD esi+obj_flags],inside_bit
	mov	edi,ebp
	call	link_into_citadel
	jmp	@@done

@@not_owner:
	or	[WORD esi+obj_flags],combat_bit+seize_bit+inside_bit

	call	get_opponent_setup
	push	edi
	mov	edi,ebp
	call	link_into_citadel
	pop	edi

@@ruler_ok:
;edi - baddy char
;esi - my char

;--- setup who's attacking who! : esi is attacking ebp's owner
	mov	ax,[edi+obj_num]
	mov	[esi+obj_opponent],ax
	mov	ax,[esi+obj_num]
	mov	[edi+obj_opponent],ax

	or	[WORD edi+obj_flags],combat_bit+seize_bit

	call	is_visible
	jnc	@@invisible

	call	start_fighting
	or	[WORD esi+obj_flags],combat_bit
	or	[WORD edi+obj_flags],combat_bit

;	mov	eax,qr_has_begun_a_seige	;JS 4/8/95
;	call	make_quest_register

	clc
	ret

@@invisible:

	call	fight_sim_battle

	test	[WORD obj_char_flags+edi],dead_bit 	;opponent dead ?
	jne	@@4					;yes

;--- ok, I must be dead!
	call	clear_seize_sem
	and	[WORD obj_flags+esi],NOT (combat_bit+en_garde_bit+seize_bit)

	inc	[WORD esi+obj_qstate]
	stc
	ret

@@itsbeensetup:
	test	[WORD esi+obj_char_flags],dead_bit
	jne	@@5					;i'm dead

	movsx	eax,[WORD esi+obj_opponent]
	mov	edi,[cast_list+eax*4]

	test	[WORD obj_char_flags+edi],dead_bit 	;opponent dead ?
	jne	@@4					;yes
@@3:
	clc						;keep in here until either is dead!
	ret

;--- kill me off
@@5:
	call	clear_seize_sem
	and	[WORD obj_flags+esi],NOT (combat_bit+en_garde_bit+seize_bit)
	call	quest_cancel
	clc
	ret

;--- kill him off and seize it
@@4:
	call	clear_seize_sem
	and	[WORD obj_flags+esi],NOT (combat_bit+en_garde_bit+seize_bit)

;--- do the seize
	movsx	eax,[WORD obj_shelter_num+esi]
	cmp	eax,-1
	jne	@@o
	message	'sieged a null shelter'
@@o:
	mov	eax,[location_list+eax*4]	;eax - seized citadel

	call	kill_citadel_armies

	cmp	eax,offset citad0027
	jne	@@no_kill_h

	call	goodie_takes_dark_citadel

@@no_kill_h:
;	movzx	ebx,[BYTE esi+obj_start_realm]
	movzx	ebx,[BYTE esi+obj_allegiance]	;set as characters allied flag JS 23/06/95
	mov	[eax+obj_owner],bx		;set new owner flag

;	bts	[WORD eax+obj_land_enemies],bx	;make land bx an enemy of this one	;not really bothered about lands JS 23/6/95

;	movzx	ecx,[BYTE eax+obj_land_realm]	;make realm of bx an enemy
;>	movzx	ecx,[WORD eax+obj_land_ruler]	;MOVED TO START (ENEMIES DUE TO ATTACK, NOT RESULT)
;>	mov	ecx,[cast_list+ecx*4]
;>	movzx	ecx,[BYTE ecx+obj_allegiance]	;make realm of allegiance of attacked treat attacker as enemy JS 23/6/95
;>	region_mul	ecx
;>	bts	[WORD ecx+realm_data+realm_enemies],bx

	mov	bx,[WORD esi+obj_num]
;	movzx	bx,[BYTE esi+obj_port]		;JS 25/5/95
	movzx	ecx,[WORD eax+obj_land_ruler]
	mov	[eax+obj_land_ruler],bx

	mov	edi,[cast_list+ecx*4]

	mov	eax,qr_has_successfully_sieged
	call	make_quest_register
@@done:
	inc	[WORD obj_qstate+esi]			;next quest process
	stc
	ret

@@do_nothing:
	movzx	eax,[WORD obj_opponent+esi]
	mov	edi,[cast_list+eax*4]
	mov	ax,[esi+obj_num]
	mov	bx,[edi+obj_num]
	inc	ax
	inc	bx
	cmp	[seize_semaphore],ax
	je	@@nd
      	cmp	[seize_semaphore],bx
	jne	@@xit
@@nd:
	test	[WORD esi+obj_char_flags],dead_bit
	jne	@@md
	test	[WORD edi+obj_char_flags],dead_bit
	je	@@xit
@@md:
;--- either me or the other player are dead
	jmp	@@itsbeensetup

@@xit:
	clc
	ret

@@cancelme:
	cmp	esi,[controlled_addr]
	jne	@@ca
;	mov	ebx,0	;transport type
	call	get_my_transport_type
	call	setup_info_params
	mov	eax,it_couldnt_seize
	call	make_info_register
	jmp	@@cb
@@ca:
	call	get_target_address
	mov	eax,qr_couldnt_seize
	call	make_quest_register
@@cb:
	call	quest_cancel
	clc
	ret

@@cancel_q:
	call	quest_cancel
	clc
	ret
ENDP

;IN eax - citadel pointer
PROC kill_citadel_armies NEAR
;--- need to kill off armies in citadel (esi MUST be doing the seizing)
	push	eax ecx
;	movzx	eax,[WORD obj_qdat2+esi]
;	mov	eax,[location_list+eax*4]
	mov	ecx,16
@@ka:
	mov	[WORD eax+ecx*2+obj_race_size-2],0
	loop	@@ka	;JS 13/7/95 am			;done in quest_seize_rout
;---
	pop	ecx eax
	ret
ENDP

;IN
;ebp - citadel address
;esi - character pointer
;OUT
;edi - baddy char
PROC get_opponent_setup NEAR
	push	ebp ebx

	mov	edi,ebp

	call	ruler_at_citadel
	cmp	ebp,-1
	jne	@@yes
;--- no
	movzx	ebp,[WORD obj_land_ruler+edi]
	mov	ebp,[cast_list+ebp*4]
	call	copy_ruler
	mov	edi,offset seize_character
	mov	bx,[WORD seize_character+obj_num]
	mov	[obj_opponent+esi],bx

	pop	ebx ebp
	clc
	ret
@@yes:
;	movzx	ebx,[WORD obj_land_owner+edi]
	movzx	ebx,[WORD obj_land_ruler+edi]	;JS 2/6/95
	mov	[obj_opponent+esi],bx
	mov	edi,[cast_list+ebx*4]

	pop	ebx ebp
	stc
	ret
ENDP

PUBLIC clear_seize_sem
PROC clear_seize_sem NEAR
	mov	[seize_semaphore],0
	or	[WORD seize_character+obj_flags2],no_process_bit
	or	[WORD seize_character+obj_char_flags],dead_bit
	ret
ENDP

;edi - citadel pointer
;ebp - ruler character pointer
;esi - my player
PUBLIC copy_ruler
PROC copy_ruler NEAR
	pushad

	test	[seize_semaphore],-1
	jne	@@no_seize

	mov	[temp_shelt2],edi	;JS 24/7/95

	push	edi
	push	esi

	mov	ax,[esi+obj_num]
	inc	ax
	mov	[seize_semaphore],ax		;-1

	mov	esi,offset seize_character
	call	delete_link

	movsx	eax,[WORD esi+obj_shelter_num]	;instead of just removing him
	mov	[temp_shelt],eax

;--- copy ruler to zbuffer (temp storage)
	push	[WORD seize_character+obj_num]
	push	[WORD seize_character+obj_next_rlink]

	mov	esi,ebp
	mov	edi,offset seize_character
	mov	ecx,character_size
	rep	movsb

	pop	[WORD seize_character+obj_next_rlink]
	pop	[WORD seize_character+obj_num]

	mov	esi,[temp_shelt2]	;JS 24/7/95 (move seize char into citadel location)
	mov	eax,[obj_x+esi]
	mov	ebx,[obj_y+esi]

	mov	edi,offset seize_character
	mov	[obj_x+edi],eax		;JS 24/7/95 (move seize character to citadel locn)
	mov	[obj_y+edi],ebx

	mov	esi,edi
	call	link_first

	mov	[WORD edi+obj_next_party],-1	;reset party members
	mov	[WORD edi+obj_prev_party],-1
	mov	[WORD edi+obj_army_size],0	;and army size to none
	and	[WORD edi+obj_char_flags],NOT (dead_bit+rescue_bit)
	mov	[WORD edi+obj_route],-1
	mov	[WORD edi+obj_quest],_qrest
	mov	[WORD edi+obj_qstate],0

	and	[WORD obj_flags2+edi],NOT no_process_bit
	and	[WORD obj_char_flags+edi],NOT dead_bit
	mov	[WORD obj_link_type+edi],-1
	mov	[WORD obj_link_num+edi],-1
;	mov	[WORD obj_next_rlink+edi],-1
	mov	[WORD obj_boat_state+edi],-1
	or	[WORD obj_flags2+edi],really_inside_bit
	or	[WORD obj_flags+edi],inside_bit

	mov	[WORD obj_weapons+edi],1
	mov	[WORD obj_weap_link+edi],-1

	mov	[BYTE obj_anim_no+edi],1
	mov	[BYTE obj_frame_no+edi],0

	mov	[WORD obj_lives+edi],0	;JS 7/8/95

	pop	esi
;edi - seize character
;esi - my character (attacker)
;ebp - citadel ruler

	mov	eax,[obj_x+esi]
	shr	eax,16
	mov	[obj_shelter_x+edi],ax
	mov	eax,[obj_y+esi]
	shr	eax,16
	mov	[obj_shelter_y+edi],ax

;	mov	ax,[obj_dir+esi]
	mov	ax,[obj_shelter_dir+esi]	;JS 16/7/95 Ta PAH!

	mov	[obj_shelter_dir+edi],ax
;	mov	ax,[obj_sprite+esi]
;DAFT CLOT! This is a bloke! JS 17/4/95
;	mov	[obj_shelter_typ+edi],ax

;	mov	ax,[obj_num+esi]

	mov	ax,[obj_shelter_num+esi]	;JS 16/7/95 : Many thanks to Paul!
						;I had to look at this about 100 times
						;to see anything wrong!
	mov	[obj_shelter_num+edi],ax

	mov	[WORD edi+obj_rstate],bld_rest

	mov	esi,edi
	pop	edi
;FIXED!

;	movsx	eax,[esi+obj_shelter_num]	;instead of just removing him
	mov	eax,[temp_shelt]
	or	eax,eax
	js	@@nin
	push	edi
	mov	edi,[location_list+eax*4]
	call	delete_from_citadel		;delete him properly! JS 6/6/95
	pop	edi
@@nin:

;esi - seize character
;edi - citadel pointer
;ebp - citadel ruler

	mov	ax,[edi+obj_sprite]
	mov	[esi+obj_shelter_typ],ax

	call	link_into_citadel

	popad
	clc
	ret

@@no_seize:
	popad
	stc
	ret

ENDP

     	IF	0
>;-------------------------------------------------------------------------
>PUBLIC quest_seize_rout
>PROC quest_seize_rout NEAR
>;do the seizing stuff inside
>
>	movzx	eax,[WORD obj_qdat1+esi]
>	movzx	ebx,[WORD obj_qdat2+esi]
>	cmp	[BYTE obj_qtype1+esi],qt_char
>	je	@@c
>
>	movzx	eax,[WORD obj_qdat3+esi]
>	movzx	ebx,[WORD obj_qdat4+esi]
>	cmp	[BYTE obj_qtype2+esi],qt_char
>	jne	@@fuck
>@@c:
>
>	mov	eax,[list_list+eax*4]
>	mov	edi,[ebx*4+eax]
>
>	call	is_visible
>	jc	@@visible
>
>	call	ruler_at_citadel	;return ruler ptr in ebp (-1 == not there)
>	mov	eax,edi
>
>	cmp	ebp,-1
>	jne	@@do_fight
>
>;no bastard ruler at citadel
>	movzx	ebp,[WORD obj_land_ruler+edi]
>	mov	ebp,[cast_list+ebp*4]
>
>;--- copy ruler to zbuffer (temp storage)
>	push	esi
>	mov	esi,ebp
>	mov	edi,offset zmap_buffer
>	mov	ecx,character_size
>	rep	movsb
>	pop	esi
>
>;--- fight battle against the copy!
>	mov	edi,offset zmap_buffer
>
>	mov	[WORD edi+obj_next_party],-1	;reset party members
>	mov	[WORD edi+obj_prev_party],-1
>	mov	[WORD edi+obj_army_size],0	;and army size to none
>	and	[WORD edi+obj_char_flags],NOT dead_bit
>	jmp	@@doit
>
>@@do_fight:
>	mov	edi,ebp
>@@doit:
>	call	fight_sim_battle
>
>;--- check to see if they died?
>	test	[WORD esi+obj_char_flags],dead_bit
>	jne	@@dead
>
>;--- do the seize
>	movzx	ebx,[BYTE esi+obj_start_realm]
>	mov	[eax+obj_owner],bx
>
>	bts	[WORD eax+obj_land_enemies],bx	;make land bx an enemy of this one
>
>	movzx	ecx,[BYTE eax+obj_land_realm]	;make realm of bx an enemy
>	region_mul	ecx
>	bts	[WORD ecx+realm_data+realm_enemies],bx
>
>;	movzx	bx,[BYTE esi+obj_port]		;JS 25/5/95
>	mov	bx,[WORD esi+obj_num]
>	mov	[eax+obj_land_ruler],bx
>
>@@x:
>	and	[WORD esi+obj_flags],NOT seize_bit
>	inc	[WORD obj_qstate+esi]		;next quest process
>	stc
>	ret
>@@fuck:
>	jmp	@@fuck
>
>@@dead:
>	and	[WORD esi+obj_flags],NOT seize_bit
>	clc
>	ret
>
>@@visible:
>	movzx	eax,[WORD obj_land_owner+edi]
>	mov	edi,[cast_list+eax*4]
>
>obj_flaps	=	obj_flags
>	mov	[watch_flag],0
>	or	[WORD esi+obj_flaps],combat_bit
>	or	[WORD edi+obj_flaps],combat_bit
>	mov	[BYTE edi+obj_anim_no],1
>	mov	[BYTE esi+obj_anim_no],1
>	call	start_fighting
>
>	or	[WORD esi+obj_flaps],combat_bit
>	or	[WORD edi+obj_flaps],combat_bit
>
>	cmp	esi,[controlled_addr]
>	jne	@@cde
>	call	load_combat_head
>@@cde:
>	clc
>	ret
>ENDP
	ENDIF

;-------------------------------------------------------------------------
PUBLIC quest_cancel
PROC quest_cancel NEAR

;--- Force character to do nothing ---

		mov	[WORD obj_quest+esi],_qrest
		mov	[WORD obj_qstate+esi],0
		mov	[WORD obj_qdat1+esi],0
		mov	[WORD obj_qdat2+esi],0
		mov	[WORD obj_qdat3+esi],0
		mov	[WORD obj_qdat4+esi],0

		mov	[BYTE obj_qtype1+esi],qt_not_defined
		mov	[BYTE obj_qtype2+esi],qt_not_defined

		mov	[WORD obj_find_status+esi],fs_none

;	mov	[WORD obj_targ_x+esi],0
;	mov	[WORD obj_targ_y+esi],0

		call	cancel_journey			;delete previous route

;	mov	[BYTE esi+obj_movetype],-1

;		stc
	clc

		ret
ENDP

;just clear esi's quest data (not journey)
PUBLIC quest_clear
PROC quest_clear NEAR
	mov	[WORD obj_quest+esi],_qrest
	mov	[WORD obj_qstate+esi],0
	mov	[WORD obj_qdat1+esi],0
	mov	[WORD obj_qdat2+esi],0
	mov	[WORD obj_qdat3+esi],0
	mov	[WORD obj_qdat4+esi],0

	mov	[BYTE obj_qtype1+esi],qt_not_defined
	mov	[BYTE obj_qtype2+esi],qt_not_defined

;		mov	[WORD obj_targ_x+esi],0
;		mov	[WORD obj_targ_y+esi],0

	mov	[WORD obj_find_status+esi],fs_none
	ret
ENDP

;-------------------------------------------------------------------------
PROC quest_restart_rout NEAR
		mov	[WORD obj_qstate+esi],0		;reset quest procedure : go back to 1st quest entry
		stc
		ret
ENDP

;-------------------------------------------------------------------------
;-------------------------------------------------------------------------
;-------------------------------------------------------------------------

PUBLIC get_target_address
PROC get_target_address NEAR
	push	eax ebx
	mov	edi,0
	movzx	eax,[WORD obj_qdat1+esi]
	movzx	ebx,[WORD obj_qdat2+esi]
	cmp	[BYTE obj_qtype1+esi],qt_char
	je	@@c
	movzx	eax,[WORD obj_qdat3+esi]
	movzx	ebx,[WORD obj_qdat4+esi]
	cmp	[BYTE obj_qtype1+esi],qt_char
	jne	@@xc
@@c:
	mov	eax,[list_list+eax*4]
	mov	edi,[eax+ebx*4]
@@xc:
	pop	ebx eax
	ret
ENDP

PROC check_in_same_building NEAR
	call	get_target_address
	cmp	edi,0
	je	@@xc

	test	[WORD edi+obj_flags],inside_bit
	je	@@xc

	mov	ax,[edi+obj_shelter_num]
	test	ax,-1
	js	@@xc

	cmp	ax,[esi+obj_shelter_num]
	jne	@@xc

	stc
	ret
@@xc:
	clc
	ret
ENDP

PUBLIC check_in_range_of_building
PROC check_in_range_of_building NEAR
	call	get_target_address
	cmp	edi,0
	je	@@xc

	movzx	eax,[WORD edi+obj_type]

	cmp	eax,typ_character
	je	@@isnt_a_shelter

	mov	ebx,real_build_flags
	bt	ebx,eax
	jc	@@is_a_cit_cas

	jmp	@@what_is_it	;probably an object? JS 28/7/95

@@isnt_a_shelter:
	test	[WORD edi+obj_flags],inside_bit
	je	@@xc

	movsx	eax,[WORD edi+obj_shelter_num]
	test	eax,-1
	js	@@xc

	mov	ebp,[location_list+eax*4]
	jmp	@@a_bloke
@@is_a_cit_cas:
	mov	ebp,edi

@@a_bloke:
	mov	eax,[obj_x+esi]
	mov	ebx,[obj_y+esi]

	mov	[proximity_obj],0
	mov	[proximity_val],-1
	mov	[scan_routine],offset enter_proximity
	call	link_map_scan

	mov	edi,[proximity_obj]
	and	edi,edi
	je	@@xc

	cmp	edi,ebp
	jne	@@xc	

	mov	ebx,real_build_flags
	movzx	eax,[WORD edi+obj_type]
	bt	ebx,eax
	jc	@@arrived	;usu for a seize. JS 28/7/95

	call	check_move_inside
	cmp	eax,MI_cancel_quest
	jne	@@n1

@@cancel:
	mov	[quest_cancelled],-1
	call	quest_cancel	;this is a problem. : JS 29/6/95 - quest cancelled flag fixes this with a fudge
	jmp	@@x
@@n1:
	cmp	eax,MI_start_seige
	jne	@@n2
	call	move_player_inside_ok
	jmp	@@x
@@n2:
	cmp	eax,MI_enter_building
	jne	@@n3
	call	check_not_enemy_owner
	jc	@@cancel
	call	move_player_inside_ok
	jmp	@@x
@@n3:
@@x:
@@arrived:
	stc
	ret
@@xc:
@@what_is_it:
	clc
	ret
ENDP

;-------------------------------------------------------------------------
;
; On Entry:	ESI	= Address of current object
;
;-------------------------------------------------------------------------
PUBLIC reached_target
PROC reached_target NEAR

;--- Throw out wildly different coordinates ---

	cmp	[WORD esi+obj_type],typ_character
	jne	@@be_careful_jim

	call	check_in_same_building
	jc	@@3

	call	check_in_range_of_building
	jc	@@3

@@be_careful_jim:
		mov	eax,[DWORD obj_x+esi]
		movzx	edx,[WORD obj_targ_x+esi]
		shl	edx,16
		sub	eax,edx
		imul	eax

		mov	ecx,edx

		mov	eax,[DWORD obj_y+esi]
		movzx	edx,[WORD obj_targ_y+esi]
		shl	edx,16
		sub	eax,edx
		imul	eax

		add	ecx,edx			;distance to target coords

		cmp	ecx,(256*256)		;anywhere near target ?
		ja	@@2			;no

		and	ecx,ecx			;reached target position ?
		je	@@3			;yes

;--- Check for target point being too close to object ---

		mov	ax,[obj_targ_x+esi]
		shl	eax,16
		mov	[temp_obj_x],eax	;target X coordinate

		mov	ax,[obj_targ_y+esi]
		shl	eax,16
		mov	[temp_obj_y],eax	;target Y coordinate

		mov	[ignore_object],0
	IF	0
;		call	collision_bounce	;this is causing problems
						;can't reach target if too close to a building
						;because collision with building stops
						;the 'reached_target' condition.

;		mov	[ignore_object],edi	;store conflicting object
	ELSE
		mov	[do_barging],0
	     	call	check_collision
		mov	[do_barging],-1
		jc	@@colliding
	ENDIF
		jnc	@@2			;no object too close

@@colliding:
;------------------------------------------------------------------------------------
;JS 7/2/95
	mov	ax,[obj_type+edi]		;check if conflicting object is a building
	cmp	ax,typ_citadel			;if it is then sensible to continue
	je	@@cc
	cmp	ax,typ_castle
	je	@@cc
	cmp	ax,typ_city
	je	@@cc
	cmp	ax,typ_building
	je	@@cc

	movzx	eax,[WORD obj_qdat3+esi]	;otherwise get the quest target object
	movzx	ebx,[WORD obj_qdat4+esi]	;and play with that instead.
	cmp	[BYTE obj_qtype2+esi],qt_char
	je	@@fc
	movzx	eax,[WORD obj_qdat1+esi]
	movzx	ebx,[WORD obj_qdat2+esi]
	cmp	[BYTE obj_qtype1+esi],qt_char
	je	@@fc

;-- couldn't find quest info.
;try and locate the shelter
;	movzx	eax,[WORD obj_qdat1+esi]
;	movzx	ebx,[WORD obj_qdat2+esi]
;	shl	eax,16
;	shl	ebx,16
	mov	eax,[obj_x+esi]
	mov	ebx,[obj_y+esi]
	mov	[shelter_dist],-1
	mov	[shelter_addr],0
	mov	[scan_routine],offset enter_proximity	;shelter_proximity
	call	link_map_scan
	cmp	[shelter_addr],-1	;tuff!
	je	@@cc
	mov	edi,[shelter_addr]
	jmp	@@cc

@@fc:
	mov	eax,[list_list+eax*4]
	mov	edi,[eax+ebx*4]		;address of object being travelled to

;------------------------------------------------------------------------------------

;--- Check if close enough to target point to stop moving ---
@@cc:
		movzx	eax,[BYTE obj_x_size+esi]
		movzx	ebx,[BYTE obj_x_size+edi]

	mov	dx,[edi+obj_type]	;half the radii of buidlings JS 2/7/95
	mov	bp,scan_flags
	bt	bp,dx
	jnc	@@nb2
	shr	ebx,1
@@nb2:
		add	eax,ebx
		imul	eax

;	shl	eax,4			;bigger radius (*16)


;	mov	edx,eax
;	shl	eax,2
;	sub	eax,edx			;*5

		cmp	ecx,eax			;within range of object ?
		jb	@@3			;yes
@@2:
		clc				;still more travelling to do
		ret
@@3:
		stc				;target position reached
		ret
ENDP


;
; On Entry:	ESI	= Address of current object
;
;-------------------------------------------------------------------------
;currently only called from shelter.asm
PUBLIC reached_coords
PROC reached_coords NEAR

;--- Throw out wildly different coordinates ---

		mov	eax,[DWORD obj_x+esi]
		movzx	edx,[WORD obj_targ_x+esi]
		shl	edx,16
		sub	eax,edx
		imul	eax

		mov	ecx,edx

		mov	eax,[DWORD obj_y+esi]
		movzx	edx,[WORD obj_targ_y+esi]
		shl	edx,16
		sub	eax,edx
		imul	eax

		add	ecx,edx			;distance to target coords

	IF	0
		cmp	ecx,(256*256)		;anywhere near target ?
		ja	@@2			;no

		and	ecx,ecx			;reached target position ?
		je	@@3			;yes
	ELSE
;------------------------------------------------------------------------
;JS 7/2/95
;		cmp	ecx,20h
		cmp	ecx,10000h		;JS 27/7/95
		ja	@@2

	mov	eax,[obj_x+esi]
	mov	ebx,[obj_y+esi]
	mov	[barge_tmp_x],eax
	mov	[barge_tmp_y],ebx

		call	delete_link
		mov	ax,[WORD obj_targ_x+esi]
		mov	bx,[WORD obj_targ_y+esi]
		shl	eax,16
		shl	ebx,16
		mov	[obj_x+esi],eax
		mov	[obj_y+esi],ebx
		call	link_first

	call	barge_emergency
;------------------------------------------------------------------------

		jmp	@@3
	ENDIF

@@2:
		clc				;still more travelling to do
		ret
@@3:
		stc				;target position reached
		ret
ENDP

;-------------------------------------------------------------------------
PROC quest_panic NEAR
	mov	eax,03f0000h
	call	set_border
@@x:
	jmp	@@x		;PANIC
ENDP

PROC quest_panic_i_know NEAR
	mov	eax,03f0000h
	call	set_border
@@x:
	jmp	@@x		;PANIC
ENDP

;-------------------------------------------------------------------------

PUBLIC set_quest_record
PROC set_quest_record NEAR
	pushfd		;preserve flags
	pushad

;	cmp	esi,[controlled_addr]
;	je	@@x

	movzx	eax,[quest_result]
	cmp	eax,tr_accept
	je	@@x

	cmp	eax,tr_take_allow
	je	@@x

;	cmp	eax,tr_B_join_A		;JS 28/7/95
;	je	@@x

	;qc_text1	==	quest
	;qc_text2	==	refuse
	;qc_text3	==	who
	;qc_text4	==	person2 (charac number)
	;qc_text5	==	object

	mov	eax,qr_change_quest
	call	make_quest_register

@@x:
	popad
	popfd
	ret
ENDP

	ENDS

;-------------------------------------------------------------------------

	END
