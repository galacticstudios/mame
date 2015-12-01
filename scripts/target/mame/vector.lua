-- license:BSD-3-Clause
-- copyright-holders:MAMEdev Team

---------------------------------------------------------------------------
--
--   vector.lua
--
--   Small driver-specific example makefile
--   Use make SUBTARGET=vector to build
--
---------------------------------------------------------------------------


--------------------------------------------------
-- Specify all the CPU cores necessary for the
-- drivers referenced in vector.lst.
--------------------------------------------------

CPUS["Z80"] = true
CPUS["M6502"] = true
CPUS["MCS48"] = true
CPUS["MCS51"] = true
CPUS["M6800"] = true
CPUS["M6809"] = true
CPUS["M680X0"] = true
CPUS["TMS9900"] = true
CPUS["TMS32010"] = true
CPUS["COP400"] = true
CPUS["CCPU"] = true

--------------------------------------------------
-- Specify all the sound cores necessary for the
-- drivers referenced in vector.lst.
--------------------------------------------------

SOUNDS["SAMPLES"] = true
SOUNDS["DAC"] = true
SOUNDS["DISCRETE"] = true
SOUNDS["AY8910"] = true
SOUNDS["YM2151"] = true
SOUNDS["ASTROCADE"] = true
SOUNDS["TMS5220"] = true
SOUNDS["OKIM6295"] = true
SOUNDS["HC55516"] = true
SOUNDS["YM3812"] = true
SOUNDS["CEM3394"] = true
SOUNDS["VOTRAX"] = true
SOUNDS["POKEY"] = true
SOUNDS["SP0250"] = true

--------------------------------------------------
-- specify available video cores
--------------------------------------------------

--------------------------------------------------
-- specify available machine cores
--------------------------------------------------

MACHINES["6821PIA"] = true
MACHINES["TTL74148"] = true
MACHINES["TTL74153"] = true
MACHINES["TTL7474"] = true
MACHINES["TIMEKPR"] = true
MACHINES["RIOT6532"] = true
MACHINES["PIT8253"] = true
MACHINES["Z80CTC"] = true
MACHINES["68681"] = true
MACHINES["BANKDEV"] = true
MACHINES["X2212"] = true
MACHINES["6522VIA"] = true
MACHINES["6840PTM"] = true


--------------------------------------------------
-- specify available bus cores
--------------------------------------------------

BUSES["GENERIC"] = true
BUSES["VECTREX"] = true

--------------------------------------------------
-- This is the list of files that are necessary
-- for building all of the drivers referenced
-- in vector.lst
--------------------------------------------------

function createProjects_mame_vector(_target, _subtarget)
	project ("mame_vector")
	targetsubdir(_target .."_" .. _subtarget)
	kind (LIBTYPE)
	uuid (os.uuid("drv-mame-vector"))
	
	includedirs {
		MAME_DIR .. "src/osd",
		MAME_DIR .. "src/emu",
		MAME_DIR .. "src/devices",
		MAME_DIR .. "src/mame",
		MAME_DIR .. "src/lib",
		MAME_DIR .. "src/lib/util",
		MAME_DIR .. "src/lib/netlist",
		MAME_DIR .. "3rdparty",
		GEN_DIR  .. "mame/layout",
	}

files {
	MAME_DIR .. "src/mame/video/avgdvg.cpp",
	MAME_DIR .. "src/mame/video/avgdvg.h",

	MAME_DIR .. "src/mame/drivers/vectrex.cpp",
	MAME_DIR .. "src/mame/includes/vectrex.h",
	MAME_DIR .. "src/mame/machine/vectrex.cpp",
	MAME_DIR .. "src/mame/video/vectrex.cpp",

	MAME_DIR .. "src/mame/drivers/arcadecl.cpp",
	MAME_DIR .. "src/mame/includes/arcadecl.h",
	MAME_DIR .. "src/mame/video/arcadecl.cpp",
	MAME_DIR .. "src/mame/drivers/asteroid.cpp",
	MAME_DIR .. "src/mame/includes/asteroid.h",
	MAME_DIR .. "src/mame/machine/asteroid.cpp",
	MAME_DIR .. "src/mame/audio/asteroid.cpp",
	MAME_DIR .. "src/mame/audio/llander.cpp",
	MAME_DIR .. "src/mame/drivers/atarifb.cpp",
	MAME_DIR .. "src/mame/includes/atarifb.h",
	MAME_DIR .. "src/mame/machine/atarifb.cpp",
	MAME_DIR .. "src/mame/audio/atarifb.cpp",
	MAME_DIR .. "src/mame/video/atarifb.cpp",
	MAME_DIR .. "src/mame/drivers/atarig1.cpp",
	MAME_DIR .. "src/mame/includes/atarig1.h",
	MAME_DIR .. "src/mame/video/atarig1.cpp",
	MAME_DIR .. "src/mame/includes/slapstic.h",
	MAME_DIR .. "src/mame/drivers/atarig42.cpp",
	MAME_DIR .. "src/mame/includes/atarig42.h",
	MAME_DIR .. "src/mame/video/atarig42.cpp",
	MAME_DIR .. "src/mame/drivers/atarigt.cpp",
	MAME_DIR .. "src/mame/includes/atarigt.h",
	MAME_DIR .. "src/mame/video/atarigt.cpp",
	MAME_DIR .. "src/mame/drivers/atarigx2.cpp",
	MAME_DIR .. "src/mame/includes/atarigx2.h",
	MAME_DIR .. "src/mame/video/atarigx2.cpp",
	MAME_DIR .. "src/mame/drivers/atarisy1.cpp",
	MAME_DIR .. "src/mame/includes/atarisy1.h",
	MAME_DIR .. "src/mame/video/atarisy1.cpp",
	MAME_DIR .. "src/mame/drivers/atarisy2.cpp",
	MAME_DIR .. "src/mame/includes/atarisy2.h",
	MAME_DIR .. "src/mame/video/atarisy2.cpp",
	MAME_DIR .. "src/mame/drivers/atarisy4.cpp",
	MAME_DIR .. "src/mame/drivers/atarittl.cpp",
	MAME_DIR .. "src/mame/drivers/avalnche.cpp",
	MAME_DIR .. "src/mame/includes/avalnche.h",
	MAME_DIR .. "src/mame/audio/avalnche.cpp",
	MAME_DIR .. "src/mame/drivers/bwidow.cpp",
	MAME_DIR .. "src/mame/includes/bwidow.h",
	MAME_DIR .. "src/mame/audio/bwidow.cpp",
	MAME_DIR .. "src/mame/drivers/bzone.cpp",
	MAME_DIR .. "src/mame/includes/bzone.h",
	MAME_DIR .. "src/mame/audio/bzone.cpp",
	MAME_DIR .. "src/mame/drivers/eprom.cpp",
	MAME_DIR .. "src/mame/includes/eprom.h",
	MAME_DIR .. "src/mame/video/eprom.cpp",
	MAME_DIR .. "src/mame/drivers/jedi.cpp",
	MAME_DIR .. "src/mame/includes/jedi.h",
	MAME_DIR .. "src/mame/audio/jedi.cpp",
	MAME_DIR .. "src/mame/video/jedi.cpp",
	MAME_DIR .. "src/mame/drivers/mhavoc.cpp",
	MAME_DIR .. "src/mame/includes/mhavoc.h",
	MAME_DIR .. "src/mame/machine/mhavoc.cpp",
	MAME_DIR .. "src/mame/drivers/quantum.cpp",
	MAME_DIR .. "src/mame/drivers/starwars.cpp",
	MAME_DIR .. "src/mame/includes/starwars.h",
	MAME_DIR .. "src/mame/machine/starwars.cpp",
	MAME_DIR .. "src/mame/audio/starwars.cpp",
	MAME_DIR .. "src/mame/drivers/tempest.cpp",
	MAME_DIR .. "src/mame/drivers/tomcat.cpp",
	MAME_DIR .. "src/mame/machine/asic65.cpp",
	MAME_DIR .. "src/mame/machine/asic65.h",
	MAME_DIR .. "src/mame/machine/atari_vg.cpp",
	MAME_DIR .. "src/mame/machine/atari_vg.h",
	MAME_DIR .. "src/mame/machine/atarigen.cpp",
	MAME_DIR .. "src/mame/machine/atarigen.h",
	MAME_DIR .. "src/mame/machine/mathbox.cpp",
	MAME_DIR .. "src/mame/machine/mathbox.h",
	MAME_DIR .. "src/mame/machine/slapstic.cpp",
	MAME_DIR .. "src/mame/audio/atarijsa.cpp",
	MAME_DIR .. "src/mame/audio/atarijsa.h",
	MAME_DIR .. "src/mame/audio/cage.cpp",
	MAME_DIR .. "src/mame/audio/cage.h",
	MAME_DIR .. "src/mame/audio/redbaron.cpp",
	MAME_DIR .. "src/mame/audio/redbaron.h",
	MAME_DIR .. "src/mame/video/atarimo.cpp",
	MAME_DIR .. "src/mame/video/atarimo.h",
	MAME_DIR .. "src/mame/video/atarirle.cpp",
	MAME_DIR .. "src/mame/video/atarirle.h",


-- createMAMEProjects(_target, _subtarget, "cinemat")
	MAME_DIR .. "src/mame/drivers/ataxx.cpp",
	MAME_DIR .. "src/mame/drivers/cinemat.cpp",
	MAME_DIR .. "src/mame/includes/cinemat.h",
	MAME_DIR .. "src/mame/audio/cinemat.cpp",
	MAME_DIR .. "src/mame/video/cinemat.cpp",
	MAME_DIR .. "src/mame/drivers/cchasm.cpp",
	MAME_DIR .. "src/mame/includes/cchasm.h",
	MAME_DIR .. "src/mame/machine/cchasm.cpp",
	MAME_DIR .. "src/mame/audio/cchasm.cpp",
	MAME_DIR .. "src/mame/video/cchasm.cpp",
	MAME_DIR .. "src/mame/drivers/dlair.cpp",
	MAME_DIR .. "src/mame/drivers/dlair2.cpp",
	MAME_DIR .. "src/mame/drivers/embargo.cpp",
	MAME_DIR .. "src/mame/drivers/jack.cpp",
	MAME_DIR .. "src/mame/includes/jack.h",
	MAME_DIR .. "src/mame/video/jack.cpp",
	MAME_DIR .. "src/mame/drivers/leland.cpp",
	MAME_DIR .. "src/mame/includes/leland.h",
	MAME_DIR .. "src/mame/machine/leland.cpp",
	MAME_DIR .. "src/mame/audio/leland.cpp",
	MAME_DIR .. "src/mame/video/leland.cpp",

-- other systems
	MAME_DIR .. "src/mame/drivers/aztarac.cpp",
	MAME_DIR .. "src/mame/includes/aztarac.h",
	MAME_DIR .. "src/mame/audio/aztarac.cpp",
	MAME_DIR .. "src/mame/video/aztarac.cpp",
	MAME_DIR .. "src/mame/drivers/omegrace.cpp",

-- sega g80
	MAME_DIR .. "src/mame/audio/segasnd.cpp",
	MAME_DIR .. "src/mame/audio/segasnd.h",
	MAME_DIR .. "src/mame/drivers/segag80r.cpp",
	MAME_DIR .. "src/mame/includes/segag80r.h",
	MAME_DIR .. "src/mame/machine/segag80.cpp",
	MAME_DIR .. "src/mame/machine/segag80.h",
	MAME_DIR .. "src/mame/audio/segag80r.cpp",
	MAME_DIR .. "src/mame/video/segag80r.cpp",
	MAME_DIR .. "src/mame/drivers/segag80v.cpp",
	MAME_DIR .. "src/mame/includes/segag80v.h",
	MAME_DIR .. "src/mame/audio/segag80v.cpp",
	MAME_DIR .. "src/mame/video/segag80v.cpp",
}
end

function linkProjects_mame_vector(_target, _subtarget)
	links {
		"mame_vector",
	}
end
