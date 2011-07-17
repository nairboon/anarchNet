
test: 
	expresso -s -I lib test/*.test.js

test-cov: 
	expresso -s -I lib -c test/*.test.js	

all:
	test

.PHONY: test test-cov
