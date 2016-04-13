import NetworkMap


test1 = NetworkMap.NetworkMap("sampleInput2.txt")
test1.findOrigin()
test1.findNetworkSize()
test1.buildNetworkTable()
test1.findCoordinates()
test1.writeCoordinatesFile('outputfile.txt')
