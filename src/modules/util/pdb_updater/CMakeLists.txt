project(anarchNet)

################################################################################
# 			P2p DB Updater		                               #
################################################################################

set(PDB_SRC 	pdb_updater.cc
	      	pdb_updater.h
	)
add_library(util_pdb_updater SHARED ${PDB_SRC})
target_link_libraries(util_pdb_updater an)
