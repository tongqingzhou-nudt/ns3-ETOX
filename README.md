ns3-ETOX
========

A coding-aware routing protocol for multi-hop wireless networks implemented in ns-3. The ETOX paper will be presented at MASS 2013 in October 2013 in Hangzhou, China.

Installation
------------

You will need ns-3.17. Not other versions of ns-3.

To install:

cd etox

cp -rf wifi TxRatio etox mync YOUR_NS_DIR/src/ 

cd YOUR_NS_DIR

./waf clean & ./waf configure -d debug

For now, please do noe enable tests. Otherwise you may run into problem when ns-3 compiling the wifi code.

I attach a very small example in the scratch folder. To use it:

	cd ETOX_DIR
	cp scratch/etox-mync.cc YOUR_NS_DIR/scratch
	cd YOUR_NS_DIR
	./waf
	./waf --run scratch/etox-mync
