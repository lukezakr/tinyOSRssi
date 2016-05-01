import NetworkMap


test1 = NetworkMap.NetworkMap("fileInputIdeal2.txt")
test1.findOrigin()
test1.findNetworkSize()
test1.buildNetworkTable()
test1.findCoordinates()
test1.writeCoordinatesFile('inputSquareIdeal.txt')
