


redesign of db store
use bcrypt

* db interface :
	get - get the document or the data
		apply patches
		cache??
		
	store - adds a new document
	update
		check owner
		check mergeconflict
		apply patch
		
	data:
		mainbranch id
		list of branches
		

		
		branch
			list of snapshots
			list of diffs
			
add a new document
	d(
		owner: me
		mainbranch: 1
		branches: (
			snapshots (
				none
			)
			diffs (
					myfirstedit ( 
						prev: none
						date:1.11.
						data: "LLLADS")
				)
		)
	
	)
	
edit document
diff + ( new edit())
	merge error:
		new branch

owner checking:
	single owner: username
	group owner: link to groupaccess list
	
	
vote process
flagvoteprotected?
	create new document vote, entries
	