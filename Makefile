
test: clean
	expresso -s -I lib test/*.test.js

test-cov: clean
	expresso -s -I lib -c test/*.test.js	

clean:
	echo "db.getCollectionNames().forEach(function(item){db[item].remove();});" > make.js && mongo --quiet testing make.js && rm make.js

all:
	test

.PHONY: test test-cov
