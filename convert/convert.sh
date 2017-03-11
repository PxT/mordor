#!/bin/csh -f
#
# Usage: $0 <c|o|r|p>
# converts creatures,objects,rooms, or players
# Set objmax,crtmax and rommax + paths below!

# Location of the executable
set convert="/home/muds/develop/mordor6.0/convert/convert"

set playerdir="../player"
set roomdir="../rooms"
set objdir="../objmon"

# The highest numbered+1 object/crt in the DB
@ objmax = 101
@ crtmax = 1000
@ rommax = 10000

if ($1 == "p") then
	cd $playerdir	
	set players=`echo *`
	foreach ply ($players)
		$convert -p -v $ply
		mv $ply.new $ply
	end
endif

if ($1 == "r") then
	cd $roomdir	
	set rooms=`echo r*`
	@ n = 0
	while($n < $rommax)
		$convert -r -v $n
		@ n = $n + 1
	end	
	foreach rom ($rooms)
		mv $rom.new $rom
	end
endif

if ($1 == "o") then
	cd $objdir	
	set objs=`echo o??`
	@ n = 0
	while ($n < $objmax)
		$convert -o -v $n
		@ n = $n + 1
	end
	foreach obj ($objs)
		mv new.$obj $obj
	end
endif

if ($1 == "c") then
	cd $objdir	
	set crts=`echo m??`
	@ n = 0
	while ($n < $crtmax)
		$convert -c -v $n
		@ n = $n + 1
	end
	foreach crt ($crts)
		mv new.$crt $crt
	end
endif
