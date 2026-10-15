;-------------------------------------------------------------------------
;			LORDS OF MIDNIGHT III
;
;			    THE CITADEL
;-------------------------------------------------------------------------
; (C) James Shaw 1995.
;-------------------------------------------------------------------------

	MODEL HUGE
	IDEAL
	NOJUMPS
	P386

;-------------------------------------------------
; Kingdom, Realm & Neighbour Information
;-------------------------------------------------

		INCLUDE	'region.equ'
		INCLUDE	'equates.asm'
		INCLUDE 'structur.asm'
		
	SEGMENT	DATA_SEG PARA PUBLIC USE32 'LOMDATA'

;-------------------------------------------------
; Neighbouring kingdoms
;-------------------------------------------------
PUBLIC kingdom_adj
kingdom_adj	dd	A_Witherlands	
		dd	A_Lee		
		dd	A_Eldmark		
		dd	A_LongMountains	
		dd	A_LastNorthing	
		dd	A_Arungor		
		dd	A_Wierdwood	
		dd	A_Dawnwood	
		dd	A_Fallows		
		dd	A_Gelm		
		dd	A_Deeping		
		dd	A_Delve		
		dd	A_Marish		
		dd	A_Glimormir	
		dd	A_GreatOcean	
		dd	A_Midnight	
		dd	A_MistsOblivion

MACRO	A	title,links
	EVEN
title		db	links
ENDM

MACRO	D	linked
		db	linked
ENDM

A A_Witherlands,4
D Midnight
D Lee
D Glimormir
D Eldmark

A A_Lee,4
D Witherlands
D LongMountains
D Glimormir
D Eldmark

A A_Eldmark,4
D Witherlands
D Lee
D LongMountains
D Wierdwood

A A_LongMountains,6
D Lee
D LastNorthing
D Dawnwood
D Deeping
D Wierdwood
D Eldmark

A A_LastNorthing,3
D LongMountains
D Dawnwood
D Arungor

A A_Arungor,1
D LastNorthing

A A_Wierdwood,4
D Eldmark
D LongMountains
D Deeping
D Gelm

A A_Dawnwood,5
D LongMountains
D LastNorthing
D Fallows
D Marish
D Deeping

A A_Fallows,3
D Dawnwood
D Delve
D Marish

A A_Gelm,3
D Wierdwood
D Deeping
D Marish

A A_Deeping,5
D LongMountains
D Dawnwood
D Marish
D Gelm
D Wierdwood

A A_Delve,2
D Fallows
D Marish

A A_Marish,5
D Gelm
D Deeping
D Dawnwood
D Fallows
D Delve

A A_Glimormir,3
D Witherlands
D Lee
D Eldmark

;A A_GreatOcean,0
;JS 2/8/95
A A_GreatOcean,6
D Arungor
D LastNorthing
D Fallows
D Delve
D Marish
D Dawnwood

A A_Midnight,1
D Witherlands

;A A_MistsOblivion,0
;JS 2/8/95
A A_MistsOblivion,1
D Dawnwood			;send to middle of map!

;-------------------------------------------------
; Kingdom table
;-------------------------------------------------

PUBLIC kingdom_list
kingdom_list	dd	K_Witherlands	
		dd	K_Lee		
		dd	K_Eldmark		
		dd	K_LongMountains	
		dd	K_LastNorthing	
		dd	K_Arungor		
		dd	K_Weirdwood	
		dd	K_Dawnwood	
		dd	K_Fallows		
		dd	K_Gelm		
		dd	K_Deeping		
		dd	K_Delve		
		dd	K_Marish		
		dd	K_Glimormir	
		dd	K_GreatOcean	
		dd	K_Midnight	
		dd	K_MistsOblivion	

MACRO	K	title,links
	EVEN
title		db	links
ENDM

MACRO	R	linked
		db	linked
ENDM

;-----------------------------------------------
; Kingdom containment lists
;-----------------------------------------------

K K_Witherlands,4
R Imilvir
R Sareon
R Eomir
R Erifel

K K_Lee,9
R Eodral
R Thumar
R Sharvik
R Imildral
R Melthor
R Varorn
R Rilnor
R Cerevere
R Ceremelth

K K_Eldmark,5
R Arelban
R Arunvere
R Meranor
R Erilan
R Imilvir

K K_LongMountains,9
R Erenim
R Isilfrey
R Thordroth
R Arelon
R Farwain
R Beomir
R Saradwin
R Scaradir
R Corlon

K K_LastNorthing,15
R Sharmark
R Sapphire
R Elormane
R Melibor
R Rilleon
R Elmir
R Ravenfrey
R Mermaids
R Erefar
R Tharran
R Arelon
R Galorbard
R Crimson
R Orcar
R Caramane

K K_Arungor,10
R Angelf
R Dragon
R Rildroth
R Erivik
R Sarnoth
R Shrygal
R Faragor
R Serpents
R Cormir
R Ashnar

K K_Weirdwood,11
R Elinvar
R Arelmar
R Merikith
R Imilvir
R Shimmering
R Forgotten
R Whispering
R Golden
R Forever
R Aravik
R Aradel

K K_Dawnwood,8
R Maralan
R Romiel
R Songs
R Emerthen
R Elorthord
R Imilvir
R Corithel
R Carafel

K K_Fallows,9
R Alathor
R Elinbrand
R Arabar
R Theodel
R Cerrelm
R Skordroth
R Roreon
R Erilmark
R Athrudan

K K_Gelm,5
R Finfyr
R Geremiel
R Emergelm
R Samarand
R Gilgrath

K K_Deeping,8
R Thunder
R Ararak
R Dawn
R Tarefyr
R Sharenor
R Tharn
R Ildemar
R Zaragorn

K K_Delve,12
R Grorn
R Finrod
R Corth
R Gogrun
R Iron
R Tark
R Othrym
R Big
R Storms
R Skirol
R Athruk
R Thordrin

K K_Marish,10
R Malgor
R Caradwin
R Dwardor
R Barathor
R Death
R Valgrorn
R Maranor
R Falthrang
R Ravens
R Burning

K K_Glimormir,6
R Northern
R Eastern
R Western
R Southern
R Middle
R Immiel

K K_GreatOcean,9
R Galorwain
R Fire
R Weeping
R Eregoth
R Grey
R Silfar
R Skulls
R Ulmor
R Shining

K K_Midnight,3
R Corelay
R Jade
R Last

K K_MistsOblivion,0

;------------------------------------------------
; Table of place pointers
;------------------------------------------------

PUBLIC place_list
place_list	dd	R_Imilvir			
		dd	R_Eodral			
		dd	R_Arelon			
		dd	R_Aravik			

		dd	R_Caradwin		
		dd	R_Falthrang		
		dd	R_Elinmor			
		dd	R_Tarefyr			
		
		dd	R_Northern		
		dd	R_Eastern			
		dd	R_Southern		
		dd	R_Western			
		
		dd	R_Middle			
		dd	R_Ulmor			
		dd	R_Skulls			
		dd	R_Shining			
		
		dd	R_Silfar			
		dd	R_Grey			
		dd	R_Eregoth			
		dd	R_Weeping			
		
		dd	R_Fire			
		dd	R_Galorwain		
		dd	R_Tharran			
		dd	R_Mermaids		
		
		dd	R_Serpents		
		dd	R_Immiel			
		dd	R_Rilleon			
		dd	R_Maranor			
		
		dd	R_Corelay			
		dd	R_Jade			
		dd	R_Last			
		dd	R_Erifel			
		
		dd	R_Sareon			
		dd	R_Eomir			
		dd	R_Rilnor			
		dd	R_Thumar			
		
		dd	R_Sharvik			
		dd	R_Imildral		
		dd	R_Erenim			
		dd	R_Isilfrey		
		
		dd	R_Thordroth		
		dd	R_Galorbard		
		dd	R_Corlon			
		dd	R_Farwain			
		
		dd	R_Maralan			
		dd	R_Beomir			
		dd	R_Ceremelth		
		dd	R_Carafel			
		
		dd	R_Saradwin		
		dd	R_Emerthen		
		dd	R_Scaradir		
		dd	R_Meranor			
		
		dd	R_Arelban			
		dd	R_Arunvere		
		dd	R_Erilan			
		dd	R_Elinvar			
		
		dd	R_Arelmar			
		dd	R_Shimmering		
		dd	R_Forgotten		
		dd	R_Whispering		
		
		dd	R_Forever			
		dd	R_Golden			
		dd	R_Finfyr			
		dd	R_Geremiel		
		
		dd	R_Samarand		
		dd	R_Emergelm		
		dd	R_Ildemar			
		dd	R_Zaragorn		
		
		dd	R_Gilgrath		
		dd	R_Tharn			
		dd	R_Sharenor		
		dd	R_Ararak			
		
		dd	R_Thunder			
		dd	R_Dawn			
		dd	R_Merikith		
		dd	R_Aradel			
		
		dd	R_Valgrorn		
		dd	R_Malgor			
		dd	R_Dwardor			
		dd	R_Death			
		
		dd	R_Barathor		
		dd	R_Burning			
		dd	R_Ravens			
		dd	R_Iron			
		
		dd	R_Tark			
		dd	R_Athruk			
		dd	R_Thordrin		
		dd	R_Storms			
		
		dd	R_Big			
		dd	R_Corth			
		dd	R_Skirol			
		dd	R_Othrym			
		
		dd	R_Grorn			
		dd	R_Gogrun			
		dd	R_Finrod			
		dd	R_Songs			
		
		dd	R_Crimson			
		dd	R_Elorthord		
		dd	R_Corithel		
		dd	R_Alathor			
		
		dd	R_Elinbrand		
		dd	R_Gliwain			
		dd	R_Athrudan		
		dd	R_Skordroth		
		
		dd	R_Erilmark		
		dd	R_Arabar			
		dd	R_Theodel			
		dd	R_Cerrelm			
		
		dd	R_Roreon			
		dd	R_Caramane		
		dd	R_Romiel			
		dd	R_Orcar			
		
		dd	R_Ravenfrey		
		dd	R_Erefar			
		dd	R_Elmir			
		dd	R_Melibor			
		
		dd	R_Sharmark		
		dd	R_Sapphire		
		dd	R_Elormane		
		dd	R_Dragon			
		
		dd	R_Rildroth		
		dd	R_Angelf			
		dd	R_Erivik			
		dd	R_Sarnoth			
		
		dd	R_Shrygal			
		dd	R_Faragor			
		dd	R_Ashnar			
		dd	R_Cormir			
		
		dd	R_Cerevere		
		dd	R_Melthor			
		dd	R_Varorn			
		
		dd	R_MistsOblivion
		dd	R_MistsOblivion
		

MACRO	M	title,links
	EVEN
title		db	links
ENDM

MACRO	L	linked
		db	linked
ENDM

;-----------------------------------------------
; Neighbour lists
;-----------------------------------------------

M R_Imilvir,25
L Corelay
L Last
L Jade
L Erifel
L Eomir
L Northern
L Meranor
L Erilan
L Saradwin
L Elinvar
L Merikith
L Shimmering
L Forgotten
L Whispering
L Aradel
L Scaradir
L Emerthen
L Carafel
L Songs
L Corithel
L Elorthord
L Southern
L Aravik
L Eregoth
L Elinmor

M R_Eodral,6
L Rilnor
L Varorn
L Thumar
L Sharvik
L Northern
L Elinmor

M R_Arelon,8
L Thordroth
L Isilfrey
L Rilleon
L Galorbard
L Erefar
L Orcar
L Tharran
L Corlon

M R_Aravik,6
L Forever
L Forgotten
L Whispering
L Finfyr
L Geremiel
L Imilvir

M R_Caradwin,11
L Tarefyr
L Silfar
L Dawn
L Malgor
L Barathor
L Grorn
L Thunder
L Corithel
L Gliwain
L Athrudan
L Erilmark

M R_Falthrang,9
L Gilgrath
L Zaragorn
L Valgrorn
L Maranor
L Death
L Gilgrath
L Burning
L Ravens
L Ulmor

M R_Elinmor,8
L Thumar
L Sharvik
L Imildral
L Melthor
L Varorn
L Imilvir
L Eodral
L Rilnor

M R_Tarefyr,8
L Caradwin
L Thunder
L Ararak
L Sharenor
L Ildemar
L Zaragorn
L Tharn
L Dawn

M R_Northern,8
L Imilvir
L Middle
L Eomir
L Rilnor
L Varorn
L Eastern
L Eodral
L Western

M R_Eastern,6
L Northern
L Southern
L Middle
L Varorn
L Cerevere
L Ceremelth

M R_Southern,6
L Western
L Middle
L Eastern
L Imilvir
L Arunvere
L Meranor

M R_Western,6
L Eomir
L Northern
L Southern
L Middle
L Arelban
L Arunvere

M R_Middle,5
L Northern
L Southern
L Eastern
L Western
L Immiel

M R_Ulmor,9
L Falthrang
L Maranor
L Ravens
L Burning
L Death
L Tark
L Athruk
L Skulls
L Shining

M R_Skulls,9
L Athruk
L Thordrin
L Ulmor
L Shining
L Storms
L Big
L Corth
L Silfar
L Grey

M R_Shining,4
L Burning
L Ulmor
L Skulls
L Grey

M R_Silfar,7
L Caradwin
L Erilmark
L Roreon
L Grey
L Finrod
L Corth
L Skulls

M R_Grey,5
L Roreon
L Weeping
L Silfar
L Skulls
L Shining

M R_Eregoth,11
L Fire
L Weeping
L Roreon
L Cerrelm
L Arabar
L Elinbrand
L Alathor
L Imilvir
L Songs
L Romiel
L Caramane

M R_Weeping,8
L Fire
L Ashnar
L Shrygal
L Eregoth
L Roreon
L Grey
L Galorwain
L Sarnoth

M R_Fire,14
L Eregoth
L Weeping
L Ashnar
L Cormir
L Melibor
L Mermaids
L Ravenfrey
L Tharran
L Caramane
L Serpents
L Faragor
L Rildroth
L Angelf
L Fire

M R_Galorwain,8
L Sharmark
L Melibor
L Elormane
L Fire
L Angelf
L Weeping
L Erivik
L Sarnoth


M R_Tharran,6
L Fire
L Caramane
L Arelon
L Erefar
L Elmir
L Ravenfrey

M R_Mermaids,3
L Ravenfrey
L Melibor
L Fire

M R_Serpents,3
L Fire
L Cormir
L Faragor

M R_Immiel,1
L Middle

M R_Rilleon,9
L Arelon
L Thordroth
L Galorbard
L Orcar
L Erefar
L Elmir
L Sapphire
L Isilfrey
L Corlon

M R_Maranor,5
L Falthrang
L Valgrorn
L Death
L Ravens
L Ulmor

M R_Corelay,2
L Imilvir
L Last

M R_Jade,5
L Imilvir
L Last
L Erifel
L Eomir
L Sareon

M R_Last,3
L Imilvir
L Corelay
L Jade

M R_Erifel,6
L Jade
L Sareon
L Rilnor
L Eomir
L Imilvir
L Eomir

M R_Sareon,4
L Jade
L Erifel
L Rilnor
L Thumar

M R_Eomir,7
L Imilvir
L Northern
L Western
L Erifel
L Rilnor
L Jade
L Arelban

M R_Rilnor,8
L Sareon
L Erifel
L Eomir
L Northern
L Varorn
L Elinmor
L Thumar
L Eodral

M R_Thumar,5
L Elinmor
L Sharvik
L Rilnor
L Sareon
L Eodral

M R_Sharvik,6
L Thumar
L Erenim
L Imildral
L Elinmor
L Thumar
L Eodral

M R_Imildral,6
L Sharvik
L Erenim
L Isilfrey
L Thordroth
L Melthor
L Elinmor

M R_Erenim,5
L Sharmark
L Sapphire
L Isilfrey
L Imildral
L Sharvik

M R_Isilfrey,7
L Erenim
L Sapphire
L Rilleon
L Arelon
L Thordroth
L Imildral
L Corlon

M R_Thordroth,8
L Arelon
L Isilfrey
L Rilleon
L Galorbard
L Farwain
L Melthor
L Imildral
L Corlon

M R_Galorbard,7
L Thordroth
L Arelon
L Rilleon
L Orcar
L Crimson
L Maralan
L Farwain

M R_Corlon,4
L Arelon
L Thordroth
L Rilleon
L Isilfrey

M R_Farwain,6
L Thordroth
L Galorbard
L Maralan
L Beomir
L Ceremelth
L Melthor

M R_Maralan,5
L Farwain
L Galorbard
L Crimson
L Carafel
L Beomir

M R_Beomir,7
L Ceremelth
L Farwain
L Maralan
L Carafel
L Scaradir
L Saradwin
L Meranor

M R_Ceremelth,6
L Cerevere
L Melthor
L Farwain
L Beomir
L Meranor
L Eastern

M R_Carafel,8
L Maralan
L Crimson
L Songs
L Elorthord
L Imilvir
L Emerthen
L Scaradir
L Beomir

M R_Saradwin,5
L Imilvir
L Beomir
L Scaradir
L Merikith
L Erilan

M R_Emerthen,5
L Carafel
L Imilvir
L Elorthord
L Corithel
L Scaradir

M R_Scaradir,9
L Imilvir
L Beomir
L Carafel
L Emerthen
L Corithel
L Thunder
L Ararak
L Merikith
L Saradwin

M R_Meranor,6
L Southern
L Imilvir
L Ceremelth
L Erilan
L Arunvere
L Beomir

M R_Arelban,3
L Eomir
L Western
L Arunvere

M R_Arunvere,5
L Western
L Southern
L Arelban
L Meranor
L Erilan

M R_Erilan,5
L Arunvere
L Meranor
L Imilvir
L Saradwin
L Elinvar

M R_Elinvar,6
L Erilan
L Imilvir
L Merikith
L Shimmering
L Forgotten
L Arelmar

M R_Arelmar,2
L Elinvar
L Forgotten

M R_Shimmering,6
L Imilvir
L Elinvar
L Merikith
L Aradel
L Whispering
L Forgotten

M R_Forgotten,7
L Arelmar
L Elinvar
L Imilvir
L Shimmering
L Whispering
L Aravik
L Forever

M R_Whispering,8
L Shimmering
L Imilvir
L Aradel
L Golden
L Finfyr
L Aravik
L Forever
L Forgotten

M R_Forever,5
L Forgotten
L Aravik
L Whispering
L Finfyr
L Geremiel

M R_Golden,5
L Whispering
L Aradel
L Ararak
L Sharenor
L Finfyr

M R_Finfyr,6
L Whispering
L Golden
L Sharenor
L Geremiel
L Aravik
L Forever

M R_Geremiel,7
L Forever
L Aravik
L Finfyr
L Sharenor
L Ildemar
L Samarand
L Emergelm

M R_Samarand,5
L Geremiel
L Ildemar
L Zaragorn
L Gilgrath
L Emergelm

M R_Emergelm,2
L Geremiel
L Samarand

M R_Ildemar,5
L Sharenor
L Tarefyr
L Zaragorn
L Samarand
L Geremiel

M R_Zaragorn,7
L Tarefyr
L Tharn
L Valgrorn
L Falthrang
L Gilgrath
L Samarand
L Ildemar

M R_Gilgrath,5
L Falthrang
L Valgrorn
L Burning
L Samarand
L Zaragorn

M R_Tharn,4
L Dawn
L Valgrorn
L Zaragorn
L tarefyr

M R_Sharenor,6
L Golden
L Ararak
L Tarefyr
L Ildemar
L Geremiel
L Finfyr

M R_Ararak,6
L Aradel
L Scaradir
L Thunder
L Tarefyr
L Sharenor
L Golden

M R_Thunder,6
L Scaradir
L Corithel
L Caradwin
L Dawn
L Tarefyr
L Ararak

M R_Dawn,7
L Caradwin
L Thunder
L Malgor
L Dwardor
L Valgrorn
L Tharn
L Tarefyr

M R_Merikith,6
L Imilvir
L Saradwin
L Scaradir
L Aradel
L Shimmering
L Elinvar

M R_Aradel,6
L Imilvir
L Merikith
L Ararak
L Golden
L Whispering
L Shimmering

M R_Valgrorn,10
L Dwardor
L Death
L Maranor
L Falthrang
L Ravens
L Gilgrath
L Zaragorn
L Tharn
L Dawn
L Burning

M R_Malgor,7
L Caradwin
L Gliwain
L Athrudan
L Barathor
L Dwardor
L Dawn
L Corithel

M R_Dwardor,5
L Malgor
L Barathor
L Death
L Valgrorn
L Dawn

M R_Death,8
L Barathor
L Maranor
L Ulmor
L Falthrang
L Valgrorn
L Dwardor
L Iron
L Tark

M R_Barathor,8
L Athrudan
L Caradwin
L Erilmark
L Grorn
L Iron
L Death
L Dwardor
L Malgor

M R_Burning,6
L Gilgrath
L Falthrang
L Ravens
L Valgrorn
L Ulmor
L Shining

M R_Ravens,5
L Falthrang
L Valgrorn
L Maranor
L Ulmor
L Burning

M R_Iron,5
L Grorn
L Gogrun
L Tark
L Death
L Barathor

M R_Tark,6
L Gogrun
L Othrym
L Athruk
L Ulmor
L Death
L Iron

M R_Athruk,5
L Othrym
L Thordrin
L Skulls
L Ulmor
L Tark

M R_Thordrin,5
L Othrym
L Skirol
L Storms
L Skulls
L Athruk

M R_Storms,4
L Skulls
L Thordrin
L Skirol
L Big

M R_Big,4
L Skulls
L Storms
L Skirol
L Corth

M R_Corth,5
L Silfar
L Skulls
L Big
L Skirol
L Finrod

M R_Skirol,7
L Corth
L Big
L Storms
L Thordrin
L Othrym
L Gogrun
L Finrod

M R_Othrym,6
L Gogrun
L Skirol
L Thordrin
L Athruk
L Tark
L Gogrun

M R_Grorn,6
L Erilmark
L Caradwin
L Finrod
L Gogrun
L Iron
L Barathor

M R_Gogrun,6
L Grorn
L Finrod
L Skirol
L Othrym
L Tark
L Iron

M R_Finrod,6
L Silfar
L Corth
L Skirol
L Gogrun
L Grorn
L Erilmark

M R_Songs,7
L Imilvir
L Eregoth
L Carafel
L Crimson
L Romiel
L Elorthord
L Alathor

M R_Crimson,6
L Songs
L Romiel
L Orcar
L Galorbard
L Maralan
L Carafel

M R_Elorthord,7
L Imilvir
L Elinbrand
L Gliwain
L Corithel
L Emerthen
L Carafel
L Songs

M R_Corithel,8
L Imilvir
L Elorthord
L Gliwain
L Caradwin
L Malgor
L Thunder
L Scaradir
L Emerthen

M R_Alathor,5
L Eregoth
L Arabar
L Elinbrand
L Songs
L Romiel

M R_Elinbrand,7
L Eregoth
L Arabar
L Alathor
L Theodel
L Skordroth
L Gliwain
L Elorthord

M R_Gliwain,7
L Elinbrand
L Skordroth
L Athrudan
L Caradwin
L Malgor
L Corithel
L Elorthord

M R_Athrudan,6
L Skordroth
L Erilmark
L Caradwin
L Barathor
L Malgor
L Gliwain

M R_Skordroth,5
L Theodel
L Erilmark
L Athrudan
L Gliwain
L Elinbrand

M R_Erilmark,10
L Cerrelm
L Roreon
L Silfar
L Finrod
L Grorn
L Caradwin
L Barathor
L Athrudan
L Skordroth
L Theodel

M R_Arabar,5
L Eregoth
L Alathor
L Cerrelm
L Theodel
L Elinbrand

M R_Theodel,5
L Arabar
L Cerrelm
L Erilmark
L Skordroth
L Elinbrand

M R_Cerrelm,5
L Eregoth
L Roreon
L Erilmark
L Theodel
L Arabar

M R_Roreon,6
L Eregoth
L Weeping
L Grey
L Silfar
L Erilmark
L Cerrelm

M R_Caramane,5
L Tharran
L Fire
L Eregoth
L Romiel
L Orcar

M R_Romiel,6
L Orcar
L Caramane
L Eregoth
L Alathor
L Songs
L Crimson

M R_Orcar,7
L Erefar
L Arelon
L Caramane
L Romiel
L Crimson
L Galorbard
L Rilleon

M R_Ravenfrey,5
L Melibor
L Mermaids
L Fire
L Tharran
L Elmir

M R_Erefar,5
L Elmir
L Tharran
L Orcar
L Arelon
L Rilleon

M R_Elmir,6
L Sapphire
L Melibor
L Ravenfrey
L Tharran
L Erefar
L Rilleon

M R_Melibor,10
L Elormane
L Galorwain
L Mermaids
L Angelf
L Rildroth
L Fire
L Ravenfrey
L Elmir
L Sapphire
L Sharmark

M R_Sharmark,5
L Galorwain
L Elormane
L Melibor
L Sapphire
L Erenim

M R_Sapphire,6
L Sharmark
L Melibor
L Elmir
L Rilleon
L Isilfrey
L Erenim

M R_Elormane,3
L Galorwain
L Melibor
L Sharmark

M R_Dragon,6
L Angelf
L Erivik
L Sarnoth
L Shrygal
L Rildroth
L Faragor

M R_Rildroth,5
L Fire
L Melibor
L Angelf
L Dragon
L Faragor

M R_Angelf,6
L Galorwain
L Dragon
L Fire
L Melibor
L Erivik
L Rildroth

M R_Erivik,4
L Angelf
L Galorwain
L Sarnoth
L Dragon

M R_Sarnoth,5
L Erivik
L Galorwain
L Shrygal
L Weeping
L Dragon

M R_Shrygal,5
L Weeping
L Ashnar
L Faragor
L Dragon
L Sarnoth

M R_Faragor,7
L Fire
L Serpents
L Rildroth
L Dragon
L Shrygal
L Ashnar
L Cormir

M R_Ashnar,5
L Weeping
L Shrygal
L Faragor
L Cormir
L Fire

M R_Cormir,4
L Serpents
L Fire
L Faragor
L Ashnar

M R_Cerevere,4
L Varorn
L Melthor
L Ceremelth
L Eastern

M R_Melthor,7
L Imildral
L Thordroth
L Farwain
L Ceremelth
L Cerevere
L Varorn
L Elinmor

M R_Varorn,7
L Elinmor
L Rilnor
L Eodral
L Northern
L Eastern
L Cerevere
L Melthor

M R_MistsOblivion,0

	ENDS


	SEGMENT _TEXT PARA PUBLIC USE32 'CODE'
	ASSUME CS:_TEXT,DS:DATA_SEG,SS:NOTHING

EXTRN modrandn:NEAR
EXTRN map_position:NEAR

; IN
;    ah, bh : region numbers
;    al, bl : realm numbers
; OUT
;    carry flag set if they are, clear if they aren't
; All registers preserved
PUBLIC regions_neighbours
PROC regions_neighbours NEAR
	push	esi eax

;--- JS 3/8/95
	cmp	al,bl			;same kingdom (good idea!)
	je	@@yes

	call	kingdoms_neighbours	;or neighbour kingdom
	jne	@@n

@@yes:
;--- Stops detection of rivers too far away.
;--- A neighbour land must be in same (or neighbouring) kingdom.

	movzx	eax,ah
	mov	esi,[place_list+eax*4]

	mov	al,[esi]		;number of entries
	test	al,-1
	je	@@n			;zero!

	add	esi,1
@@1:
	cmp	[esi],bh		;any entries = other region?
	je	@@y			;yep

	inc	esi			;next
	dec	al
	jne	@@1

	jmp	@@n			;nope
@@y:
	pop	eax esi	
    	stc
	ret
@@n:
	pop	eax esi	
	clc
	ret
ENDP

; IN
;    ah, bh : region numbers
; OUT
;    carry flag set if they are, clear if they aren't
; All registers preserved
PUBLIC regions_neighbours_not_water
PROC regions_neighbours_not_water NEAR
	push	esi eax

	movzx	eax,ah
	mov	esi,[place_list+eax*4]

	mov	al,[esi]		;number of entries
	test	al,-1
	je	@@n			;zero!

	inc	esi
@@1:
	mov	ah,[esi]

	cmp	ah,25			;not a piece of water!
	jb	@@2

	cmp	ah,Corlon
	je	@@2
	cmp	ah,Elormane
	je	@@2

	cmp	ah,bh			;any entries = other region?
	je	@@y			;yep
@@2:
	inc	esi			;next
	dec	al
	jne	@@1

	jmp	@@n			;nope
@@y:
	pop	eax esi	
    	stc
	ret
@@n:
	pop	eax esi	
	clc
	ret
ENDP



;IN
;   al, bl : kingdoms
;OUT
;   carry set if kingdoms neighbour each other
; All registers preserved
;JS 3/8/95
PROC kingdoms_neighbours NEAR
	push	eax ecx esi

	movzx	eax,al
	mov	esi,[kingdom_adj+eax*4]
	movzx	ecx,[BYTE esi]
	cmp	ecx,0
	je	@@no

	inc	esi
@@1:
	cmp	[BYTE esi],bl
	je	@@yes
	inc	esi		;JS 9/8/95 ARGH!
	loop	@@1

@@no:
	pop	esi ecx eax
	clc
	ret

@@yes:
	pop	esi ecx eax
	stc
	ret
ENDP

; IN
;    al  : Current kingdom number
; OUT
;    eax : Random region # in neighbouring kingdom
;    ebx : Kingdom #		;JS 3/8/95
;    carry set if valid result
; All other registers preserved
PUBLIC kingdom_neighbour
PROC kingdom_neighbour NEAR
	push	esi edx

	movzx	eax,al
	mov	esi,[kingdom_adj+eax*4]
	movzx	ebx,[BYTE esi]				;bug if al = 131 ie. Mists of Oblivion
							;it contains no regions!  And there's not really
							;a default answer!
	test	ebx,-1
	je	@@panic
	call	modrandn
	movzx	eax,[BYTE 1+esi+eax]

	mov	edx,eax		;JS 3/8/95

	mov	esi,[kingdom_list+eax*4]
	movzx	ebx,[BYTE esi]
	test	ebx,-1
	je	@@panic

	mov	ecx,20
@@again:
	call	modrandn
	movzx	eax,[BYTE esi+1+eax]

	dec	ecx		;panic, 20 goes with no selection
	je	@@panic		;JS 13/7/95am

	cmp	eax,25					;don't allow rivers!
	jb	@@again

	cmp	eax,Corlon
	je	@@again

	cmp	eax,Elormane
	je	@@again

	mov	ebx,edx		;JS 3/8/95

	stc
	pop	edx esi
	ret
@@panic:
	mov	ebx,edx		;JS 3/8/95

	clc
	pop	edx esi
	ret
ENDP

;---------------
;IN esi : this character
;   edi : dest character
;    ah : this region
;    bh : dest region
;
;OUT carry set if ah and bh are in the same kingdom
;
;All regs preserved
PUBLIC in_same_kingdom
PROC in_same_kingdom NEAR
	push	ecx ebx eax
	mov	eax,[esi+obj_x]
	mov	ebx,[esi+obj_y]
	shr	eax,16
	shr	ebx,16
	call	map_position
	push	eax			;realm 1

	mov	eax,[edi+obj_x]
	mov	ebx,[edi+obj_y]
	shr	eax,16
	shr	ebx,16
	call	map_position
	mov	ebx,eax			;realm 2
	pop	eax
	cmp	eax,ebx
	je	@@ok
    	clc
	jmp	@@x
@@ok:
	stc
@@x:
	pop	eax ebx ecx
	ret
ENDP

;IN  eax : realm to get a random citadel in
;    esi : this char
;OUT eax : random region number
;    carry set if ok
;    carry clear if failed
PUBLIC get_region_in_kingdom
PROC get_region_in_kingdom NEAR
	push	ebx edi

	mov	eax,[esi+obj_x]
	mov	ebx,[esi+obj_y]
	shr	eax,16
	shr	ebx,16
	call	map_position

	mov	edi,[kingdom_list+eax*4]
	movzx	ebx,[BYTE edi]
	test	ebx,-1
	je	@@n

	mov	ecx,20
@@again:
	call	modrandn
	movzx	eax,[BYTE edi+1+eax]

	dec	ecx	;panic, 20 goes with no selection!
	je	@@n	;JS 13/7/95am

	cmp	eax,25		;don't allow rivers!
	jb	@@again

	cmp	eax,Corlon	;JS 29/7/95
	je	@@again

	cmp	eax,Elormane	;JS 29/7/95
	je	@@again

	stc
	pop	edi ebx
	ret
@@n:
	clc
	pop	edi ebx
	ret
ENDP

;IN eax - land # to get neighbour of
;OUT ebx - neighbour land
PUBLIC get_neighbour_land
PROC get_neighbour_land NEAR
	push	esi
	mov	esi,[place_list+eax*4]

	movzx	ebx,[BYTE esi]
	cmp	bl,0
	je	@@panic

	inc	esi
@@1:
	call	modrandn
	movzx	ebx,[BYTE esi+eax*4]

	cmp	ebx,25
	jb	@@1

	cmp	ebx,Corlon
	je	@@1

	cmp	ebx,Elormane
	je	@@1

	pop	esi
	ret
@@panic:
@@2:
	mov	ebx,130
	call	modrandn
	mov	ebx,eax

	cmp	ebx,25
	jb	@@2

	cmp	ebx,Corlon
	je	@@2

	cmp	ebx,Elormane
	je	@@2

	pop	esi
	ret
ENDP

;IN ah - it's realm
;   bh - my realm
;OUT carry set if either in water
PUBLIC do_water_test_either
PROC do_water_test_either NEAR
	cmp	ah,25
	jb	@@xs
	cmp	ah,Corlon
	je	@@xs
	cmp	ah,Elormane
	je	@@xs

	cmp	bh,25
	jb	@@xs
	cmp	bh,Corlon
	je	@@xs
	cmp	bh,Elormane
	je	@@xs

	clc
	ret
@@xs:
	stc
	ret
ENDP

;IN ah - it's realm
;   bh - my realm
;OUT carry set if both in water
PUBLIC do_water_test_both
PROC do_water_test_both NEAR
	cmp	bh,25
	jb	@@xs
	cmp	bh,Corlon
	je	@@xs
	cmp	bh,Elormane
	je	@@xs

	clc
	ret
@@xs:
	cmp	ah,25
	jb	@@xs2
	cmp	ah,Corlon
	je	@@xs2
	cmp	ah,Elormane
	je	@@xs2

	clc
	ret
@@xs2:
	stc
	ret
ENDP

	ENDS

	END
