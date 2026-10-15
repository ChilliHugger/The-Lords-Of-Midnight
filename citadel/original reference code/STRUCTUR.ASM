;-------------------------------------------------------------------------
;			LORDS OF MIDNIGHT III
;
;			    THE CITADEL
;-------------------------------------------------------------------------
; (C) David Ollman & James Shaw 1995.
;-------------------------------------------------------------------------

;		    ---------- BIT FLAG EQUATES ----------

;*** OBJ_CHAR_FLAGS ***

recruit_bit	=	1	;set if recruited
knowledge_bit	=	2	;set if known
dead_bit	=	4	;set if dead
friendly_bit	=	8	;set if friendly
rescue_bit	=	16	;set if rescue required
detail_bit	=	32	;set if intimate details known
interest_bit	=	64	;set if interested in characters actions
rest_bit	=	128	;set if object at rest
visible_bit	=	256	;set if object was visible last movement
sink_bit	=	512	;set if object is sinking below water
major_bit	=	1024	;set if major character
accel_bit	=	2048	;set if catching up with party/unit member
hidden_bit	=	4096	;set if object hidden from view in landscape
control_bit	=	8192	;set if object can be controlled
location_bit	=	16384	;set if object location known
watch_bit	=	32768	;set if object watchable

;*** OBJ_FLAGS ***

height_bit	=	1	;set if object has relative height
static_bit	=	2	;set if object does not move
obstacle_bit	=	4	;set if object should be avoided
generated_bit	=	8	;set if object was generated not plonked
anim_bit	=	16	;set if object to be animated
process_bit	=	32	;set if object already processed this turn
corridor_bit	=	64	;set if character enterring/exitting building
en_garde_bit	=	128	;set if combat to be initialised
combat_bit	=	256	;set if character arrived at combat position
crowd_bit	=	512	;set if character crowding around combat

doorway_bit	=	512	;set if shelter doorway is being approached : crowd bit not used for shelters!

talking_bit	=	1024	;set if character involved in a conversation
initiate_bit	=	2048	;set if character starts a conversation
inside_bit	=	4096	;set if character viewed inside a building

seize_bit	=	8192	;set if character involved in a seige

available_bit	=	16384	;set if character not involved in a quest
female_bit	=	32768	;set if character female

;*** OBJ_FLAGS2 *** JS 12/2/95
autoq_bit		=	1	;set if character is auto questing
really_inside_bit	=	2	;set if character is really inside & not just resting
					;=> inside_bit is set
shelter_stop_bit	=	4	;set if using shelter code only to move player inside
meet_autoq_bit		=	8	;set if character only on a meet quest
seize_ready_bit		=	16	;set if invisible seige has been setup and is ready to go!
no_process_bit		=	32	;set if this character not to be processed
near_collapse_mess_bit	=	64	;set if a message has been posted for this character's near collapse
collapse_mess_bit	=	128	;set if a message has been posted for this character's collapse
seek_shelter_bit	=	256	;set if a message has been posted for this character seeking shelter
wait_party_bit		=	512	;set if a message has been posted for this character waiting for his party
was_hostage_bit		=	1024	;set if was a hostage

;---------------------- Weapons Equates --------------
w_none		=	0
w_sword		=	1
w_axe		=	2

;*****************************************************************************

;		---------- COMMON DATA STRUCTURE ----------

rs_set 0

rs obj_prev_link,4	; Previous link address (0=first object in list)
rs obj_next_link,4	; Next link address (0=last object in list)

rs obj_num,2		; Object ID number in relevant list (cast_list, etc..)

rs obj_type,2		; Object type (person/building/etc...)

rs obj_char_flags,2	; Character based flags (alive/sleeping/rescued)

rs obj_flags,2		; Object based flags (relative height)

rs obj_x,4		; Absolute X coordinate (16:16 format)
rs obj_y,4		; Absolute Y coordinate (16:16 format)
rs obj_z,2		; Height adjust

rs obj_dir,2		; Direction

rs obj_sprite,2		; Sprite number (Landscape representation)
rs obj_cladding,2	; Sprite graphic cladding
rs obj_print,2		; Sprite printing routine

rs obj_x_scale,4	; X scaling factor (16:16 format)
rs obj_y_scale,4	; Y scaling factor (16:16 format)

rs obj_x_size,1		; Collision detection X range
rs obj_y_size,1		; Collision detection Y range
rs obj_z_size,1		; Collision detection Z range

rs obj_cam_off,1	; Vertical camera offset (+/- 127)

;*****************************************************************************

;		---------- FEATURE DATA STRUCTURE ----------

rs_set (obj_cam_off+1)

rs common_len,0		; Length of common block


;*****************************************************************************

;		---------- CASTLE DATA STRUCTURE ----------

rs_set (obj_cam_off+1)

rs obj_shield,1		; Heraldic shield type
rs obj_port,1		; Building sprite number

rs obj_race_size,32	;(2*16)	; Warrior count (1 word per race)
rs obj_owner,2

rs obj_land_ruler,2		;current ruler
rs obj_land_war_state,1		;state of war
rs obj_land_realm,1		;which realm land is in
rs obj_land_owner,2		;original owner
rs obj_land_enemies,2		;enemy flags

rs obj_cit_link,2		;citadel linked list

rs citadel_size,0

;*****************************************************************************

;		---------- CITY DATA STRUCTURE ----------

rs_set (obj_cam_off+1)

rs obj_shield,1		; Heraldic shield type
rs obj_port,1		; Building sprite number

rs obj_race_size,32	;(2*16)	; Warrior count (1 word per race)
rs obj_owner,2

rs obj_land_ruler,2		;current ruler
rs obj_land_war_state,1		;state of war
rs obj_land_realm,1		;which realm land is in
rs obj_land_owner,2		;original owner
rs obj_land_enemies,2		;enemy flags

rs obj_cit_link,2		;citadel linked list

;*****************************************************************************

;		---------- CITADEL DATA STRUCTURE ----------

rs_set (obj_cam_off+1)

rs obj_shield,1		; Heraldic shield type
rs obj_port,1		; Building sprite number

rs obj_race_size,32	;(2*16)	; Warrior count (1 word per race)
rs obj_owner,2		; owner of citadel by realm (converted to race when printed)

rs obj_land_ruler,2		;current ruler (by Land #)
rs obj_land_war_state,1		;state of war
rs obj_land_realm,1		;which realm land is in
rs obj_land_owner,2		;original owner
rs obj_land_enemies,2		;enemy flags

rs obj_cit_link,2		;citadel occupants linked list

rs citadel_size,0

;*****************************************************************************

;		---------- WILD BEAST DATA STRUCTURE ----------

rs_set (obj_cam_off+1)

rs obj_shield,1		; Heraldic shield type
rs obj_port,1		; Portrait number

rs obj_movetype,1	; Movement type (walking/flying/floating)
rs obj_movemode,1	; Movement mode (search/evade/track/auto/pause/etc...)

rs obj_speed,2		; Speed of movement
rs obj_int_speed,1	; Integer speed of movement

rs obj_max_turn,1	; Maximum rate of turn

rs obj_targ_x,2		; Target X coordinate
rs obj_targ_y,2		; Target Y coordinate

rs obj_tsq_x,2		; Current target X square on 128x1 map
rs obj_tsq_y,2		; Current target Y square on 128x1 map

rs obj_guide_type,2	; Current guide control routine
rs obj_route,2		; Current index to object route

rs obj_link_type,2	; Type of linked object
rs obj_link_num,2	; Linked objects list index

rs obj_move_cnt,2	; Invisible movement counter
rs obj_move_x,4		; Invisible X movement
rs obj_move_y,4		; Invisible Y movement

rs obj_anim_no,1	; Current animation number
rs obj_frame_no,1	; Current animation frame number

rs obj_energy,4		; Overall energy level
rs obj_max_limit,2	; Maximum energy limit
rs obj_mid_limit,2	; Medium energy limit

rs obj_rest,2		; Maximum rest time after collapse
rs obj_rest_count,2	; Collapse rest counter

rs obj_timeout,2	; Party delay counter

rs obj_opponent,2	; Combat opponent
rs obj_strength,4	; Combat strength available
rs obj_power,2		; Combat power level
rs obj_limit,2		; Combat power level limit

rs obj_state,2		; Type of direction finding
rs obj_wander_dir,2	; Random movement direction

rs obj_anim1,1		; anim control vectors
rs obj_anim2,1
rs obj_anim3,1
rs obj_anim4,1

rs beast_len,0

;*****************************************************************************

;		---------- CHARACTER DATA STRUCTURE ----------

rs_set (obj_cam_off+1)

rs obj_shield,1		; Heraldic shield type
rs obj_port,1		; Portrait number

rs obj_movetype,1	; Movement type (walking/flying/floating)
rs obj_movemode,1	; Movement mode (search/evade/track/auto/pause/etc...)

rs obj_speed,2		; Current speed of movement
rs obj_int_speed,1	; Integer speed of movement

rs obj_max_turn,1	; Maximum rate of turn

rs obj_targ_x,2		; Target X coordinate
rs obj_targ_y,2		; Target Y coordinate

rs obj_tsq_x,2		; Current target X square on 128x1 map
rs obj_tsq_y,2		; Current target Y square on 128x1 map

rs obj_guide_type,2	; Current guide control routine
rs obj_route,2		; Current index to object route

rs obj_link_type,2	; Type of linked object
rs obj_link_num,2	; Linked objects list index

rs obj_move_cnt,2	; Invisible movement counter
rs obj_move_x,4		; Invisible X movement
rs obj_move_y,4		; Invisible Y movement

rs obj_anim_no,1	; Current animation number
rs obj_frame_no,1	; Current animation frame number

rs obj_energy,4		; Overall energy level
rs obj_max_limit,2	; Maximum energy limit
rs obj_mid_limit,2	; Medium energy limit

rs obj_rest,2		; Maximum rest time after collapse
rs obj_rest_count,2	; Collapse rest counter

rs obj_timeout,2	; Party delay counter

rs obj_opponent,2	; Combat opponent
rs obj_strength,4	; Combat strength available
rs obj_power,2		; Combat power level
rs obj_limit,2		; Combat power level limit

rs obj_state,2		; Type of direction finding
rs obj_wander_dir,2	; Random movement direction

rs obj_quest,2		; Type of quest
rs obj_qstate,2		; Status of quest
rs obj_qdat1,2		; Object list type, destination X coord
rs obj_qdat2,2		; Object number, destination Y coord
rs obj_qdat3,2		; X Position to return to after sub-quest
rs obj_qdat4,2		; Y Position to return to after sub-quest

rs obj_prev_party,2	; Link to previous party member (-1 = party leader)
rs obj_next_party,2	; Link to next party member (-1 = last party member)

rs obj_rstate,2		; Current sheltering status
rs obj_shelter_x,2	; Shelter X coordinate
rs obj_shelter_y,2	; Shelter Y coordinate
rs obj_shelter_dir,2	; Orientation of shelter graphic
rs obj_shelter_num,2	; Building index number
rs obj_shelter_typ,2	; Type of building to be used as shelter
rs obj_lstblk_x,1	; Last link-map scan origin X block
rs obj_lstblk_y,1	; Last link-map scan origin Y block

rs obj_rank,2		; Leadership ranking

rs obj_army_race,2	; Race of army (0-15)
rs obj_army_size,2	; Current army strength
rs obj_army_lost,2	; Number of army lost in battle
rs obj_army_kill,2	; Number of enemy killed in battle
rs obj_army_flag,2	; Recruitable race flags (not used)
rs obj_start_realm,1	; start realm #	(static)		;Index value of 1st associated army unit
rs obj_start_land,1	; start land # (static) (owner of...) 	;Index value of 2nd associated army unit

rs obj_sleep_time,2	; Time of the day to sleep (0-1439)
rs obj_wake_time,2	; Time of the day to wake (0-1439)

rs obj_weapons,1	; Weapons carried
rs obj_weap2,1		; Weapon details
rs obj_weap_link,2	; List # of weapon

rs obj_find_status,2	; Current hierarchy level of find quest
rs obj_qtype1,1		; What type of info is in qdat1,2
rs obj_qtype2,1		; What type of info is in qdat3,4

rs obj_time,2		; time object finished quest
rs obj_wait,2		; hours to wait once finished

rs obj_flags2,2		; more flags
;rs obj_shelt_x,2	; position of player before sheltering on time races on
rs obj_next_rlink,2	; next character# in building shelterer linked list
;rs obj_shelt_y,2

rs obj_boat_state,2	; state of boat searching on quest

rs obj_purpose,2	; general purpose
rs obj_reaction,2	; current reaction

rs obj_clothing,3	; character cladding
rs obj_allegiance,1	; (was pad) who this char is allied to (realm)

rs obj_curr_land,1	; land position for quest eval
rs obj_curr_realm,1	; realm position for quest eval

rs obj_qualities1,4	; character quality flags
rs obj_qualities2,4	; character quality flags

rs obj_xi,4		; interior x/y position
rs obj_yi,4		;
rs obj_lives,2		; hidden life count
rs obj_start_strength,4	; start strength
rs obj_start_armies,2	; start armies

rs obj_null,0		; Viewing angle (up/down)

rs obj_null,0		; Name

rs obj_null,0		; Title

rs obj_null,0		; Narrative details

rs obj_null,0		; Exhaustion/Energy level

rs obj_null,0		; Personality (ie Chicken factor - run away from fight)

rs obj_null,0		; Means of transport (foot/horse/dragon/ship)

rs obj_null,0		; Cash

rs obj_null,0		; Objects carried

rs obj_null,0		; Persuasion difficulty

rs obj_null,0		; Morale

rs obj_null,0		; List of recruited citizens

rs obj_null,0		; List of recruited warriors

rs obj_null,0		; Current army losses

rs obj_null,0		; Help info/weapons/objects/money/right of passage

rs obj_null,0		; Game time

rs character_size,0

;*****************************************************************************

;		---------- CITIZEN DATA STRUCTURE ----------

rs_set (obj_cam_off+1)

rs obj_shield,1		; Heraldic shield type
rs obj_port,1		; Portrait number

rs obj_movetype,1	; Movement type (walking/flying/floating)
rs obj_movemode,1	; Movement mode (search/evade/track/auto/pause/etc...)

rs obj_speed,2		; Current speed of movement
rs obj_int_speed,1	; Integer speed of movement

rs obj_max_turn,1	; Maximum rate of turn

rs obj_targ_x,2		; Target X coordinate
rs obj_targ_y,2		; Target Y coordinate

rs obj_tsq_x,2		; Current target X square on 128x1 map
rs obj_tsq_y,2		; Current target Y square on 128x1 map

rs obj_guide_type,2	; Current guide control routine
rs obj_route,2		; Current index to object route

rs obj_link_type,2	; Type of linked object
rs obj_link_num,2	; Linked objects list index

rs obj_move_cnt,2	; Invisible movement counter
rs obj_move_x,4		; Invisible X movement
rs obj_move_y,4		; Invisible Y movement

rs obj_anim_no,1	; Current animation number
rs obj_frame_no,1	; Current animation frame number

rs obj_energy,4		; Overall energy level
rs obj_max_limit,2	; Maximum energy limit
rs obj_mid_limit,2	; Medium energy limit

rs obj_rest,2		; Maximum rest time after collapse
rs obj_rest_count,2	; Collapse rest counter

rs obj_timeout,2	; Party delay counter

rs obj_opponent,2	; Combat opponent
rs obj_strength,4	; Combat strength available
rs obj_power,2		; Combat power level
rs obj_limit,2		; Combat power level limit

rs obj_state,2		; Type of direction finding
rs obj_wander_dir,2	; Random movement direction

rs obj_quest,2		; Type of quest
rs obj_qstate,2		; Status of quest
rs obj_qdat1,2		; Object list type, destination X coord
rs obj_qdat2,2		; Object number, destination Y coord
rs obj_qdat3,2		; X Position to return to after sub-quest
rs obj_qdat4,2		; Y Position to return to after sub-quest

rs obj_prev_party,2	; Link to previous party member (-1 = party leader)
rs obj_next_party,2	; Link to next party member (-1 = last party member)

rs citizen_size,0
rs feature_len,0	; Length of created data
rs fixed_size,0

;*****************************************************************************

;		---------- WARRIOR DATA STRUCTURE ----------

rs_set (obj_cam_off+1)

rs obj_shield,1		; Heraldic shield type
rs obj_port,1		; Portrait number

rs obj_movetype,1	; Movement type (walking/flying/floating)
rs obj_movemode,1	; Movement mode (search/evade/track/auto/pause/etc...)

rs obj_speed,2		; Current speed of movement
rs obj_int_speed,1	; Integer speed of movement

rs obj_max_turn,1	; Maximum rate of turn

rs obj_targ_x,2		; Target X coordinate
rs obj_targ_y,2		; Target Y coordinate

rs obj_tsq_x,2		; Current target X square on 128x1 map
rs obj_tsq_y,2		; Current target Y square on 128x1 map

rs obj_guide_type,2	; Current guide control routine
rs obj_route,2		; Current index to object route

rs obj_link_type,2	; Type of linked object
rs obj_link_num,2	; Linked objects list index

rs obj_move_cnt,2	; Invisible movement counter
rs obj_move_x,4		; Invisible X movement
rs obj_move_y,4		; Invisible Y movement

rs obj_anim_no,1	; Current animation number
rs obj_frame_no,1	; Current animation frame number

rs obj_energy,4		; Overall energy level
rs obj_max_limit,2	; Maximum energy limit
rs obj_mid_limit,2	; Medium energy limit

rs obj_rest,2		; Maximum rest time after collapse
rs obj_rest_count,2	; Collapse rest counter

rs obj_timeout,2	; Party delay counter

rs obj_opponent,2	; Combat opponent
rs obj_strength,4	; Combat strength available
rs obj_power,2		; Combat power level
rs obj_limit,2		; Combat power level limit

rs obj_state,2		; Type of direction finding
rs obj_wander_dir,2	; Random movement direction

rs obj_quest,2		; Type of quest
rs obj_qstate,2		; Status of quest
rs obj_qdat1,2		; Object list type, destination X coord
rs obj_qdat2,2		; Object number, destination Y coord
rs obj_qdat3,2		; X Position to return to after sub-quest
rs obj_qdat4,2		; Y Position to return to after sub-quest

rs obj_prev_party,2	; Link to previous party member (-1 = party leader)
rs obj_next_party,2	; Link to next party member (-1 = last party member)

;*****************************************************************************

;		---------- ARMY DATA STRUCTURE ----------

rs_set (obj_cam_off+1)

rs obj_shield,1		; Heraldic shield type
rs obj_port,1		; Portrait number

rs obj_movetype,1	; Movement type (walking/flying/floating)
rs obj_movemode,1	; Movement mode (search/evade/track/auto/pause/etc...)

rs obj_speed,2		; Current speed of movement
rs obj_int_speed,1	; Integer speed of movement

rs obj_max_turn,1	; Maximum rate of turn

rs obj_targ_x,2		; Target X coordinate
rs obj_targ_y,2		; Target Y coordinate

rs obj_tsq_x,2		; Current target X square on 128x1 map
rs obj_tsq_y,2		; Current target Y square on 128x1 map

rs obj_guide_type,2	; Current guide control routine
rs obj_route,2		; Current index to object route

rs obj_link_type,2	; Type of linked object
rs obj_link_num,2	; Linked objects list index

rs obj_move_cnt,2	; Invisible movement counter
rs obj_move_x,4		; Invisible X movement
rs obj_move_y,4		; Invisible Y movement

rs obj_anim_no,1	; Current animation number
rs obj_frame_no,1	; Current animation frame number

rs obj_energy,4		; Overall energy level
rs obj_max_limit,2	; Maximum energy limit
rs obj_mid_limit,2	; Medium energy limit

rs obj_rest,2		; Maximum rest time after collapse
rs obj_rest_count,2	; Collapse rest counter

rs obj_timeout,2	; Party delay counter

rs obj_opponent,2	; Combat opponent
rs obj_strength,4	; Combat strength available
rs obj_power,2		; Combat power level
rs obj_limit,2		; Combat power level limit

rs obj_state,2		; Type of direction finding
rs obj_wander_dir,2	; Random movement direction

rs obj_quest,2		; Type of quest
rs obj_qstate,2		; Status of quest
rs obj_qdat1,2		; Object list type, destination X coord
rs obj_qdat2,2		; Object number, destination Y coord
rs obj_qdat3,2		; X Position to return to after sub-quest
rs obj_qdat4,2		; Y Position to return to after sub-quest

rs obj_prev_party,2	; Link to previous party member (-1 = party leader)
rs obj_next_party,2	; Link to next party member (-1 = last party member)

rs obj_rstate,2		; Current sheltering status
rs obj_shelter_x,2	; Shelter X coordinate
rs obj_shelter_y,2	; Shelter Y coordinate
rs obj_shelter_dir,2	; Orientation of shelter graphic
rs obj_shelter_num,2	; Building index number
rs obj_shelter_typ,2	; Type of building to be used as shelter
rs obj_lstblk_x,1	; Last link-map scan origin X block
rs obj_lstblk_y,1	; Last link-map scan origin Y block

;had to add all this shit to make it work!

rs obj_rank,2		; Leadership ranking

rs obj_army_race,2	; Race of army (0-15)
rs obj_army_size,2	; Current army strength
rs obj_army_lost,2	; Number of army lost in battle
rs obj_army_kill,2	; Number of enemy killed in battle
rs obj_army_flag,2	; Recruitable race flags
rs obj_start_realm,1	; Index value of 1st associated army unit
rs obj_start_land,1	; Index value of 2nd associated army unit

rs obj_sleep_time,2	; Time of the day to sleep (0-1439)
rs obj_wake_time,2	; Time of the day to wake (0-1439)

rs obj_weapons,1	; Weapons carried
rs obj_weap2,1		; Weapon details
rs obj_weap_link,2	; List # of weapon

rs obj_find_status,2	; Current hierarchy level of find quest
rs obj_qtype1,1		; What type of info is in qdat1,2
rs obj_qtype2,1		; What type of info is in qdat3,4

rs obj_time,2		; time object finished quest
rs obj_wait,2		; hours to wait once finished

rs obj_flags2,2		; more flags
;rs obj_shelt_x,2	; position of player before sheltering on time races on
rs obj_shelt_y,2

rs army_len,0

;*****************************************************************************

;		---------- OBJECT DATA STRUCTURE ----------

rs_set (obj_cam_off+1)
rs obj_shield,1		; Heraldic shield type
rs obj_port,1		; Portrait number
rs obj_weapon_owner,2	; List # of owner
rs obj_weapon_type,1	; Weapon type
rs obj_weapon_details,1	; extra weapon info

rs object_len,0

;*****************************************************************************


