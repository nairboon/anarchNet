Anarchnet provides the following interfaces:

- bootstrap(peer)
- bootstrap(list)
- bootstrapBT([BT-masterlist])
- BFbootstrap()

- store(key,data,meta,priority,ttl,pkey)
- get(key)
- search(keyword)

- createSession(type)
	* plain text
	* streaming (video/audio)
- joinSession(sessionKey)
- stream(sessionKey)
- sendMessage(sessionKey)

Dependencies:
 * google-glog
 * google-gtest
 * google-protobuf >= 2.3
 * boost >= 1.42
 * maidsafe_dht	! requires gcc >= 4.3
 

cd third-party
sh install-dependencies.sh

This will install all the dependencies


Structure:
	*an			anarchnet packages
		*core 		plugins
		*universe	web*extensions
	*browser		browser app
		*http		http layer
	*daemon			daemon app
		*protocol	protobuf files
	*library		library interface
	*third-party		ext. software, dependencies