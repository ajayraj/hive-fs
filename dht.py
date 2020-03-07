import opendht as dht

node = dht.DhtRunner()
node.run()

# Join the network through any running node,
# here using a known bootstrap node.
node.bootstrap("bootstrap.ring.cx", "4222")

# blocking call (provide callback arguments to make the call non-blocking)
node.put(dht.InfoHash.get("unique_key"), dht.Value(b'some binary data'))

results = node.get(dht.InfoHash.get("unique_key"))
for r in results:
    print(r)
